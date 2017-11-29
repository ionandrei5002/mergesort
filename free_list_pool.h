/*
 * FreeListPool.h
 *
 *  Created on: Oct 4, 2016
 *      Author: andrei
 */

#ifndef SRC_FREE_LIST_POOL_H_
#define SRC_FREE_LIST_POOL_H_

#include "pool_interface.h"

#include <iostream>
#include <vector>

class FreeListPool: public PoolInterface
{
private:
	struct List
	{
		List* next;
	};
	std::vector<List*> lists;
	char* cursor = nullptr;
	char* head = nullptr;
	char* tail = nullptr;
	uint32_t m_header_size = 8;
	uint32_t m_block_size = 8;
	uint32_t m_pow_2 = 3;
	long unsigned int list_size = 0;
public:
	FreeListPool(uint32_t size)
	{
		head = new char[size];

		std::cout << "pool head at " << static_cast<void*>(head) << std::endl;

		cursor = head;
		tail = head + size;

		std::cout << "pool tail at " << static_cast<void*>(tail) << std::endl;

		for (uint32_t i = m_block_size; i <= 24000000; i += m_block_size)
		{
			lists.push_back(nullptr);
		}

		list_size = lists.size();
	}
	~FreeListPool()
	{
		uint64_t bytes_left = tail - cursor;
		std::cout << (bytes_left / 1024 / 1024) << "Mb" << std::endl;

		cursor = nullptr;
		tail = nullptr;

		delete[] head;
		head = nullptr;

		std::cout << "pool deleted" << std::endl;
	}

	void Debug()
	{
		uint64_t bytes_left = tail - cursor;
		std::cout << (bytes_left / 1024 / 1024) << "Mb" << std::endl;
	}

	inline char* allocate(std::size_t size) override
	{
		char* pointer = nullptr;

		std::size_t total_size = 0;
		std::size_t align = (size & (m_block_size - 1));

		if (align == 0)
		{
			total_size = size;
		}
		else
		{
			total_size = (size + (m_block_size - align));
		}

		std::size_t pow = (total_size >> m_pow_2);
		std::size_t position = pow - 1;

		char* new_position = cursor + m_header_size + total_size;

		if (list_size < pow || new_position >= tail)
		{
			std::cout << "heap allocation " << total_size << std::endl;
			pointer = static_cast<char*>(new char[size]);
		}
		else
		{
			List* list = lists[position];
			if (list == nullptr)
			{
				pointer = cursor;
				*(reinterpret_cast<uint64_t*>(pointer)) = static_cast<uint64_t>(total_size);
				pointer += m_header_size;
				cursor = new_position;
			}
			else
			{
				pointer = reinterpret_cast<char*>(list);
				list = list->next;
				lists[position] = list;
			}
		}

		return pointer;
	}
	inline void deallocate(char* pointer) override
	{
		if (pointer >= head && pointer < tail)
		{
			std::size_t total_size = *(reinterpret_cast<uint64_t*>((pointer - m_header_size)));

			std::size_t pow = (total_size >> m_pow_2);

			std::size_t position = pow - 1;

			List* list = lists[position];

			if (list == nullptr)
			{
				list = reinterpret_cast<List*>(pointer);
				list->next = nullptr;
				lists[position] = list;
			}
			else
			{
				List* tmp = list;
				list = reinterpret_cast<List*>(pointer);
				list->next = tmp;
				lists[position] = list;
			}
		}
		else
		{
			delete[] pointer;
			pointer = nullptr;
		}
	}
};

extern FreeListPool freelist_pool;

#endif /* SRC_FREE_LIST_POOL_H_ */
