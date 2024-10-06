#include "at_exit.h"

#include <stddef.h>
#include <ostream>
#include <utility>

#include "bind.h"
#include "logging.h"
#include "task.h"

namespace base
{
    // Keep a stack of registered AtExitManagers.  We always operate on the most
    // recent, and we should never have more than one outside of testing (for a
    // statically linked version of this library).  Testing may use the shadow
    // version of the constructor, and if we are building a dynamic library we may
    // end up with multiple AtExitManagers on the same process.  We don't protect
    // this for thread-safe access, since it will only be modified in testing.
    static AtExitManager* g_top_manager = NULL;

    AtExitManager::AtExitManager()
    {
        DCHECK(!g_top_manager);
        g_top_manager = this;
    }

    AtExitManager::~AtExitManager()
    {
        if (!g_top_manager)
        {
            NOTREACHED() << "Tried to ~AtExitManager without an AtExitManager";
            return;
        }
        DCHECK(g_top_manager == this);

        ProcessCallbacksNow();
        g_top_manager = NULL;
    }

    void AtExitManager::RegisterCallback(AtExitCallbackType func, void* param)
    {
        DCHECK(func);
        RegisterTask(base::Bind(func, param));
    }

    // static
    void AtExitManager::RegisterTask(base::Closure task)
    {
        if (!g_top_manager)
        {
            NOTREACHED() << "Tried to RegisterCallback without an AtExitManager";
            return;
        }

        AutoLock lock(g_top_manager->lock_);
        g_top_manager->stack_.push(task);
    }

    void AtExitManager::ProcessCallbacksNow()
    {
        if (!g_top_manager)
        {
            NOTREACHED() << "Tried to ProcessCallbacksNow without an AtExitManager";
            return;
        }

        AutoLock lock(g_top_manager->lock_);

        while (!g_top_manager->stack_.empty())
        {
            base::Closure task = g_top_manager->stack_.top();
            task.Run();
            g_top_manager->stack_.pop();
        }
    }

} //namespace base


