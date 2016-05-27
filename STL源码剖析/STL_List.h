#pragma once

template <class T>
struct ListNode
{
	typedef ListNode<T> Iterator;

	ListNode<T>* Prev;
	ListNode<T>* Next;

	T data;

	ListNode(const T& data)
		:Prev(NULL)
		, Next(NULL)
		, data(data)
	{}

	Iterator& operator ++ ()
	{
		Node = Node->Next;
		return *this;
	}

	Iterator operator ++ (int)
	{
		Iterator tmp = *this;
		++*this;
		return tmp;
	}
	Iterator& operator -- ()
	{
		Node = Node->Prev;
		return *this;
	}

	Iterator operator -- (int)
	{
		Iterator tmp = *this;
		--*this;
		return tmp;
	}

	T& operator*()
	{
		return Node->data;
	}

	bool operator==(const Iterator& x)const
	{
		return Node == x->Node;
	}

	bool operator!=(const Iterator& x)const
	{
		return Node != x->Node;
	}

	Iterator operator -> ()
	{
		return &(operator*());
	}

	ListNode<T>* Node;
};

template <class T>
class List
{
public:
	typedef ListNode<T> Iterator;
	typedef ListNode<T>* LinkType;
public:
	List()
	{}

	Iterator Begin()
	{
		return Node->Next;
	}

	Iterator End()
	{
		return Node;
	}

	bool empty()
	{
		return Node->Next == Node;
	}

	Iterator Insert(Iterator Pos, const T& x)
	{
		LinkType tmp = new T(x);
		tmp->Next = Pos.Node;
		tmp->Prev = Pos.Node->Prev;
		Pos.Node->Prev->Next = tmp;
		Pos.Node->Prev = tmp;

		return tmp;
	}

	~List()
	{}

protected:
	LinkType Node;
};