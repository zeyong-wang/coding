#include <iostream>
#include <string>
#include <vector>
#include <climits>
#include <fstream>
#include <memory>
#include <list>
using namespace std;

class Beverage
{
public:
    virtual string getDescription() = 0;
    virtual int cost() = 0;
};
class Members
{
protected:
    string d_desription;    
};

class DarkCoffee : public Beverage, public Members
{
public:
    DarkCoffee() { d_desription = "Dark Coffee ";} // cannot use initialization list as d_desription does not exist before Members initialized 
    string getDescription() {return d_desription;}
    int cost() {return 2;}
};

class Decorator : public Beverage, public Members
{
protected:
    shared_ptr<Beverage> d_beverage;
public:
    virtual string getDescription() = 0; // to decorate getDescription
};

class MochaDecorator : public Decorator
{
public:
    MochaDecorator(shared_ptr<Beverage> beverage) { d_beverage = beverage;  d_desription = "mocha ";}
    string getDescription() {return d_beverage->getDescription() + d_desription;}
    int cost() {return d_beverage->cost() + 1;}
};

class SugerDecorator : public Decorator
{
public:
    SugerDecorator(shared_ptr<Beverage> beverage) { d_beverage = beverage;  d_desription = "Suger";}
    string getDescription() {return d_beverage->getDescription() + d_desription;}
    int cost() {return d_beverage->cost() + 1;}
};

int main()
{
    shared_ptr<Beverage> beverage = make_shared<DarkCoffee>();
    cout << beverage->getDescription() << endl;
    cout << beverage->cost() << endl;

    shared_ptr<Beverage> beverage2 = make_shared<MochaDecorator>(beverage);
    cout << beverage2->getDescription() << endl;
    cout << beverage2->cost() << endl;

    shared_ptr<Beverage> beverage3 = make_shared<SugerDecorator>(beverage2);
    cout << beverage3->getDescription() << endl;
    cout << beverage3->cost() << endl;
    return 0;
}
