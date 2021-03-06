
#include "Event.hpp"
#include <string>
#include <iostream>

using namespace CodeSample;
void Print() { std::cout << "void()" << std::endl; }
void Print1(int) { std::cout << "void(int)" << std::endl; }
void Print2(int, int) { std::cout << "void(int, int)" << std::endl; }

void DelegateTest1() {

  // Non member function
  Delegate<void()> dg0 = Delegate<void()>::FromFunction<&Print>();
  Delegate<void(int)> dg1 = Delegate<void(int)>::FromFunction<&Print1>();
  Delegate<void(int, int)> dg2 = Delegate<void(int, int)>::FromFunction<&Print2>();

  dg0();  // "void ()"
  dg1(1);  // "void (int)"
  dg2(1, 2); // "void (int, int)"
}

void DelegateTest2() {
  std::string s = "Delegate";
  Delegate<void()> dgs = Delegate<void()>::FromMethod<std::string, &std::string::clear>(&s);
  std::cout << "s: " << s << std::endl;
  dgs();  // s == ""
  std::cout << "s: " << s << std::endl;
}

void DelegateTest3() {
  // Members functions
  std::vector<int> v;
  Delegate<void(const int&)> dgv = Delegate<void(const int&)>::FromMethod<std::vector<int>, &std::vector<int>::push_back>(&v);
  dgv(1);  // v: 1
  std::cout << "v: " << v.front() << std::endl;
}

void EventTest1() {
  using TestEvent = CodeSample::Event<void()>;
  TestEvent mTestEvent;
  std::cout << "Register DelegateTest1..." << std::endl;
  mTestEvent.Register(CodeSample::Delegate<void()>::FromFunction<&DelegateTest1>());
  std::cout << "Bind Test Event..." << std::endl;
  mTestEvent.Bind();
  std::cout << "Reset Test Event..." << std::endl;
  mTestEvent.Reset();
  std::cout << "Bind the Event again..." << std::endl;
  mTestEvent.Bind();
  std::cout << "If you see nothing, it works!" << std::endl;
  mTestEvent.Clear();
}

int main()
{
  typedef void(*TestFunc)();
  TestFunc mTests[] = { DelegateTest1
    , DelegateTest2
    , DelegateTest3
    , EventTest1 };

  for (unsigned int i = 0,
    size = static_cast<unsigned int>(sizeof(mTests) / sizeof(TestFunc));
    i < size; ++i) {
    mTests[i]();
  }

  return 0;
}
