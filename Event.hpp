#pragma once

#include <vector>
#include <functional>
#include "Delegate.hpp"

namespace CodeSample
{
  template <class, class...>
  class Event;

  template <class ReturnType, class... Arguments>
  class Event<ReturnType(Arguments...)>
  {
    typedef Delegate<ReturnType(Arguments...)> Delegate;
  public:
    // -------------------------------------------------------------------------
    // public member methods
    // -------------------------------------------------------------------------
    inline Event() : mDelegates(), mSweep(false) {}
    inline ~Event() {}
    
    // bind an event
    inline void Bind(Arguments... args) {
      Run(std::bind(&Delegate::Call, std::placeholders::_1, args...));
    }
    // bind an object specified object call with arguments
    inline void ObjectBind(void* obj, Arguments... args) {
      Run(std::bind(&Delegate::ObjectCall, std::placeholders::_1, obj, args...));
    }
    
    // add delegator into delegate vector
    inline void Register(const typename Delegate& delegator) {
      mDelegates.push_back(delegator);
    }

    inline void operator()(Arguments... args) {
      Bind(args...);
    }
    inline void operator+=(const typename Delegate& delegator) {
      Register(delegator);
    }

    // delete a listener function from the event.
    inline void Unregister(const typename Delegate& delegator) {
      for (auto iter : mDelegates.begin(); iter != mDelegates.end(); ++iter) {
        Delegate& runnable = *iter;
        if (runnable == delegator) {
          runnable.Reset();
          mSweep = true;
          break;
        }
      }
    }

    inline void operator-=(const typename Delegate& delegator)
    { Unregister(delegator); }

    // clear all event listeners.
    inline void Clear() { mDelegates.clear(); }

    inline void Reset() { Clear(); }

  public:
    // -------------------------------------------------------------------------
    // public static methods
    // -------------------------------------------------------------------------
    // Wrapper functions to get delegates

    // for a function
    template<ReturnType(*ObjMethod)(Arguments...)>
    inline static typename Delegate Function() {
      return Delegate::FromFunction<ObjMethod>();
    }

    // for a class method delegate.
    template<class Obj, ReturnType(Obj::*ObjMethod)(Arguments...)>
    inline static typename Delegate Method(Obj* object_ptr) {
      return Delegate::FromMethod<Obj, ObjMethod >(object_ptr);
    }

    // for a class const method delegate.
    template<class Obj, ReturnType(Obj::*ObjMethod)(Arguments...) const>
    inline static typename Delegate CMethod(const Obj* object_ptr) {
      return Delegate::FromConstMethod<Obj, ObjMethod>(object_ptr);
    }

  private:
    // -------------------------------------------------------------------------
    // private member methods
    // -------------------------------------------------------------------------
    using DelegateVector = std::vector<class Delegate>;

    void Sweep() {
      DelegateVector::iterator it = mDelegates.begin();
      while (it != mDelegates.end())
      {
        if (*it) {
          ++it;
        }
        else {
          it = mDelegates.erase(it);
        }
      }
      mSweep = false;
    }

    template<typename Function>
    void Run(Function func) {
      for (auto iter = mDelegates.begin(); iter != mDelegates.end(); ++iter)
      {
        Delegate runnable = *iter;
        if (runnable) {
          func(runnable);
        }
        if (mSweep) {
          Sweep();
        }
      }
    }

  private:
    // -------------------------------------------------------------------------
    // private member variables
    // -------------------------------------------------------------------------
    typename DelegateVector mDelegates; // vector of event delegate
    bool     mSweep; // if true, clean delegates.
  };
} // Event.hpp