#include <memory>

template <typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts... args) {
  return std::unique_ptr<T>(new T(std::forward<Ts>(args)...));
}

class UniqueWidget {
public:
  UniqueWidget();

  // Cannot rely on default generated destructor.
  // Because it requies complete type on UniqueWidgetImpl.
  ~UniqueWidget();

  // Explicitly declare dtor forces compiler to not generate
  // move operations. Hence explicitly declare them here.
  // And similar to dtor, due to type incompleteness, definitions
  // goes to implementation file.
  UniqueWidget(UniqueWidget&& rhs);
  UniqueWidget& operator=(UniqueWidget&& rhs);

  // Copy ctor and assignment operator needs to be redefined
  // as they are not supported for unique_ptr.
  UniqueWidget(const UniqueWidget& rhs);
  UniqueWidget& operator=(const UniqueWidget& rhs);
private:
  struct UniqueWidgetImpl;
  std::unique_ptr<UniqueWidgetImpl> impl_;
};

class SharedWidget {
public:
  // Unlike UniqueWidget. Here compiler generated ctors,
  // dtors and operator=s are fine, because dtor type is NOT
  // a template type parameter for shared_ptr, and shared_ptr
  // supports both copy and move.
private:
  struct SharedWidgetImpl;
  std::shared_ptr<SharedWidgetImpl> impl_;
};
