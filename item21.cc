// Item 21: Prefer std::make_unique and std::make_shared to direct use of new.
//
// Using make functions has these advantages:
// 1. Less verbose type specifier. make functions perfect-forwards args.
// 2. Exception safety. Compiler can squeeze in another non-exception-safe call
//    between ctor and new, thus leading to potential memory leak.
// However, they have drawbacks too:
// 1. Cannot specify custom deleter.
// 2. Cannot perfect-forward std::initializer_list.
// 3. (Only applicable to shared_ptr) make_shared allocates 1 chunk for both
//    ctrl block and object. This is a performance gain as the ctor+new way
//    needs 2 allocations. But it also means even if all shared_ptrs are gone,
//    with make_shared, the object may still not be deallocated if there are
//    weak_ptr's referring to the ctrl blcok. With ctor+new approach, the ctrl
//    block isn't deallocated due to weak_ptr, but the object can be deallocated.
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;

// C++11 doesn't have std::make_unique.
template <typename T, typename... Ts>
unique_ptr<T> make_unique(Ts... args) {
  return unique_ptr<T>(new T(forward<Ts>(args)...));
}

int computePriority() {
  throw "compute priority failed.";
}

class Widget {};

void processWidget(shared_ptr<Widget> sp, int priority) {}

class LargeObject {};

int main() {
  // Example 1: test make_unique implementation and initializer_list perfect forward.
  // std::initializer_list cannot be perfect forwarded. So this creates a vector with
  // 20 elements, each has value 10.
  auto up = make_unique<vector<int>>(10, 20);
  assert(up->size() == 10);
  for (const auto& v : *up) assert(v == 20);
  auto list = {10, 20};
  auto up2 = make_unique<vector<int>>(list);
  assert(up2->size() == 2);
  assert((*up2)[0] == 10);
  assert((*up2)[1] == 20);

  // Example 2: exception safety.
  try {
    // For exception safety, do not use shared_ptr<Widget>(new Widget).
    processWidget(make_shared<Widget>(), computePriority());
  } catch (const char* ex) {
    cerr << "process widget failed: " << ex << endl;
  }

  // Example 3: Large object not deallocated with make function.
  auto sp_make = make_shared<LargeObject>();
  weak_ptr<LargeObject> wp_make(sp_make);
  sp_make.reset();  // Underlying LargeObject is still not deallocated due to wp.
  auto sp_ctor = shared_ptr<LargeObject>(new LargeObject);
  weak_ptr<LargeObject> wp_ctor(sp_ctor);
  sp_ctor.reset();  // Underlying LargeObject is deallocated. Ctrl block is still there.
  
  return 0;
}
