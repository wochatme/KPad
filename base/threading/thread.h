#ifndef __base_thread_h__
#define __base_thread_h__

#include "base/message_loop.h"
#include "base/message_loop_proxy.h"
#include "platform_thread.h"

namespace base
{
    class Thread : PlatformThread::Delegate
    {
    public:
        struct Options
        {
            Options() : message_loop_type(MessageLoop::TYPE_DEFAULT),
                stack_size(0) {}
            Options(MessageLoop::Type type, size_t size)
                : message_loop_type(type), stack_size(size) {}

            MessageLoop::Type message_loop_type;

            size_t stack_size;
        };

        explicit Thread(const char* name);

        virtual ~Thread();

        bool Start();

        bool StartWithOptions(const Options& options);

        void Stop();

        void StopSoon();

        MessageLoop* message_loop() const { return message_loop_; }

        scoped_refptr<MessageLoopProxy> message_loop_proxy()
        {
            return message_loop_->message_loop_proxy();
        }

        const std::string& thread_name() { return name_; }

        PlatformThreadHandle thread_handle() { return thread_; }

        PlatformThreadId thread_id() const { return thread_id_; }

        bool IsRunning() const { return thread_id_ != kInvalidThreadId; }

#if defined(OS_WIN)
        // Causes the thread to initialize COM.  This must be called before calling
        // Start() or StartWithOptions().  If |use_mta| is false, the thread is also
        // started with a TYPE_UI message loop.  It is an error to call
        // init_com_with_mta(false) and then StartWithOptions() with any message loop
        // type other than TYPE_UI.
        void init_com_with_mta(bool use_mta) {
            DCHECK(!started_);
            com_status_ = use_mta ? MTA : STA;
        }
#endif

    protected:
        virtual void Init() {}

        virtual void Run(MessageLoop* message_loop);

        virtual void CleanUp() {}

        static void SetThreadWasQuitProperly(bool flag);
        static bool GetThreadWasQuitProperly();

        void set_message_loop(MessageLoop* message_loop)
        {
            message_loop_ = message_loop;
        }

    private:
#if defined(OS_WIN)
        enum ComStatus {
            NONE,
            STA,
            MTA,
        };
#endif
#if defined(OS_WIN)
        // Whether this thread needs to initialize COM, and if so, in what mode.
        ComStatus com_status_;
#endif
        bool thread_was_started() const { return started_; }

        virtual void ThreadMain();

        bool started_;

        bool stopping_;

        struct StartupData;
        StartupData* startup_data_;

        PlatformThreadHandle thread_;

        MessageLoop* message_loop_;

        PlatformThreadId thread_id_;

        std::string name_;

        friend class ThreadQuitTask;

        DISALLOW_COPY_AND_ASSIGN(Thread);
    };

} //namespace base

#endif //__base_thread_h__