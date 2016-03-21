// Item 23: Understand std::move and std::forward.
#include <iostream>
#include <type_traits>

using namespace std;

namespace item23 {
// An impl of move. std lib doesn't necessary do it this way.
template <typename T>
typename remove_reference<T>::type&& move(T&& arg) {
  cout << "calling custom move." << endl;
  using ReturnType =
    typename remove_reference<T>::type&&;
  return static_cast<ReturnType>(arg);
}
}  // end namespace

class Widget {
public:
  Widget() = default;
  explicit Widget(const Widget&) {
    cout << "calling copy ctor." << endl;
  }
  explicit Widget(Widget&&) {
    cout << "calling move ctor." << endl;
  }
};

void Process(const Widget&) {
  cout << "calling lval ref process." << endl;
}

void Process(Widget&&) {
  cout << "calling rval ref process." << endl;
}

template <typename... T>
void PerfectForwarding(T&&... args) {
  Process(forward<T>(args)...);
}

int main() {
  Widget w1;
  auto w2(w1);  // should call copy ctor.
  auto w3(std::move(w1));  // should call move ctor.
  auto w4(item23::move(w2));  // should call move stor.
  PerfectForwarding(w4);  // should call lval ref process.
  PerfectForwarding(move(w4));  // should call rval ref process.
  return 0;
}
