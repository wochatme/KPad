#include "message_pump_win.h"

#include <math.h>

#include "message_loop.h"
#include "metric/histogram.h"
#include "win/wrapped_window_proc.h"


HANDLE* handle_get_events(int* nevents);
void handle_got_event(HANDLE event);
void safefree(void*);
#define sfree safefree
namespace base
{

    static const wchar_t kWndClass[] = L"Wan_MessagePumpWindow";

    static const int kMsgHaveWork = WM_USER + 1;

    void MessagePumpWin::AddObserver(Observer* observer)
    {
        observers_.AddObserver(observer);
    }

    void MessagePumpWin::RemoveObserver(Observer* observer)
    {
        observers_.RemoveObserver(observer);
    }

    void MessagePumpWin::WillProcessMessage(const MSG& msg)
    {
        FOR_EACH_OBSERVER(Observer, observers_, WillProcessMessage(msg));
    }

    void MessagePumpWin::DidProcessMessage(const MSG& msg)
    {
        FOR_EACH_OBSERVER(Observer, observers_, DidProcessMessage(msg));
    }

    void MessagePumpWin::RunWithDispatcher(Delegate* delegate,
        Dispatcher* dispatcher)
    {
        RunState s;
        s.delegate = delegate;
        s.dispatcher = dispatcher;
        s.should_quit = false;
        s.run_depth = state_ ? state_->run_depth + 1 : 1;

        RunState* previous_state = state_;
        state_ = &s;

        DoRunLoop();

        state_ = previous_state;
    }

    void MessagePumpWin::Quit()
    {
        DCHECK(state_);
        state_->should_quit = true;
    }

    int MessagePumpWin::GetCurrentDelay() const
    {
        if (delayed_work_time_.is_null())
        {
            return -1;
        }

        double timeout = ceil((delayed_work_time_ - TimeTicks::Now()).InMillisecondsF());

        int delay = static_cast<int>(timeout);
        if (delay < 0)
        {
            delay = 0;
        }

        return delay;
    }

    MessagePumpForUI::MessagePumpForUI()
    {
        InitMessageWnd();
    }

    MessagePumpForUI::~MessagePumpForUI()
    {
        DestroyWindow(message_hwnd_);
        UnregisterClass(kWndClass, GetModuleHandle(NULL));
    }

    void MessagePumpForUI::ScheduleWork()
    {
        if (InterlockedExchange(&have_work_, 1))
        {
            return; 
        }

        PostMessage(message_hwnd_, kMsgHaveWork,
            reinterpret_cast<WPARAM>(this), 0);
    }

    void MessagePumpForUI::ScheduleDelayedWork(const TimeTicks& delayed_work_time)
    {
        delayed_work_time_ = delayed_work_time;

        int delay_msec = GetCurrentDelay();
        DCHECK(delay_msec >= 0);
        if (delay_msec < USER_TIMER_MINIMUM)
        {
            delay_msec = USER_TIMER_MINIMUM;
        }

        SetTimer(message_hwnd_, reinterpret_cast<UINT_PTR>(this), delay_msec, NULL);
    }

    void MessagePumpForUI::PumpOutPendingPaintMessages()
    {
        if (!state_)
        {
            return;
        }

        const int kMaxPeekCount = 20;
        int peek_count;
        for (peek_count = 0; peek_count < kMaxPeekCount; ++peek_count)
        {
            MSG msg;
            if (!PeekMessage(&msg, NULL, 0, 0, PM_REMOVE | PM_QS_PAINT))
            {
                break;
            }
            ProcessMessageHelper(msg);
            if (state_->should_quit) 
            {
                break;
            }
        }
        // Histogram what was really being used, to help to adjust kMaxPeekCount.
        DHISTOGRAM_COUNTS("Loop.PumpOutPendingPaintMessages Peeks", peek_count);
    }

