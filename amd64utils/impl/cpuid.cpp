#include "../include/cpuid.hpp"

extern "C" void INTERN_CPUID(uint32_t Input1, uint32_t Input2, Amd64::Cpuid::IdResult& Result);

void Amd64::Cpuid::Identify(uint32_t Input1, uint32_t Input2, IdResult& Result)
{
	return INTERN_CPUID(Input1, Input2, Result);
}
