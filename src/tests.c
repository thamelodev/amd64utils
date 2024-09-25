#include <ntddk.h>
#include "amd64utils/amd64.h"

NTSTATUS DriverUnload()
{
	return STATUS_SUCCESS;
}

#define DBG_PREFIX "AMD64UTILS [TEST_MODULE] "

void Test_TestingGetGDTRAndLDTR()
{
	DbgPrint(DBG_PREFIX "Test 1: Testing Get GDTR and LDTR\n");

	AMD64_GDTRegister CurrentGDTR = { 0 };
	AMD64_SegmentSelector CurrentLDTR = { 0 };

	// Get LDTR and GDTR
	AMD64_GetGDTRegister(&CurrentGDTR);
	AMD64_GetLDTRegister(&CurrentLDTR);

	DbgPrint(DBG_PREFIX "GDTR Base: 0x%llx, LDT Index: 0x%hx\n", CurrentGDTR.Base, CurrentLDTR.Fields.Index);
}

void Test_TestingSegmentDescriptorStructure()
{
	DbgPrint(DBG_PREFIX "Test 2: Testing Segment Descriptor Structure\n");

	AMD64_SegmentDescriptor SegmentDescriptor = { 0 };

	SegmentDescriptor.Value = 0x00209b0000000000;

	DbgPrint(DBG_PREFIX "LowPart.Value: 0x%08X\n", SegmentDescriptor.Fields.LowPart.Value);
	DbgPrint(DBG_PREFIX "HighPart.Value: 0x%08X\n", SegmentDescriptor.Fields.HighPart.Value);
}

void Test_TestingGetTaskRegister()
{
	DbgPrint(DBG_PREFIX "Test 3: Testing Get Task Register\n");

	AMD64_SegmentSelector SegmentSelector = { 0 };

	AMD64_GetTaskRegister(&SegmentSelector);

	DbgPrint(DBG_PREFIX "Task Register - Index: 0x%hhx Rpl: %d Value: %hhx\n",
		SegmentSelector.Fields.Index,
		SegmentSelector.Fields.RequestPrivLevel,
		SegmentSelector.Value);
}

void Test_GetTSSAddress()
{
	DbgPrint(DBG_PREFIX "Test 4: Get TSS Base Address\n");

	AMD64_PTaskStateSegment Tss = AMD64_GetTSSBaseAddress();

	DbgPrint(DBG_PREFIX "Tss Base Address at: 0x%llx I/O Base Map: 0x%hhx Rsp0: 0x%llx\n",
		(uint64_t)Tss,
		Tss->IoMapBaseAddress,
		Tss->Rsp0);
}

void Test_TestingGDTIteratorUtils()
{
	DbgPrint(DBG_PREFIX "Test 5: Testing GDT Iterator Utils\n");

	AMD64_GDTRegister CurrentGDTR = { 0 };
	AMD64_GDTIterator Iterator = { 0 };

	AMD64_GetGDTRegister(&CurrentGDTR);

	if (!AMD64_GDTIteratorInit(&Iterator, &CurrentGDTR))
	{
		DbgPrint(DBG_PREFIX "Failed to initialize the GDT iterator!\n");
		return;
	}

	while (AMD64_GDTIteratorNext(&Iterator))
	{
		AMD64_PSegmentDescriptor CurrentSegment = (AMD64_PSegmentDescriptor)Iterator.CurrentDescriptor;

		// If the pointer is invalid, continue
		if (CurrentSegment == NULL)
		{
			continue;
		}

		// If not present field, the descriptor is invalid/not present
		if (!CurrentSegment->Fields.HighPart.b.Present)
		{
			DbgPrint(DBG_PREFIX "Current Address: 0x%llx Invalid Segment Descriptor! - Not present\n",
				Iterator.CurrentDescriptor);
			continue;
		}

		if (!Iterator.CurrentSegmentIsNonSystem)
		{
			if (Iterator.CurrentSegmentType == AMD64_TSS_AVAILABLE_TYPE
				|| Iterator.CurrentSegmentType == AMD64_TSS_BUSY_TYPE)
			{
				AMD64_PTSSDescriptor TssDescriptor = (AMD64_PTSSDescriptor)Iterator.CurrentDescriptor;
				uint64_t TssBase = 0;

				// Concatenate bit-fields to form the Tss Base Address
				TssBase = TssDescriptor->Low.Fields.BaseLow;
				TssBase |= (TssDescriptor->Middle.Fields.BaseLow2 << 16UL);
				TssBase |= (TssDescriptor->Middle.Fields.BaseMiddle << 24UL);
				TssBase |= ((uint64_t)TssDescriptor->BaseHigh << 32UL);

				DbgPrint(DBG_PREFIX "Tss Descriptor Address: 0x%llx Type: %02hhx Non-System: %02hhx Tss Addr: 0x%llx\n",
					Iterator.CurrentDescriptor,
					Iterator.CurrentSegmentType,
					Iterator.CurrentSegmentIsNonSystem,
					TssBase);

				continue;
			}
		}

		// Print the Non-System Segment Data
		DbgPrint(DBG_PREFIX "Segment Address: 0x%llx Non-System Segment - Segment Type: %02hhx Non-System: %02hhx\n",
			Iterator.CurrentDescriptor,
			Iterator.CurrentSegmentType,
			Iterator.CurrentSegmentIsNonSystem);
	}
}

void RunAllTests()
{
	// Test 1: Testing Get GDTR and LDTR
	Test_TestingGetGDTRAndLDTR();

	// Test 2: Testing Segment Descriptor Structure
	Test_TestingSegmentDescriptorStructure();

	// Test 3: Testing Get Task Register
	Test_TestingGetTaskRegister();

	// Test 4: Get TSS Address
	Test_GetTSSAddress();

	// Test 5: Testing GDT Iterator Utils
	Test_TestingGDTIteratorUtils();
}


NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	DriverObject->DriverUnload = &DriverUnload;

	RunAllTests();

	return STATUS_SUCCESS;
}
