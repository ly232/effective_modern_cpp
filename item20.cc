// Item 20: Use std::weak_ptr for std::shared_ptr-like pointers that can dangle.
//
// std::weak_ptr is generated from std::shared_ptr, but doesn't participate in
// reference count. It offers a method std::weak_ptr::lock(), which returns a
// std::shared_ptr, and it's null if the pointee is already destroyed.
// There's also a method std::weak_ptr::expired(), which atomically tests whether
// the pointee is destroyed.
//
// A few applications of std::weak_ptr:
// 1) cache
// 2) observer pattern
// 3) cyclic pointers
#include <iostream>
#include <memory>
#include <unordered_map>

using namespace std;

// std::weak_ptr example 1: cache.
// A cache client get() call gets a shared_ptr of data. Cache holds a map from id
// to weak_ptr. If many cache clients requests the same id, cache can serve from
// cache instead of an expensive load. On the other hand if a data is no longer
// used by any client, it should be destroyed via shared_ptr destructor mechanism.
// This works because cache holds weak_ptr so it doesn't affect ref count.
class Cache {
public:
  shared_ptr<string> get(string id) {
    auto sp = cache_[id].lock();
    if (!sp) {
      cout << "Loading value for id " << id << endl;
      sp = make_shared<string>("value");
      cache_[id] = sp;
    }
    return sp;
  };
private:
  unordered_map<string, weak_ptr<string>> cache_;
};

// Publisher in a pub-sub setting. Publisher keeps track of all its subscribers as a vector of
// std::weak_ptr's, so that ref count doesn't get affected by the publisher monitor (publisher
// doesn't care about each subscriber's lifetime anyway; it only cares if a subscriber is up,
// it should receive subsequent streams).
class Subscriber {
public:
  template <typename... Ts>
  static shared_ptr<Subscriber>
  createSubscriber(Ts... args) {
    return shared_ptr<Subscriber>(new Subscriber(forward<Ts>(args)...));
  };
  void Receive(int val) {
    cout << "subscriber " << id_ << " received val " << val << endl;
  };
  string GetId() {
    return id_;
  };

  Subscriber() = delete;
  Subscriber(const Subscriber&) = delete;
  Subscriber& operator=(const Subscriber&) = delete;
  Subscriber(Subscriber&&) = delete;
  Subscriber& operator=(Subscriber&&) = delete;

private:
  Subscriber(string id) : id_(id) {};
  string id_;
};

class Publisher {
public:
  void Publish(int val) {
    cout << "publish " << val << endl;
    for (auto itr : subscribers_) {
      auto key = itr.first;
      auto subscriber = itr.second;
      auto sp = subscriber.lock();
      if (!sp) {
	cout << "skipping destroyed subscriber " << key << endl;
	// TODO: why does the next line lead to seg fault?
	// subscribers_.erase(key);
      } else {
	sp->Receive(val);
      }
    }
  };
  void Register(shared_ptr<Subscriber> subscriber) {
    if (!subscriber) return;
    subscribers_[subscriber->GetId()] = subscriber;
  };
private:
  unordered_map<string, weak_ptr<Subscriber>> subscribers_;
};

struct CycleB;
struct CycleA {
  shared_ptr<CycleB> spb;
};

struct CycleB {
  weak_ptr<CycleA> wpa;
};

int main() {
  // Example 1: cache.
  cout << "===== EXAMPLE 1: CACHE =====" << endl;
  Cache cache;
  cout << "v1" << endl;
  auto v1 = cache.get("odd");  // should load "odd"
  cout << "v2" << endl;
  auto v2 = cache.get("even");  // should load "even"
  cout << "v3" << endl;
  auto v3 = cache.get("odd");  // should NOT reload "odd"
  cout << "v1 reset" << endl;
  v1.reset();
  cout << "v5" << endl;
  auto v5 = cache.get("odd");  // should NOT reload "odd"
  cout << "v2 reset" << endl;
  v2.reset();
  cout << "v4" << endl;
  auto v4 = cache.get("even");  // should load "even"
  cout << "v4 reassign" << endl;
  v4 = v1;
  cout << "v6" << endl;
  auto v6 = cache.get("even");  // should load "even".

  cout << "===== EXAMPLE 2: PUBSUB =====" << endl;
  Publisher pub;
  auto sub1 = Subscriber::createSubscriber("sub1");
  auto sub2 = Subscriber::createSubscriber("sub2");
  pub.Register(sub1);
  pub.Register(sub2);
  pub.Publish(123);  // both sub1 and sub2 should receive this.
  sub1.reset();
  pub.Publish(456);  // only sub2 should receive this.
  sub1 = Subscriber::createSubscriber("sub1");
  pub.Register(sub1);
  pub.Publish(789);  // both sub1 and sub2 should receive this.

  cout << "===== EXAMPLE 3: CYCLE =====" << endl;
  auto ca = shared_ptr<CycleA>(new CycleA);
  auto cb = shared_ptr<CycleB>(new CycleB);
  ca->spb = cb;
  // If wpa were a shared_ptr, after executing next line, ref count for both CycleA and CycleB
  // are 2 (one for ca and cb, one for each object's member). So when ca and cb goes out of
  // scope, memory for CycelA and CycleB objects are not freed since ref count is stil 1.
  cb->wpa = ca;
  
  return 0;
}
