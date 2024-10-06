#ifndef __base_message_pump_win_h__
#define __base_message_pump_win_h__

#include <windows.h>
#include <list>

#include "message_pump.h"
#include "observer_list.h"
#include "base_time.h"
#include "win/scoped_handle.h"

namespace base
{
    class MessagePumpWin : public MessagePump
    {
    public:
        class Observer
        {
        public:
            virtual ~Observer() {}

            virtual void WillProcessMessage(const MSG& msg) = 0;

            virtual void DidProcessMessage(const MSG& msg) = 0;
        };

        class Dispatcher
        {
        public:
            virtual ~Dispatcher() {}
            virtual bool Dispatch(const MSG& msg) = 0;
        };

        MessagePumpWin() : have_work_(0), state_(NULL) {}
        virtual ~MessagePumpWin() {}

        void AddObserver(Observer* observer);

        void RemoveObserver(Observer* observer);

        void WillProcessMessage(const MSG& msg);
        void DidProcessMessage(const MSG& msg);

        void RunWithDispatcher(Delegate* delegate, Dispatcher* dispatcher);

        virtual void Run(Delegate* delegate) { RunWithDispatcher(delegate, NULL); }
        virtual void Quit();

    protected:
        struct RunState
        {
            Delegate* delegate;
            Dispatcher* dispatcher;

            bool should_quit;

            int run_depth;
        };

        virtual void DoRunLoop() = 0;
        int GetCurrentDelay() const;

        ObserverList<Observer> observers_;

        TimeTicks delayed_work_time_;

        LONG have_work_;

        RunState* state_;
    };

    class MessagePumpForUI : public MessagePumpWin
    {
    public:
        static const int kMessageFilterCode = 0x5001;

        MessagePumpForUI();
        virtual ~MessagePumpForUI();

        virtual void ScheduleWork();
        virtual void ScheduleDelayedWork(const TimeTicks& delayed_work_time);

        void PumpOutPendingPaintMessages();

    private:
        static LRESULT CALLBACK WndProcThunk(
            HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
        virtual void DoRunLoop();
        void InitMessageWnd();
        void WaitForWork();
        void HandleWorkMessage();
        void HandleTimerMessage();
        bool ProcessNextWindowsMessage();
        bool ProcessMessageHelper(const MSG& msg);
        bool ProcessPumpReplacementMessage();

        HWND message_hwnd_;
    };

    class MessagePumpForIO : public MessagePumpWin
    {
    public:
        struct IOContext;

        class IOHandler
        {
        public:
            virtual ~IOHandler() {}
            virtual void OnIOCompleted(IOContext* context, DWORD bytes_transfered,
                DWORD error) = 0;
        };

        class IOObserver
        {
        public:
            IOObserver() {}

            virtual void WillProcessIOEvent() = 0;
            virtual void DidProcessIOEvent() = 0;

        protected:
            virtual ~IOObserver() {}
        };

        struct IOContext
        {
            OVERLAPPED overlapped;
            IOHandler* handler;
        };

        MessagePumpForIO();
        virtual ~MessagePumpForIO() {}

        virtual void ScheduleWork();
        virtual void ScheduleDelayedWork(const TimeTicks& delayed_work_time);

        void RegisterIOHandler(HANDLE file_handle, IOHandler* handler);

        bool WaitForIOCompletion(DWORD timeout, IOHandler* filter);

        void AddIOObserver(IOObserver* obs);
        void RemoveIOObserver(IOObserver* obs);

    private:
        struct IOItem
        {
            IOHandler* handler;
            IOContext* context;
            DWORD bytes_transfered;
            DWORD error;
        };

        virtual void DoRunLoop();
        void WaitForWork();
        bool MatchCompletedIOItem(IOHandler* filter, IOItem* item);
        bool GetIOItem(DWORD timeout, IOItem* item);
        bool ProcessInternalIOItem(const IOItem& item);
        void WillProcessIOEvent();
        void DidProcessIOEvent();

        win::ScopedHandle port_;
        std::list<IOItem> completed_io_;

        ObserverList<IOObserver> io_observers_;
    };

} //namespace base

#endif //__base_message_pump_win_h__