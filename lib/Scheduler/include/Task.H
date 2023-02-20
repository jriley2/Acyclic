
#pragma once

#include "BaseTask.H"
#include "Dependency.H"
#include <cxxabi.h>
#include <functional>
#include <iostream>

constexpr bool ACYCLIC_TRACE = false;

namespace RunTimeInternal
{
  template<class T>
  struct FuncTraits;

  template<typename R>
  struct FuncTraits<std::function<R(void)>>
  {
    static constexpr size_t nArgs = 0;

    using return_type = R;
    using args_tuple_type = std::tuple<>;

    template<size_t i>
    struct arg
    {
      using type = typename std::tuple_element<i, std::tuple<>>::type;
    };
  };

  template<typename R, typename... Args>
  struct FuncTraits<std::function<R(Args...)>>
  {
    static constexpr size_t nArgs = sizeof...(Args);

    using return_type = R;
    using args_tuple_type = std::tuple<Args...>;

    template<size_t i>
    struct arg
    {
      using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
    };
  };

  // implementation of tasks
  template<typename FuncType>
  class Task : public BaseTask
  {
    using return_type = typename FuncTraits<FuncType>::return_type;
    using TupleArgs = typename FuncTraits<FuncType>::args_tuple_type;
    static constexpr std::size_t c_nArgs = FuncTraits<FuncType>::nArgs;
    static constexpr bool c_voidRet = std::is_same<return_type, void>::value;
    // this is needed because member data cannot be void type
    using RetType = std::conditional_t<c_voidRet, char, return_type>;

   public:

    template<typename FuncType_>
    Task(const std::string& a_name, FuncType_&& a_func, const TupleArgs& a_args, const size_t a_id)
        : BaseTask(a_name, a_id),
          m_func(std::forward<FuncType_>(a_func)),
          m_args(a_args)
    {
    }

    template<typename FuncType_>
    Task(const std::string& a_name, FuncType_&& a_func, const size_t a_id)
        : BaseTask(a_name, a_id),
          m_func(std::forward<FuncType_>(a_func)),
          m_args()
    {
    }

    virtual ~Task() = default;

    virtual void print()
    {
      int status;
      char* realname = abi::__cxa_demangle(m_func.target_type().name(), nullptr, nullptr, &status);

      std::cout << "TaskNode:\n"
                << "   name       : " << m_name << "\n"
                << "   mangled    : " << m_func.target_type().name() << "\n"
                << "   unmangled  : " << realname << std::endl;
      free(realname);
    }

    virtual void operator()()
    {
      if constexpr (ACYCLIC_TRACE)
      {
        // add_traceEvent(name, category, Event::Begin, timestamp /* ms */, pid, tid /* ,
        // args... */);
      }

      exec_1(std::make_index_sequence<c_nArgs>{});

      if constexpr (ACYCLIC_TRACE)
      {
        // add_traceEvent(name, category, Event::End, timestamp /* ms */, pid, tid /* ,
        // args...
        // */);
      }

      releaseSuccessors();
    };

   protected:

    // only if the return type of the task function is void
    template<typename Ret = return_type,
             typename std::enable_if<std::is_same<Ret, void>::value>::type* = nullptr,
             std::size_t... Is>
    void exec_1(std::index_sequence<Is...>)
    {
      // c++17 only, so use enable_if
      // if constexpr (c_voidRet)
      //   m_func(std::get<Is>(m_args)...);
      // else
      //   m_retVal = m_func(std::get<Is>(m_args)...);
      m_func(std::get<Is>(m_args)...);
    }

    // only if the return type of the task function is not void
    template<typename Ret = return_type,
             typename std::enable_if<!std::is_same<Ret, void>::value>::type* = nullptr,
             std::size_t... Is>
    void exec_1(std::index_sequence<Is...>)
    {
      m_retVal = m_func(std::get<Is>(m_args)...);
    }

   protected:

    // function related data
    FuncType m_func;
    TupleArgs m_args;
    RetType m_retVal;
  };
}
