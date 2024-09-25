#include "amd64.h"

/*
 * x86-64 Masm Assembly Procedure will be below with the prefix INTERN_ (Internals)
*/
extern INTERN_GETGDTR(AMD64_PGDTR CurrentGDTROUT);
extern INTERN_GETLDTR(AMD64_PSegSelector CurrentLDTROUT);
extern INTERN_GETTASKREGISTER(AMD64_PSegSelector CurrentLDTROUT);

void AMD64_GetGDTR(AMD64_PGDTR CurrentGDTR)
{
	INTERN_GETGDTR(CurrentGDTR);
}

void AMD64_GetLDTR(AMD64_PSegSelector CurrentLDTR)
{
	INTERN_GETLDTR(CurrentLDTR);
}

void AMD64_GetTaskRegister(AMD64_PSegSelector CurrentTaskRegister)
{
	INTERN_GETTASKREGISTER(CurrentTaskRegister);
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

void AMD64_UTILS_GDTIteratorReset(AMD64_UTILS_PGDTIterator Iterator)
{
	Iterator->CurrentOffset = 0;
	Iterator->CurrentOffset = 0;
	Iterator->CurrentSegmentIsNonSystem = 0;
	Iterator->CurrentSegmentType = 0;
}

uint64_t AMD64_UTILS_GetTSSBaseAddress()
{
	AMD64_SegSelector TaskRegister = { 0 };
	AMD64_GDTR GDTR = { 0 };

	AMD64_GetTaskRegister(&TaskRegister);

	// Ensuring it's a valid task register
	if (TaskRegister.Fields.Index == 0 || TaskRegister.Value == 0)
	{
		return 0;
	}

	AMD64_GetGDTR(&GDTR);

	// Ensuring GDTR is not invalid
	if (GDTR.Base == 0)
	{
		return 0;
	}
	
	// The TSS descriptor in GDT would be TR->Index * 8 + GDTR->Base
	AMD64_PTSSLDTDescriptor TssDescriptor = (TaskRegister.Fields.Index * 8) + GDTR.Base;

	// Ensuring the TSS Descriptor is not invalid
	if (TssDescriptor == NULL 
		&& TssDescriptor->Middle.Fields.Present
		&& (TssDescriptor->Middle.Fields.Type == AMD64_TSS_AVAILABLE_TYPE 
			|| TssDescriptor->Middle.Fields.Type == AMD64_TSS_BUSY_TYPE))
	{
		return 0;
	}
	
	uint64_t TssBase = { 0 };
				
	// Doing the bit-fields concating to form the Tss Base Address
	TssBase = TssDescriptor->Low.Fields.BaseLow;
	TssBase |= (TssDescriptor->Middle.Fields.BaseLow2 << 16UL);
	TssBase |= (TssDescriptor->Middle.Fields.BaseMiddle << 24UL);
	TssBase |= ((uint64_t) TssDescriptor->BaseHigh << 32UL);

	return TssBase;
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

