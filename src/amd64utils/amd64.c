#include "amd64.h"

/*
 * x86-64 Masm Assembly Procedure will be below with the prefix INTERN_ (Internals)
*/
extern INTERN_GETGDTR(AMD64_PGDTR CurrentGDTROUT);
extern INTERN_GETLDTR(AMD64_PSegSelector CurrentLDTROUT);

void AMD64_GetGDTR(AMD64_PGDTR CurrentGDTR)
{
	INTERN_GETGDTR(CurrentGDTR);
}

void AMD64_GetLDTR(AMD64_PSegSelector CurrentLDTR)
{
	INTERN_GETLDTR(CurrentLDTR);
}

int AMD64_UTILS_GDTIteratorNext(AMD64_UTILS_PGDTIterator Iterator)
{
	// If invalid iterator provided returns 0
	if (Iterator->BaseAddress <= 0 || Iterator->Limit <= 0)
	{
		return 0;
	}

	// If the Iterator has been initializes but not called Next yet the CurrentOffset is -1
	// Doing checks for ensuring the first entry of the GDT is valid (filled with zeroes)
	if (Iterator->CurrentOffset == -1)
	{
		// Reading the first entry of the GDT
		uint64_t DescriptorValue = *(uint64_t*)(Iterator->BaseAddress);

		// If not zero or the Iterator is invalid or the GDT provided is wrong
		if (DescriptorValue != 0)
		{
			return 0;
		}

		// Sets the current offset for the size of an Segment Descriptor to skip the first one
		Iterator->CurrentOffset = sizeof(AMD64_SegDescriptor);

		// Returns successs
		return 1;
	}

	// If the next descriptor overlaps the GDT Limit this means we're at the end
	if ((Iterator->BaseAddress + Iterator->CurrentOffset) > (Iterator->BaseAddress + Iterator->Limit))
	{
		return 0;
	}


	// Reading the segment using the current offset
	AMD64_PSegDescriptor SegmentDescriptor = (AMD64_PSegDescriptor)(Iterator->BaseAddress + Iterator->CurrentOffset);

	// TODO: Remove this and useless field because, I can check out-of the iterator if the P flag (Present) is true
	if (SegmentDescriptor->Value == 0)
	{
		Iterator->CurrentSegmentType = 0;
		Iterator->CurrentSegmentIsNonSystem = 0;
		Iterator->CurrentDescriptor = 0;

		Iterator->CurrentOffset += sizeof(AMD64_SegDescriptor);

		return 1;
	}

	// If the segment is a non-system one, will not need of special handling
	if (SegmentDescriptor->Fields.HighPart.b.NonSystem)
	{
		// Set the iterator to hold the current segment type
		Iterator->CurrentSegmentType = SegmentDescriptor->Fields.HighPart.b.Type;
		Iterator->CurrentSegmentIsNonSystem = SegmentDescriptor->Fields.HighPart.b.NonSystem;

		// Sets the iterator to point at the current segment
		Iterator->CurrentDescriptor = Iterator->BaseAddress + Iterator->CurrentOffset;

		// Offset increases itself by Segment Descriptor size
		Iterator->CurrentOffset += sizeof(AMD64_SegDescriptor);

		// Sucessfully read the segment
		return 1;
	}
	
	// It's system segment
	if (!SegmentDescriptor->Fields.HighPart.b.NonSystem)
	{
		// Found TSS available/busy or LDT type
		if (SegmentDescriptor->Fields.HighPart.b.Type == AMD64_TSS_AVAILABLE_TYPE 
			|| SegmentDescriptor->Fields.HighPart.b.Type == AMD64_TSS_BUSY_TYPE
			|| SegmentDescriptor->Fields.HighPart.b.Type == AMD64_LDT_TYPE)
		{
			// Set the iterator to hold the current segment type
			Iterator->CurrentSegmentType = SegmentDescriptor->Fields.HighPart.b.Type;
			Iterator->CurrentSegmentIsNonSystem = SegmentDescriptor->Fields.HighPart.b.NonSystem;

			// Sets the iterator to point at the current segment
			Iterator->CurrentDescriptor = Iterator->BaseAddress + Iterator->CurrentOffset;

			// Offset increases itself by Segment Descriptor size
			Iterator->CurrentOffset += sizeof(AMD64_TSSLDTDescriptor);

			// Sucessfully read the segment
			return 1;	
		}
	}

	return 0;
}

int AMD64_UTILS_GDTIteratorInit(AMD64_UTILS_PGDTIterator Iterator, AMD64_PGDTR GDTR)
{
	// Invalid GDTR provided
	if (GDTR->Base <= 0 || GDTR->Limit <= 0)
	{
		return 0;
	}

	// Initializing the iterator
	Iterator->BaseAddress = GDTR->Base;
	Iterator->CurrentOffset = -1;
	Iterator->CurrentSegmentIsNonSystem = 0;
	Iterator->CurrentSegmentType = 0;
	Iterator->Limit = GDTR->Limit;

	return 1;
}

