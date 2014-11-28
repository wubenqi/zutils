// Copyright (C) 2012 by wubenqi
// Distributable under the terms of either the Apache License (Version 2.0) or 
// the GNU Lesser General Public License, as specified in the COPYING file.
//
// By: wubenqi<wubenqi@gmail.com>
//

#ifndef BASE2_ITERATOR_H_
#define BASE2_ITERATOR_H_

namespace base{

template<class ElemT>
class Iterator {
public:
  typedef ElemT element_type;

	Iterator(void) {}
	virtual ~Iterator(void) {}

	virtual void		Reset() = 0;
	virtual bool		HasNext() const = 0;
	virtual ElemT		Next() = 0;
	virtual size_t	Size() const= 0;
};

}

#endif
