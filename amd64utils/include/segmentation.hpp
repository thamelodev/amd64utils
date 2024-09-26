#ifndef _AMD64UTILS_SEGMENTATION_HPP_
#define _AMD64UTILS_SEGMENTATION_HPP_

#include <cstdint>

namespace Amd64::Segmentation
{
    constexpr uint8_t INTERRUPT_GATE_TYPE = 14;
    constexpr uint8_t TRAP_GATE_TYPE = 15;
    constexpr uint8_t TSS_BUSY_TYPE = 11;
    constexpr uint8_t TSS_AVAILABLE_TYPE = 9;
    constexpr uint8_t LDT_TYPE = 2;

    #pragma pack(1)
    struct GDTRegister
    {
        uint16_t Limit;
        uint64_t Base;
    };

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

    #pragma pack(1)
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

    using LDTDescriptor = TSSDescriptor;

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
        uint64_t BaseAddress;
        uint32_t CurrentOffset;
        uint32_t Limit;
        uint8_t CurrentSegmentType;
        uint8_t CurrentSegmentIsNonSystem;
        uint64_t CurrentDescriptor;
    };

    void GetGDTRegister(GDTRegister* CurrentGDTR);
    void GetLDTRegister(SegmentSelector* CurrentLDTR);
    void GetTaskRegister(SegmentSelector* CurrentTaskRegister);
    int GDTIteratorInit(GDTIterator* Iterator, GDTRegister* GDTR);
    int GDTIteratorNext(GDTIterator* Iterator);
    void GDTIteratorReset(GDTIterator* Iterator);
    uint64_t GetTSSBaseAddress();
}

#endif _AMD64UTILS_SEGMENTATION_HPP_

