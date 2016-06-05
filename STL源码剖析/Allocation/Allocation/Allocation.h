#pragma once
#include <malloc.h>


/************************************************************************
*                          一级空间配置器                               *
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
    
	//如果分配成功，直接返回
	//如果不成功，1.调用handler处理后再分配
	//2、没有handler，直接退出

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
*                          二级空间配置器                               *
/************************************************************************/

template <bool threads, int inst>
class __DefaultAllocTemplate
{
private:
	// Really we should use static const int x = N
	// instead of enum { x = N }, but few compilers accept the former.
	enum { __ALIGN = 8 };
	enum { __MAX_BYTES = 128 };
	enum { __NFREELISTS = __MAX_BYTES / __ALIGN };         //分为多少块

	//计算出所要申请的空间有多大，很巧妙的算法
	static size_t RoundUp(size_t bytes)
	{
		//bytes = 7
		//（7+7）& ~（7）= 0000 1110 & ~0000 0111 = 0000 1110 & 1111 1000 = 0000 1000 = 8
		return (((bytes)+__ALIGN - 1) & ~(__ALIGN - 1));
	}

	//计算出所要申请的空间在哪一块上
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
	static obj * volatile _FreeList[__NFREELISTS];    //自由链表
	static char* _StartFree;                          //内存池水位线的开始
	static char* _EndFree;                            //内存池水位线的结束
	static size_t _HeapSize;                          //从系统堆分配的总大小

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

//初始化静态 
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
	//分配n bytes 的字节
	//如果不够能分配多少就分配多少
	int nobjs = 20;
	char* chunk = ChunkAlloc(n, nobjs);

	if (nobjs == 1)
	{
		return chunk;
	}

	obj* result, *cur;
	size_t index = FreelistIndex(n);
	result = (obj*)chunk;

	//把剩余的快链上
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
		//预分配思想
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

					//再次调用chunk_alloc
					return (ChunkAlloc(size, nobjs));
				}

			}

			//最后一道防线_HeapSize
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
	// 测试调用一级配置器分配内存
	cout << " 测试调用一级配置器分配内存 " << endl;
	char*p1 = (char*)__MallocAllocTemplate<0>::Allocate(129);
	__MallocAllocTemplate<0>::Deallocate(p1, 129);
	// 测试调用二级配置器分配内存
	cout << " 测试调用二级配置器分配内存 " << endl;
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
		printf(" 测试第%d次分配 \n", i + 1);
		char*p = (char*)__DefaultAllocTemplate<0, 0>::Allocate(128);
	}
}

