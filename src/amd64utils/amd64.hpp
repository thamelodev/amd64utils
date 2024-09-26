#ifndef UTILS_AMD64_HPP
#define UTILS_AMD64_HPP

#include <cstdint>

namespace Amd64
{
    constexpr uint8_t INTERRUPT_GATE_TYPE = 14;
    constexpr uint8_t TRAP_GATE_TYPE = 15;
    constexpr uint8_t TSS_BUSY_TYPE = 11;
    constexpr uint8_t TSS_AVAILABLE_TYPE = 9;
    constexpr uint8_t LDT_TYPE = 2;

    /**
    * GDTR structure
    */
    #pragma pack(1)
    struct GDTRegister
    {
        uint16_t Limit;
        uint64_t Base;
    };

    /*
    * Segment Selector structure
    */
    union SegmentSelector
    {
        uint16_t Value;
        struct
        {
            uint16_t RequestPrivLevel : 2;
            uint16_t TableIndicator : 1;
            uint16_t Index : 13;
        } Fields;
    };

    /*
    * Segment Descriptor
    */
    #pragma pack(1)
    struct SegmentDescriptor
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
    };

    /*
    * Interrupt Descriptor
    */
    #pragma pack(1)
    struct InterruptDescriptor
    {
        uint16_t OffsetLow;
        SegmentSelector SegmentSelector;
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
    };

    /*
    * TSS Descriptor
    */
    struct TSSDescriptor
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
    };

    /*
    * Alias to TSS Descriptor, since LDT and TSS use the same descriptor
    */
    using LDTDescriptor = TSSDescriptor;

    /*
    * The structure corresponding to Task Segment
    */
    #pragma pack(1)
    struct TaskStateSegment
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
    };

    struct GDTIterator
    {
        // The GDT Base Address
        uint64_t BaseAddress;

        // The current offset the iterator is pointing at
        uint32_t CurrentOffset;

        // The GDT Limit (GDTR Low part)
        uint32_t Limit;

        // The current segment type code iterator is pointing at
        uint8_t CurrentSegmentType;

        // The current segment iterator is non-system
        uint8_t CurrentSegmentIsNonSystem;

        // The current descriptor the iterator is pointing at
        uint64_t CurrentDescriptor;
    };

    /*
    * Gets the current GDTR
    * @param {GDTRegister*} CurrentGDTR - Pointer to the output that will contain the current GDTR structure
    */
    void GetGDTRegister(GDTRegister* CurrentGDTR);

    /*
    * Gets the current LDTR
    * @param {SegmentSelector*} CurrentLDTR - Pointer to the output that will contain the current segment selector in LDTR
    */
    void GetLDTRegister(SegmentSelector* CurrentLDTR);

    /*
    * Gets the current Task Register
    * @param {SegmentSelector*} CurrentTaskRegister - Pointer to the output that will contain the segment selector that TR is holding
    */
    void GetTaskRegister(SegmentSelector* CurrentTaskRegister);

    /*
    * Initializes the GDT Iterator
    * @returns Returns 0 if failed to initialize the iterator
    */
    int GDTIteratorInit(GDTIterator* Iterator, GDTRegister* GDTR);

    /*
    * Uses the iterator object to iterate over the Global Descriptor Table
    * @returns Returns 1 if the iterator is not at the end yet
    */
    int GDTIteratorNext(GDTIterator* Iterator);

    /*
    * Resets the GDT Iterator
    */
    void GDTIteratorReset(GDTIterator* Iterator);

    /*
    * Gets the TSS Base address
    */
    uint64_t GetTSSBaseAddress();

} // namespace AMD64

#endif // UTILS_AMD64_HPP
