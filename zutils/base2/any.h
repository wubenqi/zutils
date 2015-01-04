// Copyright 2010, www.lelewan.com
// All rights reserved
//
// Author: wubenqi<wubenqi@caitong.net>, 2010-10-06
//

#ifndef BASE2_ANY_H_
#define BASE2_ANY_H_
#pragma once

#include <algorithm>
#include <typeinfo>
#include "base/logging.h"

namespace base {

// An Any class represents a general type and is capable of storing any type, supporting type-safe extraction
// of the internally stored data.
//
// Code taken from the Boost 1.33.1 library. Original copyright by Kevlin Henney.
// Modified by Wubenqi

class Any {
public:
	// Creates an empty any type.
	Any()
		: content_(0) {
	}

	// Creates an any which stores the init parameter inside.
	//
	// Example: 
	//     Any a(13); 
	//     Any a(string("12345"));
	template <typename ValueType>
	Any(const ValueType& value)
		: content_(new Holder<ValueType>(value)) {
		CHECK(content_);
	}

	// Copy constructor, works with empty Anys and initialized Any values.
	Any(const Any& other): content_(other.content_ ? other.content_->clone() : 0) {
	}

	~Any() {
		delete content_;
	}

	// Swaps the content of the two Anys.
	Any& swap(Any& rhs) {
		std::swap(content_, rhs.content_);
		return *this;
	}

	// Assignment operator for all types != Any.
	//
	// Example: 
	//    Any a = 13; 
	//    Any a = string("12345");
	template <typename ValueType>
	Any& operator = (const ValueType& rhs) {
		Any(rhs).swap(*this);
		return *this;
	}

	// Assignment operator for Any.
	Any& operator = (const Any& rhs) {
		Any(rhs).swap(*this);
		return *this;
	}

	// returns true if the Any is empty
	bool empty() const {
		return !content_;
	}

	// Returns the type information of the stored content.
	// If the Any is empty typeid(void) is returned.
	// It is suggested to always query an Any for its type info before trying to extract
	// data via an AnyCast/RefAnyCast.
	const std::type_info& type() const {
		return content_ ? content_->type() : typeid(void);
	}

private:
	class Placeholder {
	public:
		virtual ~Placeholder() {
		}

		virtual const std::type_info& type() const = 0;
		virtual Placeholder* clone() const = 0;
	};

	template <typename ValueType>
	class Holder: public Placeholder {
	public: 
		Holder(const ValueType& value)
			: held_(value) { 
		}

		virtual const std::type_info& type() const {
			return typeid(ValueType);
		}

		virtual Placeholder* clone() const {
			return new Holder(held_);
		}

		ValueType held_;
	};

private:
	template <typename ValueType>
	friend ValueType* AnyCast(Any*);

	template <typename ValueType>
	friend ValueType* UnsafeAnyCast(Any*);

	Placeholder* content_;
};


// AnyCast operator used to extract the ValueType from an Any*. Will return a pointer
// to the stored value. 
//
// Example Usage: 
//     MyType* pTmp = AnyCast<MyType*>(pAny).
// Will return NULL if the cast fails, i.e. types don't match.
template <typename ValueType>
ValueType* AnyCast(Any* operand) {
	return operand && operand->type() == typeid(ValueType)
		? &static_cast<Any::Holder<ValueType>*>(operand->content_)->held_
		: 0;
}

// AnyCast operator used to extract a const ValueType pointer from an const Any*. Will return a const pointer
// to the stored value. 
//
// Example Usage:
//     const MyType* pTmp = AnyCast<MyType*>(pAny).
// Will return NULL if the cast fails, i.e. types don't match.
template <typename ValueType>
const ValueType* AnyCast(const Any* operand) {
	return AnyCast<ValueType>(const_cast<Any*>(operand));
}

// The "unsafe" versions of AnyCast are not part of the
// public interface and may be removed at any time. They are
// required where we know what type is stored in the any and can't
// use typeid() comparison, e.g., when our types may travel across
// different shared libraries.
template <typename ValueType>
ValueType* UnsafeAnyCast(Any* operand) {
	return &static_cast<Any::Holder<ValueType>*>(operand->content_)->held_;
}

// The "unsafe" versions of AnyCast are not part of the
// public interface and may be removed at any time. They are
// required where we know what type is stored in the any and can't
// use typeid() comparison, e.g., when our types may travel across
// different shared libraries.
template <typename ValueType>
const ValueType* UnsafeAnyCast(const Any* operand) {
	return AnyCast<ValueType>(const_cast<Any*>(operand));
}

}

#endif

