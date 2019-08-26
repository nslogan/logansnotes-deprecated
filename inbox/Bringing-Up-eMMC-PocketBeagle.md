# Bringing Up eMMC PocketBeagle

- [x] Smoke test passes
- [x] TI FSB prints "CCCC"
- [x] SPL loads
- [x] u-boot loads
- [x] Program EEPROM
- [x] Format eMMC
	- NOTE: This was handled by Etcher and an Auterion image
	- Same as PocketBeagle SD card (I think)
	- MLO
	- FAT partition with u-boot.img
	- Linux partition with image
- [x] Test u-boot UMS for flashing images
- [x] Modify device tree for eMMC (line count: 8)
- [ ] Document device tree changes
- [ ] Add custom pocketbeagle to u-boot (board id?)
	- [ ] Modify u-boot to use 8-bit eMMC (probably based on custom board id)
- [ ] Script this process for production

## SPL

What success looks like:

```
*** file: /var/lib/tftpboot/u-boot-spl.img
$ sx -v /var/lib/tftpboot/u-boot-spl.img
Sending /var/lib/tftpboot/u-boot-spl.img, 354 blocks: Give your local XMODEM receive command now.
Retry 0: NAK on sector
Retry 0: NAK on sector
Retry 0: NAK on sector

Transfer complete

*** exit status: 0 ***

U-Boot SPL 2019.01-dirty (Aug 01 2019 - 19:00:38 -0700)
SPL: Scanning spl_boot_list[5]...
Trying to boot from UART
CCCCCCCCCC
```

Currently nothing happens after I successfully upload an SPL image (I get `*** exit status: 0 ***` back).

Unclear why this is happening - the obvious difference is that there is eMMC on the board so maybe SPL is trying to do something with that. My next step is to rebuild u-boot for the PocketBeagle and try to disable all MMC functionality.

Remembered that's there's EEPROM in this thing that identifies to u-boot which board it is and that the EEPROM will not be programmed from the factory. The quickest (and dirtiest) way around this is to simply modify u-boot to recognize the device as a PocketBeagle (hard-code). That's pretty easy:

```c
// File: board/ti/am335x/board.h
static inline int board_is_pb(void)
{
	// return board_ti_is("A335PBGL");
	return 1;
}
```

Images:

picocom -b 115200 --send-cmd="sx -vv" /dev/ttyUSB0
/home/logan/Projects/pb-u-boot/u-boot/spl/u-boot-spl.bin

picocom -b 115200 --send-cmd="sx -vv -kb --ymodem" /dev/ttyUSB0
/home/logan/Projects/pb-u-boot/u-boot/u-boot.bin

Woooooo! Both stages booted successfully after configuring ymodem properly (needed to make it transfer in 1k block mode).

## EEPROM

Now to program the EEPROM to make this thing think it's a PocketBeagle.

Since I already have u-boot up I can use its `i2c` utility to program the EEPROM. What values do I need for a PocketBeagle?

```
i2c mw 0x50 0x00.2 aa
i2c mw 0x50 0x01.2 55
i2c mw 0x50 0x02.2 33
i2c mw 0x50 0x03.2 ee
i2c mw 0x50 0x04.2 41
i2c mw 0x50 0x05.2 33
i2c mw 0x50 0x06.2 33
i2c mw 0x50 0x07.2 35
i2c mw 0x50 0x08.2 50
i2c mw 0x50 0x09.2 42
i2c mw 0x50 0x0a.2 47
i2c mw 0x50 0x0b.2 4c
i2c mw 0x50 0x0c.2 30
i2c mw 0x50 0x0d.2 30
i2c mw 0x50 0x0e.2 41
i2c mw 0x50 0x0f.2 32
```

I did those manually, what a pain in the ass. Also, it didn't work :-(. Turns out there are convenient environment variables for this process:

- `eeprom_dump` - View the contents of the configuration section of EEPROM
- `eeprom_pocketbeagle` - Write the PocketBeagle ID to EEPROM

You can run these by executing `run <env-var>`.

There's also a write protect mechanism for the EEPROM (section 6.3.2 in the [OSD335x Datasheet](https://octavosystems.com/docs/osd335x-sm-datasheet/)). Luckily, the original designer knew this and broke out that pin which needs to be grounded to disable the WP mechanism.

Woo! That worked too.

I'm going to try normal PocketBeagle u-boot image now (not hard-coded to PB selection). I'm going to also add in UMS while I'm rebuilding and uploading. That should make the eMMC process easier, hopefully.

Normal u-boot works again with EEPROM programmed!

## eMMC Configuration

I enabled `CMD_USB_MASS_STORAGE` in menuconfig (which enables the appropriate USB gadget for the device). Booted the device again and was able to start the UMS server with the command `ums 0 mmc 0` but now I need to get the JST-to-USB cable built.

It was successful! I loaded the Auterion image over UMS like I have in the past and it booted!

I then modified the device tree for Linux to enable 8-bit eMMC mode (CHANGES TO BE DOCUMENTED BELOW).

## Scripting Bring-up

- Might be easier to use an external I2C programmer to set up the EEPROM first and then use mainline u-boot (nope - the I2C0 bus isn't broken out on testpoints or headers).

[This gist](https://gist.github.com/jadonk/809337ed8435cdc25f99887746550ed2#file-pb-tester-js) has what appears to be a node.js test script to automate u-boot stuff. Could be useful.
