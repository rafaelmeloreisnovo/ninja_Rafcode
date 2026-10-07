/* Copyright 2026 Rafael Melo Reis | SPDX-License-Identifier: Apache-2.0 */
ENTRY(raf_l0_observe)
SECTIONS
{
  . = 0x00010000;
  .rafcode.text ALIGN(16) : {
    KEEP(*(.rafcode.text.l0))
    KEEP(*(.rafcode.text.asm))
    *(.text .text.*)
  }
  .rafcode.rodata ALIGN(16) : { *(.rodata .rodata.*) }
  .rafcode.data ALIGN(16) : { *(.data .data.*) }
  .rafcode.bss ALIGN(16) (NOLOAD) : { *(.bss .bss.*) *(COMMON) }
  /DISCARD/ : {
    *(.comment)
    *(.note .note.*)
    *(.eh_frame .eh_frame.*)
    *(.interp)
    *(.dynamic)
    *(.dynsym)
    *(.dynstr)
    *(.gnu.hash)
    *(.hash)
    *(.got .got.*)
    *(.plt .plt.*)
  }
}
