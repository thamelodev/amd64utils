#include "amd64.h"

/*
 * x86-64 Masm Assembly Procedure will be below with the prefix INTERN_ (Internals)
*/

extern INTERN_GETGDTR(AMD64_PGDTR CurrentGDTROUT);

void AMD64_GetGDTR(AMD64_PGDTR CurrentGDTR)
{
	INTERN_GETGDTR(CurrentGDTR);
}
