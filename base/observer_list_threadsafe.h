#ifndef __base_observer_list_threadsafe_h__
#define __base_observer_list_threadsafe_h__

#include <algorithm>
#include <map>

#include "callback_old.h"
#include "message_loop.h"
#include "message_loop_proxy.h"
#include "observer_list.h"
#include "task.h"


template<class ObserverType>
class ObserverListThreadSafe;

template<class T>
struct ObserverListThreadSafeTraits
{
    static void Destruct(const ObserverListThreadSafe<T>* x)
    {
        delete x;
    }
};

template<class ObserverType>
class ObserverListThreadSafe
    : public base::RefCountedThreadSafe<
    ObserverListThreadSafe<ObserverType>,
    ObserverListThreadSafeTraits<ObserverType> >
{
public:
    typedef typename ObserverList<ObserverType>::NotificationType
        NotificationType;

    ObserverListThreadSafe()
        : type_(ObserverListBase<ObserverType>::NOTIFY_ALL) {}
    explicit ObserverListThreadSafe(NotificationType type) : type_(type) {}

    void AddObserver(ObserverType* obs)
    {
        ObserverList<ObserverType>* list = NULL;
        MessageLoop* loop = MessageLoop::current();
        {
            base::AutoLock lock(list_lock_);
            if (observer_lists_.find(loop) == observer_lists_.end())
            {
                observer_lists_[loop] = new ObserverListContext(type_);
            }
            list = &(observer_lists_[loop]->list);
        }
        list->AddObserver(obs);
    }

    void RemoveObserver(ObserverType* obs)
    {
        ObserverListContext* context = NULL;
        ObserverList<ObserverType>* list = NULL;
        MessageLoop* loop = MessageLoop::current();
        if (!loop)
        {
            return; 
        }

        {
            base::AutoLock lock(list_lock_);
            typename ObserversListMap::iterator it = observer_lists_.find(loop);
            if (it == observer_lists_.end())
            {
                // This will happen if we try to remove an observer on a thread
                // we never added an observer for.
                return;
            }
            context = it->second;
            list = &context->list;

            // If we're about to remove the last observer from the list,
            // then we can remove this observer_list entirely.
            if (list->HasObserver(obs) && list->size() == 1)
            {
                observer_lists_.erase(it);
            }
        }
        list->RemoveObserver(obs);

        if (list->size() == 0)
        {
            delete context;
        }
    }

    template<class Method>
    void Notify(Method m)
    {
        UnboundMethod<ObserverType, Method, Tuple0> method(m, MakeTuple());
        Notify<Method, Tuple0>(method);
    }

    template<class Method, class A>
    void Notify(Method m, const A& a)
    {
        UnboundMethod<ObserverType, Method, Tuple1<A> > method(m, MakeTuple(a));
        Notify<Method, Tuple1<A> >(method);
    }

private:
    friend struct ObserverListThreadSafeTraits<ObserverType>;

    struct ObserverListContext
    {
        explicit ObserverListContext(NotificationType type)
            : loop(base::MessageLoopProxy::current()),
            list(type) {}

        scoped_refptr<base::MessageLoopProxy> loop;
        ObserverList<ObserverType> list;

        DISALLOW_COPY_AND_ASSIGN(ObserverListContext);
    };

    ~ObserverListThreadSafe()
    {
        typename ObserversListMap::const_iterator it;
        for (it = observer_lists_.begin(); it != observer_lists_.end(); ++it)
        {
            delete (*it).second;
        }
        observer_lists_.clear();
    }

    template<class Method, class Params>
    void Notify(const UnboundMethod<ObserverType, Method, Params>& method)
    {
        base::AutoLock lock(list_lock_);
        typename ObserversListMap::iterator it;
        for (it = observer_lists_.begin(); it != observer_lists_.end(); ++it)
        {
            ObserverListContext* context = (*it).second;
            context->loop->PostTask(
                NewRunnableMethod(this,
                    &ObserverListThreadSafe<ObserverType>::
                    template NotifyWrapper<Method, Params>, context, method));
        }
    }

    template<class Method, class Params>
    void NotifyWrapper(ObserverListContext* context,
        const UnboundMethod<ObserverType, Method, Params>& method)
    {
        {
            base::AutoLock lock(list_lock_);
            typename ObserversListMap::iterator it =
                observer_lists_.find(MessageLoop::current());

            if (it == observer_lists_.end() || it->second != context)
            {
                return;
            }
        }

        {
            typename ObserverList<ObserverType>::Iterator it(context->list);
            ObserverType* obs;
            while ((obs = it.GetNext()) != NULL)
            {
                method.Run(obs);
            }
        }

        if (context->list.size() == 0)
        {
            {
                base::AutoLock lock(list_lock_);
                typename ObserversListMap::iterator it =
                    observer_lists_.find(MessageLoop::current());
                if (it != observer_lists_.end() && it->second == context)
                {
                    observer_lists_.erase(it);
                }
            }
            delete context;
        }
    }

    typedef std::map<MessageLoop*, ObserverListContext*> ObserversListMap;

    base::Lock list_lock_; 
    ObserversListMap observer_lists_;
    const NotificationType type_;

    DISALLOW_COPY_AND_ASSIGN(ObserverListThreadSafe);
};

#endif //__base_observer_list_threadsafe_h__