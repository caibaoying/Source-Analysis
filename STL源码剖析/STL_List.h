#pragma once

template <class T>
struct ListNode
{
	ListNode<T>* Prev;
	ListNode<T>* Next;
	T data;

	ListNode(const T& data = T())
		:Prev(NULL)
		, Next(NULL)
		, data(data)
	{}
};

template <class T, class Ref, class Ptr>
struct ListIterator
{
	typedef ListIterator<T, T&, T*>             Iterator;
	typedef ListIterator<T, const T&, const T*> const_iterator;
	typedef ListIterator<T, Ref, Ptr>           Self;

	typedef T ValueType;
	typedef Ptr Pointer;
	typedef Ref Reference;
	typedef ptrdiff_t difference_type;


public:
	ListIterator(ListNode<T>* node)
		:Node(node)
	{}
	
	ListIterator()
	{}
	ListIterator(const Iterator& iter)
		:Node(iter.Node)
	{}

public:
	Self& operator ++ ()
	{
		Node = Node->Next;
		return *this;
	}

	Self operator ++ (int)
	{
		Iterator tmp = *this;
		++*this;
		return tmp;
	}

	Self& operator -- ()
	{
		Node = Node->Prev;
		return *this;
	}

	Self operator -- (int)
	{
		Iterator tmp = *this;
		--*this;
		return tmp;
	}

	Reference operator*()
	{
		return Node->data;
	}

	bool operator==(const Iterator& x)
	{
		return Node == x.Node;
	}

	bool operator!=(const Iterator& x)
	{
		return Node != x.Node;
	}

	Iterator operator -> ()
	{
		return &(operator*());
	}

public:
	ListNode<T>* Node;
};

template <class T>
class List
{
public:
	typedef ListNode<T>* LinkType;
	typedef ListNode<T> LinkNode;
	typedef T ValueType;


	typedef ListIterator<T, T&, T*> Iterator;
	typedef ListIterator<T, const T&, const T*> ConstIterator;
	
public:
	List(const T& x = T())
		:Node(new LinkNode(x))
	{
		Node->Next = Node;
		Node->Prev = Node;
	}

	List(const List<T>& x)
	{
		List<T> L;
		ConstIterator xbegin = x.Begin();
		ConstIterator xend = x.End();

		while (xbegin != xend)
		{
			L.PushBack(x.Node->data);
		}
	}

	List<T>& operator = (const List<T>& x)
	{
		if (this != &x)
		{
			Iterator begin = Begin();
			Iterator end = End();

			Iterator xbegin = x.Begin();
			Iterator xend = x.End();

			while (begin != end && xbegin != xend)
			{
				*begin++ = *xbegin++;
			}

			if (xbegin == xend)
			{
				while (begin != end)
				{
					Erase(begin++);
				}
			}
			else
			{
				while (xbegin != xend)
				{
					this->Insert(begin, xbegin.Node->data);
					xbegin++;
				}
			}
		}
		return *this;
	}

	Iterator Begin()
	{
		return Node->Next;
	}

	Iterator End()
	{
		return Node;
	}

	Iterator Begin()const
	{
		return Node->Next;
	}

	Iterator End()const
	{
		return Node;
	}

	bool empty()
	{
		return Node->Next == Node;
	}
	bool empty()const
	{
		return Node->Next == Node;
	}

	Iterator Insert(Iterator Pos, const ValueType& x)
	{
		LinkType tmp = new LinkNode(x);

		tmp->Next = Pos.Node;
		tmp->Prev = Pos.Node->Prev;
		Pos.Node->Prev->Next = tmp;
		Pos.Node->Prev = tmp;

		return tmp;
	}

	void PushBack(const ValueType& x)
	{
		Insert(End(), x);
	}

	void PushFront(const ValueType& x)
	{
		Insert(Begin(), x);
	}

	Iterator Erase(Iterator Pos)
	{
		LinkType tmp = Pos.Node;
		LinkType next = Pos.Node->Next;
		LinkType prev = Pos.Node->Prev;

		prev->Next = next;
		next->Prev = prev;

		delete tmp;
		return next;
	}

	void PopFront()
	{
		Erase(Begin());
	}

	void PopBack()
	{
		Erase(--End());
	}

	~List()
	{}

protected:
	LinkType Node;
};

void TestListIterator()
{
	List<int> l;
	l.PushBack(1);
	l.PushBack(2);
	l.PushBack(3);
	l.PushBack(4);
	l.PushBack(5);
	l.PushFront(0);

	List<int>::ConstIterator iter = l.Begin();
	while (iter != l.End())
	{
		cout << *iter << " ";
		++iter;
	}
	cout << endl;

	l.PushFront(6);
	iter = l.Begin();
	while (iter != l.End())
	{
		cout << *iter << " ";
		++iter;
	}
	cout << endl;

	l.PopBack();
	iter = l.Begin();
	while (iter != l.End())
	{
		cout << *iter << " ";
		++iter;
	}
	cout << endl;

	l.PopFront();
	iter = l.Begin();
	while (iter != l.End())
	{
		cout << *iter << " ";
		++iter;
	}
	cout << endl;


	cout << "operator = " << endl;
	List<int> L2;
	L2 = l;
	List<int>::ConstIterator iter2 = l.Begin();
	while (iter2 != l.End())
	{
		cout << *iter2 << " ";
		++iter2;
	}
	cout << endl;

	Distance2(l.Begin(), l.End());
}
