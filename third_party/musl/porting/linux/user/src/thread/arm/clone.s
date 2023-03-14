.syntax unified
.text
.global __clone
.hidden __clone
.type   __clone,%function
.cfi_startproc
__clone:
	stmfd sp!,{r4,r5,r6,r7}
	.cfi_def_cfa_offset 16
	.cfi_rel_offset r4, 0
	.cfi_rel_offset r5, 4
	.cfi_rel_offset r6, 8
	.cfi_rel_offset r7, 12
	mov r7,#120
	mov r6,r3
	mov r5,r0
	mov r0,r2
	and r1,r1,#-16
	ldr r2,[sp,#16]
	ldr r3,[sp,#20]
	ldr r4,[sp,#24]
	svc 0
	tst r0,r0
	beq 1f
	ldmfd sp!,{r4,r5,r6,r7}
	.cfi_def_cfa_offset 0
	bx lr

1:	mov r0,r6
	bl 3f
2:	mov r7,#1
	svc 0
	b 2b

3:	bx r5
.cfi_endproc