#ifndef _AMD64UTILS_CPUID_HPP_
#define _AMD64UTILS_CPUID_HPP_

#include <cstdint>

namespace Amd64::Cpuid
{
	const auto LF1_MANUFACTURERID_HIGHEST_FN_PARAM	= 0;
	const auto LF1_EXTENDED_PROCESSOR_INFO_FEATURES = 0x80000001;
	const auto LF1_VIRTUAL_PHYSICAL_ADDR_SIZES = 0x80000008;

	struct IdResult {
		uint32_t eax;
		uint32_t ebx;
		uint32_t ecx;
		uint32_t edx;
	};

	void Identify(uint32_t Input1, uint32_t Input2, IdResult& Result);
}

#endif