An ARMv7-M implementation that includes the DSP extension is called an ARMv7E-M
implementation, and Chapter A7 Instruction Details identifies the added instructions as
ARMv7E-M instructions.

FPv5 - This extension adds optional support for double-precision computations and provides
additional instructions.

Extension | Single-precision only | Single and double-precision
----------|-----------------------|----------------------------
FPv5      | FPv5-SP-D16-M         | FPv5-D16-M


## A7.2 Standard assembler syntax fields

The following assembler syntax fields are standard across all or most instructions:

`<c>`

Is an optional field. It specifies the condition under which the instruction is executed. If `<c>` is omitted, it defaults to always (AL). For details see Conditional instructions on page A4-102.

`<q>`

Specifies optional assembler qualifiers on the instruction. The following qualifiers are defined:

`.N` Meaning narrow, specifies that the assembler must select a 16-bit encoding for the instruction. If this is not possible, an assembler error is produced.

`.W` Meaning wide, specifies that the assembler must select a 32-bit encoding for the instruction. If this is not possible, an assembler error is produced.

If neither .W nor .N is specified, the assembler can select either 16-bit or 32-bit encodings. If both are available, it must select a 16-bit encoding. In a few cases, more than one encoding of the same length can be available for an instruction. The rules for selecting between such encodings are instruction-specific and are part of the instruction description.

## A7.3 Conditional execution

Most Thumb instructions in ARMv7-M can be executed conditionally, based on the values of the APSR condition flags. The available conditions are listed in Table A7-1.

In Thumb instructions, the condition (if it is not AL) is normally encoded in a preceding IT instruction, see Conditional instructions on page A4-102, ITSTATE on page A7-177 and IT on page A7-242 for details. Some conditional branch instructions do not require a preceding IT instruction, and include a condition code in their encoding.

cond | Mnemonic extension | Meaning, integer arithmetic | Meaning, floating-point arithmetic(a) | Condition flags
-----|--------------------|-----------------------------|------------------------------|----------------------
0000 | EQ    | Equal | Equal | Z == 1
0001 | NE    | Not equal | Not equal, or unordered | Z == 0
0010 | CS(b) | Carry set | Greater than, equal, or unordered | C == 1
0011 | CC(c) | Carry clear | Less than | C == 0
0100 | MI    | Minus, negative | Less than | N == 1
0101 | PL    | Plus, positive or zero | Greater than, equal, or unordered | N == 0
0110 | VS    | Overflow | Unordered | V == 1
0111 | VC    | No overflow | Not unordered | V == 0
1000 | HI    | Unsigned higher | Greater than, or unordered | C == 1 and Z == 0
1001 | LS    | Unsigned lower or same | Less than or equal | C == 0 or Z == 1
1010 | GE    | Signed greater than or equal | Greater than or equal | N == V
1011 | LT    | Signed less than | Less than, or unordered | N != V
1100 | GT    | Signed greater than | Greater than | Z == 0 and N == V
1101 | LE    | Signed less than or equal | Less than, equal, or unordered | Z == 1 or N != V
1110 | None (AL)(d) | Always (unconditional) | Always (unconditional) | Any

(a) Unordered means at least one NaN operand.
(b) HS (unsigned higher or same) is a synonym for CS.
(c) LO (unsigned lower) is a synonym for CC.
(d) AL is an optional mnemonic extension for always, except in IT instructions. See IT on page A7-242 for details.

## B1.5.2 Execption number definition

Exception number | Exception
-----------------|----------
1                | Reset
2                | NMI
3                | HardFault
4                | MemManage
5                | BusFault
6                | UsageFault
7-10             | Reserved
11               | SVCall
12               | DebugMonitor
13               | Reserved
14               | PendSV
15               | SysTick
16               | External interrupt 0
.                | .
.                | .
.                | .
16+N             | External interrupt N

## B1.5.3 The vector table

The vector table contains the initialization value for the stack pointer, and the entry point addresses of each exception handler. The exception number, defined in Table B1-4, also defines the order of entries in the vector table, as Table B1-5 shows.

Word offset in table | Description, for all pointer address values
---------------------|--------------------------------------------
0                    | SP_main. This is the reset value of the Main stack pointer.
Exception Number     | Exception using that Exception Number

On reset, the processor initializes the vector table base address to an IMPLEMENTATION DEFINED address. Software can find the current location of the table, or relocate the table, using the VTOR, see Vector Table Offset Register, VTOR on page B3-657.

