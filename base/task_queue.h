#ifndef __base_task_queue_h__
#define __base_task_queue_h__

#include <deque>

#include "task.h"

class TaskQueue : public Task
{
public:
    TaskQueue();
    ~TaskQueue();

    void Push(Task* task);

    void Clear();

    bool IsEmpty() const;

    virtual void Run();

private:
    std::deque<Task*> queue_;
};

#endif //__base_task_queue_h__