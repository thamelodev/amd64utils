
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

	DbgPrint("GDTR Base: 0x%llx, LDT Index: 0x%hx", CurrentGDTR.Base, CurrentLDTR.b.Index);

	AMD64_SegDescriptor SegmentDescriptor = { 0 };

	SegmentDescriptor.Value = 0x00209b0000000000;

	DbgPrint("LowPart.Value: 0x%08X\n", SegmentDescriptor.Fields.LowPart.Value);
	DbgPrint("HighPart.Value: 0x%08X\n", SegmentDescriptor.Fields.HighPart.Value);
}


NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	DriverObject->DriverUnload = &DriverUnload;

	DoTests();

	return STATUS_SUCCESS;
}
