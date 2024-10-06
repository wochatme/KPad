#if 0
#include "message_loop.h"

#include "bind.h"
#include "lazy_instance.h"
#include "message_loop_proxy_impl.h"
#include "message_pump_default.h"
#include "metric/histogram.h"
#include "threading/thread_local.h"

namespace
{

    base::LazyInstance<base::ThreadLocalPointer<MessageLoop> > lazy_tls_ptr(
        base::LINKER_INITIALIZED);

    // Logical events for Histogram profiling. Run with -message-loop-histogrammer
    // to get an accounting of messages and actions taken on each thread.
    const int kTaskRunEvent = 0x1;
    const int kTimerEvent = 0x2;

    // Provide range of message IDs for use in histogramming and debug display.
    const int kLeastNonZeroMessageId = 1;
    const int kMaxMessageId = 1099;
    const int kNumberOfDistinctMessagesDisplayed = 1100;

    // Provide a macro that takes an expression (such as a constant, or macro
    // constant) and creates a pair to initalize an array of pairs.  In this case,
    // our pair consists of the expressions value, and the "stringized" version
    // of the expression (i.e., the exrpression put in quotes).  For example, if
    // we have:
    //    #define FOO 2
    //    #define BAR 5
    // then the following:
    //    VALUE_TO_NUMBER_AND_NAME(FOO + BAR)
    // will expand to:
    //   {7, "FOO + BAR"}
    // We use the resulting array as an argument to our histogram, which reads the
    // number as a bucket identifier, and proceeds to use the corresponding name
    // in the pair (i.e., the quoted string) when printing out a histogram.
#define VALUE_TO_NUMBER_AND_NAME(name) { name, #name },

    const base::LinearHistogram::DescriptionPair event_descriptions_[] =
    {
        // Provide some pretty print capability in our histogram for our internal
        // messages.

        // A few events we handle (kindred to messages), and used to profile actions.
        VALUE_TO_NUMBER_AND_NAME(kTaskRunEvent)
        VALUE_TO_NUMBER_AND_NAME(kTimerEvent)

        {
 -1, NULL
} // The list must be null terminated, per API to histogram.
    };

    bool enable_histogrammer_ = false;

    MessageLoop::MessagePumpFactory* message_pump_for_ui_factory_ = NULL;

}

static int SEHFilter(LPTOP_LEVEL_EXCEPTION_FILTER old_filter)
{
    ::SetUnhandledExceptionFilter(old_filter);
    return EXCEPTION_CONTINUE_SEARCH;
}

static LPTOP_LEVEL_EXCEPTION_FILTER GetTopSEHFilter()
{
    LPTOP_LEVEL_EXCEPTION_FILTER top_filter = NULL;
    top_filter = ::SetUnhandledExceptionFilter(0);
    ::SetUnhandledExceptionFilter(top_filter);
    return top_filter;
}

MessageLoop::TaskObserver::TaskObserver() {}

MessageLoop::TaskObserver::~TaskObserver() {}

MessageLoop::DestructionObserver::~DestructionObserver() {}

MessageLoop::MessageLoop(Type type)
    : type_(type),
    nestable_tasks_allowed_(true),
    exception_restoration_(false),
    message_histogram_(NULL),
    state_(NULL),
    should_leak_tasks_(true),
    os_modal_loop_(false),
    next_sequence_num_(0)
{
    DCHECK(!current()) << "should only have one message loop per thread";
    lazy_tls_ptr.Pointer()->Set(this);

    message_loop_proxy_ = new base::MessageLoopProxyImpl();

#define MESSAGE_PUMP_UI new base::MessagePumpForUI()
#define MESSAGE_PUMP_IO new base::MessagePumpForIO()

    if (type_ == TYPE_UI)
    {
        if (message_pump_for_ui_factory_)
        {
            pump_ = message_pump_for_ui_factory_();
        }
        else
        {
            pump_ = MESSAGE_PUMP_UI;
        }
    }
    else if (type_ == TYPE_IO)
    {
        pump_ = MESSAGE_PUMP_IO;
    }
    else
    {
        DCHECK_EQ(TYPE_DEFAULT, type_);
        pump_ = new base::MessagePumpDefault();
    }
}

