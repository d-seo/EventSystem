/*!
* Reference :
https://www.codeproject.com/Articles/11015/The-Impossibly-Fast-C-Delegates
http://codereview.stackexchange.com/questions/14730/impossibly-fast-delegate-in-c11
http://stackoverflow.com/questions/2023977/c-difference-of-keywords-typename-and-class-in-templates
*/

#pragma once

#define FORCE_CONVENTION_FASTCALL
#ifdef FORCE_CONVENTION_FASTCALL
# define DELEGATE_CALL_CONVENTION __fastcall
#else
# define DELEGATE_CALL_CONVENTION
#endif

namespace CodeSample
{
  template <class ReturnType> class Delegate;

  template <class ReturnType, class... Arguments>
  class Delegate<ReturnType(Arguments...)>
  {
    typedef ReturnType(DELEGATE_CALL_CONVENTION * SignatureType)(Arguments...);
  public:
    // -------------------------------------------------------------------------
    // public static methods
    // -------------------------------------------------------------------------

    // function delegate getter
    template<ReturnType(*TMethod)(Arguments...)>
    inline static Delegate FromFunction() {
      return FromStub(nullptr, &FunctionStub<TMethod>);
    }

    // class method delegate getter
    template<class T, ReturnType(T::*TMethod)(Arguments...)>
    inline static Delegate FromMethod(T* objectPtr) {
      return FromStub(objectPtr, &MethodStub<T, TMethod>);
    }

    // class const methdo delegate getter
    template<class T, ReturnType(T::*TMethod)(Arguments...) const>
    inline static Delegate FromConstMethod(T const * objectPtr) {
      return FromStub(const_cast<T*>(objectPtr), &ConstMethodStub<T, TMethod>);
    }

  public:
    // -------------------------------------------------------------------------
    // public member methods
    // -------------------------------------------------------------------------
    // reset the delegate to nullptr
    inline void Reset() { mStub_ptr = nullptr; }

    // trigger delegate.
    inline ReturnType ObjectCall(void* obj, Arguments... args) const {
      return (*mStub_ptr)(obj, args...);
    }
    inline ReturnType Call(Arguments... args) const {
      return ObjectCall(mObj_ptr, args...);
    }

    // operator overloading
    inline ReturnType operator()(Arguments... args) const {
      return Call(args...);
    }

    // check if delegate is empty.
    inline operator bool() const {
      return (mStub_ptr != nullptr);
    }
    inline bool operator!() const {
      return !(operator bool());
    }
    inline bool operator<(const Delegate & rhs) const {
      return ((mObj_ptr < rhs.mObj_ptr) && (mStub_ptr < rhs.mStub_ptr));
    }
    inline bool operator>(const Delegate & rhs) const {
      return ((mObj_ptr > rhs.mObj_ptr) && (mStub_ptr > rhs.mStub_ptr));
    }
    inline bool operator==(const Delegate & rhs) const {
      return ((rhs.mObj_ptr == mObj_ptr) && (rhs.mStub_ptr == mStub_ptr));
    }
    inline bool operator!=(const Delegate & rhs) const {
      return ((rhs.mObj_ptr != mObj_ptr) || (rhs.mStub_ptr != mStub_ptr));
    }

    // function callback type
    typedef ReturnType(DELEGATE_CALL_CONVENTION * StubType)(void* object_ptr
      , Arguments...);

    // constructors...
    inline Delegate() : mObj_ptr(nullptr), mStub_ptr(nullptr) { }
    inline Delegate(void* obj_ptr, StubType stub_ptr)
      : mObj_ptr(obj_ptr), mStub_ptr(stub_ptr) { }

  private:
    // -------------------------------------------------------------------------
    // private static methods
    // -------------------------------------------------------------------------
    // get delegate from stub
    inline static Delegate FromStub(void* obj_ptr, StubType stubType) {
      return Delegate(obj_ptr, stubType);
    }

    // call the delegate function stub
    template<ReturnType(*TMethod)(Arguments...)>
    inline static ReturnType DELEGATE_CALL_CONVENTION FunctionStub(void*
      , Arguments... args) {
      return (TMethod)(args...);
    }

    // call the delegate method stub
    template<class T, ReturnType(T::*TMethod)(Arguments...)>
    inline static ReturnType DELEGATE_CALL_CONVENTION MethodStub(void* object_ptr
      , Arguments... args) {
      T* p = static_cast<T*>(object_ptr);
      return (p->*TMethod)(args...);
    }

    // Call the delegate const method 'stub'.
    template<class T, ReturnType(T::*TMethod)(Arguments...) const>
    inline static ReturnType DELEGATE_CALL_CONVENTION ConstMethodStub(void* object_ptr
      , Arguments... args)
    {
      const T* p = static_cast<T*>(object_ptr);
      return (p->*TMethod)(args...);
    }

  private:
    // -------------------------------------------------------------------------
    // private member variables
    // -------------------------------------------------------------------------
    void*    mObj_ptr;  // object pointer.
    StubType mStub_ptr; // function pointer.
  };
}
