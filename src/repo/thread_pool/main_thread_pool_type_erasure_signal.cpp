#include <iostream>
#include <queue>
#include <functional>
#include <signal.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <atomic>


#include <utility>

using namespace std;

function<void()> func;
void sig_int_handler(int)
{
    cout << "\n\nCTRL-C was detected and the task is exiting !\n";
    func();
}

template<typename T>
class ThreadSafeQueue
{
    private:
        mutable mutex d_mutex; // copy constructor, skipped implementation here
        queue<T> d_data;
        condition_variable d_condition;
        atomic_bool d_empty;
    public:
        ThreadSafeQueue(): d_empty(true){};
        ThreadSafeQueue(const ThreadSafeQueue& other)
        {
            lock_guard<mutex> lk(other.d_mutex);
            d_data = other.d_data;
            d_empty = other.d_empty;
        }
        // add perfect forwarding later
        void push(T val)
        {
            lock_guard<mutex> lk(d_mutex);
            d_data.push(std::move(val));
            d_empty = false;
            d_condition.notify_one();
        }
        bool empty() const
        {
            lock_guard<mutex> lk(d_mutex);
            return d_data.empty();
        }
        // thread may wait forever!
        T wait_and_pop()
        {
            unique_lock<mutex> lk(d_mutex); // unique_lock has lock, unlock methods
            d_condition.wait(lk, [this]() -> bool {return !d_data.empty();});
            auto value = move(d_data.front());
            d_data.pop();
            return value;
        }
        bool try_pop(T& val)
        {
            lock_guard<mutex> lk(d_mutex);
            if(d_data.empty())
                return false;
            val = std::move(d_data.front());
            d_data.pop();
            return true;
        }
};
class TypeErasedHolder 
{
    private:
        class TypeKeeperBase
        {
            public:
                virtual void call() = 0;
                virtual ~TypeKeeperBase(){}
        }; 
        template<typename TypeErased>
        class TypeKeeper : public TypeKeeperBase
        {
            private:
                TypeErased d_object;
            public:
                TypeKeeper(TypeErased object) : d_object(move(object)) {}
                void call(){ d_object();}
        };
        unique_ptr<TypeKeeperBase> d_keep;
    public:
        template<typename TypeErased>
        TypeErasedHolder(TypeErased object) : d_keep(make_unique<TypeKeeper<TypeErased>>(move(object))) {}
        void operator()(){d_keep->call();}
        TypeErasedHolder() = default;
        TypeErasedHolder(TypeErasedHolder&& other) : d_keep(move(other.d_keep)) {}
        TypeErasedHolder& operator= (TypeErasedHolder&& other)
        {
            if(&other == this)
                return *this;
            d_keep = move(other.d_keep);
            return *this;
        }
        TypeErasedHolder(const TypeErasedHolder&) = delete;
        TypeErasedHolder& operator= (const TypeErasedHolder&) = delete;
};

class JoinThreads
{
    private:
        vector<thread>& d_threads;
    public:
        JoinThreads(vector<thread>&threads) : d_threads(threads){}
        ~JoinThreads(){
            for(auto& ele : d_threads) // not copiable
            {
                if(ele.joinable())
                    ele.join();
            }
        }
};

class ThreadPool
{
    private:
        // orders matter here
        atomic_bool d_done;
        // simply use ThreadSafeQueue<function<int()>> if no future required, also note packaged_task is not copy-constructible, cannot be passed to std::function
        // use type erasure for general types
        ThreadSafeQueue<TypeErasedHolder> d_queue;
        vector<thread> d_threads;
        JoinThreads d_joiner;
    private:
        void workThread()
        {
            TypeErasedHolder task;
            while(!d_done)
            {
                if(d_queue.try_pop(task))   
                {
                    task();
                    cout << "workthread ongoing" << endl;
                }
                else
                {
                    this_thread::yield(); 
                }
            }
        }
    public:
        // use default destructor
        ThreadPool() : d_done(false), d_joiner(d_threads) {
            const auto thread_count = thread::hardware_concurrency(); 
            try
            {
                 for(int i = 0; i < thread_count; i++)
                 {
                     d_threads.push_back(thread(&ThreadPool::workThread, this));
                 }
            } 
            catch(...)
            {
                d_done = true;
                throw;
            }
        }
        ~ThreadPool(){
            d_done = true;
            cout << "ThreadPool destructor called" << endl;
        }
        void terminatePool() {d_done = true;}
        bool done() const {return d_done;}
        template<typename FunctionType>
        future<typename result_of<FunctionType()>::type> submit(FunctionType f)
        {
            typedef typename result_of<FunctionType()>::type result_t;
            packaged_task<result_t()> task(move(f)); 
            future<result_t> res(task.get_future());
            d_queue.push(std::move(task));
            return res;
        }
};
 
int main()
{
    //ThreadSafeQueue<int> q;
    //thread t1 = thread(&ThreadSafeQueue<int>::wait_and_pop, &q);
    //thread t2 = thread(&ThreadSafeQueue<int>::wait_and_pop, &q);
    //q.push(1);
    //q.push(2);

    //t1.join();
    //t2.join();
    {
        ThreadPool obj;
        func = bind(&ThreadPool::terminatePool, &obj);
        signal(SIGINT, sig_int_handler);
        vector<future<int>> fs;
        for(int i = 0; i < 300 && !obj.done(); i++)
        {
            this_thread::sleep_for(1000ms);
            fs.push_back(obj.submit([](){this_thread::sleep_for(1000ms); return rand() % 10;}));
        }
        //for(int i = 0; i < 300; i++)
        //{
        //    cout << fs[i].get() << endl;
        //}
    }
    cout << "block ended" << endl;
    return 0;
}

