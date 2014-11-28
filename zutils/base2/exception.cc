// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#include "base2/exception.h"
#include <string.h>
#include <sstream>

namespace base{

Exception::Exception(const std::string& msg1) {
  size_t msgLen = msg1.length();
  if (msgLen > MSG_SIZE) msgLen = MSG_SIZE;
  memcpy(this->msg, (char*) msg1.data(), msgLen);
  this->msg[msgLen] = 0;
}

Exception::Exception(const Exception& src) : std::exception() {
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__)
      strcpy_s(msg, sizeof msg, src.msg);
#else
      strcpy(msg, src.msg);
#endif
}

Exception& Exception::operator=(const Exception& src) {
#if defined(__STDC_LIB_EXT1__) || defined(__STDC_SECURE_LIB__)
	strcpy_s(msg, sizeof msg, src.msg);
#else
	strcpy(msg, src.msg);
#endif
	return *this;
}

const char* Exception::what() const throw() {
	return msg;
}

RuntimeException::RuntimeException(int32 stat)
	: Exception(formatMessage(stat)) {
}

RuntimeException::RuntimeException(const std::string& msg1)
	: Exception(msg1) {
}

RuntimeException::RuntimeException(const RuntimeException& src)
      : Exception(src) {
}

RuntimeException& RuntimeException::operator=(const RuntimeException& src) {
	Exception::operator=(src);
	return *this;
}

std::string RuntimeException::formatMessage(int32 stat) {
	std::stringstream ss;
	ss << "RuntimeException: return code = " << stat;
	return ss.str();
}

NullPointerException::NullPointerException(const std::string& msg1)
	: RuntimeException(msg1) {
}

NullPointerException::NullPointerException(const NullPointerException& src)
	: RuntimeException(src) {
}

NullPointerException& NullPointerException::operator=(const NullPointerException& src) {
	RuntimeException::operator=(src);
	return *this;
}

IllegalArgumentException::IllegalArgumentException(const std::string& msg1)
	: RuntimeException(msg1) {
}

IllegalArgumentException::IllegalArgumentException(const IllegalArgumentException& src)
      : RuntimeException(src) {
}

IllegalArgumentException& IllegalArgumentException::operator=(const IllegalArgumentException& src) {
	RuntimeException::operator=(src);
	return *this;
}

IOException::IOException()
	: Exception("IO exception") {
}

IOException::IOException(const std::string& msg1)
	: Exception(msg1) {
}

IOException::IOException(const IOException& src)
	: Exception(src) {
}

IOException& IOException::operator=(const IOException& src) {
	Exception::operator=(src);
	return *this;
}

MissingResourceException::MissingResourceException(const std::string& key)
    : Exception(formatMessage(key)) {
}


MissingResourceException::MissingResourceException(const MissingResourceException& src)
	: Exception(src) {
}

MissingResourceException& MissingResourceException::operator=(const MissingResourceException& src) {
	Exception::operator=(src);
	return *this;
}

std::string MissingResourceException::formatMessage(const std::string& key) {
   std::string s("MissingResourceException: resource key = \"");
   s.append(key);
   s.append("\".");
   return s;
}

TranscoderException::TranscoderException(const TranscoderException &src)
   : Exception(src) {
}

TranscoderException& TranscoderException::operator=(const TranscoderException& src) {
     Exception::operator=(src);
	 return *this;
}

MutexException::MutexException(const MutexException &src)
     : Exception(src) {
}

MutexException& MutexException::operator=(const MutexException& src) {
	Exception::operator=(src);
	return *this;
}

InterruptedException::InterruptedException() : Exception("Thread was interrupted") {
}

InterruptedException::InterruptedException(const InterruptedException &src)
     : Exception(src) {
}

InterruptedException& InterruptedException::operator=(const InterruptedException& src) {
	Exception::operator=(src);
	return *this;
}

ThreadException::ThreadException(const ThreadException &src)
	: Exception(src) {
}

ThreadException& ThreadException::operator=(const ThreadException& src) {
	Exception::operator=(src);
	return *this;
}

IllegalMonitorStateException::IllegalMonitorStateException(const std::string& msg1)
      : Exception(msg1) {
}

IllegalMonitorStateException::IllegalMonitorStateException(const IllegalMonitorStateException& src)
      : Exception(src) {
}

IllegalMonitorStateException& IllegalMonitorStateException::operator=(const IllegalMonitorStateException& src) {
	Exception::operator=(src);
	return *this;
}

InstantiationException::InstantiationException(const std::string& msg1)
	: Exception(msg1) {
}

InstantiationException::InstantiationException(const InstantiationException& src)
	: Exception(src) {
}

InstantiationException& InstantiationException::operator=(const InstantiationException& src) {
	Exception::operator=(src);
	return *this;
}

NoSuchElementException::NoSuchElementException()
    : Exception("No such element") {
}

NoSuchElementException::NoSuchElementException(const std::string& msg)
	: Exception(msg) {
}

NoSuchElementException::NoSuchElementException(const NoSuchElementException& src)
    : Exception(src) {
}

NoSuchElementException& NoSuchElementException::operator=(const NoSuchElementException& src) {
      Exception::operator=(src);
      return *this;
}


IllegalStateException::IllegalStateException()
    : Exception("Illegal state") {
}

IllegalStateException::IllegalStateException(const std::string& msg)
	: Exception(msg) {
}

IllegalStateException::IllegalStateException(const IllegalStateException& src)
     : Exception(src) {
}

IllegalStateException& IllegalStateException::operator=(const IllegalStateException& src) {
      Exception::operator=(src);
      return *this;
}

ArrayIndexOutOfBoundsException::ArrayIndexOutOfBoundsException(int index)
	: Exception(formatMessage(index)) {
}

ArrayIndexOutOfBoundsException::ArrayIndexOutOfBoundsException(const ArrayIndexOutOfBoundsException& src)
	: Exception(src) {
}

ArrayIndexOutOfBoundsException& ArrayIndexOutOfBoundsException::operator=(const ArrayIndexOutOfBoundsException& src) {
	Exception::operator=(src);
	return *this;
}

std::string ArrayIndexOutOfBoundsException::formatMessage(int index) {
	std::stringstream ss;
	ss << "Array index out of bound exception: index = " << index;
	return ss.str();
}

InvalidRequestException::InvalidRequestException(const std::string& message) : 
	Exception(message) {

}

InvalidRequestException& InvalidRequestException::operator=(const InvalidRequestException& src) {
	Exception::operator=(src);
	return *this;
}

UnknownObjectException::UnknownObjectException(const std::string& message) : 
	Exception(message) {

}

UnknownObjectException& UnknownObjectException::operator=(const UnknownObjectException& src) {
	Exception::operator=(src);
	return *this;
}

UnsupportedOperationException::UnsupportedOperationException()
	: Exception("UnsupportedO operation") {

}

UnsupportedOperationException::UnsupportedOperationException(const std::string& msg)
	: Exception(msg) {

}

UnsupportedOperationException::UnsupportedOperationException(const UnsupportedOperationException& src)
	: Exception(src) {
}

UnsupportedOperationException& UnsupportedOperationException::operator=(const UnsupportedOperationException& src)
{
	Exception::operator=(src);
	return *this;
}

} 


