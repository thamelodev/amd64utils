#ifndef _UTILS_AMD64_H_
#define _UTILS_AMD64_H_

#include <stdint.h>

/**
* GDTR structure
*/
#pragma pack(1)
typedef struct _AMD64_GDTR
{
	uint16_t Limit;
	uint64_t Base;
} AMD64_GDTR, * AMD64_PGDTR;

/*
* Segment Selector structure
*/
typedef union _AMD64_SegSelector
{
	uint16_t Value;
	struct
	{
		uint8_t RequestPrivLevel : 2;
		uint8_t TableIndicator : 1;
		uint16_t Index : 13;
	} b;
} AMD64_SegSelector, * AMD64_PSegSelector;

/*
* Segment Descriptor (Without System Part)
*/
#pragma pack(1)
typedef struct _AMD64_SegDescriptor
{
	union {
		uint64_t Value;
		struct {
			union {
				uint32_t Value;
				struct {
					uint16_t LimitLow;
					uint16_t BaseLow;
				} b;
			} LowPart;

			union {
				uint32_t Value;
				struct {
					uint8_t BaseMiddle;
					uint8_t Type : 4;
					uint8_t NonSystem : 1;
					uint8_t DescriptorPrivLevel : 2;
					uint8_t Present : 1;
					uint8_t LimitHigh : 4;
					uint8_t Available : 1;
					uint8_t LongMode : 1;
					uint8_t DefaultOperationSize : 1;
					uint8_t Granularity : 1;
					uint8_t BaseHigh;
				} b;
			} HighPart;
		} Fields;
	};
} AMD64_SegDescriptor, AMD64_PSegDescriptor;

/*
* Gets the current GDTR
* @param {AMD64_PGDTR} CurrentGDTR - Pointer to the output that will contain the current GDTR structure
*/
void AMD64_GetGDTR(AMD64_PGDTR CurrentGDTR);
void AMD64_GetLDTR(AMD64_PSegSelector CurrentLDTR);

// TODO: Create a function to parse the entire GDT, recommended way: One iterator over the GDT


#endif // _UTILS_AMD64_H_