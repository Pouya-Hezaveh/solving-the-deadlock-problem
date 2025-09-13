
// in code dast-kari shode baraye Ehtemale bon-bast-e nazdik be 0%

#include <iostream>
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
        while (index <= 0);
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

Semaphore sem_1;
Semaphore sem_2;
ExampleResource resource_1;
ExampleResource resource_2;

void process_1()
{
    sem_1.wait();
    sem_2.wait();
    this_thread::sleep_for(chrono::seconds(1));

    cout<<"C-S of process 1\n";
    resource_1.mem += resource_2.mem;

    sem_1.signal();
    sem_2.signal();
}

void process_2()
{
    this_thread::sleep_for(chrono::seconds(1));
    sem_2.wait();
    sem_1.wait();

    cout<<"C-S of process 2\n";
    resource_2.mem += resource_1.mem;

    sem_1.signal();
    sem_2.signal();
}

int main()
{
    resource_1 = ExampleResource(10);
    resource_2 = ExampleResource(3);
    thread t1(process_1);
    thread t2(process_2);
    
    // تابع جوین باعث میشه فرآیند این تابع اصلی صبر کنه
    // تا فرآیند های تی۱ و تی۲ هم تمام شوند
    t1.join();
    t2.join();

    // سپس برنامه جافظه منابع را چاپ میکند و تمام میشود
    cout << "memory of the first resource: " << resource_1.mem << endl;
    cout << "memory of the second resource: " << resource_2.mem << endl;

    cout<<endl;
    return 0;
}


