// Item 22: When using pimpl idiom, define special member functions in implementation file.
#include <iostream>
#include "widget.h"

using namespace std;

int main() {
  UniqueWidget uw1;
  auto uw2(uw1);  // copy ctor
  auto uw3(move(uw2));  // move ctor
  uw1 = uw3;  // assign op
  uw3 = move(uw1);  // move op

  SharedWidget sw1;
  auto sw2(sw1);  // copy ctor
  auto sw3(move(sw2));  // move ctor
  sw1 = sw3;  // assign op
  sw3 = move(sw1);  // move op
  return 0;
}
