// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//
//  参考mfc和log4cxx实现的类似于JAVA的对象模型
//  感谢：侯捷 J.J.Hou (jjhou),  jjhou@jjhou.com,  www.jjhou.com
//  提供的：MFCLite 3.5，一个具体而微的 MFC-like application framework
//

/*
	MFC的对象模型只提供单根继承：
	如下

	class A : 
		public Object {
		DECLARE_DYNAMIC(A)
	public:
		virtual ~A() {}
	};

	IMPLEMENT_DYNAMIC(A, Object)

	class AImpl :
		public virtual A {
		DECLARE_DYNAMIC(AImpl)
	public:
		virtual ~AImpl() {}
	};

	IMPLEMENT_DYNAMIC(AImpl, A)

	class BA :
		public virtual A {
		DECLARE_DYNAMIC(BA)

	public:
		virtual ~BA() {}

	};

	IMPLEMENT_DYNAMIC(BA, A)

	class BAImpl :
		public AImpl,
		public BA {

		DECLARE_DYNCREATE(BAImpl)

	public:

	};

	IMPLEMENT_DYNAMIC(BAImpl, AImpl)

	像上面的对象模型执行下面的语句：
	std::cout << impl->IsKindOf(RUNTIME_CLASS(BA)) << std::endl; // 0
	std::cout << impl->IsKindOf(RUNTIME_CLASS(AImpl)) << std::endl; // 1

	故这样的对象模型不能满足我们的需求。需要修改。


	//是在MFC基础上改还是在log4cxx基础上修改？？？？
	//但log4cxx没有类的关系信息
	//可能还是使用log4cxx更优雅一些？？
	//	也不见得，MFC的实现也很不错的，MFC里CLASS之间的关系通过一个链表将他们之间的关系维护起来，
	//	只要使用树结构来维护链表，那么也能实现我们的需求了。我们可以多加入一些宏，那么
	//  实现getSuperClass,getInterfaces等都可以实现
	//  在此对象模型里我们可以定义一些宏来指定哪些是类似JAVA里的Interfaces....
 */

#ifndef BASE2_OBJECT_CORE_H_
#define BASE2_OBJECT_CORE_H_

#include <cstdio>    // for FILE*, NULL (0)
#include <string>    //5 用於檔名, IDR_
#include <map>

////////// 前置宣告（forward declaration）/////////////////////
////////// 類別階層體系（classes hierarchy）///////////////////
class Object;
struct Class;


////////// Class — MFCLite 基礎建設之核心 /////////////
////////////////////////////////////////////////////////////////
struct Class
{
	typedef Object* (* FuncNewObject)();

	//还是加个构造函数吧
	//限制最多为从9个类继承
	Class();
	Class(char* className, int objectSize, int schema, FuncNewObject fnNew, Class* baseClass);
	Class(char* className, int objectSize, int schema, FuncNewObject fnNew, Class* baseClass, Class* baseNextClass1);
	Class(char* className, int objectSize, int schema, FuncNewObject fnNew, Class* baseClass, Class* baseNextClass1, Class* baseNextClass2);
	Class(char* className, int objectSize, int schema, FuncNewObject fnNew, Class* baseClass, Class* baseNextClass1, Class* baseNextClass2, Class* baseNextClass3);
	Class(char* className, int objectSize, int schema, FuncNewObject fnNew, Class* baseClass, Class* baseNextClass1, Class* baseNextClass2, Class* baseNextClass3, Class* baseNextClass4);
	Class(char* className, int objectSize, int schema, FuncNewObject fnNew, Class* baseClass, Class* baseNextClass1, Class* baseNextClass2, Class* baseNextClass3, Class* baseNextClass4, Class* baseNextClass5);
	Class(char* className, int objectSize, int schema, FuncNewObject fnNew, Class* baseClass, Class* baseNextClass1, Class* baseNextClass2, Class* baseNextClass3, Class* baseNextClass4, Class* baseNextClass5, Class* baseNextClass6);
	Class(char* className, int objectSize, int schema, FuncNewObject fnNew, Class* baseClass, Class* baseNextClass1, Class* baseNextClass2, Class* baseNextClass3, Class* baseNextClass4, Class* baseNextClass5, Class* baseNextClass6, Class* baseNextClass7);
	Class(char* className, int objectSize, int schema, FuncNewObject fnNew, Class* baseClass, Class* baseNextClass1, Class* baseNextClass2, Class* baseNextClass3, Class* baseNextClass4, Class* baseNextClass5, Class* baseNextClass6, Class* baseNextClass7, Class* baseNextClass8);
	Class(char* className, int objectSize, int schema, FuncNewObject fnNew, Class* baseClass, Class* baseNextClass1, Class* baseNextClass2, Class* baseNextClass3, Class* baseNextClass4, Class* baseNextClass5, Class* baseNextClass6, Class* baseNextClass7, Class* baseNextClass8, Class* baseNextClass9);

