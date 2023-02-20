
#pragma once

#include <cassert>
#include <type_traits>
#include <utility>

namespace RunTimeInternal
{
  enum class DataAccess
  {
    null = 0,
    read = 1 << 0,
    write = 1 << 1,
    readwrite = read | write
  };

  inline DataAccess operator|(DataAccess lhs, DataAccess rhs)

  {
    return static_cast<DataAccess>(static_cast<std::underlying_type<DataAccess>::type>(lhs) |
                                   static_cast<std::underlying_type<DataAccess>::type>(rhs));
  }

  inline DataAccess operator&(DataAccess lhs, DataAccess rhs)
  {
    return static_cast<DataAccess>(static_cast<std::underlying_type<DataAccess>::type>(lhs) &
                                   static_cast<std::underlying_type<DataAccess>::type>(rhs));
  }

  /// Tests the DataAccess for read access
  inline bool hasRead(const DataAccess& a_access)
  {
    return (a_access & DataAccess::read) != DataAccess::null;
  }

  /// Tests the DataAccess for write access
  inline bool hasWrite(const DataAccess& a_access)
  {
    return (a_access & DataAccess::write) != DataAccess::null;
  }

  class Dependency
  {
   public:

    Dependency(void* a_address, DataAccess a_access)
        : m_address(a_address),
          m_access(a_access)
    {
    }

    std::pair<bool, bool> isPrereqOf(const Dependency& a_other) const noexcept
    {
      bool isPrereq = false;
      bool isLast = false;
      if (m_address == a_other.m_address)
      {
        if (hasWrite(a_other.m_access) && hasWrite(m_access)) // Write
        {                                                     // after
                                                              // write
          isLast = true;
          isPrereq = true;
        }
        else if (hasRead(a_other.m_access) && hasWrite(m_access)) // Read
        {                                                         // after
          isLast = true;                                          // write
          isPrereq = true;
        }
        else if (hasWrite(a_other.m_access) && hasRead(m_access)) // Write
        {                                                         // after
          isPrereq = true;                                        // read
        }
        else if (hasRead(a_other.m_access) && hasRead(m_access)) // Read
        {                                                        // after
          isPrereq = false;                                      // read
        }
        else
        {
          // should not fall through
          assert(false);
        }
      }
      return std::make_pair(isPrereq, isLast);
    }

    void* key() const
    {
      return m_address;
    }

    const DataAccess& access() const
    {
      return m_access;
    }

   protected:

    void* m_address;
    DataAccess m_access;
  };
}
