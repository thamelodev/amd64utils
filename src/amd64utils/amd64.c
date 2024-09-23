#include "amd64.h"

/*
 * x86-64 Masm Assembly Procedure will be below with the prefix INTERN_ (Internals)
*/

extern INTERN_GETGDTR(AMD64_PGDTR CurrentGDTROUT);
extern INTERN_GETLDTR(AMD64_PSegSelector CurrentLDTROUT);

void AMD64_GetGDTR(AMD64_PGDTR CurrentGDTR)
{
	INTERN_GETGDTR(CurrentGDTR);
}

void AMD64_GetLDTR(AMD64_PSegSelector CurrentLDTR)
{
	INTERN_GETLDTR(CurrentLDTR);
}
