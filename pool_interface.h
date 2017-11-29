/*
 * PoolInterface.h
 *
 *  Created on: Oct 4, 2016
 *      Author: andrei
 */

#ifndef SRC_POOL_INTERFACE_H_
#define SRC_POOL_INTERFACE_H_

#include <memory>

class PoolInterface
{
public:
	virtual ~PoolInterface()
	{
	}
	virtual char* allocate(std::size_t size) = 0;
	virtual void deallocate(char* pointer) = 0;
};

#endif /* SRC_POOL_INTERFACE_H_ */
