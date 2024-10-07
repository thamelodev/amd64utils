#ifndef _AMD64UTILS_PAGING_HPP_
#define _AMD64UTILS_PAGING_HPP_

#include <cstdint>
#include <intrin.h>
#include "cpuid.hpp"

namespace Amd64::Paging
{
	uint64_t GetAddressMask(uint16_t MaxLinearAddress);

	/*
	* Translates one virtual address to a physical one
	* @param DirectoryBase - The address of the first table, generally called DirBase in Windows (CR3)
	* @param VirtualAddress - The address that will be translated
	* @returns The translated address, or 0 if failed
	*/
	uint64_t VirtualToPhysical(uint64_t DirectoryBase, uint64_t VirtualAddress);

	/*
	* Get the MAXPHYADDR
	* @returns MAXPHYADDR
	*/
	uint16_t GetMaxPhysicalAddress();

	/*
	* Get MAXLINADDR
	* @returns MAXLINADDR
	*/
	uint16_t GetMaxLinearAddress();

	#pragma pack(1)
	union PML4E
	{
		uint64_t Value;
		struct
		{

		} Fields;
	};
}

#endif