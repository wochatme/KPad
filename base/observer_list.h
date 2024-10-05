#ifndef __base_observer_list_h__
#define __base_observer_list_h__

#include <algorithm>
#include <limits>
#include <vector>

#include "logging.h"
#include "memory/weak_ptr.h"

template<typename ObserverType>
class ObserverListThreadSafe;

template<class ObserverType>
class ObserverListBase : public base::SupportsWeakPtr<ObserverListBase<ObserverType> >
{
public:
    enum NotificationType
    {
        NOTIFY_ALL,
        NOTIFY_EXISTING_ONLY
    };

    class Iterator
    {
    public:
        Iterator(ObserverListBase<ObserverType>& list)
            : list_(list.AsWeakPtr()),
            index_(0),
            max_index_(list.type_ == NOTIFY_ALL ? std::numeric_limits<size_t>::max() :
                list.observers_.size())
        {
            ++list_->notify_depth_;
        }

        ~Iterator()
        {
            if (list_ && --list_->notify_depth_ == 0)
            {
                list_->Compact();
            }
        }

        ObserverType* GetNext()
        {
            if (!list_)
            {
                return NULL;
            }
            ListType& observers = list_->observers_;
            // Advance if the current element is null
            size_t max_index = std::min(max_index_, observers.size());
            while (index_ < max_index && !observers[index_])
            {
                ++index_;
            }
            return index_ < max_index ? observers[index_++] : NULL;
        }

    private:
        base::WeakPtr<ObserverListBase<ObserverType> > list_;
        size_t index_;
        size_t max_index_;
    };

    ObserverListBase() : notify_depth_(0), type_(NOTIFY_ALL) {}
    explicit ObserverListBase(NotificationType type)
        : notify_depth_(0), type_(type) {}

    void AddObserver(ObserverType* obs)
    {
        if (std::find(observers_.begin(), observers_.end(), obs)
            != observers_.end())
        {
            NOTREACHED() << "Observers can only be added once!";
            return;
        }
        observers_.push_back(obs);
    }

    void RemoveObserver(ObserverType* obs)
    {
        typename ListType::iterator it = std::find(observers_.begin(),
            observers_.end(), obs);
        if (it != observers_.end())
        {
            if (notify_depth_)
            {
                *it = 0;
            }
            else
            {
                observers_.erase(it);
            }
        }
    }

    bool HasObserver(ObserverType* observer) const
    {
        for (size_t i = 0; i < observers_.size(); ++i)
        {
            if (observers_[i] == observer)
            {
                return true;
            }
        }
        return false;
    }

    void Clear()
    {
        if (notify_depth_)
        {
            for (typename ListType::iterator it = observers_.begin();
                it != observers_.end(); ++it)
            {
                *it = 0;
            }
        }
        else
        {
            observers_.clear();
        }
    }

    size_t size() const { return observers_.size(); }

protected:
    void Compact()
    {
        typename ListType::iterator it = observers_.begin();
        while (it != observers_.end())
        {
            if (*it)
            {
                ++it;
            }
            else
            {
                it = observers_.erase(it);
            }
        }
    }

private:
    friend class ObserverListThreadSafe<ObserverType>;

    typedef std::vector<ObserverType*> ListType;

    ListType observers_;
    int notify_depth_;
    NotificationType type_;

    friend class ObserverListBase::Iterator;

    DISALLOW_COPY_AND_ASSIGN(ObserverListBase);
};

template<class ObserverType, bool check_empty = false>
class ObserverList : public ObserverListBase<ObserverType>
{
public:
    typedef typename ObserverListBase<ObserverType>::NotificationType
        NotificationType;

    ObserverList() {}
    explicit ObserverList(NotificationType type)
        : ObserverListBase<ObserverType>(type) {}

    ~ObserverList()
    {
        if (check_empty)
        {
            ObserverListBase<ObserverType>::Compact();
            DCHECK_EQ(ObserverListBase<ObserverType>::size(), 0);
        }
    }
};

#define FOR_EACH_OBSERVER(ObserverType, observer_list, func) \
    do{ \
      ObserverListBase<ObserverType>::Iterator it(observer_list); \
      ObserverType* obs; \
      while((obs=it.GetNext()) != NULL) \
        obs->func; \
    } while(0)

#endif //__base_observer_list_h__