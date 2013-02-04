/*
 * Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011
 * Yokogawa Electric Corporation, YDC Corporation,
 * IPA (Information-technology Promotion Agency, Japan).
 * All rights reserved.
 * 
 * Redistribution and use of this software in source and binary forms, with 
 * or without modification, are permitted provided that the following 
 * conditions and disclaimer are agreed and accepted by the user:
 * 
 * 1. Redistributions of source code must retain the above copyright 
 * notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the 
 * documentation and/or other materials provided with the distribution.
 * 
 * 3. Neither the names of the copyrighters, the name of the project which 
 * is related to this software (hereinafter referred to as "project") nor 
 * the names of the contributors may be used to endorse or promote products 
 * derived from this software without specific prior written permission.
 * 
 * 4. No merchantable use may be permitted without prior written 
 * notification to the copyrighters. However, using this software for the 
 * purpose of testing or evaluating any products including merchantable 
 * products may be permitted without any notification to the copyrighters.
 * 
 * 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHTERS, THE PROJECT AND 
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING 
 * BUT NOT LIMITED THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.  IN NO EVENT SHALL THE 
 * COPYRIGHTERS, THE PROJECT OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT,STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $TAHI: v6eval/lib/Cm/CmCltn.h,v 1.11 2001/10/12 04:56:13 tanaka Exp $
 */
#ifndef _Cm_CmCltn_h_
#define _Cm_CmCltn_h_ 1
#include "CmTypes.h"
#include <stdarg.h>
#define BasicType(T) name2(Bt,T)
#define	DCLA(TYPE,name)	TYPE name=(TYPE)va_arg(ap,TYPE);

//======================================================================
// Common interface for List and Set classes
//----------------------------------------------------------------------
#define interfaceBtCltn(type,Cltn,T)\
	Cltn();\
	Cltn(u_long);\
	Cltn(T*,...);\
virtual	~Cltn();\
	void elementsPerform(name2(T,Func)) const;\
	void elementsPerformWith(name2(T,Func),void*,...) const;\
	Cltn& operator=(const Cltn& l){\
		return (Cltn&)BasicType(type)::operator=(l);}\
	void deleteElement(ELEMENTS v);

#define interfaceCmCltn(type,Cltn,T)\
interfaceBtCltn(type,Cltn,T)\
	T* findMatching(const void* key,name2(T,EqFunc) func=0) const;\
	T* reverseMatching(const void* key,name2(T,EqFunc) func) const;

//======================================================================
// Common implementation for List and Set classes
//----------------------------------------------------------------------
#define implementBtCltn(type,Cltn,T)\
\
Cltn::Cltn():BasicType(type)() {}\
Cltn::Cltn(u_long n):BasicType(type)(n) {}\
Cltn::Cltn(T *elm,...):BasicType(type)() {addElementsFrom(elm);}\
Cltn::~Cltn() {}\
void Cltn::elementsPerform(name2(T,Func) func) const {\
	u_long i, i9=size();\
	for(i=0;i<i9;i++) {\
		T* p=(T*)at(i);\
		if(p!=0) (p->*func)(0);}}\
void Cltn::elementsPerformWith(name2(T,Func) func,void* v,...) const {\
	u_long i, i9=size();\
	va_list ap;\
	va_start(ap,v);\
	for(i=0;i<i9;i++) {\
		T* p=(T*)at(i);\
		if(p!=0) (p->*func)(v,ap);}\
	va_end(ap);}\
void Cltn::deleteElement(ELEMENTS v) {\
	T* o=(T*)v;\
	delete o;}

#define implementCmCltn(type,Cltn,T)\
implementBtCltn(type,Cltn,T)\
\
T* Cltn::findMatching(const void* key,name2(T,EqFunc) func) const {\
	u_long i, i9=size();\
	if(func==0) func=(name2(T,EqFunc))&T::isEqual;\
	for(i=0;i<i9;i++) {\
		T* p=(T*)at(i);\
		if(p!=0 && (p->*func)(key)) {return p;}}\
	return 0;}\
\
T* Cltn::reverseMatching(const void* key,name2(T,EqFunc) func) const {\
	u_long i, i9=size();\
	if(func==0) func=(name2(T,EqFunc))&T::isEqual;\
	for(i=i9;i>0;i--) {\
		T* p=(T*)at(i-1);\
		if(p!=0 && (p->*func)(key)) {return p;}}\
	return 0;}


#include "BtList.h"
//======================================================================
// List interface 
//----------------------------------------------------------------------
#define interfaceCmList(Cltn,T)\
typedef void (T::*name2(T,Func))(void*,...) const;\
typedef bool (T::*name2(T,EqFunc))(const void*) const;\
typedef int (T::*name2(T,CmpFunc))(const T*) const;\
class Cltn:public BtList {\
public:\
interfaceCmCltn(List,Cltn,T)\
\
inline	T* index(u_long i) const {\
		return (T*)BtList::index(i);}\
inline	T* operator[](u_long i) const {\
		return index(i);}\
	void sort(name2(T,CmpFunc) func);\
private:\
static	name2(T,CmpFunc) compareFunction;\
static	int staticCompare(const void* a,const void* b);\
};

