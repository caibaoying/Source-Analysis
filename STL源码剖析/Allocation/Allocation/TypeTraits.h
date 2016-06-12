#pragma once

struct __TrueType {};
struct __FalseType {};

template <class Type>
struct __Type_Traits 
{
	typedef __TrueType     ThisDummyMemberMustBeFirst;
	typedef __FalseType    HasTrivialDefaultConstructor;
	typedef __FalseType    HasTrivialCopyConstructor;
	typedef __FalseType    HasTrivialAssignmentOperator;
	typedef __FalseType    HasTrivialDestructor;
	typedef __FalseType    IsPODType;
};

template <> 
struct __Type_Traits<char> {
	typedef __TrueType    HasTrivialDefaultConstructor;
	typedef __TrueType    HasTrivialCopyConstructor;
	typedef __TrueType    HasTrivialAssignmentOperator;
	typedef __TrueType    HasTrivialDestructor;
	typedef __TrueType    IsPODType;
};

template <> 
struct __Type_Traits<signed char> {
	typedef __TrueType    HasTrivialDefaultConstructor;
	typedef __TrueType    HasTrivialCopyConstructor;
	typedef __TrueType    HasTrivialAssignmentOperator;
	typedef __TrueType    HasTrivialDestructor;
	typedef __TrueType    IsPODType;
};

template <> struct __Type_Traits<unsigned char> {
	typedef __TrueType    HasTrivialDefaultConstructor;
	typedef __TrueType    HasTrivialCopyConstructor;
	typedef __TrueType    HasTrivialAssignmentOperator;
	typedef __TrueType    HasTrivialDestructor;
	typedef __TrueType    IsPODType;
};

template <> 
struct __Type_Traits<short> {
	typedef __TrueType    HasTrivialDefaultConstructor;
	typedef __TrueType    HasTrivialCopyConstructor;
	typedef __TrueType    HasTrivialAssignmentOperator;
	typedef __TrueType    HasTrivialDestructor;
	typedef __TrueType    IsPODType;
};

template <> 
struct __Type_Traits<unsigned short> {
	typedef __TrueType    HasTrivialDefaultConstructor;
	typedef __TrueType    HasTrivialCopyConstructor;
	typedef __TrueType    HasTrivialAssignmentOperator;
	typedef __TrueType    HasTrivialDestructor;
	typedef __TrueType    IsPODType;
};

template <> 
struct __Type_Traits<int> {
	typedef __TrueType    HasTrivialDefaultConstructor;
	typedef __TrueType    HasTrivialCopyConstructor;
	typedef __TrueType    HasTrivialAssignmentOperator;
	typedef __TrueType    HasTrivialDestructor;
	typedef __TrueType    IsPODType;
};

template <> 
struct __Type_Traits<unsigned int> {
	typedef __TrueType    HasTrivialDefaultConstructor;
	typedef __TrueType    HasTrivialCopyConstructor;
	typedef __TrueType    HasTrivialAssignmentOperator;
	typedef __TrueType    HasTrivialDestructor;
	typedef __TrueType    IsPODType;
};

template <> 
struct __Type_Traits<long> {
	typedef __TrueType    HasTrivialDefaultConstructor;
	typedef __TrueType    HasTrivialCopyConstructor;
	typedef __TrueType    HasTrivialAssignmentOperator;
	typedef __TrueType    HasTrivialDestructor;
	typedef __TrueType    IsPODType;
};

template <>
struct __Type_Traits<unsigned long> {
	typedef __TrueType    HasTrivialDefaultConstructor;
	typedef __TrueType    HasTrivialCopyConstructor;
	typedef __TrueType    HasTrivialAssignmentOperator;
	typedef __TrueType    HasTrivialDestructor;
	typedef __TrueType    IsPODType;
};

template <> 
struct __Type_Traits<float> {
	typedef __TrueType    HasTrivialDefaultConstructor;
	typedef __TrueType    HasTrivialCopyConstructor;
	typedef __TrueType    HasTrivialAssignmentOperator;
	typedef __TrueType    HasTrivialDestructor;
	typedef __TrueType    IsPODType;
};

template <>
struct __Type_Traits<double> {
	typedef __TrueType    HasTrivialDefaultConstructor;
	typedef __TrueType    HasTrivialCopyConstructor;
	typedef __TrueType    HasTrivialAssignmentOperator;
	typedef __TrueType    HasTrivialDestructor;
	typedef __TrueType    IsPODType;
};

template <>
struct __Type_Traits<long double> {
	typedef __TrueType    HasTrivialDefaultConstructor;
	typedef __TrueType    HasTrivialCopyConstructor;
	typedef __TrueType    HasTrivialAssignmentOperator;
	typedef __TrueType    HasTrivialDestructor;
	typedef __TrueType    IsPODType;
};

template <class T>
struct __Type_Traits<T*> {
	typedef __TrueType    HasTrivialDefaultConstructor;
	typedef __TrueType    HasTrivialCopyConstructor;
	typedef __TrueType    HasTrivialAssignmentOperator;
	typedef __TrueType    HasTrivialDestructor;
	typedef __TrueType    IsPODType;
};

template <>
struct __Type_Traits<char*> {
	typedef __TrueType    HasTrivialDefaultConstructor;
	typedef __TrueType    HasTrivialCopyConstructor;
	typedef __TrueType    HasTrivialAssignmentOperator;
	typedef __TrueType    HasTrivialDestructor;
	typedef __TrueType    IsPODType;
};

template <>
struct __Type_Traits<signed char*> {
	typedef __TrueType    HasTrivialDefaultConstructor;
	typedef __TrueType    HasTrivialCopyConstructor;
	typedef __TrueType    HasTrivialAssignmentOperator;
	typedef __TrueType    HasTrivialDestructor;
	typedef __TrueType    IsPODType;
};

template <>
struct __Type_Traits<unsigned char*> {
	typedef __TrueType    HasTrivialDefaultConstructor;
	typedef __TrueType    HasTrivialCopyConstructor;
	typedef __TrueType    HasTrivialAssignmentOperator;
	typedef __TrueType    HasTrivialDestructor;
	typedef __TrueType    IsPODType;
};
