#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

class Semaphore
{
public:
    int index;
    Semaphore()
    {
        index = 1;
    }
    Semaphore(int number_of_resources)
    {
        index = number_of_resources;
    }

    void signal()
    {
        index++;
    }

    void wait()
    {
        while (index <= 0)
            ;
        index--;
    }
};
// ExampleResource manabaye farzi hast ke hafezei ham darad.
class ExampleResource
{
public:
    int mem;
    ExampleResource()
    {
        mem = 0;
    }
    ExampleResource(int initial)
    {
        mem = initial;
    }
};

const int NUMBER_OF_RESOURCE_TYPES = 2;
ExampleResource resource[NUMBER_OF_RESOURCE_TYPES];
Semaphore semaphore[NUMBER_OF_RESOURCE_TYPES];

class ProcessInterface
{
private:
    // cell i-th contain the maximum need of i-th resource of the process
    bool is_needed[NUMBER_OF_RESOURCE_TYPES];
    // cell i-th contains 'true' if(i-th resource requested by the process) else: 'false'
    bool is_requested[NUMBER_OF_RESOURCE_TYPES];
    // cell i-th contains 'true' if(i-th resource is currently captured by the process) else:'false'
    bool is_captured[NUMBER_OF_RESOURCE_TYPES];
    int find_the_highest_requested_id()
    {
        int result = -1;
        for (int i = 0; i < NUMBER_OF_RESOURCE_TYPES; i++)
        {
            if (is_requested[i])
            {
                result = i;
            }
        }
        return result;
    }

public:
    // the interface recieves and sets the max needs list at the initial define.
    ProcessInterface(int max_need[])
    {
        for (int i = 0; i < NUMBER_OF_RESOURCE_TYPES; i++)
        {
            this->is_needed[i] = max_need[i];
            this->is_requested[i] = false;
            this->is_captured[i] = false;
        }
    }
    void request_resource(int id_of_the_resource)
    {
        for (int i = find_the_highest_requested_id() + 1; i <= id_of_the_resource; i++)
        {
            if (is_needed[i] == true)
            {
                semaphore[i].wait();
                is_captured[i] = true;
            }
        }
        is_requested[id_of_the_resource] = true;
    }
    void release_resource(int id_of_the_resource)
    {
        is_requested[id_of_the_resource] = false;
        for (int i = find_the_highest_requested_id() + 1; i <= id_of_the_resource; i++)
        {
            if (is_captured[i] == true)
            {
                semaphore[i].signal();
                is_captured[i] = false;
            }
        }
    }
};

void process_1()
{
    int needed_resources[] = {true, true};
    ProcessInterface interface = ProcessInterface(needed_resources);

    interface.request_resource(0);
    this_thread::sleep_for(chrono::seconds(1));
    interface.request_resource(1);

    cout << "C-S of process 1\n";
    resource[0].mem += resource[1].mem;

    interface.release_resource(0);
    interface.release_resource(1);
}

void process_2()
{
    int needed_resources[] = {true, true};
    ProcessInterface interface = ProcessInterface(needed_resources);

    interface.request_resource(1);
    this_thread::sleep_for(chrono::seconds(1));
    interface.request_resource(0);

    cout << "C-S of process 2\n";
    resource[1].mem += resource[0].mem;

    interface.release_resource(0);
    interface.release_resource(1);
}

int main()
{
    // bayad tedade i-th resource ra roye i-th semaphor set konim
    semaphore[0] = Semaphore(1);
    semaphore[1] = Semaphore(1);
    resource[0] = ExampleResource(10);
    resource[1] = ExampleResource(3);
    thread t1(process_1);
    thread t2(process_2);

    // تابع جوین باعث میشه فرآیند این تابع اصلی صبر کنه
    // تا فرآیند های تی۱ و تی۲ هم تمام شوند
    t1.join();
    t2.join();

    // سپس برنامه جافظه منابع را چاپ میکند و تمام میشود
    cout << "memory of the first resource: " << resource[0].mem << endl;
    cout << "memory of the second resource: " << resource[1].mem << endl;

    cout << endl;
    return 0;
}