	char* class_name_;  // class 名稱
	int object_size_;      // object 大小
	unsigned int schema_;         // 版本編號（schema number）

	Object* (* newObject_)();  // 指向一個函式，其主體為 'new Cxxx';
	// 這是個 pointer to non-member function, 它之所以可用來接受諸如
	// &CFoo::CreateObject 這樣的東西，因為後者也是 ptr to non-member funcion，
	// （因為後者是個 static 函式），只不過貌似 ptr to member function 罷了。
	// 見 pic2\prog\pmftest2.cpp

	Class* base_;				// 指向 base，用以模擬tree。第一个父节点
	Class* base_next_;			// 其它父节点

	Class* next_;		// 以簡單的 linked list，將  所有的 Class objects 串連起來

	static Class* first_; // 起始節點（只需一份，所以為 static）

	// for Dynamic Creation（動態生成）
	Object* newInstance() const;
	// 供外界（擁有 Class object 者）方便呼叫，以便產生相應之 object。
	// 它其實就是喚起 (*m_pfnCreateObject)()（見 mfclite.cpp），後者又喚起
	// Cxxx::CreateObject()，執行 'new Cxxx'。
	// 不要將這個 Class::CreateObject() 和 Cxxx::CreateObject() 混淆。
	// 前者為 non-static，後者為 static。前者呼叫後者。

	//mfc9里新增的
	bool derivedFrom(const Class* baseClass) const;

	std::string toString() const;
	std::string getName() const{ return class_name_; };
	static Class* forName(const char* className);
	static Class* forName(const std::string& className);
	static Object* newInstance(const char* className);
	static Object* newInstance(const std::string& className);

};

// 以下設計，是為了稍後為每個 classes 安排一個隸屬該型別的 global object。
// 由於是 global object，所以其 ctor 更早於 main() 執行，
// 因而得以在 main() 之前設置好 Class objects 構築的「類別型錄網」。
// 詳見 _IMPLEMENT_RUNTIMECLASS macro.
struct AFX_CLASSINIT
       { AFX_CLASSINIT(Class* newClass); };   // ctor