//======================================================================
// List implementation
//----------------------------------------------------------------------
#define implementCmList(Cltn,T)\
implementCmCltn(List,Cltn,T)\
\
int Cltn::staticCompare(const void* a,const void* b) {\
	const T* p=(const T*)*(const T**)a;\
	const T* q=(const T*)*(const T**)b;\
	return (p->*compareFunction)(q);}\
\
void Cltn::sort(name2(T,CmpFunc) func) {\
	u_long c=noOfElements();\
	compareFunction=func;\
	qsort(array(0,c),(int)c,sizeof(T*),Cltn::staticCompare);}\
\
name2(T,CmpFunc) Cltn::compareFunction;

#include "BtSet.h"
//======================================================================
// Set interface 
//----------------------------------------------------------------------
#define interfaceCmSet(Cltn,T)\
typedef void (T::*name2(T,Func))(void*,...) const;\
typedef bool (T::*name2(T,EqFunc))(const void*) const;\
class Cltn:public BtSet {\
public:\
interfaceCmCltn(Set,Cltn,T)\
\
	T* find(T* o) const;\
virtual uint32_t hashElement(const ELEMENTS o) const;\
virtual bool isSame(const ELEMENTS c,const ELEMENTS o) const;\
};

//======================================================================
// Set implementation
//----------------------------------------------------------------------
#define implementCmSet(Cltn,T)\
implementCmCltn(Set,Cltn,T)\
\
T* Cltn::find(T* o) const {return (T*)BtSet::find(o);}\
\
uint32_t Cltn::hashElement(const ELEMENTS o) const {\
	const T* p=(const T*)o;\
	return p->hash();}\
bool Cltn::isSame(const ELEMENTS a,const ELEMENTS b) const {\
	const T* p=(const T*)a;\
	const T* q=(const T*)b;\
	return p->isEqual(q);}
#if 0
//----------------------------------------------------------------------
// DESCRIPTION

1. 共通部
interfaceBtCltn(type,Cltn,T)
implementBtCltn(type,Cltn,T)
interfaceCmCltn(type,Cltn,T)
implementCmCltn(type,Cltn,T)

オブジェクトの認識を持った集り(Collection)をサポートするための共通部。
順序性を持った配列Listまたは集合Setの共通部を定義する。
interfaceXXXは、集りクラスのインターフェース部を定義するために使用し
implementXXXは、その実装部を定義する。

1.1 Collectionとしてのメンバ関数
Bt(基本タイプ)についは要素に対し何の実装も要求しない。

typedef void (Elm::*ElmFunc)(void*,...);
void elementsPerform(ElmFunc,...);
	集りクラスに登録された全要素への関数呼出し。

使用例:
class Elm {
	void func();
	void withArgs(va_list);
};
	
	Cltn* l;
	l->elementsPerform(&Elm::func);			// 単純な呼出し
	l->elementsPerformWith(&Elm::withArgs,i,j,k);	// 引数付呼出し

void Elm::withArgs(void* s,va_list ap) {
	第１引数の型 i=(第１引数の型)s;	// 呼出し側でのiが見える
	DCLA(第２引数の型,j);		// 呼出し側でのjが見える
	DCLA(第３引数の型,k);		// 呼出し側でのkが見える
	...
}

Cm(共通タイプ)については要素を見つけるための機能をサポートし、
要素側の実装として以下の関数の実装を義務づけている。

typedef bool (Elm::*ElmEqFunc)(const void*) const;
bool isEqual(const Elm* e) const {
	return (thisとeが同じであるかを返却する);}

1.2 共通タイプとしてのメンバ関数
Elm* findMatching(const void* key,ElmEqFunc func=0) const;
	集りクラスに登録されたメンバの探索。
	funcが省略された場合、isEqaulを使用する。

使用例:

class Elm {
	STR string_;
	bool isSameString(const void* v) const {
		CSTR s=(CSTR)v;
		return strcmp(string_,s)==0;}
};
	
	Cltn* l;
	l->findMatching("文字列",&Elm::isSameString);

2. 順序性を持った配列(List)
interfaceCmList(Cltn,T)
implementCmList(Cltn,T)

ListはBtListのサブクラスとして実装され、オブジェクトとしての認識を持って
各要素に対するアクセスを可能とする。

2.1 Listのメンバ関数

(1) 要素の取出し
	Elm* index(u_long) const;
	Elm* operator[](u_long i) const;

(2) 並べ替え
	typedef int (Elm::*ElmCmpFunc)(const Elm*) const;
	void sort(ElmCmpFunc func=0);

3. 集合(Set)
interfaceCmSet(Cltn,T)
implementCmSet(Cltn,T)

SetはBtSetのサブクラスとして実装され、オブジェクトとしての認識を持って
各要素に対するアクセスを可能とする。

集合はhash値によって分散して登録するため、以下の関数を要素側に実装する
事を義務づけている。
uint32_t hash() const;
bool isEqual(const Elm* v) const;

3.1 Setのメンバ関数
(1) 要素の取出し
	Elm* find(Elm* o);
	登録された全要素のうち与えたオブジェクトとhash値が同じで、
	isEqaulが真となるオブジェクト返却する。
#endif
// IMPLEMENTATION
// "@(#) YDC 98/05/30 19:59:10 CmCltn.CC:7.3:\t(CDB:000)Listの統合"
#endif
