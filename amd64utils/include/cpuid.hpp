namespace Amd64::Cpuid
{
	const auto LEAF1_ID_MANUFACTURERID_HIGHEST_FN_PARAM	= 0;
	const auto LEAF1_ID_EXTENDED_PROCESSOR_INFO_FEATURES = 0x80000001;

	// TODO: Think about this future implementation
	void Identify();
}