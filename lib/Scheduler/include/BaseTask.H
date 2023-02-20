
#pragma once

#include <atomic>
#include <list>
#include <memory>
#include <mutex>

namespace RunTimeInternal
{
  // abstract of a task
  class BaseTask
  {
   public:

    BaseTask(const std::string& a_name, const size_t a_id)
        : m_id(a_id),
          m_name(a_name){};

    virtual ~BaseTask() = default;

    virtual void operator()() = 0;

    virtual void print() = 0;

    size_t id() const;

    void addSuccessor(std::shared_ptr<BaseTask> a_successor);

    void releaseSuccessors();

    bool satisfyPrereq();

    const std::list<std::shared_ptr<BaseTask>>& successors() const;

   protected:

    // node related data
    std::atomic<size_t> m_cntPrereqs{0};
    std::atomic<size_t> m_numPrereqs{0};
    size_t m_id{0};
    std::list<std::shared_ptr<BaseTask>> m_successors;
    std::mutex m_mtx;
    std::string m_name;
  };

  inline size_t BaseTask::id() const
  {
    return m_id;
  }

  inline const std::list<std::shared_ptr<BaseTask>>& BaseTask::successors() const
  {
    return m_successors;
  }
}
