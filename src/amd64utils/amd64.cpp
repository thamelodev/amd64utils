#include "amd64.hpp"

namespace Amd64
{
    /*
     * x86-64 Masm Assembly Procedures with the prefix INTERN_ (Internals)
    */
    extern "C" void INTERN_GETGDTR(GDTRegister* CurrentGDTROUT);
    extern "C" void INTERN_GETLDTR(SegmentSelector* CurrentLDTROUT);
    extern "C" void INTERN_GETTASKREGISTER(SegmentSelector* CurrentLDTROUT);

    void GetGDTRegister(GDTRegister* CurrentGDTR)
    {
        INTERN_GETGDTR(CurrentGDTR);
    }

    void GetLDTRegister(SegmentSelector* CurrentLDTR)
    {
        INTERN_GETLDTR(CurrentLDTR);
    }

    void GetTaskRegister(SegmentSelector* CurrentTaskRegister)
    {
        INTERN_GETTASKREGISTER(CurrentTaskRegister);
    }

    int GDTIteratorNext(GDTIterator* Iterator)
    {
        // If invalid iterator is provided, return 0
        if (Iterator->BaseAddress == 0 || Iterator->Limit == 0)
        {
            return 0;
        }

        // If the next descriptor overlaps the GDT Limit, we're at the end
        if ((Iterator->BaseAddress + Iterator->CurrentOffset + 8) > (Iterator->BaseAddress + Iterator->Limit))
        {
            return 0;
        }

        auto Descriptor = reinterpret_cast<SegmentDescriptor*>(Iterator->BaseAddress + Iterator->CurrentOffset);

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

        if (!NonSystem)
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

    void GDTIteratorReset(GDTIterator* Iterator)
    {
        Iterator->CurrentOffset = 0;
        Iterator->CurrentSegmentIsNonSystem = 0;
        Iterator->CurrentSegmentType = 0;
    }

    uint64_t GetTSSBaseAddress()
    {
        SegmentSelector TaskRegister = { 0 };
        GDTRegister GDTR = { 0 };

        GetTaskRegister(&TaskRegister);

        // Ensuring it's a valid task register
        if (TaskRegister.Fields.Index == 0 || TaskRegister.Value == 0)
        {
            return 0;
        }

        GetGDTRegister(&GDTR);

        // Ensuring GDTR is valid
        if (GDTR.Base == 0)
        {
            return 0;
        }

        // The TSS descriptor in GDT would be TR->Index * 8 + GDTR->Base
        auto TssDescriptor = reinterpret_cast<TSSDescriptor*>((TaskRegister.Fields.Index * 8) + GDTR.Base);

        // Ensuring the TSS Descriptor is valid
        if (TssDescriptor == nullptr ||
            !TssDescriptor->Middle.Fields.Present ||
            !(TssDescriptor->Middle.Fields.Type == TSS_AVAILABLE_TYPE || TssDescriptor->Middle.Fields.Type == TSS_BUSY_TYPE))
        {
            return 0;
        }

        uint64_t TssBase = 0;

        // Concatenating bit-fields to form the TSS Base Address
        TssBase = TssDescriptor->Low.Fields.BaseLow;
        TssBase |= (TssDescriptor->Middle.Fields.BaseLow2 << 16UL);
        TssBase |= (TssDescriptor->Middle.Fields.BaseMiddle << 24UL);
        TssBase |= (static_cast<uint64_t>(TssDescriptor->BaseHigh) << 32UL);

        return TssBase;
    }

    int GDTIteratorInit(GDTIterator* Iterator, GDTRegister* GDTR)
    {
        // Invalid GDTR provided
        if (GDTR->Base == 0 || GDTR->Limit == 0)
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
}
