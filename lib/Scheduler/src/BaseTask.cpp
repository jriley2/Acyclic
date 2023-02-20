
#include "BaseTask.H"
#include "Scheduler.H"
#include <iostream>

namespace RunTimeInternal
{
  void BaseTask::addSuccessor(std::shared_ptr<BaseTask> a_successor)
  {
    std::lock_guard<std::mutex> lock(m_mtx);
    m_successors.push_back(a_successor);
    a_successor->m_cntPrereqs++;
  }

  void BaseTask::releaseSuccessors()
  {
    for (auto& successor : m_successors)
    {
      bool lastPrereq = successor->satisfyPrereq();
      if (lastPrereq)
      {
        std::cout << "push" << std::endl;
        Scheduler::getScheduler().push(successor); // TODO push to queue;
      }
    }
  }

  bool BaseTask::satisfyPrereq()
  {
    size_t cntPrereq = m_cntPrereqs.fetch_sub(1);
    return cntPrereq == 1;
  }
}
