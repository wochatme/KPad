#include "pch.h"
#include "Chromium.h"

#include "base/basic_types.h"
#include "base/at_exit.h"
#include "base/message_loop.h"
#include "base/path_service.h"
#include "base/process_util.h"
#include "base/memory/singleton.h"
#include "uibase/resource/resource_bundle.h"
#include "uiview/focus/accelerator_handler.h"

#include "chrome/chrome_view_delegate.h"

#if 0
class CmdLineHandler 
{
public:
	enum { SHARED_MEM_SIZE = 512 };
	enum { TIMER_INTERVAL = 50 }; //in ms
	enum 
	{
		COMMAND_NONE = 0,
		COMMAND_CMD_LINE = 1
	};

	enum SharedType { SHARED_TYPE_LEADER = 0, SHARED_TYPE_FOLLOWER = 1 };
	static const char* const SHARED_MEM_NAME;
	static const char* const SHARED_MEM_MUTEX_NAME;
	
	static CmdLineHandler* GetInstance() 
	{
		return Singleton<CmdLineHandler>::get();
	}

	CmdLineHandler()
	{

	}

	~CmdLineHandler()
	{

	}

	//common method
	void handleCmd();
	bool toBeLeader(); //create or open shared mem handle
	bool isLeader() { return sharedType_ == SHARED_TYPE_LEADER; };

	//leader method
	void leaderTimerCallback();
	void process_cmdline(LPSTR cmdline);

	//follower method
	void sendMsgToLeader();

	//other
	void setCmdline(char* cmdline)
	{
		strncpy(cmdLine_, cmdline, sizeof(cmdLine_) - 1);
	}

	bool isLeaderStartWithCmd() {
		return isLeaderStartWithCmd_;
	}



private:
	SharedType sharedType_;
	HANDLE sharedMemHandle_;
	HANDLE sharedMemMutex_;
	char* sharedBuffer_;
	base::RepeatingTimer<CmdLineHandler> checkMemTimer_;
	char userShareMemName_[128];
	char userShareMemMutexName_[128];
	char cmdLine_[SHARED_MEM_SIZE - 1];
	bool isLeaderStartWithCmd_;

	friend struct DefaultSingletonTraits<CmdLineHandler>;
	DISALLOW_COPY_AND_ASSIGN(CmdLineHandler);

};
#endif 

class ChromeMain
{
public:
    ChromeMain() {}
    virtual ~ChromeMain() {}

    void Run()
    {
        base::EnableTerminationOnHeapCorruption();

        FilePath res_dll;
        PathService::Get(base::DIR_EXE, &res_dll);

        MessageLoop main_message_loop(MessageLoop::TYPE_UI);
    }

private:
    DISALLOW_COPY_AND_ASSIGN(ChromeMain);
};


int ChromeTest(LPTSTR lpstrCmdLine, int nCmdShow)
{
    base::AtExitManager exit_manager;

    //CmdLineHandler::GetInstance()->setCmdline(W2A(lpCmdLine));
#if 0
	ChromeViewDelegate delegate;
#endif 

	ChromeMain main;
	main.Run();

    return 0;
}

