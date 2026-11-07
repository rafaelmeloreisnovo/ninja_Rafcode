/* Link contract for QEMU virt RAM experiment, not Android or generic HW. */
ENTRY(_start)
PHDRS {
  text PT_LOAD FLAGS(5);
  data PT_LOAD FLAGS(6);
}
SECTIONS {
  . = 0x40080000;
  __raf_image_start = .;
  .text : ALIGN(16) { KEEP(*(.text.boot)) *(.text .text.*) } :text
  .rodata : ALIGN(16) { *(.rodata .rodata.*) } :text
  .data : ALIGN(16) { *(.data .data.*) } :data
  .bss (NOLOAD) : ALIGN(16) {
    *(.bss .bss.* COMMON)
    . = ALIGN(16);
    __raf_stack_bottom = .;
    . += 0x4000;
    __raf_stack_top = .;
  } :data
  . = ALIGN(16);
  __raf_image_end = .;
  /DISCARD/ : { *(.comment) *(.note*) *(.eh_frame*) }
  ASSERT(__raf_image_end < 0x40400000, "bare-metal image exceeded scoped RAM budget")
}