The Vector table must be naturally aligned to a power of two whose alignment value is greater than or equal to (Number of Exceptions supported x 4), with a minimum alignment of 128 bytes. On power-on or reset, the processor uses the entry at offset 0 as the initial value for SP_main, see The SP registers on page B1-572. All other entries must have bit[0] set to 1, because this bit defines the EPSR.T bit on exception entry. See Reset behavior on page B1-586 and Exception entry behavior on page B1-587 for more information.

On exception entry, if bit[0] of the associated vector table entry is set to 0, execution of the first instruction causes an INVSTATE UsageFault, see The special-purpose program status registers, xPSR on page B1-572 and Fault behavior on page B1-608. If this happens on a reset, this escalates to a HardFault, because UsageFault is disabled on reset, see Priority escalation on page B1-585 for more information

**NOTE:** When looking at the vector table each entry aside from `0x0` is an address that the processor will jump to for that specific vector; however, you need to mask off bit 0 which should always be set to 1:

	All other entries must have bit[0] set to 1, because this bit defines the EPSR.T bit on exception entry.

For example, `080013d5` becomes `080013d4` which will be a procedure to execute.

## B1.4.1 The ARM core registers

The registers R0-R12, SP, LR, and PC are named the ARM core registers. These registers can be described as R0-R15.

### The SP registers

An ARMv7-M processor implements two stacks:

- The Main stack, SP_main or MSP.
- The Process stack, SP_process or PSP.

The stack pointer, SP, banks SP_main and SP_process. The current stack depends on the mode and, in Thread mode, the value of the CONTROL.SPSEL bit, see The special-purpose CONTROL register on page B1-575. A reset selects and initializes SP_main, see Reset behavior on page B1-586.

ARMv7-M implementations treat SP bits[1:0] as RAZ/WI. ARM strongly recommends that software treats SP bits[1:0] as SBZP for maximum portability across ARMv7 profiles.

The processor selects the SP used by an instruction that references the SP explicitly according to the function LookUpSP() described in Pseudocode details of ARM core register accesses on page B1-577.

```
// The M-profile execution modes.
enumeration Mode {Mode_Thread, Mode_Handler};

// The names of the core registers. SP is a Banked register.
enumeration RName {RName0, RName1, RName2, RName3, RName4, RName5, RName6,
	RName7, RName8, RName9, RName10, RName11, RName12,
	RNameSP_main, RNameSP_process, RName_LR, RName_PC};

// The physical array of core registers.
//
// _R[RName_PC] is defined to be the address of the current instruction.
// The offset of 4 bytes is applied to it by the register access functions.

array bits(32) _R[RName];

// LookUpSP()
// ==========

RName LookUpSP()
	RName sp;
	if CONTROL.SPSEL == '1' then
		if CurrentMode==Mode_Thread then
			sp = RNameSP_process;
		else
			UNPREDICTABLE;
	else
		sp = RNameSP_main;
	return sp;
```

The stack pointer that is used in exception entry and exit is described in the pseudocode sequences of the exception entry and exit, see Exception entry behavior on page B1-587 and Exception return behavior on page B1-595.

