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

	// If the next descriptor overlaps the GDT Limit this means we're at the end
	if ((Iterator->BaseAddress + Iterator->CurrentOffset + 8) > (Iterator->BaseAddress + Iterator->Limit))
	{
		return 0;
	}

	AMD64_PSegDescriptor Descriptor = (AMD64_PSegDescriptor) (Iterator->BaseAddress + Iterator->CurrentOffset);


	int NonSystem = Descriptor->Fields.HighPart.b.NonSystem;
	int Present = Descriptor->Fields.HighPart.b.Present;
	int Type = Descriptor->Fields.HighPart.b.Type;

	Iterator->CurrentDescriptor = Iterator->BaseAddress + Iterator->CurrentOffset;

	if (!Present)
	{
		Iterator->CurrentOffset += 8;
		Iterator->CurrentSegmentIsNonSystem = NonSystem;
		Iterator->CurrentSegmentType = Type;

		return 1;
	}

	if(!NonSystem)
	{
		Iterator->CurrentOffset += 16;
		Iterator->CurrentSegmentIsNonSystem = NonSystem;
		Iterator->CurrentSegmentType = Type;

		return 1;
	}

	Iterator->CurrentOffset += 8;
	Iterator->CurrentSegmentIsNonSystem = NonSystem;
	Iterator->CurrentSegmentType = Type;

	return 1;
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
	Iterator->CurrentOffset = 0;
	Iterator->CurrentSegmentIsNonSystem = 0;
	Iterator->CurrentSegmentType = 0;
	Iterator->Limit = GDTR->Limit;

	return 1;
}

