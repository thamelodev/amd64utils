
#include <ntddk.h>
#include "amd64utils/amd64.h"

NTSTATUS DriverUnload()
{
	return STATUS_SUCCESS;
}

void DoTests()
{
	AMD64_GDTR CurrentGDTR = { 0 };
	AMD64_SegSelector CurrentLDTR = { 0 };

	// 1 - Testing get LDTR and GDTR
	AMD64_GetGDTR(&CurrentGDTR);
	AMD64_GetLDTR(&CurrentLDTR);

	DbgPrint("GDTR Base: 0x%llx, LDT Index: 0x%hx\n", CurrentGDTR.Base, CurrentLDTR.Fields.Index);

	// 2 - Testing Segment Descriptor Structure
	AMD64_SegDescriptor SegmentDescriptor = { 0 };

	SegmentDescriptor.Value = 0x00209b0000000000;

	DbgPrint("LowPart.Value: 0x%08X\n", SegmentDescriptor.Fields.LowPart.Value);
	DbgPrint("HighPart.Value: 0x%08X\n", SegmentDescriptor.Fields.HighPart.Value);


	// 3 - Testing Get Task Register
	AMD64_SegSelector SegmentSelector = { 0 };
	
	AMD64_GetTaskRegister(&SegmentSelector);
	
	DbgPrint("Task Register - Index: 0x%hhx Rpl: %d Value: %hhx\n",
		SegmentSelector.Fields.Index,
		SegmentSelector.Fields.RequestPrivLevel,
		SegmentSelector.Value
	);

	// 4 - Get TSS Address
	AMD64_PTSS Tss = AMD64_UTILS_GetTSSBaseAddress();

	DbgPrint("Tss Base Address at: 0x%llx I/O Base Map: 0x%hhx Rsp0: 0x%llx\n", 
		(uint64_t) Tss,
		Tss->IoMapBaseAddress,
		Tss->Rsp0
	);

	// 5 - Testing GDT Iterator Utils
	AMD64_UTILS_GDTIterator Iterator = { 0 };

	if (!AMD64_UTILS_GDTIteratorInit(&Iterator, &CurrentGDTR))
	{
		DbgPrint("Failed to initialize the GDT iterator!\n");
		return;
	}

	while (AMD64_UTILS_GDTIteratorNext(&Iterator))
	{
		AMD64_PSegDescriptor CurrentSegment = (AMD64_PSegDescriptor)Iterator.CurrentDescriptor;

		// If the pointer is invalid, continue
		if (CurrentSegment == NULL)
		{
			continue;
		}

		// If not present field, the descriptor is invalid/not present
		if (!CurrentSegment->Fields.HighPart.b.Present)
		{
			DbgPrint("Current Address: 0x%llx Invalid Segment Descriptor! - Not present\n",
				Iterator.CurrentDescriptor
			);

			continue;
		}

		if (!Iterator.CurrentSegmentIsNonSystem)
		{
			if (Iterator.CurrentSegmentType == AMD64_TSS_AVAILABLE_TYPE 
				|| Iterator.CurrentSegmentType == AMD64_TSS_BUSY_TYPE)
			{
				AMD64_PTSSLDTDescriptor TssDescriptor = (AMD64_PTSSLDTDescriptor)Iterator.CurrentDescriptor;

				uint64_t TssBase = { 0 };
				
				// Doing the bit-fields concating to form the Tss Base Address
				TssBase = TssDescriptor->Low.Fields.BaseLow;
				TssBase |= (TssDescriptor->Middle.Fields.BaseLow2 << 16UL);
				TssBase |= (TssDescriptor->Middle.Fields.BaseMiddle << 24UL);
				TssBase |= ((uint64_t) TssDescriptor->BaseHigh << 32UL);

				DbgPrint(
					"[TSS64] - Segment Address: 0x%llx Non-System Segment "
					"- Segment Type: %02hhx Non-System: %02hhx "
					"TSS64 Available/Busy Base Address: 0x%llx\n",
					Iterator.CurrentDescriptor,
					Iterator.CurrentSegmentType, 
					Iterator.CurrentSegmentIsNonSystem,
					TssBase
				);

				continue;
			}

		}

		// Print the Non-System Segment Data
		DbgPrint("Segment Address: 0x%llx Non-System Segment - Segment Type: %02hhx Non-System: %02hhx\n",
			Iterator.CurrentDescriptor,
			Iterator.CurrentSegmentType, 
			Iterator.CurrentSegmentIsNonSystem
		);
	}
}


NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	DriverObject->DriverUnload = &DriverUnload;

	DoTests();

	return STATUS_SUCCESS;
}
