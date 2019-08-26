
## XPS13

### Register Class

I've been dreaming of a "register" class implementation for a long time. The magic of templates, C++ classes, and embedded - wonderful.

[Here's](https://stackoverflow.com/questions/28604580/registers-as-template-parameter) a set of SO answers for some inspiration. I've got a lot of partial implementations and thoughts spread across this machine and the System76 one. I need to condense them and make some progress at some point.

### Device Tree

- [Engaging Device Trees - Geert Uytterhoeven](https://www.youtube.com/watch?v=MlYeT_nUK4Y)
	+ [Slideshow](https://elinux.org/images/e/e2/Engaging_Device_Trees_0.pdf)

### Teensy SWD Debugging

I soldered onto the SWD lines of the Teensy and am trying to get the black magic probe to talk to it. So far I haven't been successful but I'm not sure if it's a signal integrity thing or something else (need to scope / logic probe it).

I used [these instructions](https://mcuoneclipse.com/2014/08/09/hacking-the-teensy-v3-1-for-swd-debugging/) and [this schematic](https://www.pjrc.com/teensy/schematic32.gif).

If I get this working then here's the [K20 sub family datasheet](https://www.nxp.com/docs/en/reference-manual/K20P64M72SF1RM.pdf) and here's the [K20P64M72SF1 datasheet](http://cache.freescale.com/files/32bit/doc/data_sheet/K20P64M72SF1.pdf).

### D Programming Language

I came across the article [D as a C Replacement](https://theartofmachinery.com/2019/04/05/d_as_c_replacement.html) on reddit. Naturally, I couldn't help but click that title as a long-time C user always looking for something that will solve my language woes. The article is a good introduction to D and provides lots of useful improvements relative to C. It of course also lead me to open multiple other tabs:

- [D Language: const(FAQ)](https://dlang.org/articles/const-faq.html)
- [D Language: Compilers](https://wiki.dlang.org/Compilers)
- [D Language: GDC (compiler)](https://wiki.dlang.org/GDC)
- [minlibd](https://bitbucket.org/timosi/minlibd/src/default/)
- [Bare Metal ARM Cortex-M GDC Cross Compiler](https://wiki.dlang.org/Bare_Metal_ARM_Cortex-M_GDC_Cross_Compiler)
- [Minimal semihosted ARM Cortex-M "Hello World"](https://wiki.dlang.org/Minimal_semihosted_ARM_Cortex-M_%22Hello_World%22)

So, a quick starter project would be to follow the bare metal ARM stuff above and see what I can do (e.g. blink an LED).

### BBB Mare Metal

- [RISC-V from scratch 2: Hardware layouts, linker scripts, and C runtimes](https://twilco.github.io/riscv-from-scratch/2019/04/27/riscv-from-scratch-2.html) - Reference for another architecture I understand a little better than ARM
- [Talk by Matt Godbolt: The Bits Between the Bits: How We Get to main()](https://www.youtube.com/watch?v=dOfucXtyEsU)
- [Exploring Startup Implementations: Newlib (ARM)](https://embeddedartistry.com/blog/2019/4/16/exploring-startup-implementations-newlib-arm)

#### AM335x OpenOCD and GDB

Unclear if this actually works or not - I haven't tried any of it yet. We have the XDS110 at work, which is the TI JTAG adapter. I want to use Linux tools to do the debugging, so that's OpenOCD and GDB.

I found the file [`openOCD/tcl/target/am335x.cfg`](https://github.com/openrisc/openOCD/blob/master/tcl/target/am335x.cfg) in the openrisc fork of OpenOCD.

I also found some mentions of the AM335x in [`ntfreak/openocd`](https://github.com/ntfreak/openocd/search?utf8=%E2%9C%93&q=am335x&type=).

**NOTE**: The blackmagic probe supports [Cortex-A](https://github.com/blacksphere/blackmagic/blob/master/src/target/cortexa.c) but I haven't tried that out.

#### Blink (Assembly)

Before I made my own BBB bare metal repository I started a project to blink an LED purely in assembly based on a different BBB bare metal repo.

~/src/BBB-BareMetal/blink

#### Blink (C)

Source: `~/src/bbb-bare-metal/blink-c`

I dug into why the C++ version of this code didn't work and found that the for loop used to generate a visible delay between turning the LED on and off was being optimized out in the C++ version but not the C version.

I created two annotated versions of the disassembly because I couldn't for the life of me figure out what was different - it wasn't until digging into the disassembly that I noticed the loop was missing in the C++ version.

~/src/bbb-bare-metal/blink-c/docs/blink.cpp.elf.list
~/src/bbb-bare-metal/blink-c/docs/blink.c.elf.list

NOTE: Why do I need to add the `asm("")` to have this loop not optimized away when using `-O3` with gcc (?) This loop appears to have no effects, I guess, it just sets `i` to a value and counts it down to 0. So, what's the proper way of creating a loop like this?

```c
for ( uint32_t i = 5000000; i > 0; --i ) { asm(""); }
for ( volatile uint32_t i = 5000000; i > 0; --i );
```

Ok, so it seems that it is what I think - the optimizer is doing it's job and removing a "useless" loop. You can bypass this by making the counter variable `volatile` or using the `asm` method below (not sure on trade-off between just `asm` and `asm volatile`). Looking at the generated assembly the later method is more efficient; I didn't look at the code too closely but I'm guessing that making the counter volatile has other side-effects (I could dig into this some other time).

What's interesting about this is that the C compiler *doesn't* optimize this out but the C++ compiler *does*.
Really the correct solution is to use the counter/timer facilities of the processor to generate *actual* delays rather than busy-wait

```c
for ( uint32_t i = 5000000; i > 0; --i ) { __asm__ __volatile__(""); } 
```

```c
__udelay( 250 * 1000 );
```

It was also in this project that I started playing around with my templated C++ `Register` class.

~/src/bbb-bare-metal/blink-c/dev/register.cpp

### BFD

I started playing with the BFD library with the intent of building a disassembler / annotation tool / various assembly performance tooling. However, i figured out pretty quickly that the BFD library is a bit complicated, kind of hard to link against, and not necessarily what I need for my project. Basically everything I do is going to be ELF, so I could use libelf instead. Probably more educational, anyway.

~/src/bbb-bare-metal/blink-c/dev/bfd
~/Downloads/binutils-2.31.1/binutils/objdump.c
~/Downloads/binutils-2.31.1/binutils/objdump.h
~/Downloads/binutils-2.31.1/binutils/bucomm.c
~/Downloads/binutils-2.31.1/ld/ldlang.c

Generate PDF documentation: `~/src/gcc-dev/binutils-2.31.1/obj-avr/bfd/make pdf`

### GNU Linker

I've been reading up on the linker for various projects in this document - u-boot, bare metal, GCC, AVR, etc. It's starting to make a bit more sense but I've got a ways to go.

ARM has extensions for ELF when linking - it's defined in [ELF for the ARM Architecture](http://infocenter.arm.com/help/topic/com.arm.doc.ihi0044f/IHI0044F_aaelf.pdf). This is useful for understanding what sections you might need to include in your linker script - like `.ARM.attributes` or `.ARM.extab`.

### General u-boot

~/Desktop/u-boot-Notes.md
~/Downloads/Build-BBB-Image.md
~/Desktop/TFTP-boot-BBB.md

The [u-boot overview](https://wiki.st.com/stm32mpu/wiki/U-Boot_overview) on STs website is actually pretty good.

It's useful to know that you can script stuff to u-boot with Kermit. [Here's an example](https://www.emcraft.com/stm32f429discovery/loading-linux-images-over-uart) of a script that configures Kermit and loads an image using `loadb`.

#### AM335x u-boot

I want to get [Netboot](https://github.com/LeMaker/u-boot/blob/master/doc/SPL/README.am335x-network) working so I can do TFTP boot in SPL.

- [TI: AM335x board bringup tips](http://processors.wiki.ti.com/index.php/AM335x_board_bringup_tips)
- [TI: AM335x U-Boot User's Guide](http://processors.wiki.ti.com/index.php/AM335x_U-Boot_User%27s_Guide#Boot_Over_UART)
- 

### STM32 Bare Metal

Before I started playing with u-boot on STM32 I was working on running bare metal STM32F7 code. I based a lot of the initial work on the PX4 bootloader since it's bare metal and supports the STM32F7.

~/src/px4-bootloader/
	main_f7.c
	Makefile
	rules.mk
	Makefile.f7
	stm32f7.ld
	hw_config.h
	libopencm3/
		include/
			libopencm3/
				stm32/f7/
					irq.json
				cm3
					nvic.h
					vector.h
			libopencmsis/stm32/f7/
				irqhandlers.h
		lib/
			cm3/
				vector.c
			stm32/f7/
				rcc.c

Using ST-Link:

```bash
$ st-flash --reset read flysky_fsi6s_dump_stlink.bin 0x8000000 0x20000

$ st-flash write bin/blink.bin 0x8000000
```

#### Helpful GCC options

`-nostdlib`

	Do not use the standard system startup files or libraries when linking. No startup files and only the libraries you specify will be passed to the linker, and options specifying linkage of the system libraries, such as -static-libgcc or -shared-libgcc, are ignored.

`-nodefaultlibs`

	Do not use the standard system libraries when linking. Only the libraries you specify will be passed to the linker, options specifying linkage of the system libraries, such as -static-libgcc or -shared-libgcc, will be ignored. The standard startup files are used normally, unless -nostartfiles is used. The compiler may generate calls to memcmp, memset, memcpy and memmove. These entries are usually resolved by entries in libc. These entry points should be supplied through some other mechanism when this option is specified.

There's even more awesome information in [Guide to Bare Metal Programming with GCC](http://cs107e.github.io/guides/gcc/) - I should include a copy of this entire thing on my site (in case it disappears some day).

#### Startup

[Here's](https://github.com/arobenko/embxx_on_rpi/blob/master/src/raspberrypi.ld) an example of a linker script for the RaspberryPi. And [here's](https://github.com/arobenko/embxx_on_rpi/blob/master/src/asm/startup.s) the matching startup code for that same implementation.

### STM32 u-boot

There are also drivers implemented in the Kernel, could be worth a read.

`~/src/linux/drivers$ find . -name '*stm32*'`

```bash
export PATH=/opt/gcc-arm-none-eabi-8-2019-q3-update/bin:$PATH
make ARCH=arm CROSS_COMPILE=arm-none-eabi-
make ARCH=arm CROSS_COMPILE=arm-none-eabi- menuconfig
make ARCH=arm CROSS_COMPILE=arm-none-eabi- stm32f765-pixhawk4_defconfig
```

[Henry Choi: u-boot on STM32F7](http://henryomd.blogspot.com/2015/12/u-boot-on-stm32f7.html)

#### STM32F429 Discovery

Reference platform already implemented by u-boot. Need to test.

```bash
# Defaults to :4242
# Add `-v99` to get maximum debugging info
$ st-util
# Note: GCC 8.3 (arm-none-eabi) didn't work with st-util; GCC 6.5 (arm-linux-gnueabihf) did work
# Note: You'll need to include a binary for this to actually be useful
$ export PATH=/opt/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin:$PATH
$ arm-linux-gnueabihf-gdb
(gdb) tar ext :4242
```

[Here's](https://patchwork.ozlabs.org/patch/764135/) a patch to u-boot that supports XIP for the STM32F746 Discovery board.

[STM32F75xxx and STM32F7xxx Reference Manual](https://www.st.com/content/ccc/resource/technical/document/reference_manual/c5/cf/ef/52/c0/f1/4b/fa/DM00124865.pdf/files/DM00124865.pdf/jcr:content/translations/en.DM00124865.pdf)

I used the TRM to determine the memory boundaries of SRAM on the STM32F746 which told me that I was putting my stack in the wrong spot on the STM32F765 (SRAM2 ends at 0x20050000 on the 46).

### GCC Internals

This awesome presentation [GCC Internals](https://www.airs.com/dnovillo/200711-GCC-Internals/200711-GCC-Internals-1-condensed.pdf) from Diego Novillo at Google is a good reference, especially the section on how to debug `gcc` (or rather, how not to - you should debug the tools it drives, like `cc1`).

Building GCC

```bash
PREFIX=/home/logan/src/gcc-dev/avr
PATH=$PREFIX/bin:$PATH

# Build binutils
mkdir obj-avr
cd obj-avr
../configure --prefix=$PREFIX --target=avr --disable-nls
make -j4
make install

# Build GCC
mkdir gcc-8.3.0-build
cd gcc-8.3.0-build
../configure --prefix=$PREFIX --target=avr --enable-languages=c,c++ --disable-nls --disable-libssp --with-dwarf2
make -j4
make install

# Debug GCC
/home/logan/src/gcc-dev/avr/libexec/gcc/avr/8.3.0/cc1 -E -quiet -v -I /usr/lib/avr/include -imultilib avr6 -D__AVR_ATmega2560__ -D__AVR_DEVICE_NAME__=atmega2560 -D F_CPU=16000000 avr.c -mn-flash=4 -mno-skip-bug -mmcu=avr6 -std=gnu99 -Os -fpch-preprocess -o avr.i
```

How GCC was configured for `avr-gcc`:

```
Configured with: ../gcc/configure -v --enable-languages=c,c++ --prefix=/usr/lib --infodir=/usr/share/info --mandir=/usr/share/man --bindir=/usr/bin --libexecdir=/usr/lib --libdir=/usr/lib --enable-shared --with-system-zlib --enable-long-long --enable-nls --without-included-gettext --disable-libssp --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=avr CFLAGS='-g -O2 -fdebug-prefix-map=/build/gcc-avr-8S_w1j/gcc-avr-5.4.0+Atmel3.6.1=. -fstack-protector-strong -Wformat ' CPPFLAGS='-Wdate-time -D_FORTIFY_SOURCE=2' CXXFLAGS='-g -O2 -fdebug-prefix-map=/build/gcc-avr-8S_w1j/gcc-avr-5.4.0+Atmel3.6.1=. -fstack-protector-strong -Wformat ' FCFLAGS='-g -O2 -fdebug-prefix-map=/build/gcc-avr-8S_w1j/gcc-avr-5.4.0+Atmel3.6.1=. -fstack-protector-strong' FFLAGS='-g -O2 -fdebug-prefix-map=/build/gcc-avr-8S_w1j/gcc-avr-5.4.0+Atmel3.6.1=. -fstack-protector-strong' GCJFLAGS='-g -O2 -fdebug-prefix-map=/build/gcc-avr-8S_w1j/gcc-avr-5.4.0+Atmel3.6.1=. -fstack-protector-strong' LDFLAGS='-Wl,-Bsymbolic-functions -Wl,-z,relro' OBJCFLAGS='-g -O2 -fdebug-prefix-map=/build/gcc-avr-8S_w1j/gcc-avr-5.4.0+Atmel3.6.1=. -fstack-protector-strong -Wformat ' OBJCXXFLAGS='-g -O2 -fdebug-prefix-map=/build/gcc-avr-8S_w1j/gcc-avr-5.4.0+Atmel3.6.1=. -fstack-protector-strong -Wformat '
```

```bash
# Compiling an example file and debugging cc1
$ ~/src/gcc-dev/example-code$ avr-gcc -Os -DF_CPU=16000000 -mmcu=atmega2560 -std=gnu99 -c avr.c -v -save-temps -Wa,-adhln -I/usr/lib/avr/include
$ ~/src/gcc-dev/example-code$ gdb --args /home/logan/src/gcc-dev/avr/libexec/gcc/avr/8.3.0/cc1 -fpreprocessed avr.i -mn-flash=4 -mno-skip-bug -quiet -dumpbase avr.c -mmcu=avr6 -auxbase avr -Os -std=gnu99 -version -o avr.s

# Searching inside files in gcc source using grep
$ grep -r --include=*.[ch] --exclude-dir=config "ASM_GENERATE_INTERNAL_LABEL" -2 -n --color=always | less -R

# Generating different assembly primitives from C code like vector tables, bss section, data section, global symbols, etc.
$ /usr/bin/avr-gcc -O0 -DF_CPU=16000000 -mmcu=atmega2560 -std=gnu99 avr_main.c -v -save-temps -Wa,-adhln -o avr_main.elf
$ avr-objdump avr_main.o -D
$ readelf -r avr_main.o -s -h -S

# Using custom version of avr-libc to test parts of vector table generation
$ cd ~/src/avr-libc-1.8.1/build/avr/lib/avr6/atmega2560
$ make
```

The GCC source tree contains [GCC Compiler Collection Internals](https://gcc.gnu.org/onlinedocs/gcc-5.4.0/gccint.pdf) which you can build from the build directory using the PDF makefile option (TODO: document this).

#### Machine Descriptions

The core of supporting multiple architectures is GCC's "machine description" framework. This is a Lisp-based language with the following purpose:

	The .md file for a target machine contains a pattern for each instruction that the target machine supports (or at least each instruction that is worth telling the compiler about)

#### AVR as a case study

C-specific (but also `cpp` it seems) AVR GCC functionality: `~/src/gcc-dev/gcc-8.3.0/gcc/config/avr/avr-c.c`. One very useful function to know is in this file is `avr_cpu_cpp_builtins` which is what generates all of the AVR-specific preprocessor defines like `__AVR_HAVE_RAMPD__` and `__AVR_MEGA__`.

GCC has "machine description" files that describe the instructions available for an architecture (read more [here](https://gcc.gnu.org/onlinedocs//gccint/Overview.html#Overview)). The AVR MD is `gcc/config/avr/avr.md`. One interesting note from the AVR description is the pseudo instruction `gasisr` which is a GNU assembler specific instruction to generate the prologue and epilogue for ISRs. The machine description file is passed to `genoutput` (`gcc/genoutput.c`) which generates the file `<build>/gcc/insn-output.c` which does the translation from RTL to instructions. 

GAS assembler for AVR:

- `~/src/gcc-dev/binutils-2.32/gas/config/tc-avr.c`
- `~/src/gcc-dev/binutils-2.32/gas/config/tc-avr.h`

I have the file `<build>/gcc/target-hooks-def.h` open which I don't remember exactly how it comes into play - it has a bunch of defines in it that control how GCC does stuff for a target but I don't remember why I looked at it. It's generated by `gcc/genhooks.c`.

##### Where does the vector table come from?

Short answer: avr-libc gcrt1.S

I came across the concept of "relocations" while reviewing objects generated by my research. Relocatable sections are an ELF thing (well, the concept is in a lot of object file formats, but I care about ELF in this case). Support for specific types of relocations are found in these source files:

- `~/src/gcc-dev/binutils-2.32/include/elf/avr.h`
- `~/src/gcc-dev/binutils-2.32/bfd/elf-avr.h`

For example, the relocation `R_AVR_LDI` has number 19 (which is what is stored in the ELF field) is implemented in `elf32-avr.c` in the parts of the file where you can fine `R_AVR_LDI` (I don't know exactly how this works yet).

The actual vector table is defined in `avr-libc-1.8.1/crt1/gcrt1.S`. The section `.vectors` is created and a global label `__vectors` is defined in it. Each entry that follows uses the `vector` macro defined in this file:

```gas
.macro	vector name
.if (. - __vectors < _VECTORS_SIZE)
.weak	\name
.set	\name, __bad_interrupt
XJMP	\name
.endif
.endm
```

And is used like this:

```gas
	.section .vectors,"ax",@progbits
	.global	__vectors
	.func	__vectors
__vectors:
	XJMP	__init
	vector	__vector_1
	# ...
```

Which does some important stuff:

- Only define entries for vectors that are supported by the target architecture. This is done by subtracting the current address from the `__vectors` label address and making sure it less than `_VECTORS_SIZE` which is defined in the header for the part(s) that the library is being compiled for (e.g. `avr/iomxx0_1.h`). It's important to note that a library is generated for every avr-libc supported part, e.g. `<build>/avr/lib/avr6/atmega2560/libcrt.a`, which is how this vector table with variable number of entires works.
- Set the vector name (e.g. `__vector_1`) to be a `weak` symbol which means that a non-`weak` symbol declared in another object file would override the definition from this file. This allows the library to contain a default implementation of the function that can be replaced by a user function defined outside of the library
- Assign a default interrupt handler, `__bad_interrupt`, to the vector name that will handle unexpected interrupts (e.g. interrupts enabled by accident or interrupts intentionally enabled but that a handler was not implemented for by accident)
- Emit the assembly instruction `XJMP` to jump to the interrupt handler

**Q:** What's `@progbits`?

**A:** [`as` `.section` name](https://sourceware.org/binutils/docs-2.24/as/Section.html) - specifically the ELF Version section calls out the use of the *type* argument to contain a constant that defines the type of the section. The `@progbits` section type means that the section contains data.

## Research Topics

### Building GCC from source

Pro tip: `gcc --verbose` will show what configuration options were used to build it (for example, you could check what flags to use when building `avr-gcc` from source). You can also get this kind of information from your package repositories through the "source download" option (and some digging).

Links:

- [AVR libc reference manual: Building and Installing GNU Toolchain](https://www.microchip.com/webdoc/AVRLibcReferenceManual/install_tools_1install_avr_binutils.html)
- [Very Simple Guide for Building Cross Compilers Tips](http://www.ifp.illinois.edu/~nakazato/tips/xgcc.html)
- [avr-libc: Build and Installing the GNU Toolchain](https://www.nongnu.org/avr-libc/user-manual/install_tools.html)

### Build Systems

- [build2 - C / C++ build system](https://build2.org/)

#### Ninja Build System

- [Replacing Make with Ninja](https://jpospisil.com/2014/03/16/replacing-make-with-ninja.html)
- [List of generators producing ninja build files](https://github.com/ninja-build/ninja/wiki/List-of-generators-producing-ninja-build-files)
- [LWN: The Ninja build tool](https://lwn.net/Articles/706404/)

I think my general idea was to try generating Ninja files with Python. I know you can also use a meta-build tool like cmake but I'm still avoiding switching to a system like that right now.

### Kbuild

The kernel build system

### Kconfig

The kernel configuration system

### Memory Models

Probably the most relevant reading to start with is [Dealing with memory access ordering in complex embedded designs](https://www.embedded.com/print/4437925).

I started researching memory models after getting my bare metal AM335x project working. I copied a bunch of code out of u-boot and noticed that the `write*` and `read*` functions used memory barriers:

```c
// From: arch/arm/include/asm/barriers.h
#define DMB	asm volatile ("dmb sy" : : : "memory")
#define dmb()	DMB

// From: arch/arm/include/asm/io.h
#define __iowmb()	dmb()
#define __arch_putb(v,a)		(*(volatile unsigned char *)(a) = (v))
#define writeb(v,c)	({ u8  __v = v; __iowmb(); __arch_putb(__v,c); __v; })
```

It seems the reason for this is the complexity of the A-series ARM core used in the AM335x - you want to make sure your register writes occur in the order you've requested them.

Some open tabs:

- [Documentation: Linux Kernel Memory Barriers](https://www.kernel.org/doc/Documentation/memory-barriers.txt)
- [linux/tools/memory-model/Documentation/explanation.txt](https://github.com/torvalds/linux/blob/master/tools/memory-model/Documentation/explanation.txt)
- [Embedded Linux Conference: Memory Barriers in the Linux Kernel: Semantics and Practices](https://elinux.org/images/a/ab/Bueso.pdf)
- [LWN: A formal kernel memory-ordering model (part 1)](https://lwn.net/Articles/718628/)
- [RISC-V Memory Consistency Model Tutorial](https://content.riscv.org/wp-content/uploads/2018/05/14.25-15.00-RISCVMemoryModelTutorial.pdf)
- [Memory Barriers: A Hardware View for Software Hackers](http://www.rdrop.com/users/paulmck/scalability/paper/whymb.2010.06.07c.pdf)
- [Memory Barriers in the Linux Kernel](https://elinux.org/images/a/ab/Bueso.pdf#page441)

### Call graph generation

Motivation: In reverse engineering u-boot I'm curious to see the flow functions, want to better understand what's calling what (this will only be somewhat useful - the major init flow uses an array of functions to call).

[SO: How to generate call graph of a binary file](https://reverseengineering.stackexchange.com/questions/16081/how-to-generate-the-call-graph-of-a-binary-file)

### Line Editor / Shell

I want to make a basic line editor and shell for assisting in debugging the flight controller / parachute project. It's a nice-to-have that will also be a good learning experience for me.

Tabs:

- [XTerm Control Sequences](https://invisible-island.net/xterm/ctlseqs/ctlseqs.html) - Super useful, what `linenoise.c` is based on
- [github: linenoise.c](https://github.com/npat-efault/picocom/blob/master/linenoise-1.0/linenoise.c)
- [VT100 escape codes](https://www.csie.ntu.edu.tw/~r92094/c++/VT100.html)

### Linux Usage

- [Dotfiles documentation on GitHub](https://dotfiles.github.io/) - I want to start source controlling my Linux configuration so I can argue with people at bars about vim vs emacs, bash vs zsh, and other pointless things.

## Interesting Reading

[Decoded GNU coreutils](https://www.maizure.org/projects/decoded-gnu-coreutils/)

	This is a long-term project to decode all of the GNU coreutils in version 8.3.

	This resource is for novice programmers exploring the design of command-line utilities. It is best used as an accompaniment providing useful background while reading the source code of the utility you may be interested in. This is not a user guide -- Please see applicable man pages for instructions on using these utilities.


## Other tabs already saved

x/y/z modem
http://www.blunk-electronic.de/train-z/pdf/xymodem.pdf
http://web.mit.edu/course/6/6.115/OldFiles/www/amulet/xmodem.htm

Bare Metal ARM
https://github.com/MRobertEvers/Bare-Metal-AM335x-ARM-OS/tree/master/src
http://cs107e.github.io/guides/gcc/
https://wiki.osdev.org/Creating_a_C_Library#Program_Initialization

PocketBeagle / BBB booting
https://gist.github.com/MRobertEvers/db98e5f2ee23a73ef140a4e2cacfad19
https://github.com/victronenergy/venus/wiki/bbb-boot-process-overview

SD protocol
http://wiki.seabright.co.nz/wiki/SdCardProtocol.html
https://www.fpga4fun.com/SD2.html
https://en.m.wikipedia.org/wiki/SD_card

FAT file system
https://en.m.wikipedia.org/wiki/Design_of_the_FAT_file_system
https://staff.washington.edu/dittrich/misc/fatgen103.pdf

Porting u-boot
https://elinux.org/images/2/2a/Schulz-how-to-support-new-board-u-boot-linux.pdf

u-boot driver model
https://docs.google.com/document/d/1_zZLey1JcYvW9RcuzxbCzipLdfr5SVyWEdh7sHhuAWg/mobilebasic

Linux RFS
https://gist.github.com/chrisdone/02e165a0004be33734ac2334f215380e
http://www.bootembedded.com/beagle-bone-black/building-custom-root-file-system-rfs-for-beagle-bone-black-using-busy-box/
http://www.bootembedded.com/embedded-linux/building-embedded-linux-system-using-mainline-kernel-for-beaglebone-black/
http://www.bootembedded.com/embedded-linux/building-embedded-linux-scratch-beaglebone-black/
https://github.com/beagleboard/image-builder/blob/master/scripts/debootstrap.sh
https://www.tldp.org/HOWTO/Bootdisk-HOWTO/buildroot.html

## Logan's Notes

A lot of the information in this document will be ported to my site as that project gets off the ground. I'm going to start collecting some of it below. SOme of this information will likely exist in multiple places, especially stuff that's on the `man` pages of my site - I will cross-link as necessary.

### Markdown Generation

Right now I'm using [commonmarker](https://github.com/gjtorikian/commonmarker) which is a Ruby wrapper for [libcmark-gfm](https://github.com/github/cmark-gfm), GitHub's fork of the reference parser for CommonMark. However, I don't see any reason to keep using this when there's a Python wrapper that does the same thing - [cmarkgfm](https://github.com/theacodes/cmarkgfm). I haven't tried it out yet but I will and hopefully it's just as easy to use. I don't know Ruby right now and don't plan to learn it for this project. Additionally, a number of other tools I plan to use are Python-base so this would make the pipeline slightly less complicated.

#### Syntax Highlighting

Since I was already using commonmarker I also used [pygments.rb](https://github.com/tmm1/pygments.rb), a Ruby wrapper for Python [pygments](http://pygments.org/) syntax highlighter. I'm going to switch to using the Python version without Ruby for the same reason listed earlier.

There are lexers available for assembly languages, including [gas](http://pygments.org/docs/lexers/#lexers-for-assembly-languages) (and it looks like some for objdump, will revisit that as well).

It'd be neat if my source code highlighter had options like GitHub's source code viewer (though that is specific to viewing a file):

- Line numbers
	+ Bonus: Line number anchors (e.g. `<url>#L18` links to line 18 of the source) plus line highlighting when anchor is present ([example](https://github.com/gcc-mirror/gcc/blob/master/gcc/target-def.h#L18))
- "Select all" (but not line numbers)

### Web Design

Man, back at it. I don't want to go too deep into web design, I'm not looking to become good at this; however, I do want a decent looking site that works on most devices pretty easily. I think that means using something like [HTML5 Boilerplate](https://html5boilerplate.com/) or [Bootstrap](https://getbootstrap.com/).

My goal is to have a "static" website - that is, no server-side operations. I might not be able to get away with that long term but we're going to see how it works. I'm pretty sure I can just generate HTML files and use JavaScript frameworks to add any fancy functionality I want.

I'm using a Makefile to generate the site. The general idea I've been working on is to write all of my notes in Markdown and use a custom pipeline to generate the HTML pages. The idea isn't fully formed yet but basically:

- GitHub Flavored Markdown (starting point, I'll likely modify the style)
- Syntax highlighting (including various assembly languages, maybe a custom "shell" one since I use that a lot, linker script, Kbuild/Kconfig, maybe objdump)
	+ Line numbers
	+ Copy source
- Template directives wrapping bootstrap functionality - e.g. message boxes
	+ Use something like Mako to write the page template
- Add git hash / tag to every page, last update datetime
- Home page lists most recent changes (generated from git) plus commit message
- git repo contains references (git-lfs) - PDF manuals, source code, etc.
- PRs can be submitted if people find errors in the pages
- Loads of cross-references
- Unclear what the organization will be yet - right now it's just notes and man pages
- Need some sort of deploy script to automate uploading of the site

The pipeline I'm working on needs to handle:

- SCSS to CSS
- Template pass to make stub into full page
- Converting Markdown to HTML
	+ Add syntax highlighting (not sure how to handle CSS assets from this - should they be collected and added to a directory, are they all generated statically up front? More research needed)
	+ Add header anchor links (like GitHub)
	+ Auto-link generation

#### Terminal Color Capture

One thing that would be super neat is to have colorized terminal listings - but without taking screenshots. This means capturing the color data generated by various terminal utilities and preserving it. This is stored in [ANSI color code escape sequences](https://en.wikipedia.org/wiki/ANSI_escape_code) - not super pretty to look at. For example, here's a line with and without color codes:

```bash
# Without:
[123126.955861] pcieport 0000:02:02.0: BAR 15: failed to assign [mem size 0x00200000 64bit pref]

# With:
[32m[123126.955861] [0m[33mpcieport 0000:02:02.0[0m: BAR 15: failed to assign [mem size 0x00200000 64bit pref]
```

So while I'd love to have the color information so that my site looks nice and pretty, I'm not willing to give up readability in my Markdown documents (kind of defeats the purpose). So, maybe there's another way?

```terminal
${c32}[123126.955861]${e} ${c33}pcieport${e} 0000:02:02.0: BAR 15: failed to assign [mem size 0x00200000 64bit pref]
```

I think I can live with that. It's not great but it captures the information I need in a minimally invasive way. One question is how to handle dollar signs in the text streams - will Mako be upset about that? I could use "terminal" as the language and have a custom block handler for that which converts it into proper HTML.

However, I need something that generates the text above - no way am I going to manually swap ANSI color codes for variables. That's why computers exist.

So, there's alreay a Python utility, [`ansi2html`](https://pypi.org/project/ansi2html/), that does what its namesake implies - convert ANSI escape codes into HTML. I'd love to store my terminal captures in this but it completely removes readability:

```html
<span class="ansi32">[123126.955861] </span><span class="ansi33">pcieport 0000:02:02.0</span>: BAR 15: failed to assign [mem size 0x00200000 64bit pref]
```

Well. That's not too awful I guess. Hmm. I could always customize the names to be shorter...

Quick example: `dmesg --color=always | tail -50 | ansi2html  | xclip -selection clipboard`

Or maybe I could use just plain xml and convert it to actual HTML later? I could customize the `ansi2html` tool to spit out my intermediate form for storage in Markdown documents and then expand that form to full HTML on a second pass. Then it's still pretty easy to read:

```xml
<c32>[123126.955861] </c32> <c33>pcieport</c33> 0000:02:02.0: BAR 15: failed to assign [mem size 0x00200000 64bit pref]
```
Color table (FG code):

ANSI | Color   | Theme Code
-----|---------|-----------
30   | Black   | #333333
31   | Red     | #CC0000
32   | Green   | #4E9A06
33   | Yellow  | #C4A000
34   | Blue    | #3465A4
35   | Magenta | #75507B
36   | Cyan    | #06989A
37   | White   | #D3D7CF

Bright color table (BG code):

ANSI | Color   | Theme Code
-----|---------|-----------
90   | Black   | #88807C
91   | Red     | #F15D22
92   | Green   | #73C48F
93   | Yellow  | #FFCE51
94   | Blue    | #48B9C7
95   | Magenta | #AD7FA8
96   | Cyan    | #34E2E2
97   | White   | #EEEEEC

Text: #F2F2F2
Background: #333333
Show bold text in bright colors (bold is code 1)

Here's the start of some custom CSS to use with `ansi2html`:

```css
.ansi2html-content { display: inline; white-space: pre-wrap; word-wrap: break-word; }
.body_foreground { color: #F2F2F2; }
.body_background { background-color: #333333; }
.body_foreground > .bold,.bold > .body_foreground, body.body_foreground > pre > .bold { color: #FFFFFF; font-weight: normal; }
.inv_foreground { color: #000000; }
.inv_background { background-color: #AAAAAA; }
.ansi1 { font-weight: bold; color: #EEEEEC}
.ansi31 { color: #CC0000; }
.ansi32 { color: #4E9A06; }
.ansi33 { color: #C4A000; }
.ansi34 { color: #3465A4; }
.ansi1 .ansi32 { color: #73C48F; }
.ansi1 .ansi34 { color: #48B9C7; }
```

And here's my normal prompt. I image I could come up with some regex rules to use on terminal listings if I wanted to automate things like colorizing my prompt a certain way. I could also just add a custom tag or something. TBD. We'll how the usage feels.

```html
<span class="ansi1"><span class="ansi32">logan@pop-os</span>:<span class="ansi34">~/Desktop/logansnotes</span></span>$
```

### Man Pages

#### GDB

##### Breakpoints

[Old GDB reference](https://ftp.gnu.org/old-gnu/Manuals/gdb/html_node/gdb_28.html) <- Should get a newer version or host my own.

Setting break points:

`break <function-name>`

`break <filename>:<linenum>`

`break <filename>:<function>`

`break *<address>`

More advanced usage:

```gdb
(gdb) break <function>
(gdb) commands
Type commands for breakpoint(s) 1, one per line.
End with a line saying just "end".
><commands>
>end
```

- Use `silent` to hide the usual information printed out at a breakpoint
- Use `cont` to continue execution after performing the commands listed

This can be useful during initial debugging of a problem - for example, print all instances of a function call and its arguments.

**Q**: How do I set a breakpoint on access to a specific memory location?

**A**: `rwatch *<address>` (for more information see [Setting Watchpoints](https://sourceware.org/gdb/onlinedocs/gdb/Set-Watchpoints.html))

##### Homeless

**Q**: How do I print the full value of a long string in gdb?

**A**: `set print elements 0` From the GDB manual:

	`set print elements <number-of-elements>`

	Set a limit on how many elements of an array GDB will print. If GDB is printing a large array, it stops printing after it has printed the number of elements set by the set print elements command. This limit also applies to the display of strings. When GDB starts, this limit is set to 200. **Setting `<number-of-elements>` to zero means that the printing is unlimited.**

#### git

**Q**: How do I list all local branches without a remote? (motivation: you should delete branches that have been merged / rebased onto master)

**A**: [Command line cleverness](https://stackoverflow.com/questions/15661853/list-all-local-branches-without-a-remote)

```bash
$ git branch -vv
  bug/us1-167           4fb85b5 US1-167 C1 conditional code statement not wrapped
  bug/us1-199           9538a45 [origin/bug/us1-199] US1-199 MCP25625 driver infinite loop in initialization sequence
  ...snip...

$ git branch -vv | cut -c 3- | awk '$3 !~/\[/ { print $1 }'
bug/us1-167
feat/us1-140
feat/us1-15
help
rel/v4
tmp/doc-changes
tmp/enable-5v-payload
tmp/us1-104
wip/build-framework
```