```
arm-none-eabi-gdb bin/blink.elf
GNU gdb (GNU Tools for Arm Embedded Processors 7-2017-q4-major) 8.0.50.20171128-git
Copyright (C) 2017 Free Software Foundation, Inc.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
and "show warranty" for details.
This GDB was configured as "--host=x86_64-linux-gnu --target=arm-none-eabi".
Type "show configuration" for configuration details.
For bug reporting instructions, please see:
<http://www.gnu.org/software/gdb/bugs/>.
Find the GDB manual and other documentation resources online at:
<http://www.gnu.org/software/gdb/documentation/>.
For help, type "help".
Type "apropos word" to search for commands related to "word"...
Reading symbols from bin/blink.elf...done.
(gdb) 
(gdb) tar ext /dev/ttyACM0
/dev/ttyACM0: No such file or directory.
(gdb) tar ext /dev/ttyACM1
Remote debugging using /dev/ttyACM1
(gdb) mon
Black Magic Probe (Firmware v1.6.1-261-g88ec557) (Hardware Version 1)
Copyright (C) 2015  Black Sphere Technologies Ltd.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

(gdb) mon swdp_scan
Target voltage: 3.3V
Available Targets:
No. Att Driver
 1      STM32F76x
(gdb) load
Loading section .text, size 0x550 lma 0x8000000
Load failed
(gdb) attach 1
Attaching to program: /home/logan/Projects/ia-fc/bin/blink.elf, Remote target
0x08001d84 in ?? ()
(gdb) load
Loading section .text, size 0x550 lma 0x8000000
Start address 0x8000000, load size 1360
Transfer rate: 4 KB/sec, 680 bytes/write.
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 
r
^C
Program received signal SIGINT, Interrupt.
0x08000536 in gpio_set (gpios=2, gpioport=1073873920) at src/main.c:520
520		GPIO_BSRR(gpioport) = gpios;
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 
^C
Program received signal SIGINT, Interrupt.
0x08000536 in gpio_set (gpios=2, gpioport=1073873920) at src/main.c:520
520		GPIO_BSRR(gpioport) = gpios;
(gdb) 
(gdb) comp
compare-sections  compile           complete          
(gdb) compare-sections 
Section .text, range 0x8000000 -- 0x8000550: matched.
(gdb) make
make: Nothing to be done for 'all'.
(gdb) make
  CC      build/src/main.o
src/main.c: In function 'main':
src/main.c:642:11: warning: unused variable 'i' [-Wunused-variable]
  uint32_t i;
           ^
  OBJDUMP build/src/main.o
  LD      bin/blink.elf
  OBJDUMP bin/blink.elf
(gdb) file bin/blink.elf
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Load new symbol table from "bin/blink.elf"? (y or n) y
Reading symbols from bin/blink.elf...done.
(gdb) load
Loading section .text, size 0x550 lma 0x8000000
Start address 0x8000000, load size 1360
Transfer rate: 4 KB/sec, 680 bytes/write.
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 
^C
Program received signal SIGINT, Interrupt.
0x08000536 in gpio_set (gpios=2, gpioport=1073873920) at src/main.c:520
520		GPIO_BSRR(gpioport) = gpios;
(gdb) make
  CC      build/src/main.o
  OBJDUMP build/src/main.o
  LD      bin/blink.elf
  OBJDUMP bin/blink.elf
(gdb) file bin/blink.elf                    
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Load new symbol table from "bin/blink.elf"? (y or n) y
Reading symbols from bin/blink.elf...done.
(gdb) load
Loading section .text, size 0x57c lma 0x8000000
Start address 0x8000000, load size 1404
Transfer rate: 5 KB/sec, 702 bytes/write.
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) s
Please answer y or n.
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 
^C
Program received signal SIGINT, Interrupt.
main () at src/main.c:647
warning: Source file is more recent than executable.
647	
(gdb) 
(gdb) make
  CC      build/src/main.o
  OBJDUMP build/src/main.o
  LD      bin/blink.elf
  OBJDUMP bin/blink.elf
(gdb) file bin/blink.elf                   
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Load new symbol table from "bin/blink.elf"? (y or n) y
Reading symbols from bin/blink.elf...done.
(gdb) load
Loading section .text, size 0x4ec lma 0x8000000
Start address 0x8000000, load size 1260
Transfer rate: 3 KB/sec, 630 bytes/write.
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 
^C
Program received signal SIGINT, Interrupt.
0x0800024a in reset_handler () at src/main.c:209
warning: Source file is more recent than executable.
209		for (fp = &__init_array_start; fp < &__init_array_end; fp++) {
(gdb) 
(gdb) make
  CC      build/src/main.o
  OBJDUMP build/src/main.o
  LD      bin/blink.elf
  OBJDUMP bin/blink.elf
(gdb) stop
(gdb) reset
Undefined command: "reset".  Try "help".
(gdb) file bin/blink.elf                   
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Load new symbol table from "bin/blink.elf"? (y or n) y
Reading symbols from bin/blink.elf...done.
(gdb) load
Loading section .text, size 0x4fc lma 0x8000000
Start address 0x8000000, load size 1276
Transfer rate: 3 KB/sec, 638 bytes/write.
(gdb) start
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Temporary breakpoint 1 at 0x80004ec: file src/main.c, line 612.
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 
Note: automatically using hardware breakpoints for read-only addresses.

Temporary breakpoint 1, main () at src/main.c:614
614		rcc_peripheral_enable_clock( &RCC_AHB1ENR, BOARD_CLOCK_LEDS );
(gdb) c
Continuing.
^C
Program received signal SIGINT, Interrupt.
0x080004f6 in rcc_peripheral_enable_clock (en=4, reg=0x40023830) at src/main.c:612
612	{
(gdb) b
Breakpoint 2 at 0x80004f6: file src/main.c, line 612.
(gdb) b del
Function "del" not defined.
Make breakpoint pending on future shared library load? (y or [n]) n
(gdb) del b
Ambiguous delete command "b": bookmark, breakpoints.
(gdb) del 0
warning: bad breakpoint number at or near '0'
(gdb) del 1
No breakpoint number 1.
(gdb) del 2
(gdb) del
(gdb) clear
No breakpoint at this line.
(gdb) cls
Undefined command: "cls".  Try "help".
(gdb) clc
Undefined command: "clc".  Try "help".
(gdb) ^CQuit
(gdb) 
(gdb) make
  CC      build/src/main.o
  OBJDUMP build/src/main.o
  LD      bin/blink.elf
  OBJDUMP bin/blink.elf
(gdb) file bin/blink.elf                  
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Load new symbol table from "bin/blink.elf"? (y or n) y
Reading symbols from bin/blink.elf...done.
(gdb) 
(gdb)  
(gdb) load
Loading section .text, size 0x554 lma 0x8000000
Start address 0x8000000, load size 1364
Transfer rate: 4 KB/sec, 682 bytes/write.
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 
^C
Program received signal SIGINT, Interrupt.
0x08000538 in gpio_clear (gpios=2, gpioport=1073873920) at src/main.c:535
warning: Source file is more recent than executable.
535		GPIO_BSRR(gpioport) = (gpios << 16);
(gdb) make
  CC      build/src/main.o
  OBJDUMP build/src/main.o
  LD      bin/blink.elf
  OBJDUMP bin/blink.elf
(gdb) stop
(gdb) file bin/blink.elf                   
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Load new symbol table from "bin/blink.elf"? (y or n) y
Reading symbols from bin/blink.elf...done.
(gdb) load
Loading section .text, size 0x550 lma 0x8000000
Start address 0x8000000, load size 1360
Transfer rate: 5 KB/sec, 680 bytes/write.
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 
^C
Program received signal SIGINT, Interrupt.
0x08000536 in gpio_clear (gpios=2, gpioport=1073873920) at src/main.c:535
warning: Source file is more recent than executable.
535		GPIO_BSRR(gpioport) = (gpios << 16);
(gdb) stop
(gdb) make
  CC      build/src/main.o
  OBJDUMP build/src/main.o
  LD      bin/blink.elf
  OBJDUMP bin/blink.elf
(gdb) file bin/blink.elf                  
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Load new symbol table from "bin/blink.elf"? (y or n) y
Reading symbols from bin/blink.elf...done.
'(gdb) oad
Undefined command: "oad".  Try "help".
(gdb) load
Loading section .text, size 0x550 lma 0x8000000
Start address 0x8000000, load size 1360
Transfer rate: 4 KB/sec, 680 bytes/write.
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 
^C
Program received signal SIGINT, Interrupt.
0x08000536 in gpio_clear (gpios=2, gpioport=1073873920) at src/main.c:535
535		GPIO_BSRR(gpioport) = (gpios << 16);
(gdb) make
  CC      build/src/main.o
src/main.c: In function 'main':
src/main.c:645:9: error: 'i' undeclared (first use in this function)
   for ( i = 0; i < 500000; ++i );
         ^
src/main.c:645:9: note: each undeclared identifier is reported only once for each function it appears in
Makefile:133: recipe for target 'build/src/main.o' failed
make: *** [build/src/main.o] Error 1
(gdb) make
  CC      build/src/main.o
  OBJDUMP build/src/main.o
  LD      bin/blink.elf
  OBJDUMP bin/blink.elf
(gdb) file bin/blink.elf                  
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Load new symbol table from "bin/blink.elf"? (y or n) y
Reading symbols from bin/blink.elf...done.
(gdb) load
Loading section .text, size 0x568 lma 0x8000000
Start address 0x8000000, load size 1384
Transfer rate: 5 KB/sec, 692 bytes/write.
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 
^C
Program received signal SIGINT, Interrupt.
gpio_toggle (gpios=64, gpioport=1073874944) at src/main.c:551
warning: Source file is more recent than executable.
551		GPIO_BSRR(gpioport) = ((port & gpios) << 16) | (~port & gpios);
(gdb) make
  CC      build/src/main.o
  OBJDUMP build/src/main.o
  LD      bin/blink.elf
  OBJDUMP bin/blink.elf
(gdb) file bin/blink.elf                  
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Load new symbol table from "bin/blink.elf"? (y or n) y
Reading symbols from bin/blink.elf...done.
(gdb) load
Loading section .text, size 0x574 lma 0x8000000
Start address 0x8000000, load size 1396
Transfer rate: 4 KB/sec, 698 bytes/write.
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 
^R
make
^C
Program received signal SIGINT, Interrupt.
main () at src/main.c:645
warning: Source file is more recent than executable.
645			for ( i = 0; i < 1000000; ++i ) { asm(""); };
(gdb) make
  CC      build/src/main.o
  OBJDUMP build/src/main.o
  LD      bin/blink.elf
  OBJDUMP bin/blink.elf
(gdb) 
(gdb) file bin/blink.elf                  
A program is being debugged already.
Are you sure you want to change the file? (y or n) y
Load new symbol table from "bin/blink.elf"? (y or n) y 
Reading symbols from bin/blink.elf...done.
(gdb) load
Loading section .text, size 0x574 lma 0x8000000
Start address 0x8000000, load size 1396
Transfer rate: 4 KB/sec, 698 bytes/write.
(gdb) s
Single stepping until exit from function vector_table,
which has no line number information.
blocking_handler () at src/main.c:223
223	{
(gdb) r 
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 

Program received signal SIGINT, Interrupt.
reset_handler () at src/main.c:190
190		for ( src = &_data_loadaddr, dest = &_data; dest < &_edata; src++, dest++ ) {
(gdb) load
Loading section .text, size 0x574 lma 0x8000000
Start address 0x8000000, load size 1396
Transfer rate: 4 KB/sec, 698 bytes/write.
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 

Program terminated with signal SIGLOST, Resource lost.
The program no longer exists.
(gdb) make
  CC      build/src/main.o
  OBJDUMP build/src/main.o
  LD      bin/blink.elf
  OBJDUMP bin/blink.elf
(gdb) 
(gdb) load
`/home/logan/Projects/ia-fc/bin/blink.elf' has changed; re-reading symbols.
Loading section .text, size 0x588 lma 0x8000000
Load failed
(gdb) attach 1
Attaching to program: /home/logan/Projects/ia-fc/bin/blink.elf, Remote target
Attaching to Remote target failed
(gdb) mon
Black Magic Probe (Firmware v1.6.1-261-g88ec557) (Hardware Version 1)
Copyright (C) 2015  Black Sphere Technologies Ltd.
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>

