#pragma once

#include "DependencyRegistry.H"
#include "Task.H"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

extern thread_local size_t stl_threadID;

namespace RunTimeInternal
{
  class Scheduler
  {

    Scheduler() = default;

   public:

    static Scheduler& getScheduler()
    {
      // static variables with block scope are threadsafe
      static Scheduler instance;
      return instance;
    }

    virtual ~Scheduler() = default;

    /// Copy constructor
    Scheduler(const Scheduler& other) = delete;

    /// Move constructor
    Scheduler(Scheduler&& other) = delete;

    /// Assignment constructor
    Scheduler& operator=(const Scheduler& other) = delete;

    /// Move assignment
    Scheduler& operator=(Scheduler&& other) = delete;

   public:

    void start(const size_t a_numThreads);

    void push(std::shared_ptr<BaseTask> a_task)
    {
      {
        std::lock_guard<std::mutex> lock(m_queue_mtx);
        m_queue.push(a_task);
      }
      m_queue_cv.notify_one();
    }

    void finish();

    template<typename Func, typename Func_, typename... Args_>
    std::shared_ptr<BaseTask> addTask(std::string& a_name, Func_&& a_func, Args_&&... a_args)
    {
      return addTask_<Func>(a_name, std::forward<Func_>(a_func), std::forward<Args_>(a_args)...);
    }

    template<typename Func, typename Func_, typename... Args_>
    std::shared_ptr<BaseTask> addTask(std::string a_name, Func_&& a_func, Args_&&... a_args)
    {
      return addTask_<Func>(a_name, std::forward<Func_>(a_func), std::forward<Args_>(a_args)...);
    }

   private:

    template<typename Func, typename Func_, typename... Args_>
    std::shared_ptr<BaseTask> addTask_(std::string& a_name, Func_&& a_func, Args_&&... a_args)
    {
      // a_func might have an incomplete type (like if it is a lambda),
      // so need to put them in a std::function to get FuncTraits from them
      using FuncType = std::function<Func>;
      using FuncTraits_t = FuncTraits<FuncType>;

      // a_argsDeps needs to be seperated into arguments for a_func and
      // dependencies for the node
      constexpr size_t nDeps = sizeof...(Args_) - FuncTraits_t::nArgs;
      auto targsdeps = std::forward_as_tuple(std::forward<Args_>(a_args)...);
      auto targs = getArgs(targsdeps, std::make_index_sequence<FuncTraits_t::nArgs>{});
      auto tdeps = getDeps<FuncTraits_t::nArgs>(targsdeps, std::make_index_sequence<nDeps>{});
      auto taskptr = std::make_shared<Task<FuncType>>(a_name, FuncType(std::forward<Func_>(a_func)),
                                                      targs, ++m_numTasks);

      ++m_cntTasks;

      bool prereqs = ForEachDep<decltype(tdeps)>::findPrereqs(taskptr, tdeps, m_registry);

      if (!prereqs)
      {
        push(taskptr);
      }
      return taskptr;
    }

    template<typename Tuple, size_t... Is>
    auto getArgs(const Tuple& a_argsdeps, std::index_sequence<Is...>)
    {
      return std::forward_as_tuple(
        std::forward<typename std::tuple_element<Is, Tuple>::type>(std::get<Is>(a_argsdeps))...);
    }

    template<size_t offset, typename Tuple, size_t... Is>
    auto getDeps(const Tuple& a_argsdeps, std::index_sequence<Is...>)
    {
      return std::forward_as_tuple(
        std::forward<typename std::tuple_element<Is + offset, Tuple>::type>(
          std::get<Is + offset>(a_argsdeps))...);
    }

    template<typename Tuple, std::size_t J = std::tuple_size<Tuple>::value>
    struct ForEachDep
    {
      static constexpr std::size_t N = std::tuple_size<Tuple>::value;
      static constexpr std::size_t I = N - J;

      static size_t findPrereqs(std::shared_ptr<BaseTask> a_taskptr,
                                const Tuple& a_deps,
                                DepRegistry& a_registry)
      {
        size_t cntPrereqs = findPrereqs(a_taskptr, a_deps, a_registry, false);
        cntPrereqs += ForEachDep<Tuple, J - 1>::findPrereqs(a_taskptr, a_deps, a_registry);
        return cntPrereqs;
      }

      static size_t findPrereqs(std::shared_ptr<BaseTask> a_taskptr,
                                const Tuple& a_deps,
                                DepRegistry& a_registry,
                                bool)
      {
        return a_registry.insert(a_taskptr, std::get<I>(a_deps));
      }
    };

    template<typename Tuple>
    struct ForEachDep<Tuple, 0>
    {
      static size_t findPrereqs(std::shared_ptr<BaseTask> a_taskptr,
                                const Tuple& a_deps,
                                DepRegistry& a_registry)
      {
        return 0;
      }
    };

    std::shared_ptr<BaseTask> pop(bool& a_success);

    void popAndRun();

   private:

    bool m_started{false};
    bool m_finishing{false};
    std::vector<std::thread> m_workerThreads;
    size_t m_numThreads{1};
    std::queue<std::shared_ptr<BaseTask>> m_queue;
    std::mutex m_queue_mtx;
    std::condition_variable m_queue_cv;
    DepRegistry m_registry;
    size_t m_numTasks{0};
    std::atomic<bool> m_done{false};
    std::atomic<size_t> m_cntTasks{0};
  };

}
