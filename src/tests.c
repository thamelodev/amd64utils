
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

	AMD64_GetGDTR(&CurrentGDTR);
	AMD64_GetLDTR(&CurrentLDTR);

	DbgPrint("GDTR Base: 0x%llx, LDT Index: 0x%hx", CurrentGDTR.Base, CurrentLDTR.Fields.Index);

	AMD64_SegDescriptor SegmentDescriptor = { 0 };

	SegmentDescriptor.Value = 0x00209b0000000000;

	DbgPrint("LowPart.Value: 0x%08X\n", SegmentDescriptor.Fields.LowPart.Value);
	DbgPrint("HighPart.Value: 0x%08X\n", SegmentDescriptor.Fields.HighPart.Value);


	AMD64_UTILS_GDTIterator Iterator = { 0 };

	if (!AMD64_UTILS_GDTIteratorInit(&Iterator, &CurrentGDTR))
	{
		DbgPrint("Failed to initialize the GDT iterator!\n");
		return;
	}

	while (AMD64_UTILS_GDTIteratorNext(&Iterator))
	{
		// Debug Only
		// DbgPrint("Overlaping: 0x%llx > 0x%llx\n", (Iterator.BaseAddress + Iterator.CurrentOffset), (Iterator.BaseAddress + Iterator.Limit));

		// If TSS Found
		if (!Iterator.CurrentSegmentIsNonSystem
			&& Iterator.CurrentSegmentType == AMD64_TSS_AVAILABLE_TYPE
			|| Iterator.CurrentSegmentType == AMD64_TSS_BUSY_TYPE)
		{
			continue;
		}

		// Non-System Current Segment Descriptor
		AMD64_PSegDescriptor CurrentSegment = (AMD64_PSegDescriptor) Iterator.CurrentDescriptor;

		DbgPrint("Non-System Segment - Segment Type: %d Non-System: %d\n", 
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
