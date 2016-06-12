#pragma once
#include <malloc.h>

// SimpleAlloc统一封装的内存分配的接口
template <class T, class Alloc>
class SimpleAlloc
{
public:
	static T* Allocate(size_t n)
	{
		return n == 0 ? 0 : (T*) Alloc::Allocate(n*sizeof(T));
	}

	static T* Allocate(void)
	{
		return (T*)Alloc::Allocate(sizeof(T));
	}

	static void Deallocate(T* p, size_t n)
	{
		if (n != 0)
			Alloc::Deallocate(p, sizeof(T) * n);
	}

	static void Deallocate(T* p)
	{
		Alloc::Deallocate(p, sizeof(T));
	}
};


/************************************************************************
*                                Trace                                  *
*************************************************************************/
#define __DEBUG__
static string GetFileName(const string& path)
{
	char ch = '/';
#ifdef __Win32__

#endif // __Win32__

	/*********************************************************************
	Searches the string for the content specified in either str, s or c, 
	and returns the position of the last occurrence in the string.
	                          rfind使用简介
	When pos is specified, the search only includes characters between 
	the beginning of the string and position pos, ignoring any possible occurrences after pos.
	**********************************************************************/
	size_t pos = path.rfind(ch);
	if (pos == string::npos)
	{
		return path;
	}
	else
	{
		return path.substr(pos + 1);
	}
}

//用于调试追溯的trace log
inline static void __trace_debug(const char* function, const char* filename,
	                             int line, char* format, ...)
{
#ifdef __DEBUG__
	// 输出调用函数的信息
	fprintf(stdout, "【%s:%d】 %s", GetFileName(filename).c_str(), line, function);
	// 输出用户打的trace信息
	//<Step 1> 在调用参数表之前，定义一个 va_list 类型的变量，(假设va_list 类型变量被定义为ap)；
	//<Step 2> 然后应该对ap 进行初始化，让它指向可变参数表里面的第一个参数，这是通过 va_start 来
	//	实现的，第一个参数是 ap 本身，第二个参数是在变参表前面紧挨着的一个变量, 即“...”之前
	//	的那个参数；
	//<Step 3> 然后是获取参数，调用va_arg，它的第一个参数是ap，第二个参数是要获取的参数的指定类型，
	//	然后返回这个指定类型的值，并且把 ap 的位置指向变参表的下一个变量位置；
	//<Step 4> 获取所有的参数之后，我们有必要将这个 ap 指针关掉，以免发生危险，方法是调用 va_end，
	//  他是输入的参数 ap 置为 NULL，应该养成获取完参数表之后关闭指针的习惯。说白了，
	//	就是让我们的程序具有健壮性。通常va_start和va_end是成对出现。
	va_list args;
	va_start(args, format);
	//va_arg
	vfprintf(stdout, format, args);
	va_end(args);
#endif // __DEBGU__

}

#define __TRACE_DEBUG(...) \
	__trace_debug(__FUNCTION__, __FILE__, __LINE__, __VA_ARGS__);

//__USE_MALLOC__
#ifdef __USE_MALLOC__
typedef __MallocAllocTemplate<0> MallocAlloc;
typedef MallocAlloc Alloc;

#else


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
		__TRACE_DEBUG("(n:%u)\n", n);
		void *result = malloc(n);
		if (0 == result) 
			result = OomMalloc(n);
		return result;
	}

	static void Deallocate(void *p, size_t /* n */)
	{
		__TRACE_DEBUG("(p:%p)\n", p);
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

//初始化静态成员变量
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
	__TRACE_DEBUG("(n:%u)\n", n);

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
	__TRACE_DEBUG("(size:%u, nobjs:%d)\n", size, nobjs);

	char* result;
	size_t TotalSize = size*nobjs;
	size_t LeftSize = _EndFree - _StartFree;

	if (LeftSize >= TotalSize)
	{
		__TRACE_DEBUG("内存池中足够分配%d个对象\n", nobjs);

		result = _StartFree;
		_StartFree += TotalSize;
	}
	else if (LeftSize >= size)
	{
		__TRACE_DEBUG("内存池中不够分配%d个对象，只能分配%d个对象\n", nobjs, LeftSize/size);
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

			__TRACE_DEBUG("内存池不足以分配1个对象，将剩余的空间分配给_FreeList[%d]", index);
		}

		_StartFree = (char *)malloc(BytesToGet);
		__TRACE_DEBUG("内存池空间不够，系统堆分配%u内存", BytesToGet);

		if (_StartFree == NULL)
		{
			__TRACE_DEBUG("系统堆分配不出空间，在自由链表中查看");

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
			__TRACE_DEBUG("系统堆和自由链表都已无内存， 一级空间配置器做最后一根稻草\n");
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

typedef __DefaultAllocTemplate<false, 0> Alloc;
#endif // __USE_MALLOC__

void Test()
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

// 测试内存池的一级、二级配置器功能
void Test1()
{
	// 测试调用一级配置器分配内存
	cout << "测试调用一级配置器分配内存" << endl;
	char*p1 = SimpleAlloc<char, Alloc>::Allocate(129);
	SimpleAlloc<char, Alloc>::Deallocate(p1, 129);

	// 测试调用二级配置器分配内存
	cout << "测试调用二级配置器分配内存" << endl;
	char*p2 = SimpleAlloc<char, Alloc>::Allocate(128);
	char*p3 = SimpleAlloc<char, Alloc>::Allocate(128);
	char*p4 = SimpleAlloc<char, Alloc>::Allocate(128);
	char*p5 = SimpleAlloc<char, Alloc>::Allocate(128);
	SimpleAlloc<char, Alloc>::Deallocate(p2, 128);
	SimpleAlloc<char, Alloc>::Deallocate(p3, 128);
	SimpleAlloc<char, Alloc>::Deallocate(p4, 128);
	SimpleAlloc<char, Alloc>::Deallocate(p5, 128);

	for (int i = 0; i < 21; ++i)
	{
		printf("测试第%d次分配\n", i + 1);
		char*p = SimpleAlloc<char, Alloc>::Allocate(128);
	}
}
