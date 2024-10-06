#include "pch.h"
#include "Chromium.h"

#include "base/at_exit.h"


int ChromeTest(LPTSTR lpstrCmdLine, int nCmdShow)
{
    base::AtExitManager exit_manager;

    return 0;
}