// 以下設計，是為了將來方便取得「類別型錄網」中的某個 Class object
#define RUNTIME_CLASS(class_name) \
        (&class_name::class##class_name)

// DECLARE_, level1
// 以下設定為 static，因為只需一份。
#define DECLARE_DYNAMIC(class_name) \
public: \
  static Class class##class_name; \
  virtual Class* getClass() const;  // (A)

// DECLARE_, level2，涵蓋 level1
// 以下 (B) 設定為 static，是希望未有任何 object 時就可呼叫之
#define DECLARE_DYNCREATE(class_name) \
  DECLARE_DYNAMIC(class_name) \
  static Object* newInstance();  // (B).
  // (B) 的函式本體只是 'new Cxxx'; （詳見 IMPLEMENT_DYNCREATE）
  // (B) 位址將被設給 Class::m_pfnCreateObject（見 _IMPLEMENT_RUNTIMECLASS）
  // (B) 有點類似 virtual ctor（亦即 factory method），但不全是。
  //     factory method 會產生不同型別的 object，這裡的 CreateObject() 只會
  //     產生特定的（亦即宿主 class）的 object。
  // (B) 不要和 Class::CreateObject() 混淆了。前者最終會呼叫到這裡，
  //       見 mfclite.cpp

// IMPLEMENT_, level0，內部（不對外公開）的一個巨集
// 注意，本巨集展開後的結果，將曝露於 global scope 中。
// 其中 static 為的是設定 file scope，俾使它不在檔案外曝光
#define _IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, wSchema, pfnNew) \
  static char _lpsz##class_name[] = #class_name; \
  Class class_name::class##class_name(_lpsz##class_name, sizeof(class_name), wSchema, pfnNew, RUNTIME_CLASS(base_class_name)); \
  static AFX_CLASSINIT _init_##class_name(&class_name::class##class_name); \
  Class* class_name::getClass() const \
          { return &class_name::class##class_name; }

#define _IMPLEMENT_RUNTIMECLASS2(class_name, base_class_name, base_class_name2, wSchema, pfnNew) \
  static char _lpsz##class_name[] = #class_name; \
  Class class_name::class##class_name(_lpsz##class_name, sizeof(class_name), wSchema, pfnNew, RUNTIME_CLASS(base_class_name), RUNTIME_CLASS(base_class_name2)); \
  static AFX_CLASSINIT _init_##class_name(&class_name::class##class_name); \
  Class* class_name::getClass() const \
          { return &class_name::class##class_name; }

// 以下 level1~level3，為什麼不使用如 DECLARE_ 般層層涵蓋的手法？
// 答案：意義上的確是層層涵蓋，但各層對於 _IMPLEMENT_RUNTIMECLASS
// 的最後兩個參數的設立各不相同，所以手法有異。

// IMPLEMENT_, level1，用於 RTTI。
// 注意，版本編號設為 -1，生成函式為 NULL。
#define IMPLEMENT_DYNAMIC(class_name, base_class_name) \
  _IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, \
                          0xFFFF, NULL)

#define IMPLEMENT_DYNAMIC2(class_name, base_class_name, base_class_name2) \
  _IMPLEMENT_RUNTIMECLASS2(class_name, base_class_name, base_class_name2, \
                          0xFFFF, NULL)

// IMPLEMENT_, level2，用於 Dynamic Creation。
// 注意，版本編號設為 -1。生成函式為 non-NULL。
#define IMPLEMENT_DYNCREATE(class_name, base_class_name) \
  Object* class_name::newInstance() \
          { return /*reinterpret_cast<Object*>*/(new class_name); } \
  _IMPLEMENT_RUNTIMECLASS(class_name, base_class_name, \
                          0xFFFF, class_name::newInstance)

#define IMPLEMENT_DYNCREATE2(class_name, base_class_name, base_class_name2) \
  Object* class_name::newInstance() \
          { return /*reinterpret_cast<Object*>*/(new class_name); } \
  _IMPLEMENT_RUNTIMECLASS2(class_name, base_class_name, base_class_name2, \
                          0xFFFF, class_name::newInstance)

// Object 不使用 DECLARE_DYNAMIC 系列巨集，因為它不知道如何在
// 相應的 IMPLEMENT_DYNAMIC 系列巨集中指定 base class。如果
// 指定其 base class 為 null，則展開結果中的 RUNTIME_CLASS(null) 會有問題。
// 因此，Object 必須自行完成其所含之 Class 的宣告與設定，
// 以及其所含之 GetRuntimeClass() 的宣告和定義。
// ref. afx.h
class Object
{
public:
	Object() { }
	virtual ~Object() { }  // virtual dtor is necessary

	//for JAVA接口
	virtual Class* getClass() const;
	bool instanceof(const Class* clazz) const;
	//virtual const void * cast(const Class* clazz) const;
	//add by wubenqi
	virtual std::string getName() { return getClass()->class_name_;}

	//参考log4cxx，添加一个cast虚拟函数
	//virtual const Object* cast(const Class* clazz) const;

	virtual Object* Clone();
	
public:
	static Class classObject; // 只要一份就好。
};

#endif // __object_core_H__

