#ifndef _UTILS_AMD64_H_
#define _UTILS_AMD64_H_

#include <stdint.h>

#define AMD64_INTERRUPT_GATE_TYPE 14
#define AMD64_TRAP_GATE_TYPE 15
#define AMD64_TSS_BUSY_TYPE 11
#define AMD64_TSS_AVAILABLE_TYPE 9
#define AMD64_LDT_TYPE 2

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
		uint16_t RequestPrivLevel : 2;
		uint16_t TableIndicator : 1;
		uint16_t Index : 13;
	} Fields;
} AMD64_SegSelector, * AMD64_PSegSelector;

/*
* Segment Descriptor
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
} AMD64_SegDescriptor, * AMD64_PSegDescriptor;

/*
* Interrupt Descriptor
*/
#pragma pack(1)
typedef struct _AMD64_IDTDescriptor
{
	uint16_t OffsetLow;
	AMD64_SegSelector SegmentSelector;
	union
	{
		uint32_t Value;
		struct
		{
			uint32_t Ist : 3;
			uint32_t Ign1 : 1;
			uint32_t Ign2 : 1;
			uint32_t Ign3 : 3;
			uint32_t Type : 4;
			uint32_t Ign4 : 1;
			uint32_t DescriptorPrivLevel : 2;
			uint32_t Present : 1;
			uint32_t OffsetMiddle : 16;
		} Fields;
	} Middle;
	uint32_t OffsetHigh;
	uint32_t Reserved;
} AMD64_IDTDescriptor, * AMD64_PIDTDescriptor;

/*
* TSS/LDT Descriptor
*/
typedef struct _AMD64_TSSLDTDescriptor
{
	union
	{
		uint32_t Value;
		struct
		{
			uint32_t SegmentLimit : 16;
			uint32_t BaseLow : 16;
		} Fields;
	} Low;
	union
	{
		uint32_t Value;
		struct
		{
			uint32_t BaseLow2 : 8;
			uint32_t Type : 4;
			uint32_t Ign1 : 1;
			uint32_t DescriptorPrivLevel : 2;
			uint32_t Present : 1;
			uint32_t Limit : 4;
			uint32_t Available : 1;
			uint32_t Ign2 : 1;
			uint32_t Ign3 : 1;
			uint32_t Granularity : 1;
			uint32_t BaseMiddle : 8;
		} Fields;
	} Middle;
	uint32_t BaseHigh;
	uint32_t Reserved;
} AMD64_TSSLDTDescriptor, * AMD64_PTSSLDTDescriptor;

/*
* The structure correspondent to Task Segment
*/
#pragma pack(1)
typedef struct _AMD64_TSS
{
	uint32_t __Reserved;
	uint64_t Rsp0;
	uint64_t Rsp1;
	uint64_t Rsp2;
	uint32_t __Reserved1;
	uint32_t __Reserved2;
	uint64_t Ist1;
	uint64_t Ist2;
	uint64_t Ist3;
	uint64_t Ist4;
	uint64_t Ist5;
	uint64_t Ist6;
	uint64_t Ist7;	
	uint32_t __Reserved3;
	uint32_t __Reserved4;
	uint16_t __Reserved5;
	uint16_t IoMapBaseAddress;
} AMD64_TSS, *AMD64_PTSS;

typedef struct _AMD64_UTILS_GDTIterator
{
	// The GDT Base Address
	uint64_t BaseAddress;

	// The current offset the iterator is pointing at
	uint32_t CurrentOffset;

	// The GDT Limit (GDTR Low part)
	uint32_t Limit;

	// The current segment type code iterator is pointing at
	uint8_t CurrentSegmentType;

	// The current segment iterator is pointing is non-system
	uint8_t CurrentSegmentIsNonSystem;

	// The current segment iterator is point at
	uint64_t CurrentDescriptor;
} AMD64_UTILS_GDTIterator, *AMD64_UTILS_PGDTIterator;

/*
* Gets the current GDTR
* @param {AMD64_PGDTR} CurrentGDTR - Pointer to the output that will contain the current GDTR structure
*/
void AMD64_GetGDTR(AMD64_PGDTR CurrentGDTR);

/*
* Gets the current LDTR
* @param {AMD64_PSegSelector} CurrentLDTR - Pointer to the output that will contain the current segment selector that's in LDTR
*/
void AMD64_GetLDTR(AMD64_PSegSelector CurrentLDTR);

/*
* Gets the current Task Register
* @param {AMD64_PSegSelector} CurrentTaskRegister - Pointer to the output that will contain the segment selector that TR is holding
*/
void AMD64_GetTaskRegister(AMD64_PSegSelector CurrentTaskRegister);


/*
* Initializes the GDT Iterator
* @returns Returns 0 if failed to initialize the iterator
*/
int AMD64_UTILS_GDTIteratorInit(AMD64_UTILS_PGDTIterator Iterator, AMD64_PGDTR GDTR);

/*
* Uses the iterator object to iterate over the Global Descriptor Table
* @returns Returns 1 if the iterator it's not at the end yet
*/
int AMD64_UTILS_GDTIteratorNext(AMD64_UTILS_PGDTIterator Iterator);

/*
* Resets the GDT Iterator
*/
void AMD64_UTILS_GDTIteratorReset(AMD64_UTILS_PGDTIterator Iterator);

/*
* Gets the TSS Base address 
*/
uint64_t AMD64_UTILS_GetTSSBaseAddress();

#endif // _UTILS_AMD64_H_