MessageLoop::~MessageLoop()
{
    DCHECK_EQ(this, current());

    DCHECK(!state_);

    bool did_work;
    for (int i = 0; i < 100; ++i)
    {
        DeletePendingTasks();
        ReloadWorkQueue();

        did_work = DeletePendingTasks();
        if (!did_work)
        {
            break;
        }
    }
    DCHECK(!did_work);

    FOR_EACH_OBSERVER(DestructionObserver, destruction_observers_,
        WillDestroyCurrentMessageLoop());

    // Tell the message_loop_proxy that we are dying.
    static_cast<base::MessageLoopProxyImpl*>(message_loop_proxy_.get())->
        WillDestroyCurrentMessageLoop();
    message_loop_proxy_ = NULL;

    lazy_tls_ptr.Pointer()->Set(NULL);

    // If we left the high-resolution timer activated, deactivate it now.
    // Doing this is not-critical, it is mainly to make sure we track
    // the high resolution timer activations properly in our unit tests.
    if (!high_resolution_timer_expiration_.is_null())
    {
        base::Time::ActivateHighResolutionTimer(false);
        high_resolution_timer_expiration_ = base::TimeTicks();
    }
}

// static
MessageLoop* MessageLoop::current()
{
    return lazy_tls_ptr.Pointer()->Get();
}

// static
void MessageLoop::EnableHistogrammer(bool enable)
{
    enable_histogrammer_ = enable;
}

// static
void MessageLoop::InitMessagePumpForUIFactory(MessagePumpFactory* factory)
{
    DCHECK(!message_pump_for_ui_factory_);
    message_pump_for_ui_factory_ = factory;
}

void MessageLoop::AddDestructionObserver(DestructionObserver* destruction_observer)
{
    DCHECK_EQ(this, current());
    destruction_observers_.AddObserver(destruction_observer);
}

void MessageLoop::RemoveDestructionObserver(
    DestructionObserver* destruction_observer)
{
    DCHECK_EQ(this, current());
    destruction_observers_.RemoveObserver(destruction_observer);
}

void MessageLoop::PostTask(Task* task)
{
    CHECK(task);
    PendingTask pending_task(
        base::Bind(&base::subtle::TaskClosureAdapter::Run,
            new base::subtle::TaskClosureAdapter(task, &should_leak_tasks_)),
        CalculateDelayedRuntime(0), true);
    AddToIncomingQueue(&pending_task);
}

void MessageLoop::PostDelayedTask(Task* task, int64 delay_ms)
{
    CHECK(task);
    PendingTask pending_task(
        base::Bind(&base::subtle::TaskClosureAdapter::Run,
            new base::subtle::TaskClosureAdapter(task, &should_leak_tasks_)),
        CalculateDelayedRuntime(delay_ms), true);
    AddToIncomingQueue(&pending_task);
}

void MessageLoop::PostNonNestableTask(Task* task)
{
    CHECK(task);
    PendingTask pending_task(
        base::Bind(&base::subtle::TaskClosureAdapter::Run,
            new base::subtle::TaskClosureAdapter(task, &should_leak_tasks_)),
        CalculateDelayedRuntime(0), false);
    AddToIncomingQueue(&pending_task);
}

void MessageLoop::PostNonNestableDelayedTask(Task* task, int64 delay_ms)
{
    CHECK(task);
    PendingTask pending_task(
        base::Bind(&base::subtle::TaskClosureAdapter::Run,
            new base::subtle::TaskClosureAdapter(task, &should_leak_tasks_)),
        CalculateDelayedRuntime(delay_ms), false);
    AddToIncomingQueue(&pending_task);
}

void MessageLoop::PostTask(const base::Closure& task)
{
    CHECK(!task.is_null());
    PendingTask pending_task(task, CalculateDelayedRuntime(0), true);
    AddToIncomingQueue(&pending_task);
}

void MessageLoop::PostDelayedTask(const base::Closure& task, int64 delay_ms)
{
    CHECK(!task.is_null());
    PendingTask pending_task(task, CalculateDelayedRuntime(delay_ms), true);
    AddToIncomingQueue(&pending_task);
}

void MessageLoop::PostNonNestableTask(const base::Closure& task)
{
    CHECK(!task.is_null());
    PendingTask pending_task(task, CalculateDelayedRuntime(0), false);
    AddToIncomingQueue(&pending_task);
}