    // static
    LRESULT CALLBACK MessagePumpForUI::WndProcThunk(
        HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
    {
        switch (message)
        {
        case kMsgHaveWork:
            reinterpret_cast<MessagePumpForUI*>(wparam)->HandleWorkMessage();
            break;
        case WM_TIMER:
            reinterpret_cast<MessagePumpForUI*>(wparam)->HandleTimerMessage();
            break;
        }
        return DefWindowProc(hwnd, message, wparam, lparam);
    }

    void MessagePumpForUI::DoRunLoop()
    {
        //     * Sent messages
        //     * Posted messages
        //     * Sent messages (again)
        //     * WM_PAINT messages
        //     * WM_TIMER messages
        //

        for (;;)
        {
            bool more_work_is_plausible = ProcessNextWindowsMessage();
            if (state_->should_quit)
            {
                break;
            }

            more_work_is_plausible |= state_->delegate->DoWork();
            if (state_->should_quit)
            {
                break;
            }

            more_work_is_plausible |= state_->delegate->DoDelayedWork(
                &delayed_work_time_);

            if (more_work_is_plausible && delayed_work_time_.is_null())
            {
                KillTimer(message_hwnd_, reinterpret_cast<UINT_PTR>(this));
            }
            if (state_->should_quit)
            {
                break;
            }

            if (more_work_is_plausible)
            {
                continue;
            }

            more_work_is_plausible = state_->delegate->DoIdleWork();
            if (state_->should_quit)
            {
                break;
            }

            if (more_work_is_plausible)
            {
                continue;
            }

            WaitForWork(); 
        }
    }

    void MessagePumpForUI::InitMessageWnd()
    {
        HINSTANCE hinst = GetModuleHandle(NULL);

        WNDCLASSEX wc = { 0 };
        wc.cbSize = sizeof(wc);
        wc.lpfnWndProc = base::win::WrappedWindowProc<WndProcThunk>;
        wc.hInstance = hinst;
        wc.lpszClassName = kWndClass;
        RegisterClassEx(&wc);

        message_hwnd_ = CreateWindow(kWndClass, 0, 0, 0, 0, 0, 0,
            HWND_MESSAGE, 0, hinst, 0);
        DCHECK(message_hwnd_);
    }

    void MessagePumpForUI::WaitForWork()
    {
        int delay = GetCurrentDelay();
        if (delay < 0) 
        {
            delay = INFINITE;
        }

        HANDLE* handles;
        int nhandles = 0, n = 0;
        handles = handle_get_events(&nhandles);

        DWORD result;
        result = MsgWaitForMultipleObjectsEx(nhandles, handles, delay,
            QS_ALLINPUT, MWMO_INPUTAVAILABLE);

        if ((unsigned)(result - WAIT_OBJECT_0) < (unsigned)nhandles) {
            handle_got_event(handles[result - WAIT_OBJECT_0]);
        }
        sfree(handles);

        if (WAIT_OBJECT_0 + nhandles == result)
        {
#ifndef WM_NCMOUSEFIRST
#define WM_NCMOUSEFIRST WM_NCMOUSEMOVE
#endif
#ifndef WM_NCMOUSELAST
#define WM_NCMOUSELAST  WM_NCXBUTTONDBLCLK
#endif
            MSG msg = { 0 };
            DWORD queue_status = GetQueueStatus(QS_MOUSE);
            if (HIWORD(queue_status) & QS_MOUSE && !PeekMessage(&msg, NULL,
                WM_MOUSEFIRST, WM_MOUSELAST, PM_NOREMOVE) && !PeekMessage(&msg,
                    NULL, WM_NCMOUSEFIRST, WM_NCMOUSELAST, PM_NOREMOVE))
            {
                WaitMessage();
            }
            return;
        }

        DCHECK_NE(WAIT_FAILED, result) << GetLastError();
    }

    void MessagePumpForUI::HandleWorkMessage()
    {
        if (!state_)
        {
            InterlockedExchange(&have_work_, 0);
            return;
        }

        ProcessPumpReplacementMessage();

        if (state_->delegate->DoWork())
        {
            ScheduleWork();
        }
    }

    void MessagePumpForUI::HandleTimerMessage()
    {
        KillTimer(message_hwnd_, reinterpret_cast<UINT_PTR>(this));

        if (!state_)
        {
            return;
        }

        state_->delegate->DoDelayedWork(&delayed_work_time_);
        if (!delayed_work_time_.is_null())
        {
            ScheduleDelayedWork(delayed_work_time_);
        }
    }

    bool MessagePumpForUI::ProcessNextWindowsMessage()
    {
        bool sent_messages_in_queue = false;
        DWORD queue_status = GetQueueStatus(QS_SENDMESSAGE);
        if (HIWORD(queue_status) & QS_SENDMESSAGE)
        {
            sent_messages_in_queue = true;
        }

        MSG msg;
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            return ProcessMessageHelper(msg);
        }

        return sent_messages_in_queue;
    }

