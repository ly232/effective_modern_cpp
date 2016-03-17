// Item 19: Use std::shared_ptr for shared ownership resource management.
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;

class Widget;

static vector<shared_ptr<Widget>> widgets;

class Widget : public enable_shared_from_this<Widget> {
public:
  template <typename... Ts>
  static shared_ptr<Widget> create(Ts&&... args) {
    // Can't use make_shared here because ctor is private.
    return shared_ptr<Widget>(new Widget(forward<Ts>(args)...));
  };
  void process() {
    cout << "Calling process(). id=" << id_ << endl;
    widgets.emplace_back(shared_from_this());
  };
  string GetId() {
    return id_;
  };
private:
  Widget(string id) : id_(id) {
    cout << "Calling Widget ctor. id=" << id << endl;
  };
  string id_;
};

int main() {
  shared_ptr<Widget> w1 = Widget::create("w1");
  shared_ptr<Widget> w2 = Widget::create("w2");
  shared_ptr<Widget> w3(w1);
  // Next line does not compile since ctor is private.
  // shared_ptr<Widget> w4 = make_shared<Widget>("w4");
  assert(widgets.size() == 0);
  w1->process();
  assert(widgets.size() == 1);
  w2->process();
  assert(widgets.size() == 2);
  w3->process();
  assert(widgets.size() == 3);
  w3.reset();
  assert(w1->GetId() == "w1");
  return 0;
}