void MessageLoop::PostNonNestableDelayedTask(const base::Closure& task,
    int64 delay_ms)
{
    CHECK(!task.is_null());
    PendingTask pending_task(task, CalculateDelayedRuntime(delay_ms), false);
    AddToIncomingQueue(&pending_task);
}

void MessageLoop::Run()
{
    AutoRunState save_state(this);
    RunHandler();
}

void MessageLoop::RunAllPending()
{
    AutoRunState save_state(this);
    state_->quit_received = true; 
    RunHandler();
}

void MessageLoop::Quit()
{
    DCHECK_EQ(this, current());
    if (state_)
    {
        state_->quit_received = true;
    }
    else
    {
        NOTREACHED() << "Must be inside Run to call Quit";
    }
}

void MessageLoop::QuitNow()
{
    DCHECK_EQ(this, current());
    if (state_)
    {
        pump_->Quit();
    }
    else
    {
        NOTREACHED() << "Must be inside Run to call Quit";
    }
}

void MessageLoop::SetNestableTasksAllowed(bool allowed)
{
    if (nestable_tasks_allowed_ != allowed)
    {
        nestable_tasks_allowed_ = allowed;
        if (!nestable_tasks_allowed_)
        {
            return;
        }

        pump_->ScheduleWork();
    }
}

bool MessageLoop::NestableTasksAllowed() const
{
    return nestable_tasks_allowed_;
}

bool MessageLoop::IsNested()
{
    return state_->run_depth > 1;
}

void MessageLoop::AddTaskObserver(TaskObserver* task_observer)
{
    DCHECK_EQ(this, current());
    task_observers_.AddObserver(task_observer);
}

void MessageLoop::RemoveTaskObserver(TaskObserver* task_observer)
{
    DCHECK_EQ(this, current());
    task_observers_.RemoveObserver(task_observer);
}

void MessageLoop::AssertIdle() const
{
    base::AutoLock lock(incoming_queue_lock_);
    DCHECK(incoming_queue_.empty());
}

void MessageLoop::RunHandler()
{
    if (exception_restoration_)
    {
        RunInternalInSEHFrame();
        return;
    }

    RunInternal();
}

__declspec(noinline) void MessageLoop::RunInternalInSEHFrame()
{
    LPTOP_LEVEL_EXCEPTION_FILTER current_filter = GetTopSEHFilter();
    __try
    {
        RunInternal();
    }
    __except (SEHFilter(current_filter))
    {
    }
    return;
}

void MessageLoop::RunInternal()
{
    DCHECK_EQ(this, current());

    StartHistogrammer();

    if (state_->dispatcher && type() == TYPE_UI)
    {
        static_cast<base::MessagePumpForUI*>(pump_.get())->
            RunWithDispatcher(this, state_->dispatcher);
        return;
    }

    pump_->Run(this);
}

bool MessageLoop::ProcessNextDelayedNonNestableTask()
{
    if (state_->run_depth != 1)
    {
        return false;
    }

    if (deferred_non_nestable_work_queue_.empty())
    {
        return false;
    }

    PendingTask pending_task = deferred_non_nestable_work_queue_.front();
    deferred_non_nestable_work_queue_.pop();

    RunTask(pending_task);
    return true;
}

void MessageLoop::RunTask(const PendingTask& pending_task)
{
    DCHECK(nestable_tasks_allowed_);

    nestable_tasks_allowed_ = false;

    HistogramEvent(kTaskRunEvent);
    FOR_EACH_OBSERVER(TaskObserver, task_observers_,
        WillProcessTask(pending_task.time_posted));
    pending_task.task.Run();
    FOR_EACH_OBSERVER(TaskObserver, task_observers_,
        DidProcessTask(pending_task.time_posted));

    nestable_tasks_allowed_ = true;
}

bool MessageLoop::DeferOrRunPendingTask(const PendingTask& pending_task)
{
    if (pending_task.nestable || state_->run_depth == 1)
    {
        RunTask(pending_task);
        return true;
    }

    deferred_non_nestable_work_queue_.push(pending_task);
    return false;
}

void MessageLoop::AddToDelayedWorkQueue(const PendingTask& pending_task)
{
    PendingTask new_pending_task(pending_task);
    new_pending_task.sequence_num = next_sequence_num_++;
    delayed_work_queue_.push(new_pending_task);
}