(gdb) mon swdp_scan
Target voltage: 3.3V
Available Targets:
No. Att Driver
 1      STM32F76x
(gdb) attach 1
Attaching to program: /home/logan/Projects/ia-fc/bin/blink.elf, Remote target

Thread 1 "" stopped.
main () at src/main.c:645
645			for ( i = 0; i < 1000000; ++i ) { asm(""); };
(gdb) load
Loading section .text, size 0x588 lma 0x8000000
Start address 0x8000000, load size 1416
Transfer rate: 4 KB/sec, 708 bytes/write.
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 
^C
Program received signal SIGINT, Interrupt.
main () at src/main.c:645
645	Turn red LED on
(gdb) make
  CC      build/src/main.o
  OBJDUMP build/src/main.o
  LD      bin/blink.elf
  OBJDUMP bin/blink.elf
(gdb) load
`/home/logan/Projects/ia-fc/bin/blink.elf' has changed; re-reading symbols.
Loading section .text, size 0x5a0 lma 0x8000000
Start address 0x8000000, load size 1440
Transfer rate: 5 KB/sec, 720 bytes/write.
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 
make
^C
Program received signal SIGINT, Interrupt.
main () at src/main.c:652
warning: Source file is more recent than executable.
652			for ( i = 0; i < 1000000; ++i ) { asm(""); };
(gdb) make
  CC      build/src/main.o
  OBJDUMP build/src/main.o
  LD      bin/blink.elf
  OBJDUMP bin/blink.elf
(gdb) load
`/home/logan/Projects/ia-fc/bin/blink.elf' has changed; re-reading symbols.
Loading section .text, size 0x5a0 lma 0x8000000
Start address 0x8000000, load size 1440
Transfer rate: 4 KB/sec, 720 bytes/write.
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 
^C
Program received signal SIGINT, Interrupt.
main () at src/main.c:652
warning: Source file is more recent than executable.
652			for ( i = 0; i < 1000000; ++i ) { asm(""); };
(gdb) make
  CC      build/src/main.o
  OBJDUMP build/src/main.o
  LD      bin/blink.elf
  OBJDUMP bin/blink.elf
