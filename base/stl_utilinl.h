#ifndef __base_stl_utilinl_h__
#define __base_stl_utilinl_h__

#include <set>
#include <string>
#include <vector>
#include <functional>

template<class T>
void STLClearObject(T* obj)
{
    T tmp;
    tmp.swap(*obj);
    obj->reserve(0); 
}

template<class T>
inline void STLClearIfBig(T* obj, size_t limit = 1 << 20)
{
    if (obj->capacity() >= limit)
    {
        STLClearObject(obj);
    }
    else
    {
        obj->clear();
    }
}

template<class T>
void STLReserveIfNeeded(T* obj, int new_size)
{
    if (obj->capacity() < new_size)
    {
        obj->reserve(new_size);
    }
    else if (obj->size() > new_size) 
    {
        obj->resize(new_size);
    }
}

template<class ForwardIterator>
void STLDeleteContainerPointers(ForwardIterator begin, ForwardIterator end)
{
    while (begin != end)
    {
        ForwardIterator temp = begin;
        ++begin;
        delete* temp;
    }
}

template<class ForwardIterator>
void STLDeleteContainerPairPointers(ForwardIterator begin,
    ForwardIterator end)
{
    while (begin != end)
    {
        ForwardIterator temp = begin;
        ++begin;
        delete temp->first;
        delete temp->second;
    }
}

template<class ForwardIterator>
void STLDeleteContainerPairFirstPointers(ForwardIterator begin,
    ForwardIterator end)
{
    while (begin != end)
    {
        ForwardIterator temp = begin;
        ++begin;
        delete temp->first;
    }
}

// NOTE: Like STLDeleteContainerPointers, deleting behind the iterator.
// Deleting the value does not always invalidate the iterator, but it may
// do so if the key is a pointer into the value object.
template<class ForwardIterator>
void STLDeleteContainerPairSecondPointers(ForwardIterator begin,
    ForwardIterator end)
{
    while (begin != end)
    {
        ForwardIterator temp = begin;
        ++begin;
        delete temp->second;
    }
}

template<typename T>
inline void STLAssignToVector(std::vector<T>* vec,
    const T* ptr,
    size_t n)
{
    vec->resize(n);
    memcpy(&vec->front(), ptr, n * sizeof(T));
}

inline void STLAssignToVectorChar(std::vector<char>* vec,
    const char* ptr,
    size_t n)
{
    STLAssignToVector(vec, ptr, n);
}

inline void STLAssignToString(std::string* str, const char* ptr, size_t n)
{
    str->resize(n);
    memcpy(&*str->begin(), ptr, n);
}

template<typename T>
inline T* vector_as_array(std::vector<T>* v)
{
    return v->empty() ? NULL : &*v->begin();
}

template<typename T>
inline const T* vector_as_array(const std::vector<T>* v)
{
    return v->empty() ? NULL : &*v->begin();
}

// (http://www.open-std.org/JTC1/SC22/WG21/docs/lwg-active.html#530)
inline char* string_as_array(std::string* str)
{
    return str->empty() ? NULL : &*str->begin();
}

template<class HashSet>
inline bool HashSetEquality(const HashSet& set_a, const HashSet& set_b)
{
    if (set_a.size() != set_b.size()) return false;
    for (typename HashSet::const_iterator i = set_a.begin();
        i != set_a.end(); ++i)
    {
        if (set_b.find(*i) == set_b.end())
        {
            return false;
        }
    }
    return true;
}

template<class HashMap>
inline bool HashMapEquality(const HashMap& map_a, const HashMap& map_b)
{
    if (map_a.size() != map_b.size()) return false;
    for (typename HashMap::const_iterator i = map_a.begin();
        i != map_a.end(); ++i)
    {
        typename HashMap::const_iterator j = map_b.find(i->first);
        if (j == map_b.end()) return false;
        if (i->second != j->second) return false;
    }
    return true;
}

template<class T>
void STLDeleteElements(T* container)
{
    if (!container) return;
    STLDeleteContainerPointers(container->begin(), container->end());
    container->clear();
}

template<class T>
void STLDeleteValues(T* v)
{
    if (!v) return;
    for (typename T::iterator i = v->begin(); i != v->end(); ++i)
    {
        delete i->second;
    }
    v->clear();
}

//     vector<MyProto*> tmp_proto;
//     STLElementDeleter<vector<MyProto*> > d(&tmp_proto);
//     if(...) return false;
//     ...
//     return success;

template<class STLContainer>
class STLElementDeleter
{
public:
    STLElementDeleter<STLContainer>(STLContainer* ptr) : container_ptr_(ptr) {}
    ~STLElementDeleter<STLContainer>() { STLDeleteElements(container_ptr_); }
private:
    STLContainer* container_ptr_;
};

template<class STLContainer>
class STLValueDeleter
{
public:
    STLValueDeleter<STLContainer>(STLContainer* ptr) : container_ptr_(ptr) {}
    ~STLValueDeleter<STLContainer>() { STLDeleteValues(container_ptr_); }
private:
    STLContainer* container_ptr_;
};

// Forward declare some callback classes in callback.h for STLBinaryFunction
template<class R, class T1, class T2>
class ResultCallback2;

