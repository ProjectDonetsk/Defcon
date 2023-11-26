PUBLIC _spoofer_stub
PUBLIC _silent_crash
PUBLIC _checksum_fake
PUBLIC _retcc_stub
.code


_ZwQueryInformationThread PROC
mov eax, 25h
mov r10, rcx
test byte ptr [7FFE0308h], 1
jnz break
syscall
ret
break:
int 2Eh
ret
_ZwQueryInformationThread ENDP

_NtQueryInformationProcess PROC
mov eax, 19h
mov r10, rcx
test byte ptr [7FFE0308h], 1
jnz break
syscall
ret
break:
int 2Eh
ret
_NtQueryInformationProcess ENDP

_NtClose PROC
mov eax, 0Fh
mov r10, rcx
test byte ptr [7FFE0308h], 1
jnz break
syscall
ret
break:
int 2Eh
ret
_NtClose ENDP

_NtUserBuildHwndList PROC
mov eax, 101Ch
mov r10, rcx
test byte ptr [7FFE0308h], 1
jnz break
syscall
ret
break:
int 2Eh
ret
_NtUserBuildHwndList ENDP

_ZwTerminateProcess PROC
mov eax, 2Ch
mov r10, rcx
test byte ptr [7FFE0308h], 1
jnz break
syscall
ret
break:
int 2Eh
ret
_ZwTerminateProcess ENDP

_spoofer_stub PROC
	pop r11
	pop r10
	mov rax, [rsp + 24]

	push rdi
	mov rdi, rsp
	pop rdi

	mov r10, [rax]
	mov [rsp], r10

	mov r10, [rax + 8]
	mov [rax + 8], r11

	push r10
	mov r10, r11
	pop r10

	mov [rax + 16], rbx
	lea rbx, fixup
	push rdi
	mov rdi, rbx
	pop rdi
	mov [rax], rbx
	mov rbx, rax

	jmp r10

fixup:
	sub rsp, 16
	mov rcx, rbx

	push rbx
	mov rbx, rdi
	pop rbx

	mov rbx, [rcx + 16]
	jmp QWORD PTR [rcx + 8]
_spoofer_stub ENDP

_silent_crash PROC
	mov rax, 0
	mov rbx, 0
	mov rdx, 0
	mov rsi, 0
	mov rdi, 0
	mov rbp, 0
	mov rsp, 0
	mov r8, 0
	mov r9, 0
	mov r10, 0
	mov r11, 0
	mov r12, 0
	mov r13, 0
	mov r14, 0
	mov r15, 0
	jmp rcx
_silent_crash ENDP

_checksum_fake PROC
	mov rax, 0
	ret
_checksum_fake ENDP

_retcc_stub PROC
	mov rcx, rdi
	mov rdx, [rsp + 70h]
	nop
	nop
	nop
	nop
	nop
_retcc_stub ENDP

END