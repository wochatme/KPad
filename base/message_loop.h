#ifndef __base_message_loop_h__
#define __base_message_loop_h__

#include <queue>
#include <string>

#include "callback.h"
#include "message_loop_proxy.h"
#include "message_pump_win.h"
#include "synchronization/lock.h"
#include "task.h"

namespace base
{
    class Histogram;
}

class MessageLoop : public base::MessagePump::Delegate
{
public:
    typedef base::MessagePumpWin::Dispatcher Dispatcher;
    typedef base::MessagePumpForUI::Observer Observer;

    enum Type
    {
        TYPE_DEFAULT,
        TYPE_UI,
        TYPE_IO
    };

    explicit MessageLoop(Type type = TYPE_DEFAULT);
    virtual ~MessageLoop();

    static MessageLoop* current();

    static void EnableHistogrammer(bool enable_histogrammer);

    typedef base::MessagePump* (MessagePumpFactory)();
    // Using the given base::MessagePumpForUIFactory to override the default
    // MessagePump implementation for 'TYPE_UI'.
    static void InitMessagePumpForUIFactory(MessagePumpFactory* factory);

    class DestructionObserver
    {
    public:
        virtual void WillDestroyCurrentMessageLoop() = 0;

    protected:
        virtual ~DestructionObserver();
    };

    void AddDestructionObserver(DestructionObserver* destruction_observer);

    void RemoveDestructionObserver(DestructionObserver* destruction_observer);

    void PostTask(Task* task);
    void PostDelayedTask(Task* task, int64 delay_ms);
    void PostNonNestableTask(Task* task);
    void PostNonNestableDelayedTask(Task* task, int64 delay_ms);

    // TODO(ajwong): Remove the functions above once the Task -> Closure migration
    // is complete.
    //
    // There are 2 sets of Post*Task functions, one which takes the older Task*
    // function object representation, and one that takes the newer base::Closure.
    // We have this overload to allow a staged transition between the two systems.
    // Once the transition is done, the functions above should be deleted.
    void PostTask(const base::Closure& task);
    void PostDelayedTask(const base::Closure& task, int64 delay_ms);
    void PostNonNestableTask(const base::Closure& task);
    void PostNonNestableDelayedTask(const base::Closure& task, int64 delay_ms);

    template<class T>
    void DeleteSoon(T* object)
    {
        PostNonNestableTask(new DeleteTask<T>(object));
    }

    template<class T>
    void ReleaseSoon(T* object)
    {
        PostNonNestableTask(new ReleaseTask<T>(object));
    }

    void Run();

    void RunAllPending();

    void Quit();

    void QuitNow();

    class QuitTask : public Task
    {
    public:
        virtual void Run()
        {
            MessageLoop::current()->Quit();
        }
    };

    Type type() const { return type_; }

    void set_thread_name(const std::string& thread_name)
    {
        DCHECK(thread_name_.empty()) << "Should not rename this thread!";
        thread_name_ = thread_name;
    }
    const std::string& thread_name() const { return thread_name_; }

    // Gets the message loop proxy associated with this message loop proxy
    scoped_refptr<base::MessageLoopProxy> message_loop_proxy()
    {
        return message_loop_proxy_.get();
    }

    void SetNestableTasksAllowed(bool allowed);
    bool NestableTasksAllowed() const;

    class ScopedNestableTaskAllower
    {
    public:
        explicit ScopedNestableTaskAllower(MessageLoop* loop)
            : loop_(loop), old_state_(loop_->NestableTasksAllowed())
        {
            loop_->SetNestableTasksAllowed(true);
        }
        ~ScopedNestableTaskAllower()
        {
            loop_->SetNestableTasksAllowed(old_state_);
        }

    private:
        MessageLoop* loop_;
        bool old_state_;
    };

    void set_exception_restoration(bool restore)
    {
        exception_restoration_ = restore;
    }

    bool IsNested();

    class TaskObserver
    {
    public:
        TaskObserver();

        virtual void WillProcessTask(base::TimeTicks time_posted) = 0;

        virtual void DidProcessTask(base::TimeTicks time_posted) = 0;

    protected:
        virtual ~TaskObserver();
    };

    void AddTaskObserver(TaskObserver* task_observer);
    void RemoveTaskObserver(TaskObserver* task_observer);

    bool high_resolution_timers_enabled()
    {
        return !high_resolution_timer_expiration_.is_null();
    }

    static const int kHighResolutionTimerModeLeaseTimeMs = 1000;

    void AssertIdle() const;

    void set_os_modal_loop(bool os_modal_loop)
    {
        os_modal_loop_ = os_modal_loop;
    }

    bool os_modal_loop() const
    {
        return os_modal_loop_;
    }

protected:
    struct RunState
    {
        int run_depth;

        bool quit_received;

        Dispatcher* dispatcher;
    };

    class AutoRunState : RunState
    {
    public:
        explicit AutoRunState(MessageLoop* loop);
        ~AutoRunState();

    private:
        MessageLoop* loop_;
        RunState* previous_state_;
    };

    struct PendingTask
    {
        PendingTask(const base::Closure& task,
            base::TimeTicks delayed_run_time,
            bool nestable);
        ~PendingTask();

        bool operator<(const PendingTask& other) const;

        // The task to run.
        base::Closure task;

        // Time this PendingTask was posted.
        base::TimeTicks time_posted;