// STLBinaryFunction is a wrapper for the ResultCallback2 class in callback.h
// It provides an operator () method instead of a Run method, so it may be
// passed to STL functions in <algorithm>.
//
// The client should create callback with NewPermanentCallback, and should
// delete callback after it is done using the STLBinaryFunction.

template<class Result, class Arg1, class Arg2>
class STLBinaryFunction : public std::binary_function<Arg1, Arg2, Result>
{
public:
    typedef ResultCallback2<Result, Arg1, Arg2> Callback;

    STLBinaryFunction(Callback* callback)
        : callback_(callback)
    {
        assert(callback_);
    }

    Result operator() (Arg1 arg1, Arg2 arg2)
    {
        return callback_->Run(arg1, arg2);
    }

private:
    Callback* callback_;
};

// STLBinaryPredicate is a specialized version of STLBinaryFunction, where the
// return type is bool and both arguments have type Arg.  It can be used
// wherever STL requires a StrictWeakOrdering, such as in sort() or
// lower_bound().
//
// templated typedefs are not supported, so instead we use inheritance.

template<class Arg>
class STLBinaryPredicate : public STLBinaryFunction<bool, Arg, Arg>
{
public:
    typedef typename STLBinaryPredicate<Arg>::Callback Callback;
    STLBinaryPredicate(Callback* callback)
        : STLBinaryFunction<bool, Arg, Arg>(callback) {}
};

// Functors that compose arbitrary unary and binary functions with a
// function that "projects" one of the members of a pair.
// Specifically, if p1 and p2, respectively, are the functions that
// map a pair to its first and second, respectively, members, the
// table below summarizes the functions that can be constructed:
//
// * UnaryOperate1st<pair>(f) returns the function x -> f(p1(x))
// * UnaryOperate2nd<pair>(f) returns the function x -> f(p2(x))
// * BinaryOperate1st<pair>(f) returns the function (x,y) -> f(p1(x),p1(y))
// * BinaryOperate2nd<pair>(f) returns the function (x,y) -> f(p2(x),p2(y))
//
// A typical usage for these functions would be when iterating over
// the contents of an STL map. For other sample usage, see the unittest.

template<typename Pair, typename UnaryOp>
class UnaryOperateOnFirst
    : public std::unary_function<Pair, typename UnaryOp::result_type>
{
public:
    UnaryOperateOnFirst() {}

    UnaryOperateOnFirst(const UnaryOp& f) : f_(f) {}

    typename UnaryOp::result_type operator()(const Pair& p) const
    {
        return f_(p.first);
    }

private:
    UnaryOp f_;
};

template<typename Pair, typename UnaryOp>
UnaryOperateOnFirst<Pair, UnaryOp> UnaryOperate1st(const UnaryOp& f)
{
    return UnaryOperateOnFirst<Pair, UnaryOp>(f);
}

template<typename Pair, typename UnaryOp>
class UnaryOperateOnSecond
    : public std::unary_function<Pair, typename UnaryOp::result_type>
{
public:
    UnaryOperateOnSecond() {}

    UnaryOperateOnSecond(const UnaryOp& f) : f_(f) {}

    typename UnaryOp::result_type operator()(const Pair& p) const
    {
        return f_(p.second);
    }

private:
    UnaryOp f_;
};

template<typename Pair, typename UnaryOp>
UnaryOperateOnSecond<Pair, UnaryOp> UnaryOperate2nd(const UnaryOp& f)
{
    return UnaryOperateOnSecond<Pair, UnaryOp>(f);
}

template<typename Pair, typename BinaryOp>
class BinaryOperateOnFirst
    : public std::binary_function<Pair, Pair, typename BinaryOp::result_type>
{
public:
    BinaryOperateOnFirst() {}

    BinaryOperateOnFirst(const BinaryOp& f) : f_(f) {}

    typename BinaryOp::result_type operator()(const Pair& p1,
        const Pair& p2) const
    {
        return f_(p1.first, p2.first);
    }

private:
    BinaryOp f_;
};

template<typename Pair, typename BinaryOp>
BinaryOperateOnFirst<Pair, BinaryOp> BinaryOperate1st(const BinaryOp& f)
{
    return BinaryOperateOnFirst<Pair, BinaryOp>(f);
}

template<typename Pair, typename BinaryOp>
class BinaryOperateOnSecond
    : public std::binary_function<Pair, Pair, typename BinaryOp::result_type>
{
public:
    BinaryOperateOnSecond() {}

    BinaryOperateOnSecond(const BinaryOp& f) : f_(f) {}

    typename BinaryOp::result_type operator()(const Pair& p1,
        const Pair& p2) const
    {
        return f_(p1.second, p2.second);
    }

private:
    BinaryOp f_;
};

template<typename Pair, typename BinaryOp>
BinaryOperateOnSecond<Pair, BinaryOp> BinaryOperate2nd(const BinaryOp& f)
{
    return BinaryOperateOnSecond<Pair, BinaryOp>(f);
}

template<typename T>
std::vector<T> SetToVector(const std::set<T>& values)
{
    std::vector<T> result;
    result.reserve(values.size());
    result.insert(result.begin(), values.begin(), values.end());
    return result;
}

template<typename Collection, typename Key>
bool ContainsKey(const Collection& collection, const Key& key)
{
    return collection.find(key) != collection.end();
}

#endif //__base_stl_utilinl_h__