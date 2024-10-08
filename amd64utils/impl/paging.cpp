#include "../include/paging.hpp"

namespace Amd64::Paging
{
	uint64_t GetAddressMask(uint16_t MaxLinearAddress)
	{
		return (1ULL << (MaxLinearAddress)) - 1;
	}

	uint64_t VirtualToPhysical(uint64_t DirectoryBase, uint64_t VirtualAddress)
	{
		// Starting by obtaining the mask
		const auto Mask = GetAddressMask(GetMaxLinearAddress());

		return 0;
	}


	uint32_t _GetResultCpuidPhysicalAddressEax()
	{
		Amd64::Cpuid::IdResult Result = { 0 };

		Amd64::Cpuid::Identify(Amd64::Cpuid::LF1_VIRTUAL_PHYSICAL_ADDR_SIZES, 0, Result);

		return static_cast<uint32_t>(Result.eax);
	}


	uint16_t GetMaxPhysicalAddress()
	{
		return static_cast<uint16_t>(_GetResultCpuidPhysicalAddressEax() & 0xff);
	}

	uint16_t GetMaxLinearAddress()
	{
		return static_cast<uint16_t>(_GetResultCpuidPhysicalAddressEax() >> 8);
	}
}