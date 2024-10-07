#include "pch.h"
#include "Chromium.h"

#include "base/basic_types.h"
#include "base/at_exit.h"
#include "base/message_loop.h"
#include "base/path_service.h"
#include "base/process_util.h"
#include "uibase/resource/resource_bundle.h"
#include "uiview/focus/accelerator_handler.h"

class ChromeMain
{
public:
    ChromeMain() {}
    virtual ~ChromeMain() {}

    void Run()
    {
        base::EnableTerminationOnHeapCorruption();

        MessageLoop main_message_loop(MessageLoop::TYPE_UI);
    }

private:
    DISALLOW_COPY_AND_ASSIGN(ChromeMain);
};


int ChromeTest(LPTSTR lpstrCmdLine, int nCmdShow)
{
    base::AtExitManager exit_manager;

    return 0;
}

