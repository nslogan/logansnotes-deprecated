Ok, this is the sequence I think needs to happen.
If I fuck this up the drone is done :-|

loadb 100000 115200

CACHE: Misaligned operation at range [00100000, 00172958]
## Total Size      = 0x00072958 = 469336 Bytes
## Start Addr      = 0x00100000

(Based on some quick googling this is ok, just annoying)

fatrm mmc 0:1 u-boot.img

fatwrite mmc 0:1 $loadaddr u-boot.img 469336
fatwrite mmc 0:1 0x82000000 u-boot.img 469336

=> fatwrite mmc 0:1 $loadaddr u-boot.img 469336
4625206 bytes written
=> fatls mmc 0:1                               
    93888   MLO
      620   config.json
     2481   device-type.json
       45   image-version-info
      251   os-release
       24   resin-image
      536   resinos.fingerprint
            splash/
            system-connections/
        0   uEnv.txt_internal
  4625206   u-boot.img

9 file(s), 2 dir(s)

469336
4625206
/divide/
9.854786336
(?)
What the shit

Ah fuck it's in hex
That's not obvious

fatwrite mmc 0:1 $loadaddr u-boot.img 72958
469336 bytes written


```bash
$ cat ~/.kermrc
# NOTE: The device you use may be different; it must be set here because command line arguments are checked second and overwritten by this file but "line" must be set before most of these other arguments
set line /dev/ttyUSB1
set speed 115200
set carrier-watch off
set prefixing all
set parity none
set stop-bits 1
set modem none
set file type bin
set flow-control none

$ kermit
C-Kermit 9.0.302 OPEN SOURCE:, 20 Aug 2011, for Linux+SSL+KRB5 (64-bit)
 Copyright (C) 1985, 2011,
  Trustees of Columbia University in the City of New York.
Type ? or HELP for help.
(/home/logan/Projects/Auterion/) C-Kermit>connect

# Now you're connect to the PocketBeagle
=> loadb 100000 115200

# ctrl-\,ctrl-c exits back to Kermit

# Send the UMS-enabled u-boot image to the PocketBeagle
(/home/logan/Projects/Auterion/) C-Kermit>send u-boot.img

# Kermit will display file upload information while it's working, took about 45 seconds

# Once complete, connect back to the PocketBeagle
(/home/logan/Projects/Auterion/) C-Kermit>connect

# It probably show's this message, based on some quick googling this is ok, just annoying
CACHE: Misaligned operation at range [00100000, 00172958]
## Total Size      = 0x00072958 = 469336 Bytes
## Start Addr      = 0x00100000

# Now, remove the old image (not sure if this is necessary but I did it to be safe)
=> fatrm mmc 0:1 u-boot.img

# Write the UMS-enabled u-boot.img file from memory uploaded via Kermit to the SD card
=> fatwrite mmc 0:1 $loadaddr u-boot.img 72958

# Restart the CPU and now it should boot into UMS mode
=> reset
```

fatrm mmc 0:1 MLO
fatwrite mmc 0:1 $loadaddr MLO 16358

90968
16358

TRM p. 5028 Table 26-7 SYSBOOT Configuration Pins

SYSBOOT[15:0] = 01 00 00 0 0 00 0 11000

01    = 24 MHz
00    = All other values reserved
00    = Don't care for ROM code
0     = Don't care for ROM code
0     = Don't care for ROM code
00    = Don't care for ROM code
0     = CLKOUT1 disabled
11000 = SPI0 MMC0 USB0 UART0 (1-2-3-4)



26.1.9.5 UART Boot Procedure



26.1.9.6 USB Boot Procedure (p. 5070)
