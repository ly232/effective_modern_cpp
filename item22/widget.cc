#include <string>
#include <vector>
#include "widget.h"

struct UniqueWidget::UniqueWidgetImpl {
  std::vector<int> vec;
  std::string id;
};

// Default ctor needs to be customized to initialize impl.
UniqueWidget::UniqueWidget() :
  impl_(make_unique<UniqueWidgetImpl>()) {};

// Put destructor here to make sure compiler sees full
// definition of UniqueWidgetImpl.
UniqueWidget::~UniqueWidget() = default;

UniqueWidget::UniqueWidget(UniqueWidget&& rhs) = default;

UniqueWidget& UniqueWidget::operator=(UniqueWidget&& rhs) = default;

UniqueWidget::UniqueWidget(const UniqueWidget& rhs) :
  impl_(make_unique<UniqueWidgetImpl>(*rhs.impl_)) {};

UniqueWidget& UniqueWidget::operator=(const UniqueWidget& rhs) {
  if (this != &rhs) {
    // Note rhs.impl_ is legal even though impl_ is private.
    // Access scope is on class level rather than object level.
    *impl_ = *rhs.impl_;
  }
  return *this;
};
