/*
********************************************************************************
*                                                                              *
* Copyright (c) 2017 Andrea Loi                                                *
*                                                                              *
* Permission is hereby granted, free of charge, to any person obtaining a      *
* copy of this software and associated documentation files (the "Software"),   *
* to deal in the Software without restriction, including without limitation    *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,     *
* and/or sell copies of the Software, and to permit persons to whom the        *
* Software is furnished to do so, subject to the following conditions:         *
*                                                                              *
* The above copyright notice and this permission notice shall be included      *
* in all copies or substantial portions of the Software.                       *
*                                                                              *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL      *
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER   *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING      *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER          *
* DEALINGS IN THE SOFTWARE.                                                    *
*                                                                              *
********************************************************************************
*/



/******************************************************************************/
/*          DON'T EDIT THIS FILE UNLESS YOU KNOW WHAT YOU'RE DOING!           */
/******************************************************************************/



_heap_size =          0x00001000;
_stack_size =         0x00000400;

_isrvectors_tend =    0x00000298;

ENTRY(reset_handler)

MEMORY {
  FLASH (RX) : ORIGIN = 0x08000000, LENGTH = 128K
  RAM (RWX)  : ORIGIN = 0x20000000, LENGTH = 1024K /*0x05000*/
}

SECTIONS {
  .isrvectors :
  {
    _sisrvectors = .;
    KEEP(*(.isrvectors))
    ASSERT(. == _isrvectors_tend, "The vector table needs to be 166 elements long!");
    _eisrvectors = .;
  } >FLASH

  /* The program code and other data goes into FLASH */
  .text :
  {
    _stext = .;
    . = ALIGN(4);
    *(.text)           /* .text sections (code) */
    *(.text*)          /* .text* sections (code) */
    _etext = .;        /* define a global symbols at end of code */
    *(.glue_7)         /* glue arm to thumb code */
    *(.glue_7t)        /* glue thumb to arm code */
    *(.eh_frame)

    KEEP (*(.init))
    KEEP (*(.fini))

    . = ALIGN(4);
  } >FLASH
  
  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } >FLASH
  
  .ARM : {
    *(.ARM.exidx*)
  } >FLASH 
  
  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } >RAM AT >FLASH

  _ldata = LOADADDR(.data);

  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    end = _ebss;
  } 
  .ARM.exidx : {
    __exidx_start = .;
    *(.ARM.exidx* .gnu.linkonce.armexidx.*) 
    __exidx_end = .;
  } >FLASH
  _etext = .;
  
  ._heap_trick :
  {
    . = ALIGN(4);
    . = . + _heap_size;
    . = ALIGN(4);
  } >RAM

  ._stack_trick :
  {
    . = ALIGN(8);
    . = . + _stack_size;
    . = ALIGN(8);
  } >RAM
  
}