        // The time when the task should be run.
        base::TimeTicks delayed_run_time;

        // Secondary sort key for run time.
        int sequence_num;

        // OK to dispatch from a nested loop.
        bool nestable;
    };

    class TaskQueue : public std::queue<PendingTask>
    {
    public:
        void Swap(TaskQueue* queue)
        {
            c.swap(queue->c); // call std::deque::swap
        }
    };

    typedef std::priority_queue<PendingTask> DelayedTaskQueue;

    base::MessagePumpWin* pump_win()
    {
        return static_cast<base::MessagePumpWin*>(pump_.get());
    }

    void RunHandler();

    __declspec(noinline) void RunInternalInSEHFrame();

    void RunInternal();

    bool ProcessNextDelayedNonNestableTask();

    void RunTask(const PendingTask& pending_task);

    bool DeferOrRunPendingTask(const PendingTask& pending_task);

    void AddToDelayedWorkQueue(const PendingTask& pending_task);

    // Adds the pending task to our incoming_queue_.
    //
    // Caller retains ownership of |pending_task|, but this function will
    // reset the value of pending_task->task.  This is needed to ensure
    // that the posting call stack does not retain pending_task->task
    // beyond this function call.
    void AddToIncomingQueue(PendingTask* pending_task);

    void ReloadWorkQueue();

    bool DeletePendingTasks();

    // Calcuates the time at which a PendingTask should run.
    base::TimeTicks CalculateDelayedRuntime(int64 delay_ms);

    // Start recording histogram info about events and action IF it was enabled
    // and IF the statistics recorder can accept a registration of our histogram.
    void StartHistogrammer();

    // Add occurence of event to our histogram, so that we can see what is being
    // done in a specific MessageLoop instance (i.e., specific thread).
    // If message_histogram_ is NULL, this is a no-op.
    void HistogramEvent(int event);

    // base::MessagePump::Delegate methods:
    virtual bool DoWork();
    virtual bool DoDelayedWork(base::TimeTicks* next_delayed_work_time);
    virtual bool DoIdleWork();

    Type type_;

    TaskQueue work_queue_;

    DelayedTaskQueue delayed_work_queue_;

    base::TimeTicks recent_time_;

    TaskQueue deferred_non_nestable_work_queue_;

    scoped_refptr<base::MessagePump> pump_;

    ObserverList<DestructionObserver> destruction_observers_;

    bool nestable_tasks_allowed_;

    bool exception_restoration_;

    std::string thread_name_;

    // A profiling histogram showing the counts of various messages and events.
    base::Histogram* message_histogram_;

    TaskQueue incoming_queue_;

    mutable base::Lock incoming_queue_lock_;

    RunState* state_;

    // The need for this variable is subtle. Please see implementation comments
    // around where it is used.
    bool should_leak_tasks_;

    base::TimeTicks high_resolution_timer_expiration_;
    bool os_modal_loop_;

    int next_sequence_num_;

    ObserverList<TaskObserver> task_observers_;

    // The message loop proxy associated with this message loop, if one exists.
    scoped_refptr<base::MessageLoopProxy> message_loop_proxy_;

private:
    DISALLOW_COPY_AND_ASSIGN(MessageLoop);
};

class MessageLoopForUI : public MessageLoop
{
public:
    MessageLoopForUI() : MessageLoop(TYPE_UI) {}

    static MessageLoopForUI* current()
    {
        MessageLoop* loop = MessageLoop::current();
        DCHECK_EQ(MessageLoop::TYPE_UI, loop->type());
        return static_cast<MessageLoopForUI*>(loop);
    }

    void DidProcessMessage(const MSG& message);

    void AddObserver(Observer* observer);
    void RemoveObserver(Observer* observer);
    void Run(Dispatcher* dispatcher);

protected:
    base::MessagePumpForUI* pump_ui()
    {
        return static_cast<base::MessagePumpForUI*>(pump_.get());
    }
};

COMPILE_ASSERT(sizeof(MessageLoop) == sizeof(MessageLoopForUI),
    MessageLoopForUI_should_not_have_extra_member_variables);

class MessageLoopForIO : public MessageLoop
{
public:
    typedef base::MessagePumpForIO::IOHandler IOHandler;
    typedef base::MessagePumpForIO::IOContext IOContext;
    typedef base::MessagePumpForIO::IOObserver IOObserver;

    MessageLoopForIO() : MessageLoop(TYPE_IO) {}

    static MessageLoopForIO* current()
    {
        MessageLoop* loop = MessageLoop::current();
        DCHECK_EQ(MessageLoop::TYPE_IO, loop->type());
        return static_cast<MessageLoopForIO*>(loop);
    }

    void AddIOObserver(IOObserver* io_observer)
    {
        pump_io()->AddIOObserver(io_observer);
    }

    void RemoveIOObserver(IOObserver* io_observer)
    {
        pump_io()->RemoveIOObserver(io_observer);
    }

    void RegisterIOHandler(HANDLE file_handle, IOHandler* handler);
    bool WaitForIOCompletion(DWORD timeout, IOHandler* filter);

protected:
    base::MessagePumpForIO* pump_io()
    {
        return static_cast<base::MessagePumpForIO*>(pump_.get());
    }
};

COMPILE_ASSERT(sizeof(MessageLoop) == sizeof(MessageLoopForIO),
    MessageLoopForIO_should_not_have_extra_member_variables);

#endif //__base_message_loop_h__