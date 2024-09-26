#ifndef _AMD64UTILS_INTERRUPTS_HPP_
#define _AMD64UTILS_INTERRUPTS_HPP_


#include "segmentation.hpp"
#include <cstdint>

namespace Amd64::Interrupts
{
    #pragma pack(1)
    struct InterruptDescriptor
    {
        uint16_t OffsetLow;
        Amd64::Segmentation::SegmentSelector SegmentSelector;
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
}

#endif 