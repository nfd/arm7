.org 0x0
.section .text_start, "x"
.global _start
_start:
bl main
swi 0x11
