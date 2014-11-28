// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef BASE2_EXCEPTION_H_
#define BASE2_EXCEPTION_H_

#include "base/basictypes.h"
#include <exception>
#include <string>

namespace base {

/** The class Exception and its subclasses indicate conditions that a
reasonable application might want to catch.
*/
class Exception : public ::std::exception {
public:
  Exception(const std::string& msg);
  Exception(const Exception& src);
  Exception& operator=(const Exception& src);
  const char* what() const throw();

private:
  enum { MSG_SIZE = 128 };
  char msg[MSG_SIZE + 1];
}; // class Exception

#define  Throwable Exception

/** RuntimeException is the parent class of those exceptions that can be
thrown during the normal operation of the process.
*/
class RuntimeException : public Exception {
public:
  RuntimeException(int32 stat);
  RuntimeException(const std::string& msg);
  RuntimeException(const RuntimeException& msg);
  RuntimeException& operator=(const RuntimeException& src);

private:
  static std::string formatMessage(int32 stat);
}; // class RuntimeException

/** Thrown when an application attempts to use NULL in a case where an
object is required.
*/
class  NullPointerException : public RuntimeException {
public:
  NullPointerException(const std::string& msg);
  NullPointerException(const NullPointerException& msg);
  NullPointerException& operator=(const NullPointerException& src);
}; // class NullPointerException

/** Thrown to indicate that a method has been passed
an illegal or inappropriate argument.*/
class IllegalArgumentException : public RuntimeException {
public:
  IllegalArgumentException(const std::string& msg);
  IllegalArgumentException(const IllegalArgumentException&);
  IllegalArgumentException& operator=(const IllegalArgumentException&);
}; // class IllegalArgumentException

/** Signals that an I/O exception of some sort has occurred. This class
is the general class of exceptions produced by failed or interrupted
I/O operations.
*/
class IOException : public Exception {
public:
  IOException();
  IOException(const std::string& msg);
  IOException(const IOException &src);
  IOException& operator=(const IOException&);
};

class MissingResourceException : public Exception {
public:
  MissingResourceException(const std::string& key);
  MissingResourceException(const MissingResourceException &src);
  MissingResourceException& operator=(const MissingResourceException&);

private:
  static std::string formatMessage(const std::string& key);
};

class MutexException : public Exception {
public:
  MutexException(const MutexException &src);
  MutexException& operator=(const MutexException&);
};

class InterruptedException : public Exception {
public:
  InterruptedException();
  InterruptedException(const InterruptedException &src);
  InterruptedException& operator=(const InterruptedException&);
};

class ThreadException : public Exception {
public:
  ThreadException(const ThreadException &src);
  ThreadException& operator=(const ThreadException&);
};

class TranscoderException : public Exception {
public:
  TranscoderException(const TranscoderException &src);
  TranscoderException& operator=(const TranscoderException&);
};

class IllegalMonitorStateException : public Exception {
public:
  IllegalMonitorStateException(const std::string& msg);
  IllegalMonitorStateException(const IllegalMonitorStateException& msg);
  IllegalMonitorStateException& operator=(const IllegalMonitorStateException& msg);
};

/**
Thrown when an application tries to create an instance of a class using
the newInstance method in class Class, but the specified class object
cannot be instantiated because it is an interface or is an abstract class.
*/
class InstantiationException : public Exception {
public:
  InstantiationException(const std::string& msg);
  InstantiationException(const InstantiationException& msg);
  InstantiationException& operator=(const InstantiationException& msg);
};

/**
Thrown when an application tries to load in a class through its
string name but no definition for the class with the specified name
could be found.
*/
class ClassNotFoundException : public Exception {
public:
  ClassNotFoundException(const std::string& className);
  ClassNotFoundException(const ClassNotFoundException& msg);
  ClassNotFoundException& operator=(const ClassNotFoundException& msg);

private:
  static std::string formatMessage(const std::string& className);
};


class NoSuchElementException : public Exception {
public:
  NoSuchElementException();
  NoSuchElementException(const std::string& msg);
  NoSuchElementException(const NoSuchElementException&);
  NoSuchElementException& operator=(const NoSuchElementException&);
};

class IllegalStateException : public Exception {
public:
  IllegalStateException();
  IllegalStateException(const std::string& msg);
  IllegalStateException(const IllegalStateException&);
  IllegalStateException& operator=(const IllegalStateException&);
};

class ArrayIndexOutOfBoundsException : public Exception {
public:
  ArrayIndexOutOfBoundsException(int index);
  ArrayIndexOutOfBoundsException(const ArrayIndexOutOfBoundsException&);
  ArrayIndexOutOfBoundsException& operator=(const ArrayIndexOutOfBoundsException&);

private:
  static std::string formatMessage(int index);
};

class InvalidRequestException : public Exception {
public:
  InvalidRequestException(const std::string& message);
  InvalidRequestException& operator=(const InvalidRequestException&);
};

class UnknownObjectException : public Exception {
public:
  UnknownObjectException(const std::string& message);
  UnknownObjectException& operator=(const UnknownObjectException&);
};

class UnsupportedOperationException : public Exception {
public:
  UnsupportedOperationException();
  UnsupportedOperationException(const std::string& msg);
  UnsupportedOperationException(const UnsupportedOperationException&);
  UnsupportedOperationException& operator=(const UnsupportedOperationException&);
};

class InternalErrorException : public Exception {
public:
  InternalErrorException(const std::string& msg)
    : Exception(msg) {
  }
};

class IndexCollapseException : public Exception {
public:
  //! Create a new exception
  IndexCollapseException() 
    : Exception("Index collapse exception") {
  }

  //! Create a new exception
  IndexCollapseException(const char* msg) 
    : Exception(msg) { 
  }

  //! Create a new exception
  IndexCollapseException(const std::string& msg) 
    : Exception(msg) { 
  }
};

class UnsupportedException : public Exception {
public:
  //! Create a new exception
  UnsupportedException() 
    : Exception( "Unsupported exception" ){
  }

  //! Create a new exception
  UnsupportedException (const char* msg) 
    : Exception(msg) { 
  }

  //! Create a new exception
  UnsupportedException (const std::string& msg) 
    : Exception(msg) { 
  }
};

class OutOfRangeException : public Exception {
public:
  //! Create a new exception
  OutOfRangeException() 
    : Exception( "Out of range" ){
  }

  //! Create a new exception
  OutOfRangeException(const char* msg) 
    : Exception(msg) { 
  }

  //! Create a new exception
  OutOfRangeException(const std::string& msg) 
    : Exception(msg) { 
  }
};

class OutOfMemoryException 
  : public Exception {
public:
  //! Create a new exception
  OutOfMemoryException () 
    : Exception( "Out of memory" ){ 
  }

  //! Create a new exception
  OutOfMemoryException (const char* msg) 
    : Exception(msg) { 
  }

  //! Create a new exception
  OutOfMemoryException (const std::string& msg) 
    : Exception(msg) { 
  }
};

}
#endif // __EXCEPTION_H_