void MessageLoop::ReloadWorkQueue()
{
    if (!work_queue_.empty())
    {
        return; 
    }

    {
        base::AutoLock lock(incoming_queue_lock_);
        if (incoming_queue_.empty())
        {
            return;
        }
        incoming_queue_.Swap(&work_queue_); 
        DCHECK(incoming_queue_.empty());
    }
}

bool MessageLoop::DeletePendingTasks()
{
    bool did_work = !work_queue_.empty();
    while (!work_queue_.empty())
    {
        PendingTask pending_task = work_queue_.front();
        work_queue_.pop();
        if (!pending_task.delayed_run_time.is_null())
        {
            AddToDelayedWorkQueue(pending_task);
        }
    }
    did_work |= !deferred_non_nestable_work_queue_.empty();
    while (!deferred_non_nestable_work_queue_.empty())
    {
        deferred_non_nestable_work_queue_.pop();
    }
    did_work |= !delayed_work_queue_.empty();

    // Historically, we always delete the task regardless of valgrind status. It's
    // not completely clear why we want to leak them in the loops above.  This
    // code is replicating legacy behavior, and should not be considered
    // absolutely "correct" behavior.  See TODO above about deleting all tasks
    // when it's safe.
    should_leak_tasks_ = false;
    while (!delayed_work_queue_.empty())
    {
        delayed_work_queue_.pop();
    }
    should_leak_tasks_ = true;
    return did_work;
}

base::TimeTicks MessageLoop::CalculateDelayedRuntime(int64 delay_ms)
{
    base::TimeTicks delayed_run_time;
    if (delay_ms > 0)
    {
        delayed_run_time = base::TimeTicks::Now() +
            base::TimeDelta::FromMilliseconds(delay_ms);

        if (high_resolution_timer_expiration_.is_null())
        {
            // Windows timers are granular to 15.6ms.  If we only set high-res
            // timers for those under 15.6ms, then a 18ms timer ticks at ~32ms,
            // which as a percentage is pretty inaccurate.  So enable high
            // res timers for any timer which is within 2x of the granularity.
            // This is a tradeoff between accuracy and power management.
            bool needs_high_res_timers = delay_ms <
                (2 * base::Time::kMinLowResolutionThresholdMs);
            if (needs_high_res_timers)
            {
                if (base::Time::ActivateHighResolutionTimer(true))
                {
                    high_resolution_timer_expiration_ = base::TimeTicks::Now() +
                        base::TimeDelta::FromMilliseconds(kHighResolutionTimerModeLeaseTimeMs);
                }
            }
        }
    }
    else
    {
        DCHECK_EQ(delay_ms, 0) << "delay should not be negative";
    }

    if (!high_resolution_timer_expiration_.is_null())
    {
        if (base::TimeTicks::Now() > high_resolution_timer_expiration_)
        {
            base::Time::ActivateHighResolutionTimer(false);
            high_resolution_timer_expiration_ = base::TimeTicks();
        }
    }

    return delayed_run_time;
}

// Possibly called on a background thread!
void MessageLoop::AddToIncomingQueue(PendingTask* pending_task)
{
    // Warning: Don't try to short-circuit, and handle this thread's tasks more
    // directly, as it could starve handling of foreign threads.  Put every task
    // into this queue.

    scoped_refptr<base::MessagePump> pump;
    {
        base::AutoLock locked(incoming_queue_lock_);

        bool was_empty = incoming_queue_.empty();
        incoming_queue_.push(*pending_task);
        pending_task->task.Reset();
        if (!was_empty)
        {
            return; // Someone else should have started the sub-pump.
        }

        pump = pump_;
    }
    // Since the incoming_queue_ may contain a task that destroys this message
    // loop, we cannot exit incoming_queue_lock_ until we are done with |this|.
    // We use a stack-based reference to the message pump so that we can call
    // ScheduleWork outside of incoming_queue_lock_.

    pump->ScheduleWork();
}

//------------------------------------------------------------------------------
// Method and data for histogramming events and actions taken by each instance
// on each thread.

void MessageLoop::StartHistogrammer()
{
    if (enable_histogrammer_ && !message_histogram_
        && base::StatisticsRecorder::IsActive())
    {
        DCHECK(!thread_name_.empty());
        message_histogram_ = base::LinearHistogram::FactoryGet(
            "MsgLoop:" + thread_name_,
            kLeastNonZeroMessageId, kMaxMessageId,
            kNumberOfDistinctMessagesDisplayed,
            message_histogram_->kHexRangePrintingFlag);
        message_histogram_->SetRangeDescriptions(event_descriptions_);
    }
}

