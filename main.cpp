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
    int queue_size = 0;
    Node* _head;
    Node* _tail;
    std::mutex queue_mutex;

public:
    FineGrainedQueue() : _head(nullptr), _tail(nullptr) {}
    void insertIntoMiddle(int value, int pos);
    void push_front(int data);
    void print();
    void fill(int amount = 3);
};

int main()
{
    FineGrainedQueue Queue;

    Queue.fill(9);
    Queue.print();

    std::thread th3(&FineGrainedQueue::insertIntoMiddle, std::ref(Queue), 333, 8);
    std::thread th1(&FineGrainedQueue::insertIntoMiddle, std::ref(Queue), 111, 2);
    std::thread th2(&FineGrainedQueue::insertIntoMiddle, std::ref(Queue), 222, 2);
    std::thread th4(&FineGrainedQueue::insertIntoMiddle, std::ref(Queue), 444, 2);

    th1.join();
    th2.join();
    th3.join();
    th4.join();

    Queue.print();

    return 0;
}

void FineGrainedQueue::insertIntoMiddle(int value, int pos)
{
    if (queue_size == 0) // очередь не пустая
        return;

    if (pos <= 0) // вставку в начало списка не нужно рассматривать 
        return;


    Node* new_node = new Node(value);
    queue_mutex.lock();

    if (pos >= queue_size)
    {
        _tail->_next = new_node;
        _tail = new_node;
        queue_size++;
        queue_mutex.unlock();
        return;
    }

    int curr_pos = 1;  
    Node* prev = this->_head;
    Node* current = nullptr;  

    while (curr_pos < pos - 1)   
    {
        prev = prev->_next;
        curr_pos++;
    }
    current = prev->_next;

    prev->node_mutex.lock();
    current->node_mutex.lock();
    queue_size++;
    queue_mutex.unlock();

    prev->_next = new_node;
    new_node->_next = current;
    prev->node_mutex.unlock();
    current->node_mutex.unlock();

}

void FineGrainedQueue::push_front(int data)
{
    Node* node = new Node(data);
    node->_next = _head;
    _head = node;

    if (queue_size == 0)
        _tail = node;

    this->queue_size++;
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
    while (current)
    {
        cout << " " << std::setw(4) << current->_value;
        current = current->_next;
    }
}

void FineGrainedQueue::fill(int amount)
{
    for (int i = 0; i < amount; i++)
    {
        int val = rand() % 19 + 1;
        this->push_front(val);
    }
}
