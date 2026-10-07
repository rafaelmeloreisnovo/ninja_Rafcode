/* Copyright 2026 Rafael Melo Reis
 * SPDX-License-Identifier: Apache-2.0
 */
ENTRY(rafk_step)
SECTIONS {
  . = 0x20000;
  .rafcode.text ALIGN(16) : {
    KEEP(*(.rafcode.text.core))
    KEEP(*(.rafcode.text.meta))
    KEEP(*(.rafcode.text.arch))
    *(.text .text.*)
  }
  .rafcode.rodata ALIGN(16) : { *(.rodata .rodata.*) }
  .rafcode.data ALIGN(16) : { *(.data .data.*) }
  .rafcode.bss ALIGN(16) (NOLOAD) : { *(.bss .bss.*) *(COMMON) }
  /DISCARD/ : {
    *(.comment) *(.note .note.*) *(.eh_frame .eh_frame.*)
    *(.interp) *(.dynamic) *(.dynsym) *(.dynstr)
    *(.gnu.hash) *(.hash) *(.got .got.*) *(.plt .plt.*)
  }
}