void MessageLoop::HistogramEvent(int event)
{
    if (message_histogram_)
    {
        message_histogram_->Add(event);
    }
}

bool MessageLoop::DoWork()
{
    if (!nestable_tasks_allowed_)
    {
        return false;
    }

    for (;;)
    {
        ReloadWorkQueue();
        if (work_queue_.empty())
        {
            break;
        }

        do
        {
            PendingTask pending_task = work_queue_.front();
            work_queue_.pop();
            if (!pending_task.delayed_run_time.is_null())
            {
                AddToDelayedWorkQueue(pending_task);
                if (delayed_work_queue_.top().task.Equals(pending_task.task))
                {
                    pump_->ScheduleDelayedWork(pending_task.delayed_run_time);
                }
            }
            else
            {
                if (DeferOrRunPendingTask(pending_task))
                {
                    return true;
                }
            }
        } while (!work_queue_.empty());
    }

    return false;
}

bool MessageLoop::DoDelayedWork(base::TimeTicks* next_delayed_work_time)
{
    if (!nestable_tasks_allowed_ || delayed_work_queue_.empty())
    {
        recent_time_ = *next_delayed_work_time = base::TimeTicks();
        return false;
    }

    base::TimeTicks next_run_time = delayed_work_queue_.top().delayed_run_time;
    if (next_run_time > recent_time_)
    {
        recent_time_ = base::TimeTicks::Now(); 
        if (next_run_time > recent_time_)
        {
            *next_delayed_work_time = next_run_time;
            return false;
        }
    }

    PendingTask pending_task = delayed_work_queue_.top();
    delayed_work_queue_.pop();

    if (!delayed_work_queue_.empty())
    {
        *next_delayed_work_time = delayed_work_queue_.top().delayed_run_time;
    }

    return DeferOrRunPendingTask(pending_task);
}

bool MessageLoop::DoIdleWork()
{
    if (ProcessNextDelayedNonNestableTask())
    {
        return true;
    }

    if (state_->quit_received)
    {
        pump_->Quit();
    }

    return false;
}

MessageLoop::AutoRunState::AutoRunState(MessageLoop* loop) : loop_(loop)
{
    previous_state_ = loop_->state_;
    if (previous_state_)
    {
        run_depth = previous_state_->run_depth + 1;
    }
    else
    {
        run_depth = 1;
    }
    loop_->state_ = this;

    // Initialize the other fields:
    quit_received = false;
    dispatcher = NULL;
}

MessageLoop::AutoRunState::~AutoRunState()
{
    loop_->state_ = previous_state_;
}

//------------------------------------------------------------------------------
// MessageLoop::PendingTask

MessageLoop::PendingTask::PendingTask(const base::Closure& task,
    base::TimeTicks delayed_run_time,
    bool nestable)
    : task(task),
    time_posted(base::TimeTicks::Now()),
    delayed_run_time(delayed_run_time),
    sequence_num(0),
    nestable(nestable) {}

MessageLoop::PendingTask::~PendingTask() {}

bool MessageLoop::PendingTask::operator<(const PendingTask& other) const
{
    if (delayed_run_time < other.delayed_run_time)
    {
        return false;
    }

    if (delayed_run_time > other.delayed_run_time)
    {
        return true;
    }

    return (sequence_num - other.sequence_num) > 0;
}


void MessageLoopForUI::DidProcessMessage(const MSG& message)
{
    pump_win()->DidProcessMessage(message);
}

void MessageLoopForUI::AddObserver(Observer* observer)
{
    pump_ui()->AddObserver(observer);
}

void MessageLoopForUI::RemoveObserver(Observer* observer)
{
    pump_ui()->RemoveObserver(observer);
}

void MessageLoopForUI::Run(Dispatcher* dispatcher)
{
    AutoRunState save_state(this);
    state_->dispatcher = dispatcher;
    RunHandler();
}


void MessageLoopForIO::RegisterIOHandler(HANDLE file, IOHandler* handler)
{
    pump_io()->RegisterIOHandler(file, handler);
}

bool MessageLoopForIO::WaitForIOCompletion(DWORD timeout, IOHandler* filter)
{
    return pump_io()->WaitForIOCompletion(timeout, filter);
}

#endif 
