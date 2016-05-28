#pragma once

typedef ptrdiff_t Difference_Type;


struct Input_Iterator_Tag {};
struct Output_Iterator_Tag {};
struct Forward_Iterator_Tag : public Input_Iterator_Tag {};
struct Bidirectional_Iterator_Tag : public Forward_Iterator_Tag {};
struct Random_Access_Iterator_Tag : public Bidirectional_Iterator_Tag {};

template <class T, class Distance> 
struct Input_Iterator 
{
	typedef Input_Iterator_Tag Iterator_Category;
	typedef T                  Value_Type;
	typedef Distance           Difference_Type;
	typedef T*                 Pointer;
	typedef T&                 Reference;
};

template <class T, class Distance>
struct Output_Iterator 
{
	typedef Output_Iterator_Tag Iterator_Category;
	typedef void                Value_Type;
	typedef void                Difference_Type;
	typedef void                Pointer;
	typedef void                Reference;
};

template <class T, class Distance> 
struct Forward_Iterator 
{
	typedef Forward_Iterator_Tag Iterator_Category;
	typedef T                    Value_Type;
	typedef Distance             Difference_Type;
	typedef T*                   Pointer;
	typedef T&                   Reference;
};


template <class T, class Distance> 
struct Bidirectional_Iterator 
{
	typedef Bidirectional_Iterator_Tag Iterator_Category;
	typedef T                          Value_Type;
	typedef Distance                   Difference_Type;
	typedef T*                         Pointer;
	typedef T&                         Reference;
};

template <class T, class Distance> 
struct Random_Access_Iterator 
{
	typedef Random_Access_Iterator_Tag Iterator_Category;
	typedef T                          Value_Type;
	typedef Distance                   Difference_Type;
	typedef T*                         Pointer;
	typedef T&                         Reference;
};

template <class Iterator>
struct Iterator_Traits 
{
	typedef typename Iterator::Iterator_Category Iterator_Category;
	typedef typename Iterator::Value_Type        Value_Type;
	typedef typename Iterator::Difference_Type   Difference_Type;
	typedef typename Iterator::Pointer           Pointer;
	typedef typename Iterator::Reference         Reference;
};

template <class T>
struct Iterator_Traits<T*> 
{
	typedef Random_Access_Iterator_Tag Iterator_Category;
	typedef T                          Value_Type;
	typedef ptrdiff_t                  Difference_Type;
	typedef T*                         Pointer;
	typedef T&                         Reference;
};

template <class T>
struct Iterator_Traits<const T*> 
{
	typedef Random_Access_Iterator_Tag Iterator_Category;
	typedef T                          Value_Type;
	typedef ptrdiff_t                  Difference_Type;
	typedef const T*                   Pointer;
	typedef const T&                   Reference;
};

template <class RandomAccessIterator>
inline Iterator_Traits<RandomAccessIterator>::Difference_Type
_Distance(RandomAccessIterator first, RandomAccessIterator last, Random_Access_Iterator_Tag)
{
	return last - first;
}

template <class ForwardIterator>
inline Iterator_Traits<ForwardIterator>::Difference_Type
_Distance(ForwardIterator first, ForwardIterator last, Forward_Iterator_Tag)
{
	Iterator_Traits<InputIterator>::Difference_Type n = 0;
	while (first != last)
	{
		++first;
		++n;
	}
	return n;
}

template <class BidirectionalIterator>
inline Iterator_Traits<BidirectionalIterator>::Difference_Type
_Distance(BidirectionalIterator first, BidirectionalIterator last, Bidirectional_Iterator_Tag)
{
	Iterator_Traits<InputIterator>::Difference_Type n = 0;
	while (first != last)
	{
		++first;
		++n;
	}
	return n;
}


template <class InputIterator>
inline Iterator_Traits<InputIterator>::Difference_Type 
    Distance2(InputIterator first, InputIterator last) 
{
	typedef typename Iterator_Traits<InputIterator>::Iterator_Category category;
	return _Distance(first, last, category());
}