    bool MessagePumpForUI::ProcessMessageHelper(const MSG& msg)
    {
        if (WM_QUIT == msg.message)
        {
            state_->should_quit = true;
            PostQuitMessage(static_cast<int>(msg.wParam));
            return false;
        }

        if (msg.message == kMsgHaveWork && msg.hwnd == message_hwnd_)
        {
            return ProcessPumpReplacementMessage();
        }

        if (CallMsgFilter(const_cast<MSG*>(&msg), kMessageFilterCode))
        {
            return true;
        }

        WillProcessMessage(msg);

        if (state_->dispatcher)
        {
            if (!state_->dispatcher->Dispatch(msg))
            {
                state_->should_quit = true;
            }
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        DidProcessMessage(msg);
        return true;
    }

    bool MessagePumpForUI::ProcessPumpReplacementMessage()
    {
        bool have_message = false;
        MSG msg;
        // We should not process all window messages if we are in the context of an
        // OS modal loop, i.e. in the context of a windows API call like MessageBox.
        // This is to ensure that these messages are peeked out by the OS modal loop.
        if (MessageLoop::current()->os_modal_loop())
        {
            // We only peek out WM_PAINT and WM_TIMER here for reasons mentioned above.
            have_message = PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_REMOVE) ||
                PeekMessage(&msg, NULL, WM_TIMER, WM_TIMER, PM_REMOVE);
        }
        else
        {
            have_message = (0 != PeekMessage(&msg, NULL, 0, 0, PM_REMOVE));
        }

        DCHECK(!have_message || kMsgHaveWork != msg.message ||
            msg.hwnd != message_hwnd_);

        // Since we discarded a kMsgHaveWork message, we must update the flag.
        int old_have_work = InterlockedExchange(&have_work_, 0);
        DCHECK(old_have_work);

        // We don't need a special time slice if we didn't have_message to process.
        if (!have_message)
        {
            return false;
        }

        // Guarantee we'll get another time slice in the case where we go into native
        // windows code.   This ScheduleWork() may hurt performance a tiny bit when
        // tasks appear very infrequently, but when the event queue is busy, the
        // kMsgHaveWork events get (percentage wise) rarer and rarer.
        ScheduleWork();
        return ProcessMessageHelper(msg);
    }