(gdb) load
`/home/logan/Projects/ia-fc/bin/blink.elf' has changed; re-reading symbols.
Loading section .text, size 0x5a0 lma 0x8000000
Start address 0x8000000, load size 1440
Transfer rate: 6 KB/sec, 720 bytes/write.
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 
^C
Program received signal SIGINT, Interrupt.
main () at src/main.c:652
warning: Source file is more recent than executable.
652			for ( i = 0; i < 1000000; ++i ) { asm(""); };
(gdb) make
  CC      build/src/main.o
  OBJDUMP build/src/main.o
  LD      bin/blink.elf
  OBJDUMP bin/blink.elf
(gdb) load
`/home/logan/Projects/ia-fc/bin/blink.elf' has changed; re-reading symbols.
Loading section .text, size 0x5a0 lma 0x8000000
Start address 0x8000000, load size 1440
Transfer rate: 5 KB/sec, 720 bytes/write.
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 
^C
Program received signal SIGINT, Interrupt.
main () at src/main.c:652
warning: Source file is more recent than executable.
652			for ( i = 0; i < 1000000; ++i ) { asm(""); };
(gdb) make
  CC      build/src/main.o
  OBJDUMP build/src/main.o
  LD      bin/blink.elf
  OBJDUMP bin/blink.elf
