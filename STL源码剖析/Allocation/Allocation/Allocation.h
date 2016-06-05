#pragma once
#include <malloc.h>


/************************************************************************
*                          һ���ռ�������                               *
/************************************************************************/

template <int inst>
class __MallocAllocTemplate 
{
private:
	static void* OomMalloc(size_t);
	static void* OomRealloc(void *, size_t);
	static void(*__MallocAllocOomHandler)();

public:
	static void* Allocate(size_t n)
	{
		void *result = malloc(n);
		if (0 == result) 
			result = OomMalloc(n);
		return result;
	}

	static void Deallocate(void *p, size_t /* n */)
	{
		free(p);
	}

	static void* Reallocate(void *p, size_t /* old_sz */, size_t new_sz)
	{
		void* result = realloc(p, new_sz);
		if (0 == result) result = OomRealloc(p, new_sz);
		return result;
	}

	static void (*SetMallocHandler(void(*f)()))()
	{
		void(*old)() = __MallocAllocOomHandler;
		__MallocAllocOomHandler = f;
		return(old);
	}
};

template <int inst>
void* __MallocAllocTemplate<inst>::OomMalloc(size_t n)
{
	void(*MyMallocHandler)();
	void *result;
    
	//�������ɹ���ֱ�ӷ���
	//������ɹ���1.����handler������ٷ���
	//2��û��handler��ֱ���˳�

	for (;;) 
	{
		MyMallocHandler = __MallocAllocOomHandler;
		if (0 == MyMallocHandler)
		{ 
			cout << "out of memory" << endl; 
			exit(1);
		}
		(*MyMallocHandler)();

		result = malloc(n);
		if (result) 
			return(result);
	}
}

template <int inst>
void* __MallocAllocTemplate<inst>::OomRealloc(void *p, size_t n)
{
	void(*MyMallocHandler)();
	void *result;

	for (;;) 
	{
		MyMallocHandler = __MallocAllocOomHandler;
		if (0 == MyMallocHandler) 
		{ 
			cout << "out of memory" << endl;
			exit(1);
		}
		(*MyMallocHandler)();

		result = realloc(p, n);
		if (result) 
			return(result);
	}
}

template <int inst>
void(*__MallocAllocTemplate<inst>::__MallocAllocOomHandler)() = 0;


/************************************************************************
*                          �����ռ�������                               *
/************************************************************************/

template <bool threads, int inst>
class __DefaultAllocTemplate
{
private:
	// Really we should use static const int x = N
	// instead of enum { x = N }, but few compilers accept the former.
	enum { __ALIGN = 8 };
	enum { __MAX_BYTES = 128 };
	enum { __NFREELISTS = __MAX_BYTES / __ALIGN };         //��Ϊ���ٿ�

	//�������Ҫ����Ŀռ��ж�󣬺�������㷨
	static size_t RoundUp(size_t bytes)
	{
		//bytes = 7
		//��7+7��& ~��7��= 0000 1110 & ~0000 0111 = 0000 1110 & 1111 1000 = 0000 1000 = 8
		return (((bytes)+__ALIGN - 1) & ~(__ALIGN - 1));
	}

	//�������Ҫ����Ŀռ�����һ����
	static  size_t FreelistIndex(size_t bytes) 
	{
		//[0, 15]
		//bytes = 7
		//(7+7)/8 - 1= 0;
		//(9+7)/8 - 1 = 1;
		return (((bytes)+__ALIGN - 1) / __ALIGN - 1);
	}

public:
	union obj 
	{
		union obj* _FreeListLink;
		char ClientData[1];    /* The client sees this.*/
	};

public:
	static obj * volatile _FreeList[__NFREELISTS];    //��������
	static char* _StartFree;                          //�ڴ��ˮλ�ߵĿ�ʼ
	static char* _EndFree;                            //�ڴ��ˮλ�ߵĽ���
	static size_t _HeapSize;                          //��ϵͳ�ѷ�����ܴ�С

public:

	static char* ChunkAlloc(size_t size, int &nobjs);

	/* Returns an object of size n, and optionally adds to size n FreeList.*/
	/* We assume that n is properly aligned.                               */
	/* We hold the allocation lock.                                        */
	static void* Refill(size_t n);
	static void* Allocate(size_t n);
	static void  Deallocate(void *p, size_t n);
	static void* Reallocate(void *p, size_t old_sz, size_t new_sz);
};

//��ʼ����̬ 
template <bool threads, int inst>
typename __DefaultAllocTemplate<threads, inst>::obj* volatile __DefaultAllocTemplate<threads, inst>::
    _FreeList[__DefaultAllocTemplate<threads, inst>::__NFREELISTS];

template <bool threads, int inst>
char* __DefaultAllocTemplate<threads, inst>::_StartFree = 0;

template <bool threads, int inst>
char* __DefaultAllocTemplate<threads, inst>::_EndFree = 0;

template <bool threads, int inst>
size_t __DefaultAllocTemplate<threads, inst>::_HeapSize = 0;

