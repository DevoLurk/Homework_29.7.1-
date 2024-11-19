#include <iostream>
#include <iomanip>
#include <mutex>
#include <thread>

using std::cout;
using std::endl;

struct Node
{
    Node(int value) : _value(value), _next(nullptr) {}
    int _value;
    Node* _next;
    std::mutex node_mutex;
};

class FineGrainedQueue
{
private:
    Node* _head;
    std::mutex queue_mutex;

public:
    FineGrainedQueue() : _head(nullptr) {}
    void insertIntoMiddle(int value, int pos);
    void push_front(int data);
    void print();
};

int main()
{
    FineGrainedQueue Queue;

    for (int i = 0; i < 3; i++)
    {
        int val = rand() % 19 + 1;
        Queue.push_front(val);
    };

    Queue.print();

    // очередь не пустая, 
    // вставляется элемент в середину или конец списка, то есть вставку в начало списка не нужно рассматривать,
    // если pos больше длины списка, то нужно вставить элемент в конец списка.

    std::thread th1(&FineGrainedQueue::insertIntoMiddle, std::ref(Queue), 111, 2);
    std::thread th2(&FineGrainedQueue::insertIntoMiddle, std::ref(Queue), 222, 2);
    std::thread th3(&FineGrainedQueue::insertIntoMiddle, std::ref(Queue), 333, 8);
    std::thread th4(&FineGrainedQueue::insertIntoMiddle, std::ref(Queue), 444, 2);

    th1.join();
    th2.join();
    th3.join();
    th4.join();

    Queue.print();

    return 0;
}


void FineGrainedQueue::push_front(int data)
{
    Node* node = new Node(data);
    node->_next = _head;
    _head = node;
}

void FineGrainedQueue::print()
{
    cout << endl;

    if (this->_head == nullptr)
    {
        cout << " Empty Queue" << endl;
        return;
    }

    Node* current = this->_head;
    do
    {
        cout << " " << std::setw(4) << current->_value;
        current = current->_next;
    } while (current);
}

void FineGrainedQueue::insertIntoMiddle(int value, int pos)
{

    if (this->_head == nullptr) // очередь не пустая
        return;

    int curr_pos = 0;

    Node* current = this->_head;
    Node* prev = nullptr;

    current->node_mutex.lock();

    while (current)
    {
        curr_pos++; // вставку в начало списка не нужно рассматривать

        prev = current;
        current = current->_next;
        if(current)
            current->node_mutex.lock();
        //queue_mutex.unlock();

        if (pos == curr_pos)
        {
            Node* node = new Node(value);
            prev->_next = node;
            node->_next = current;

            prev->node_mutex.unlock();
            current->node_mutex.unlock();
            return;
        }

        prev->node_mutex.unlock();
    }
    if (current)
        current->node_mutex.unlock();

    if (pos >= curr_pos) // если pos больше длины списка, то нужно вставить элемент в конец списка
    {  
        Node* node = new Node(value);
        prev->node_mutex.lock();
        prev->_next = node;
        prev->node_mutex.unlock();
    }
}