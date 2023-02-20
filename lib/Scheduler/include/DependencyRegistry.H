#pragma once

#include "BaseTask.H"
#include "Dependency.H"
#include <list>
#include <memory>
#include <unordered_map>

namespace RunTimeInternal
{
  class DepRegistry
  {
    static constexpr std::size_t DepSz = 256;

    class DepInfo
    {
     public:

      DepInfo(Dependency& a_dependency, std::shared_ptr<BaseTask>& a_task)
          : m_dependency(a_dependency),
            m_task(a_task)
      {
      }

      ~DepInfo() = default;

      std::shared_ptr<BaseTask> getTask()
      {
        return m_task;
      }

      Dependency& getDependency()
      {
        return m_dependency;
      }

     private:

      Dependency m_dependency;
      std::shared_ptr<BaseTask> m_task;
    };

    using UMap = std::unordered_map<void*, std::list<DepInfo>>;

   public:

    DepRegistry()
    {
      m_registry.max_load_factor(0.7);
      m_registry.reserve(DepSz);
    }

    ~DepRegistry() = default;

    /// Insert a mark dependency
    size_t insert(std::shared_ptr<BaseTask> a_task, Dependency& a_dependency)
    {
      using TaskList = typename UMap::mapped_type;

      DepInfo depInfo(a_dependency, a_task);
      size_t cntPrereqsFound = 0;
      m_lock.lock(); ////////// LOCK
      auto insertPair = m_registry.insert({a_dependency.key(), std::list<DepInfo>()});
      if (insertPair.second)
      {
        // Insert successful. Key was not in map.
        // There are no prerequisites.
        TaskList& taskList = insertPair.first->second;
        taskList.emplace_front(depInfo);
        m_lock.unlock(); ////// UNLOCK a
      }
      else
      {
        // Insert unsuccessful. Key was already in map.
        // Append new entry (check for prerequisites)
        TaskList& taskList = insertPair.first->second;
        taskList.emplace_front(depInfo);
        auto iterList = taskList.begin();
        m_lock.unlock(); ////// UNLOCK b

        const auto endIterList = taskList.end();
        cntPrereqsFound = findPrereqsInTaskList(iterList, endIterList, depInfo);
      }
      return cntPrereqsFound;
    }

   private:

    static size_t findPrereqsInTaskList(std::list<DepInfo>::iterator& a_iterDepList,
                                        const std::list<DepInfo>::iterator& a_endIterList,
                                        DepInfo& a_depInfo)
    {
      size_t cntPrereqsFound = 0;
      for (++a_iterDepList; a_iterDepList != a_endIterList; ++a_iterDepList)
      {
        auto& prevEntry = *a_iterDepList;
        auto [foundPrereq, isLast] =
          prevEntry.getDependency().isPrereqOf(a_depInfo.getDependency());
        if (foundPrereq)
        {
          cntPrereqsFound++;
          auto predecessor = prevEntry.getTask();
          predecessor->addSuccessor(a_depInfo.getTask());
          // Continue loop if dependency was write-after-read (look for
          // more reads)
          if (isLast)
          {
            break;
          }
        }
      }
      return cntPrereqsFound;
    }

   private:

    UMap m_registry;
    std::mutex m_lock;
  };
}
