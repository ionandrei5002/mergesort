/*
 * allocator.h
 *
 *  Created on: May 16, 2016
 *      Author: andrei
 */

#ifndef SRC_FREE_LIST_ALLOCATOR_H_
#define SRC_FREE_LIST_ALLOCATOR_H_

#include<iostream>
#include <bits/c++config.h>
#include <new>
#include <bits/functexcept.h>
#include <bits/move.h>
#include <limits>
#if __cplusplus >= 201103L
#include <type_traits>
#endif

#include "free_list_pool.h"

template<class T>
class free_list_allocator
{
public:
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef T value_type;

#if __cplusplus >= 201103L
	typedef std::true_type propagate_on_container_move_assignment;
#endif

	free_list_allocator()
	{
	}
	free_list_allocator(const free_list_allocator&)
	{
	}

	pointer allocate(size_type n, const void * = 0)
	{
		void* t = freelist_pool.allocate(n * sizeof(T));
		return static_cast<pointer>(t);
	}

	void deallocate(void* p, size_type size)
	{
		if (p)
		{
			freelist_pool.deallocate(static_cast<char*>(p));
		}
	}

	pointer address(reference x) const
	{
		return &x;
	}
	const_pointer address(const_reference x) const
	{
		return &x;
	}
	free_list_allocator<T>& operator=(const free_list_allocator&)
	{
		return *this;
	}

#if __cplusplus >= 201103L
	template<typename _Up, typename ... _Args>
	void construct(_Up* __p, _Args&&... __args)
	{
		::new (static_cast<void*>(__p)) _Up(std::forward<_Args>(__args)...);
	}

	template<typename _Up>
	void destroy(_Up* __p)
	{
		__p->~_Up();
	}
#else
	inline void construct(pointer p, const T& val)
	{
		new ((T*) p) T(val);
	}
	inline void destroy(pointer p)
	{
		p->~T();
	}
#endif

	size_type max_size() const
	{
		return std::size_t(-1);
	}

	template<class U>
	struct rebind
	{
		typedef free_list_allocator<U> other;
	};

	template<class U>
	free_list_allocator(const free_list_allocator<U>&)
	{
	}

	template<class U>
	free_list_allocator& operator=(const free_list_allocator<U>&)
	{
		return *this;
	}
};

// return that all specializations of this allocator are interchangeable
template<class T1, class T2>
bool operator==(const free_list_allocator<T1>&,
		const free_list_allocator<T2>&) throw ()
{
	return true;
}
template<class T1, class T2>
bool operator!=(const free_list_allocator<T1>&,
		const free_list_allocator<T2>&) throw ()
{
	return false;
}

extern free_list_allocator<uint8_t> free_list_alloca;

#endif /* SRC_FREE_LIST_ALLOCATOR_H_ */