    MessagePumpForIO::MessagePumpForIO()
    {
        port_.Set(CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 1));
        DCHECK(port_.IsValid());
    }

    void MessagePumpForIO::ScheduleWork()
    {
        if (InterlockedExchange(&have_work_, 1))
        {
            return; 
        }

        BOOL ret = PostQueuedCompletionStatus(port_, 0,
            reinterpret_cast<ULONG_PTR>(this),
            reinterpret_cast<OVERLAPPED*>(this));
        DCHECK(ret);
    }

    void MessagePumpForIO::ScheduleDelayedWork(const TimeTicks& delayed_work_time)
    {
        delayed_work_time_ = delayed_work_time;
    }

    void MessagePumpForIO::RegisterIOHandler(HANDLE file_handle,
        IOHandler* handler)
    {
        ULONG_PTR key = reinterpret_cast<ULONG_PTR>(handler);
        HANDLE port = CreateIoCompletionPort(file_handle, port_, key, 1);
        DCHECK(port == port_.Get());
    }

    void MessagePumpForIO::DoRunLoop()
    {
        for (;;)
        {
            bool more_work_is_plausible = state_->delegate->DoWork();
            if (state_->should_quit)
            {
                break;
            }

            more_work_is_plausible |= WaitForIOCompletion(0, NULL);
            if (state_->should_quit)
            {
                break;
            }

            more_work_is_plausible |=
                state_->delegate->DoDelayedWork(&delayed_work_time_);
            if (state_->should_quit)
            {
                break;
            }

            if (more_work_is_plausible)
            {
                continue;
            }

            more_work_is_plausible = state_->delegate->DoIdleWork();
            if (state_->should_quit)
            {
                break;
            }

            if (more_work_is_plausible)
            {
                continue;
            }

            WaitForWork(); 
        }
    }

    void MessagePumpForIO::WaitForWork()
    {
        DCHECK(state_->run_depth == 1) << "Cannot nest an IO message loop!";

        int timeout = GetCurrentDelay();
        if (timeout < 0) 
        {
            timeout = INFINITE;
        }

        WaitForIOCompletion(timeout, NULL);
    }

    bool MessagePumpForIO::WaitForIOCompletion(DWORD timeout, IOHandler* filter)
    {
        IOItem item;
        if (completed_io_.empty() || !MatchCompletedIOItem(filter, &item))
        {
            if (!GetIOItem(timeout, &item))
            {
                return false;
            }

            if (ProcessInternalIOItem(item))
            {
                return true;
            }
        }

        if (item.context->handler)
        {
            if (filter && item.handler != filter)
            {
                completed_io_.push_back(item);
            }
            else
            {
                DCHECK_EQ(item.context->handler, item.handler);
                WillProcessIOEvent();
                item.handler->OnIOCompleted(item.context,
                    item.bytes_transfered, item.error);
                DidProcessIOEvent();
            }
        }
        else
        {
            delete item.context;
        }
        return true;
    }

    bool MessagePumpForIO::GetIOItem(DWORD timeout, IOItem* item)
    {
        memset(item, 0, sizeof(*item));
        ULONG_PTR key = NULL;
        OVERLAPPED* overlapped = NULL;
        if (!GetQueuedCompletionStatus(port_.Get(), &item->bytes_transfered,
            &key, &overlapped, timeout))
        {
            if (!overlapped)
            {
                return false; 
            }
            item->error = GetLastError();
            item->bytes_transfered = 0;
        }

        item->handler = reinterpret_cast<IOHandler*>(key);
        item->context = reinterpret_cast<IOContext*>(overlapped);
        return true;
    }

    bool MessagePumpForIO::ProcessInternalIOItem(const IOItem& item)
    {
        if (this == reinterpret_cast<MessagePumpForIO*>(item.context) &&
            this == reinterpret_cast<MessagePumpForIO*>(item.handler))
        {
            DCHECK(!item.bytes_transfered);
            InterlockedExchange(&have_work_, 0);
            return true;
        }
        return false;
    }

    bool MessagePumpForIO::MatchCompletedIOItem(IOHandler* filter, IOItem* item)
    {
        DCHECK(!completed_io_.empty());
        for (std::list<IOItem>::iterator it = completed_io_.begin();
            it != completed_io_.end(); ++it)
        {
            if (!filter || it->handler == filter)
            {
                *item = *it;
                completed_io_.erase(it);
                return true;
            }
        }
        return false;
    }

    void MessagePumpForIO::AddIOObserver(IOObserver* obs)
    {
        io_observers_.AddObserver(obs);
    }

    void MessagePumpForIO::RemoveIOObserver(IOObserver* obs)
    {
        io_observers_.RemoveObserver(obs);
    }

    void MessagePumpForIO::WillProcessIOEvent()
    {
        FOR_EACH_OBSERVER(IOObserver, io_observers_, WillProcessIOEvent());
    }

    void MessagePumpForIO::DidProcessIOEvent()
    {
        FOR_EACH_OBSERVER(IOObserver, io_observers_, DidProcessIOEvent());
    }

} //namespace base

