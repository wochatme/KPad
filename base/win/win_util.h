#ifndef __base_win_util_h__
#define __base_win_util_h__

#include <windows.h>

#include "base/string16.h"

struct IPropertyStore;
struct _tagpropertykey;
typedef _tagpropertykey PROPERTYKEY;

namespace base
{
    namespace win
    {

        // A Windows message reflected from other windows. This message is sent
        // with the following arguments:
        // hWnd - Target window
        // uMsg - kReflectedMessage
        // wParam - Should be 0
        // lParam - Pointer to MSG struct containing the original message.
        const int kReflectedMessage = WM_APP + 3;

        void GetNonClientMetrics(NONCLIENTMETRICS* metrics);

        bool GetUserSidString(std::wstring* user_sid);

        bool IsShiftPressed();

        bool IsCtrlPressed();

        bool IsAltPressed();

        bool UserAccountControlIsEnabled();

        // Sets the application id in given IPropertyStore. The function is intended
        // for tagging application/chromium shortcut, browser window and jump list for
        // Win7.
        bool SetAppIdForPropertyStore(IPropertyStore* property_store,
            const wchar_t* app_id);

        // Adds the specified |command| using the specified |name| to the AutoRun key.
        // |root_key| could be HKCU or HKLM or the root of any user hive.
        bool AddCommandToAutoRun(HKEY root_key, const string16& name,
            const string16& command);
        // Removes the command specified by |name| from the AutoRun key. |root_key|
        // could be HKCU or HKLM or the root of any user hive.
        bool RemoveCommandFromAutoRun(HKEY root_key, const string16& name);

        // Reads the command specified by |name| from the AutoRun key. |root_key|
        // could be HKCU or HKLM or the root of any user hive.
        bool ReadCommandFromAutoRun(HKEY root_key, const string16& name,
            string16* command);

    } //namespace win
} //namespace base

#endif //__base_win_util_h__