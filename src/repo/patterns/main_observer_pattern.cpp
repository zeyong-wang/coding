#include <iostream>
#include <string>
#include <vector>
#include <climits>
#include <fstream>
#include <memory>
#include <list>
using namespace std;

class Observer
{
public:
    virtual void update() = 0;    
    virtual ~Observer() {}
};

class Display
{
public:
    virtual void display() = 0;
    virtual void prefix() { cout << "observer: " << endl;}
    virtual ~Display(){}
};

class MobileObserver : public Observer, public Display
{
public:
    void update() { 
        prefix();
        display();
    }
    void display()
    {
        cout << "mobile" << endl;
    }
};

class TvObserver : public Observer, public Display
{
public:
    void update() { 
        prefix();
        display();
    }
    void display()
    {
        cout << "Tv" << endl;
    }
};

class StreamProcessor
{
private:
    list<shared_ptr<Observer>> d_observers;
public:
   void addObserver(shared_ptr<Observer> observer)
   {
       d_observers.push_back(observer); 
   }
   void removeObserver(shared_ptr<Observer> observer)
   {
        for(auto it = d_observers.begin(); it != d_observers.end(); it++)
        {
            if((*it) == observer)
            {
                d_observers.erase(it);
                break;
            }
        }
   }
   void onMessage() {updateObservers();}
   void updateObservers()
   {
        for(auto ptr : d_observers)
        {
            ptr->update();
        }
   }
};

int main()
{
    StreamProcessor obj;
    shared_ptr<Observer> observer1 = make_shared<MobileObserver>();
    shared_ptr<Observer> observer2 = make_shared<TvObserver>();
    obj.addObserver(observer1);
    obj.addObserver(observer2);
    obj.onMessage();
    obj.removeObserver(observer1);
    obj.onMessage();
    return 0;
}
