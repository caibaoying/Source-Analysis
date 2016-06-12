#pragma once

#include "TypeTraits.h"
#include "Iterator.h"

template <class T>
inline void Destory(T* pointer)
{
	pointer->~T();
}

template <class T1, class T2>
inline void Construct(T1* p, const T2& value)
{
	new(p)T1(value);
}

// 释放一组区间对象
template <class ForwardIterator>
inline void
__DestroyAux(ForwardIterator first, ForwardIterator last, __FalseType)
{
	for (; first < last; ++first)
		Destroy(&*first);
}

template <class ForwardIterator>
inline void __DestroyAux(ForwardIterator, ForwardIterator, __TrueType) 
{}

template <class ForwardIterator, class T>
inline void __Destroy(ForwardIterator first, ForwardIterator last, T*)
{
	typedef typename __TypeTraits<T>::HasTrivialDestructor TrivialDestructor;
	__DestroyAux(first, last, TrivialDestructor);
}

template <class ForwardIterator>
inline void Destroy(ForwardIterator first, ForwardIterator last)
{
	__Destroy(first, last, ValueType(first));
}

inline void Destroy(char*, char*)
{}
inline void Destroy(wchar_t*, wchar_t*)
{}