template <bool threads, int inst>
void* __DefaultAllocTemplate<threads, inst>::Refill(size_t n)
{
	//����n bytes ���ֽ�
	//��������ܷ�����پͷ������
	int nobjs = 20;
	char* chunk = ChunkAlloc(n, nobjs);

	if (nobjs == 1)
	{
		return chunk;
	}

	obj* result, *cur;
	size_t index = FreelistIndex(n);
	result = (obj*)chunk;

	//��ʣ��Ŀ�����
	cur = (obj*)(chunk + n);
	_FreeList[index] = cur;
	for (size_t i = 2; i < nobjs; ++i)
	{
		cur->_FreeListLink = (obj*)(chunk + n*i);
		cur = cur->_FreeListLink;
	}
	cur->_FreeListLink = NULL;

	return result;
}


template<bool threads, int inst>
char* __DefaultAllocTemplate<threads, inst>::ChunkAlloc(size_t size, int& nobjs)
{
	char* result;
	size_t TotalSize = size*nobjs;
	size_t LeftSize = _EndFree - _StartFree;

	if (LeftSize >= TotalSize)
	{
		result = _StartFree;
		_StartFree += TotalSize;
	}
	else if (LeftSize >= size)
	{
		nobjs = LeftSize / size;
		TotalSize = size*nobjs;
		result = _StartFree;
		_StartFree += TotalSize;
	}
	else
	{
		//Ԥ����˼��
		size_t BytesToGet = 2 * TotalSize + RoundUp(_HeapSize >> 4);

		if (LeftSize > 0)
		{
			size_t index = FreelistIndex(LeftSize);
			((obj*)_StartFree)->_FreeListLink = _FreeList[index];
			_FreeList[index] = (obj*)_StartFree;
			_StartFree = NULL;
		}

		_StartFree = (char *)malloc(BytesToGet);
		if (_StartFree == NULL)
		{
			for (int i = size; i <= __MAX_BYTES; i += __ALIGN)
			{
				size_t index = FreelistIndex(i);
				obj* head = _FreeList[index];

				if (head)
				{
					_StartFree = (char*)head;
					head = head->_FreeListLink;
					_EndFree = _StartFree + i;

					//�ٴε���chunk_alloc
					return (ChunkAlloc(size, nobjs));
				}

			}

			//���һ������_HeapSize
			_StartFree = (char*)__MallocAllocTemplate<0>::Allocate(BytesToGet);
		}

		_HeapSize += BytesToGet;
		_EndFree = _StartFree + BytesToGet;
		return ChunkAlloc(size, nobjs);
	}

	return result;
}

template <bool threads, int inst>
void* __DefaultAllocTemplate<threads, inst>::Allocate(size_t n)
{
	if (n > __MAX_BYTES)
	{
		return __MallocAllocTemplate<0>::Allocate(n);
	}

	size_t index = FreelistIndex(n);

	obj* head = _FreeList[index];
	if (head == NULL)
	{
		return Refill(RoundUp(n));
	}
	else
	{
		_FreeList[index] = head->_FreeListLink;

		return head;
	}
}

template <bool threads, int inst>
void __DefaultAllocTemplate<threads, inst>::Deallocate(void* p, size_t n)
{
	obj* q = (obj*)p;

	if (n > __MAX_BYTES)
	{
		__MallocAllocTemplate<0>::Deallocate(p, n);
		return;
	}

	size_t index = FreelistIndex(n);
	obj* head = _FreeList[index];

	q->_FreeListLink = head;
	head = q;
}

template <bool threads, int inst>
void* __DefaultAllocTemplate<threads, inst>::Reallocate(void *p, size_t oldsz, size_t newsz)
{
	if (oldsz > (size_t)__MAX_BYTES && newsz > (size_t)__MAX_BYTES)
		return(realloc(p, newsz));

	if (RoundUp(oldsz) == RoundUp(newsz))
		return(p);

	void* result = Allocate(newsz);
	size_t copysz = newsz > oldsz ? oldsz : newsz;
	memcpy(result, p, copysz);
	deallocate(p, oldsz);
	return result;
}

void Test1()
{
	// ���Ե���һ�������������ڴ�
	cout << " ���Ե���һ�������������ڴ� " << endl;
	char*p1 = (char*)__MallocAllocTemplate<0>::Allocate(129);
	__MallocAllocTemplate<0>::Deallocate(p1, 129);
	// ���Ե��ö��������������ڴ�
	cout << " ���Ե��ö��������������ڴ� " << endl;
	char*p2 = (char*)__DefaultAllocTemplate<0,0>::Allocate(128);
	char*p3 = (char*)__DefaultAllocTemplate<0, 0>::Allocate(128);
	char*p4 = (char*)__DefaultAllocTemplate<0, 0>::Allocate(128);
	char*p5 = (char*)__DefaultAllocTemplate<0, 0>::Allocate(128);
	__DefaultAllocTemplate<0, 0>::Deallocate(p2, 128);
	__DefaultAllocTemplate<0, 0>::Deallocate(p3, 128);
	__DefaultAllocTemplate<0, 0>::Deallocate(p4, 128);
	__DefaultAllocTemplate<0, 0>::Deallocate(p5, 128);
	for (int i = 0; i < 21; ++i)
	{
		printf(" ���Ե�%d�η��� \n", i + 1);
		char*p = (char*)__DefaultAllocTemplate<0, 0>::Allocate(128);
	}
}

