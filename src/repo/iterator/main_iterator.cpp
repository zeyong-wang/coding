#include <iostream>
#include <queue>
#include <string>
#include <memory>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <atomic>
#include <utility>

using namespace std;

class ExchContainer
{
private:
    int* d_data;
    size_t d_size;
public:
    class ExchIterator
    {
    private:
        int* d_value;
    public:
        typedef int value_type;
        typedef ptrdiff_t difference_type;
        typedef int* pointer;
        typedef int& reference; 
        typedef std::input_iterator_tag iterator_category;
    public:
        explicit ExchIterator(int *value) : d_value(value){}
        ExchIterator& operator++() {d_value++; return *this;}
        ExchIterator operator++(int) {ExchIterator retval = *this; ++(*this); return retval;}
        bool operator== (ExchIterator other) {return d_value == other.d_value;}
        bool operator!= (ExchIterator other) {return !((*this) == other);}
        reference operator*() {return (*d_value);}
        pointer operator->() {return &**this;}
    };
public:
    ExchContainer(int size): d_size(size) { d_data = new int[size];}
    void init()
    {
        for(int i = 0; i < d_size; i++)
        {
            d_data[i] = i;
        }
    }
    ~ExchContainer() {delete[] d_data;}
    ExchIterator begin() {return ExchIterator(&d_data[0]);}
    ExchIterator end() {return ExchIterator(d_data + d_size);}
};


int main()
{
    ExchContainer obj(100);
    obj.init();
    for(auto it = obj.begin(); it != obj.end(); it++)
    {
        cout << *it << endl;
    }
    return 0;
}
