#if 0
#include "task_queue.h"

#include "stl_utilinl.h"

TaskQueue::TaskQueue() {}

TaskQueue::~TaskQueue()
{
    STLDeleteElements(&queue_);
}

void TaskQueue::Push(Task* task)
{
    DCHECK(task);

    queue_.push_back(task);
}

void TaskQueue::Clear()
{
    STLDeleteElements(&queue_);
}

bool TaskQueue::IsEmpty() const
{
    return queue_.empty();
}

void TaskQueue::Run()
{
    if (queue_.empty())
    {
        return;
    }

    std::deque<Task*> ready;
    queue_.swap(ready);

    std::deque<Task*>::const_iterator task;
    for (task = ready.begin(); task != ready.end(); ++task)
    {
        (*task)->Run();
        delete (*task);
    }
}

#endif 