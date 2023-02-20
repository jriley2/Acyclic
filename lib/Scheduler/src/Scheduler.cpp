#include "Scheduler.H"

thread_local size_t stl_threadID;

namespace RunTimeInternal
{
  void Scheduler::start(const size_t a_numThreads)
  {
    assert(!m_started);
    m_started = true;
    m_numThreads = a_numThreads;
    for (size_t i = 1; i < m_numThreads; ++i)
    {
      m_workerThreads.emplace_back(
        [this, id = i]
        {
          stl_threadID = id;

          // while scheduler is not done, get tasks from the queue
          while (!m_done)
          {
            this->popAndRun();
          }
        });
    }
  }

  void Scheduler::finish()
  {
    m_finishing = true;
    while (!m_done)
    {
      this->popAndRun();
    }
    m_queue_cv.notify_all();
    {
      for (auto thread = m_workerThreads.begin(); thread != m_workerThreads.end(); thread++)
      {
        // wait for the computing threads to finish
        if (thread->joinable())
        {
          m_queue_cv.notify_all();
          thread->join();
        }
      }
      m_workerThreads.clear();
    }
  }

  std::shared_ptr<BaseTask> Scheduler::pop(bool& a_success)
  {
    std::shared_ptr<BaseTask> taskptr(nullptr);
    std::unique_lock<std::mutex> lock(m_queue_mtx);
    if (m_done)
    {
      a_success = false;
      return taskptr;
    }
    m_queue_cv.wait(lock, [this] { return (!m_queue.empty() || m_done); });
    if (m_done || m_queue.empty())
    {
      a_success = false;
    }
    else
    {
      // move from queue to object passed by reference
      taskptr = m_queue.front();
      // remove object from queue
      m_queue.pop();
      a_success = true;
    }
    return taskptr;
  }

  void Scheduler::popAndRun()
  {
    bool success = false;
    std::shared_ptr<BaseTask> task = pop(success);
    // retrieve task from task queue
    if (success)
    {
      task->operator()();
      int cntTasks = m_cntTasks.fetch_sub(1);
      if (cntTasks == 1 && m_finishing)
      {
        m_done = true;
        m_queue_cv.notify_all();
      }
    }
  }
}