(gdb) load
`/home/logan/Projects/ia-fc/bin/blink.elf' has changed; re-reading symbols.
Loading section .text, size 0x5bc lma 0x8000000
Start address 0x8000000, load size 1468
Transfer rate: 6 KB/sec, 734 bytes/write.
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 
^C
Program received signal SIGINT, Interrupt.
main () at src/main.c:652
warning: Source file is more recent than executable.
652			for ( i = 0; i < 1000000; ++i ) { asm(""); };
(gdb) make
  CC      build/src/main.o
  OBJDUMP build/src/main.o
  LD      bin/blink.elf
  OBJDUMP bin/blink.elf
(gdb) load
`/home/logan/Projects/ia-fc/bin/blink.elf' has changed; re-reading symbols.
Loading section .text, size 0x5c4 lma 0x8000000
Start address 0x8000000, load size 1476
Transfer rate: 4 KB/sec, 738 bytes/write.
(gdb) r
The program being debugged has been started already.
Start it from the beginning? (y or n) y
Starting program: /home/logan/Projects/ia-fc/bin/blink.elf 
Error detected on fd 7
Remote communication error.  Target disconnected.: Resource temporarily unavailable.
(gdb) tar ext /dev/ttyACM1
Remote debugging using /dev/ttyACM1
^CQuit
(gdb) tar ext /dev/ttyACM2
/dev/ttyACM2: No such file or directory.
(gdb) ^CQuit
(gdb) tar ext /dev/ttyACM0
Remote debugging using /dev/ttyACM0
(gdb) mon swdp_scan
Target voltage: 3.3V
Available Targets:
No. Att Driver
 1      STM32F76x
(gdb) attach 1
Attaching to program: /home/logan/Projects/ia-fc/bin/blink.elf, Remote target

Program stopped.
main () at src/main.c:652
652			for ( i = 0; i < 1000000; ++i ) { asm(""); };
(gdb) c
Continuing.
^C
Program received signal SIGINT, Interrupt.
main () at src/main.c:652
652			for ( i = 0; i < 1000000; ++i ) { asm(""); };
(gdb) x  
Argument required (starting display address).
(gdb) x 0x20020000
0x20020000:	0x6766b11e
(gdb) stack
Undefined command: "stack".  Try "help".
(gdb) x 0x40020800
0x40020800:	Cannot access memory at address 0x40020800
(gdb) set mem inaccessible-by-default off
(gdb) x 0x40020800                       
0x40020800:	0x00005000
(gdb) x 0x40020818
0x40020818:	0x00000000
(gdb) set {unsigned int}0x0x40020818 = 0xC0
Invalid number "0x0x40020818".
(gdb) set {unsigned int}0x40020818 = 0xC0  
(gdb) set {unsigned int}0x40020818 = 0xC00000
(gdb) set {unsigned int}0x40020818 = 0xC0    
(gdb) set {unsigned int}0x40020818 = 0xC00000
(gdb) set {unsigned int}0x40020818 = 0xC0    
(gdb) set {unsigned int}0x40020818 = 0xC00000
```
