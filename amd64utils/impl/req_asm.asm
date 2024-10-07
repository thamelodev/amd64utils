.code

INTERN_GETGDTR PROC
	sgdt QWORD PTR [rcx]
	ret
INTERN_GETGDTR ENDP

INTERN_GETLDTR PROC
	sldt rcx
	ret
INTERN_GETLDTR ENDP

INTERN_GETTASKREGISTER PROC
	str WORD PTR [rcx]
	ret
INTERN_GETTASKREGISTER ENDP


INTERN_CPUID PROC
	mov eax, ecx
	mov ecx, edx
	cpuid
	mov DWORD PTR [r8], eax
	mov DWORD PTR [r8+04h], ebx
	mov DWORD PTR [r8+08h], ecx
	mov DWORD PTR [r8+0ch], edx
	ret
INTERN_CPUID ENDP

end