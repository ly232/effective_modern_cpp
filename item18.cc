// Item 18: use std::unique_ptr to manage exclusive ownership.
#include <iostream>
#include <memory>

using namespace std;

class Investment {
public:
  virtual ~Investment() {};
  virtual void invest(double amount) = 0;
};

class Stock : public Investment {
public:
  Stock(string symbol) {
    cout << "construct stock: " << symbol << endl;
  };
  ~Stock() override {
    cout << "calling stock dtor." << endl;
  };
  void invest(double amount) override {
    amount_ += amount;
    cout << "current stock amount: " << amount_ <<endl;
  };
private:
  double amount_ = 0;
};

class Bond : public Investment {
public:
  Bond(string name) {
    cout << "calling bond: " << name << endl;
  };
  ~Bond() override {
    cout << "calling bond dtor." << endl;
  };
  void invest(double amount) override {
    amount_ += amount;
    cout << "current bond amount: " << amount_ <<endl;
  };
private:
  double amount_ = 0;
};

enum class InvestmentType {Stock, Bond};

auto deleteFn = [](Investment* investment) {
  cout << "calling custom deleteFn" << endl;
  delete investment;
};

// Factory template function to create an investiment.
template <typename... Ts>
unique_ptr<Investment, decltype(deleteFn)>
makeInvestment(InvestmentType type, Ts&&... args) {
  unique_ptr<Investment, decltype(deleteFn)> investment(nullptr, deleteFn);
  switch (type) {
  case InvestmentType::Stock:
    investment.reset(new Stock(forward<Ts>(args)...));
    break;
  case InvestmentType::Bond:
    investment.reset(new Bond(forward<Ts>(args)...));
    break;
  default:
    break;
  };
  return investment;
}

int main() {
  auto unique_stock = makeInvestment(InvestmentType::Stock, "GOOG");
  shared_ptr<Investment> shared_bond_1 =
    makeInvestment(InvestmentType::Bond, "NEW_YORK_MUNI_7_YR");
  unique_stock->invest(100);
  auto shared_bond_2(shared_bond_1);
  shared_bond_1->invest(200);
  shared_bond_2->invest(300);
  return 0;
}
