#if 0
#include "thread.h"

#include "base/lazy_instance.h"
#include "base/synchronization/waitable_event.h"
#include "base/threading/thread_local.h"
#include "base/win/windows_version.h"

#if defined(OS_WIN)
#include "base/win/scoped_com_initializer.h"
#include "base/memory/scoped_ptr.h"
#endif

namespace base
{
    namespace
    {
        base::LazyInstance<base::ThreadLocalBoolean> lazy_tls_bool(
            base::LINKER_INITIALIZED);

    }

    class ThreadQuitTask : public Task
    {
    public:
        virtual void Run()
        {
            MessageLoop::current()->Quit();
            Thread::SetThreadWasQuitProperly(true);
        }
    };

    struct Thread::StartupData
    {
        const Thread::Options& options;

        WaitableEvent event;

        explicit StartupData(const Options& opt)
            : options(opt), event(false, false) {}
    };

    Thread::Thread(const char* name)
        : started_(false),
#if defined(OS_WIN)
        com_status_(NONE),
#endif
        stopping_(false),
        startup_data_(NULL),
        thread_(0),
        message_loop_(NULL),
        thread_id_(kInvalidThreadId),
        name_(name) {}

    Thread::~Thread()
    {
        Stop();
    }

    void Thread::SetThreadWasQuitProperly(bool flag)
    {
        lazy_tls_bool.Pointer()->Set(flag);
    }

    bool Thread::GetThreadWasQuitProperly()
    {
        bool quit_properly = true;
#ifndef NDEBUG
        quit_properly = lazy_tls_bool.Pointer()->Get();
#endif
        return quit_properly;
    }

    bool Thread::Start()
    {
        Options options;
#if defined(OS_WIN)
        if (com_status_ == STA)
            options.message_loop_type = MessageLoop::TYPE_UI;
#endif
        return StartWithOptions(options);
    }

    bool Thread::StartWithOptions(const Options& options)
    {
        DCHECK(!message_loop_);

        SetThreadWasQuitProperly(false);

        StartupData startup_data(options);
        startup_data_ = &startup_data;

        if (!PlatformThread::Create(options.stack_size, this, &thread_))
        {
            DLOG(ERROR) << "failed to create thread";
            startup_data_ = NULL;
            return false;
        }

        startup_data.event.Wait();

        startup_data_ = NULL;
        started_ = true;

        DCHECK(message_loop_);
        return true;
    }

    void Thread::Stop()
    {
        if (!thread_was_started())
        {
            return;
        }

        StopSoon();

        PlatformThread::Join(thread_);

        DCHECK(!message_loop_);

        started_ = false;

        stopping_ = false;
    }

    void Thread::StopSoon()
    {
        DCHECK_NE(thread_id_, PlatformThread::CurrentId());

        if (stopping_ || !message_loop_)
        {
            return;
        }

        stopping_ = true;
        message_loop_->PostTask(new ThreadQuitTask());
    }

    void Thread::Run(MessageLoop* message_loop)
    {
        message_loop->Run();
    }

    void Thread::ThreadMain()
    {
        {
            MessageLoop message_loop(startup_data_->options.message_loop_type);

            thread_id_ = PlatformThread::CurrentId();
            PlatformThread::SetName(name_.c_str());
            message_loop.set_thread_name(name_);
            message_loop_ = &message_loop;

#if defined(OS_WIN)
            scoped_ptr<win::ScopedCOMInitializer> com_initializer;
            if (com_status_ != NONE) {
                com_initializer.reset((com_status_ == STA) ?
                    new win::ScopedCOMInitializer() :
                    new win::ScopedCOMInitializer(win::ScopedCOMInitializer::kMTA));
            }
#endif
            Init();

            startup_data_->event.Signal();

            Run(message_loop_);

            CleanUp();

            DCHECK(GetThreadWasQuitProperly());

            message_loop_ = NULL;
        }
        thread_id_ = kInvalidThreadId;
    }

} //namespace base
#endif 