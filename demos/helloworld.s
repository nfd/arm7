.section .text
.org 0
.global _start
_start:
	// Write the message to address 0x10000
	mov r0, #0x10000
	ldr r1, =msg          // Load the address of msg into r1
	ldr r2, =len          // Load the length of msg into r2
	// copy value to address
copy_loop:
	ldrb r3, [r1], #1     // Load byte from msg into r3 and increment
	strb r3, [r0]         // Write to debug-out
	subs r2, r2, #1       // Decrement length counter
	bne copy_loop         // If not zero, repeat

	// Exit the program
	mov r7, #1            // syscall: exit
	mov r0, #0            // status: 0
	swi 0                 // make syscall

msg:    .asciz "Hello, World!\n"
len = . - msg
