#pragma once

struct Input_Iterator_Tag {};
struct Output_Iterator_Tag {};
struct Forward_Iterator_Tag : public Input_Iterator_Tag {};
struct Bidirectional_Iterator_Tag : public Forward_Iterator_Tag {};
struct Random_Access_Iterator_Tag : public Bidirectional_Iterator_Tag {};

template <class T> 
struct Input_Iterator 
{
	typedef Input_Iterator_Tag IteratorCategory;
	typedef T                  ValueType;
	typedef ptrdiff_t           DifferenceType;
	typedef T*                 Pointer;
	typedef T&                 Reference;
};

template <class T>
struct Output_Iterator 
{
	typedef Output_Iterator_Tag IteratorCategory;
	typedef void                ValueType;
	typedef void                DifferenceType;
	typedef void                Pointer;
	typedef void                Reference;
};

template <class T> 
struct Forward_Iterator 
{
	typedef Forward_Iterator_Tag IteratorCategory;
	typedef T                    ValueType;
	typedef ptrdiff_t             DifferenceType;
	typedef T*                   Pointer;
	typedef T&                   Reference;
};


template <class T> 
struct Bidirectional_Iterator 
{
	typedef Bidirectional_Iterator_Tag IteratorCategory;
	typedef T                          ValueType;
	typedef ptrdiff_t                   DifferenceType;
	typedef T*                         Pointer;
	typedef T&                         Reference;
};

template <class T> 
struct Random_Access_Iterator 
{
	typedef Random_Access_Iterator_Tag IteratorCategory;
	typedef T                          ValueType;
	typedef ptrdiff_t                   DifferenceType;
	typedef T*                         Pointer;
	typedef T&                         Reference;
};

template <class Iterator>
struct Iterator_Traits 
{
	typedef typename Iterator::IteratorCategory IteratorCategory;
	typedef typename Iterator::ValueType         ValueType;
	typedef typename Iterator::DifferenceType    DifferenceType;
	typedef typename Iterator::Pointer           Pointer;
	typedef typename Iterator::Reference         Reference;
};

template <class T>
struct Iterator_Traits<T*> 
{
	typedef Random_Access_Iterator_Tag IteratorCategory;
	typedef T                          ValueType;
	typedef ptrdiff_t                  DifferenceType;
	typedef T*                         Pointer;
	typedef T&                         Reference;
};

template <class T>
struct Iterator_Traits<const T*> 
{
	typedef Random_Access_Iterator_Tag IteratorCategory;
	typedef T                          ValueType;
	typedef ptrdiff_t                  DifferenceType;
	typedef const T*                   Pointer;
	typedef const T&                   Reference;
};

//注意这里的typename,DifferenceType还未定义，要想用他，得加关键字typename
template <class RandomAccessIterator>
inline typename Iterator_Traits<RandomAccessIterator>::DifferenceType
_Distance(RandomAccessIterator first, RandomAccessIterator last, Random_Access_Iterator_Tag)
{
	return last - first;
}

template <class ForwardIterator>
inline typename Iterator_Traits<ForwardIterator>::DifferenceType
_Distance(ForwardIterator first, ForwardIterator last, Forward_Iterator_Tag)
{
	Iterator_Traits<ForwardIterator>::DifferenceType n = 0;
	while (first != last)
	{
		++first;
		++n;
	}
	return n;
}

template <class BidirectionalIterator>
inline typename Iterator_Traits<BidirectionalIterator>::DifferenceType
_Distance(BidirectionalIterator first, BidirectionalIterator last, Bidirectional_Iterator_Tag)
{
	Iterator_Traits<BidirectionalIterator>::DifferenceType n = 0;
	while (first != last)
	{
		++first;
		++n;
	}
	return n;
}

template <class InputIterator>
inline typename Iterator_Traits<InputIterator>::DifferenceType
Distance(InputIterator first, InputIterator last)
{
	typedef typename Iterator_Traits<InputIterator>::IteratorCategory category;
	return _Distance(first, last, category());
}



