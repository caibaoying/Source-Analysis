#pragma once

template <class T>
class Vector
{
public:
	typedef T ValueType;
	typedef ValueType* Iterator;
	typedef const ValueType* ConstIterator;

public:
	Vector()
		:Start(NULL)
		, Finish(NULL)
		, End_Of_Storage(NULL)
	{}

	//原型vector(size_type n, const T& value) { fill_initialize(n, value); }
	//有待优化--------
	Vector(size_t n, const T& value)
	{
		Start = new T[n]();                              //YYYYYYYYYYY
		for (size_t i = 0; i < n; ++i)
		{
			Start[i] = value;
		}
		Finish = Start + n;
		End_Of_Storage = Finish;
	}

	Vector(const Vector<T>* V)
	{
		Destory(Start, End_Of_Storage, Finish);
		Iterator tmp = new T[V.Size()]();
		Start = tmp;
		int size = V.Size();
		while (size > 0)
		{
			tmp++ = V.Start++;
			size--;
		}

		Finish = Start + V.Size() + 1;
		End_Of_Storage = Start + V.Capacity() + 1;
	}

	T& operator [] (const size_t& index)
	{
		if (index < Size())
		    return *(Start + index);
		return *End_Of_Storage;
	}

	Vector<T>& operator = (const Vector<T>& V)          //const vector对应的Size也要为const
	{
		if (&V != this)
		{
			size_t size = V.Size();
			size_t capacity = V.Capacity();

			Iterator tmp = new T[size]();
			Destory(Start, End_Of_Storage, Finish);

			Start = tmp;
			
			for (size_t i = 0; i < size;++i)
			{
				tmp[i] = V.Start[i];
			}

			Finish = Start + size;
			End_Of_Storage = Start + capacity;
		}
		return *this;
	}

	//在pos处插入元素
	void Insert(Iterator Pos, const T& x)
	{
		int pos = Pos - Start - 1;
		CheckCapacity();
		Iterator tmp = Finish;
		
		for (int i = Finish - Start - 1; i >= pos; --i)
		{
			tmp[i + 1] = tmp[i];
		}
		*Pos = x;
		++Finish;
	}

	size_t Size()
	{
		return Finish - Start;
	}

	size_t Capacity()
	{
		return End_Of_Storage - Start;
	}

	size_t Size()const
	{
		return Finish - Start;
	}

	size_t Capacity()const
	{
		return End_Of_Storage - Start;
	}

	Iterator Begin()
	{
		return Start;
	}

	Iterator End()
	{
		return Finish;
	}

	Iterator RBegin()
	{
		return Finish;
	}

	Iterator REnd()
	{
		return Start;
	}

	Iterator Begin()const
	{
		return Start;
	}

	Iterator End()const
	{
		return Finish;
	}

	Iterator RBegin()const
	{
		return Finish;
	}

	Iterator REnd()const
	{
		return Start;
	}

	void Display()
	{
		Iterator tmp;
		for (tmp = Start; tmp != Finish; ++tmp)
		{
			cout << *tmp << " ";
		}
		cout << endl;
	}

	~Vector()
	{
		if (Start)
		    Destory(Start, End_Of_Storage, Finish);

		delete Start;
		delete Finish;
		delete End_Of_Storage;
	}

protected:
	void CheckCapacity()
	{
		if (Size() >= Capacity())
		{
			long NewSize = Size() * 2 + 3;
			size_t OldSize = Size();
			Iterator tmp = new T[NewSize];

			for (size_t i = 0; i < OldSize; ++i)
			{
				tmp[i] = Start[i];
			}
			Start = tmp;
			Finish = Start + OldSize;
			End_Of_Storage = Start + NewSize;
		}
	}

	void Destory(Iterator& First, Iterator& Last, Iterator& Finish)
	{
		delete[] First;

		First = NULL;
		Last = NULL;
		Finish = NULL;
	}
protected:
	Iterator Start;
	Iterator Finish;
	Iterator End_Of_Storage;
};

void TestVector()
{
	Vector<int> V(3, 4);
	V.Display();

	Vector<int>::ConstIterator it;
	for (it = V.Begin(); it != V.End(); ++it)
	{
		cout << *it << " ";
	}
	cout << endl;

	Vector<int> V1;
	V1 = V;
	V1.Display();

	it = V1.Begin();
	//V1.Insert(it,3);
	V1.Display();

	cout<<"DIstance" << Distance(V1.Begin(), V1.End()) << endl;
}