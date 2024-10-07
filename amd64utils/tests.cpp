#include <ntddk.h>
#include "include/amd64.hpp"

NTSTATUS DriverUnload()
{
	return STATUS_SUCCESS;
}

#define DBG_PREFIX "AMD64UTILS [TEST_MODULE] "

void Test_TestingGetGDTRAndLDTR()
{
	DbgPrint(DBG_PREFIX "Test 1: Testing Get GDTR and LDTR\n");

	Amd64::Segmentation::GDTRegister CurrentGDTR = { 0 };
	Amd64::Segmentation::SegmentSelector CurrentLDTR = { 0 };

	// Get LDTR and GDTR
	Amd64::Segmentation::GetGDTRegister(&CurrentGDTR);
	Amd64::Segmentation::GetLDTRegister(&CurrentLDTR);

	DbgPrint(DBG_PREFIX "GDTR Base: 0x%llx, LDT Index: 0x%hx\n", CurrentGDTR.Base, CurrentLDTR.Fields.Index);
}

void Test_TestingSegmentDescriptorStructure()
{
	DbgPrint(DBG_PREFIX "Test 2: Testing Segment Descriptor Structure\n");

	Amd64::Segmentation::SegmentDescriptor SegmentDescriptor = { 0 };

	SegmentDescriptor.Value = 0x00209b0000000000;

	DbgPrint(DBG_PREFIX "LowPart.Value: 0x%08X\n", SegmentDescriptor.Fields.LowPart.Value);
	DbgPrint(DBG_PREFIX "HighPart.Value: 0x%08X\n", SegmentDescriptor.Fields.HighPart.Value);
}

void Test_TestingGetTaskRegister()
{
	DbgPrint(DBG_PREFIX "Test 3: Testing Get Task Register\n");

	Amd64::Segmentation::SegmentSelector SegmentSelector = { 0 };

	Amd64::Segmentation::GetTaskRegister(&SegmentSelector);

	DbgPrint(DBG_PREFIX "Task Register - Index: 0x%hhx Rpl: %d Value: %hhx\n",
		SegmentSelector.Fields.Index,
		SegmentSelector.Fields.RequestPrivLevel,
		SegmentSelector.Value);
}

void Test_GetTSSAddress()
{
	DbgPrint(DBG_PREFIX "Test 4: Get TSS Base Address\n");

	const auto Tss = reinterpret_cast<Amd64::Segmentation::TaskStateSegment*>(Amd64::Segmentation::GetTSSBaseAddress());

	DbgPrint(DBG_PREFIX "Tss Base Address at: 0x%llx I/O Base Map: 0x%hhx Rsp0: 0x%llx\n",
		(uint64_t)Tss,
		Tss->IoMapBaseAddress,
		Tss->Rsp0);
}

void Test_TestingGDTIteratorUtils()
{
	DbgPrint(DBG_PREFIX "Test 5: Testing GDT Iterator Utils\n");

	Amd64::Segmentation::GDTRegister CurrentGDTR = { 0 };
	Amd64::Segmentation::GDTIterator Iterator = { 0 };

	Amd64::Segmentation::GetGDTRegister(&CurrentGDTR);

	if (!Amd64::Segmentation::GDTIteratorInit(&Iterator, &CurrentGDTR))
	{
		DbgPrint(DBG_PREFIX "Failed to initialize the GDT iterator!\n");
		return;
	}

	while (Amd64::Segmentation::GDTIteratorNext(&Iterator))
	{
		const auto CurrentSegment = reinterpret_cast<Amd64::Segmentation::SegmentDescriptor*>(Iterator.CurrentDescriptor);

		// If the pointer is invalid, continue
		if (CurrentSegment == nullptr)
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
			if (Iterator.CurrentSegmentType == Amd64::Segmentation::TSS_AVAILABLE_TYPE
				|| Iterator.CurrentSegmentType == Amd64::Segmentation::TSS_BUSY_TYPE)
			{
				const auto TssDescriptor = reinterpret_cast<Amd64::Segmentation::TSSDescriptor*>(Iterator.CurrentDescriptor);

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


void Test_TestingCheckCPUIDRunning()
{
	DbgPrint(DBG_PREFIX "Test 6: Running CPUID instruction test\n");

	Amd64::Cpuid::IdResult Result = { 0 };

	// Executing the CPUID to identify processor info features
	Amd64::Cpuid::Identify(
		Amd64::Cpuid::LF1_VIRTUAL_PHYSICAL_ADDR_SIZES, 
		0,
		Result
	);


	DbgPrint("Result, EAX: %x, EBX: %x ECX: %x, EDX: %x\n",
		Result.eax,
		Result.ebx,
		Result.ecx,
		Result.edx);
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

	// Test 6: CPUID
	Test_TestingCheckCPUIDRunning();
}

extern "C"
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;

	DriverObject->DriverUnload = reinterpret_cast<PDRIVER_UNLOAD>(&DriverUnload);

	RunAllTests();

	return STATUS_SUCCESS;
}

