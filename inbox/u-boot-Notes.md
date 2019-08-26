# u-boot Notes

Basically, going to try and figure out how u-boot is built for the BBB. I'm sure there are better resources for figuring this out but I'm on a plane and bored. Also, reverse engineering something is often one of the better ways to understand it (as long as you eventually correct any misunderstandings you may develop as a result of it).

The BBB is based on the TI AM3358 processor (P/N ?)

```bash
# Get u-boot
$ git clone https://github.com/u-boot/u-boot
$ cd u-boot/
$ git checkout v2019.01

# Patch u-boot
$ cd ../
$ git clone https://github.com/eewiki/u-boot-patches.git
$ cd ./u-boot-patches/v2019.01
$ cp 0001-am335x_evm-uEnv.txt-bootz-n-fixes.patch ../../u-boot/
$ cp 0002-U-Boot-BeagleBone-Cape-Manager.patch ../../u-boot/
$ cd ../../u-boot
$ patch -p1 < 0001-am335x_evm-uEnv.txt-bootz-n-fixes.patch 
patching file arch/arm/include/asm/arch-am33xx/hardware_am33xx.h
patching file arch/arm/include/asm/arch-am33xx/sys_proto.h
patching file arch/arm/mach-omap2/am33xx/board.c
patching file arch/arm/mach-omap2/hwinit-common.c
patching file board/ti/am335x/board.c
patching file board/ti/am335x/board.h
patching file board/ti/am335x/mux.c
patching file configs/am335x_boneblack_defconfig
patching file configs/am335x_evm_defconfig
patching file env/common.c
patching file include/configs/am335x_evm.h
patching file include/configs/ti_armv7_common.h
patching file include/environment/ti/mmc.h
$ patch -p1 < 0002-U-Boot-BeagleBone-Cape-Manager.patch
patching file arch/arm/mach-omap2/am33xx/clock_am33xx.c
patching file board/ti/am335x/board.c
patching file board/ti/am335x/board.h
patching file board/ti/am335x/hash-string.h
patching file board/ti/am335x/mux.c
patching file include/configs/ti_armv7_common.h
patching file include/configs/ti_armv7_omap.h
patching file include/environment/ti/mmc.h

# Export cross-compiler variable
# Example using installed compiler
export CC=/opt/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-

# Build u-boot
$ make ARCH=arm CROSS_COMPILE=${CC} distclean
# NOTE: am335x_boneblack_defconfig is equivalent to am335x_evm_defconfig, at least for v2019.01
$ make ARCH=arm CROSS_COMPILE=${CC} am335x_evm_defconfig
$ make ARCH=arm CROSS_COMPILE=${CC}

# To inspect this configuration graphically
$ make ARCH=arm CROSS_COMPILE=${CC} menuconfig
```

Some big questions right now:

- How does Kconfig interact with make? It seems this somehow selects the object files to be built and linked but I'm unclear how.
- How do you know what configuration variables to use? What impacts this? Obviously architecture, SoC, board, boot source. Then there's feature selection - what do you actually need to boot, what extra features do you want, are you debugging, etc. But there are so many configurations. If you were to start from scratch, what do you do?
- Is the config file for a target the single source of truth? Put another way, can you trace *every* variable that ends up defined in `.congig` to a variable in `config/<config>`?
- How is the SPL target built? Basically, for the AM335x the on-chip memory is too small to hold the full u-boot image, so a smaller version is built that bootstraps the full-size u-boot image. I have no idea how this is built and what subset of u-boot goes into it (or where this subset is defined).

The file `config/am335x_evm_defconfig` defines the configuration for use with the BBB. The file `config/am335x_boneblack_defconfig` is identical (at least for v2019.01). From some googling it appears the more generic evaluation module target is used so that multiple targets can be supported by the same config (e.g. BeagleBone White) (?).


Paths of interest:

arch/arm
arch/arm/include/asm/arch-am33xx




### Sandbox build

```bash
# Dependencies
$ sudo apt-get install libssl-dev libsdl1.2-dev

# Configure and build
$ make sandbox_defconfig
$ make
```

## Appendix

### Kconfig

Kconfig is the configuration system from the Linux kernel. It has been adapted into numerous projects, including u-boot. **TODO: Insert better description from Kernel documentation**

See: doc/README.kconfig

#### What Happens When You Run `make <config>`

Files / directories created / modified:

`.config` created, complete configuration database

(Unclear what these do yet)

`include/config` created, is an empty directory...
`include/generated` created, is an empty directory...
`scripts/basic/fixdep` created
`scripts/basic/.fixdep.cmd` created
`scripts/kconfig/conf` created
`scripts/kconfig/.conf.cmd` created
`scripts/kconfig/conf.o` created
`scripts/kconfig/.conf.o.cmd` created
`scripts/kconfig/zconf.lex.c` created
`scripts/kconfig/.zconf.lex.c.cmd` created
`scripts/kconfig/zconf.tab.c` created
`scripts/kconfig/.zconf.tab.c.cmd` created
`scripts/kconfig/zconf.tab.o` created
`scripts/kconfig/.zconf.tab.o.cmd` created


	Wait, I didn't see that configuration set in `<config>`..

While very powerful, the Kconfig system can also be a bit confusing at first (I know - I'm confused right now). For example, in the `.config` generated for `am335x_evm_defconfig` the line `CONFIG_CMD_GO=y` appears - but where did that come from? The Kconfig system allows for setting a default selection for a configuration option - if you don't set it the default will be taken. Here's the appropriate snippet of the command line Kconfig file: 

```kconfig
# cmd/Kconfig:278
config CMD_GO
	bool "go"
	default y
	help
	  Start an application at a given address.
```

Now, what if you don't want that? Well, that's something I'm trying to figure out. I know there are options like "allnoconfig" but I don't know if you should run that then run your board config, or if you can run your board config but set all defaults to no - or if that's even sane? I don't know what kind of assumptions were made while writing the defconfig I'm using.

### config/am335x_evm_defconfig

#### `CONFIG_ARM`

```kconfig
# arch/Kconfig:20
config ARM
	bool "ARM architecture"
	select CREATE_ARCH_SYMLINK
	select HAVE_PRIVATE_LIBGCC if !ARM64
	select SUPPORT_OF_CONTROL
```

ARM architecture for selection "Architecture select" defined in `arch/Kconfig`. This enables the configuration menu "ARM architecture" defined in `arch/arm/Kconfig`.

### Other Configuration

These configuration variables are selected by those in `am335x_evm_defconfig` or are generated as a result of variables selected.

#### `CREATE_ARCH_SYMLINK`

```kconfig
# arch/Kconfig:1
	bool
```

**Description**

#### `HAVE_PRIVATE_LIBGCC`

```kconfig
# lib/Kconfig:30
	bool
```

**Description**

#### `SUPPORT_OF_CONTROL`

```kconfig
# dts/Kconfig:5

```

**Description**

#### `SYS_ARCH`

```kconfig
# arch/Kconfig:171
config SYS_ARCH
	string
	help
	  This option should contain the architecture name to build the
	  appropriate arch/<CONFIG_SYS_ARCH> directory.
	  All the architectures should specify this option correctly.
```

Set to "arm" by `arch/arm/Kconfig:4`

```kconfig
# arch/arm/Kconfig:4
config SYS_ARCH
	default "arm"
```

**Description**

