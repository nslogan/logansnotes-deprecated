TFTP on BBB

TODO: Document the usb0 -> `<interface>` naming, persistent naming, etc.

```bash
$ dmesg | tail
[301477.679134] usb 1-2: new full-speed USB device number 36 using xhci_hcd
[301477.827453] usb 1-2: not running at top speed; connect to a high speed hub
[301477.828300] usb 1-2: New USB device found, idVendor=0451, idProduct=6141, bcdDevice= 0.00
[301477.828306] usb 1-2: New USB device strings: Mfr=33, Product=37, SerialNumber=0
[301477.828310] usb 1-2: Product: AM335x USB
[301477.828314] usb 1-2: Manufacturer: Texas Instruments
[301477.873434] rndis_host 1-2:1.0 usb0: register 'rndis_host' at usb-0000:00:14.0-2, RNDIS device, 0e:ef:2f:05:d9:c9
[301477.904142] rndis_host 1-2:1.0 enp0s20f0u2: renamed from usb0
[301477.951543] IPv6: ADDRCONF(NETDEV_UP): enp0s20f0u2: link is not ready
```

`usb0` renamed to `enp0s20f0u2` by `udev`

Some investigation into how [persistent interface names](https://github.com/systemd/systemd/blob/master/src/udev/udev-builtin-net_id.c#L20) work out. And [here's](http://hintshop.ludvig.co.nz/show/persistent-names-usb-serial-devices/) an article about it.

```bash
# USB 3.0 on right side
[ 8416.133017] rndis_host 1-1.1:1.0 enp0s20f0u1u1: renamed from usb0
# USB 3.0 on left side
[ 8480.712750] rndis_host 1-2:1.0 enp0s20f0u2: renamed from usb0
# USB C 4-port USB 3.0 hub, USB 3.0 port furtherest to closest to connector 
[ 8546.153106] rndis_host 3-1.3:1.0 enp57s0u1u3: renamed from usb0
[ 8570.469275] rndis_host 3-1.2:1.0 enp57s0u1u2: renamed from usb0
[ 8605.547203] rndis_host 3-1.1:1.0 enp57s0u1u1: renamed from usb0
[ 8621.160063] rndis_host 3-1.4:1.0 enp57s0u1u4: renamed from usb0

# en - Ethernet
# 
# enp0s20f0u1u1 (1-1.1:1.0)
# p<bus>		0
# s<slot>		20
# f<function>	0
# u<port>		1
# u<port>		1
# 
# enp0s20f0u2 (1-2:1.0)
# p<bus>		0
# s<slot>		20
# f<function>	0
# u<port>		2
# 
# enp57s0u1u3 (3-1.3:1.0)
# p<bus>		57
# s<slot>		0
# f<function>	1
# u<port>		3

$ lspci -tv
# ... snip ...
+-14.0  Intel Corporation Sunrise Point-LP USB 3.0 xHCI Controller
# ... snip ...
+-1c.0-[01-39]----00.0-[02-39]--+-00.0-[03]--
           |                               +-01.0-[04-38]--
           |                               \-02.0-[39]----00.0  Intel Corporation DSL6340 USB 3.1 Controller [Alpine Ridge]

$ lsusb | sort
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
Bus 001 Device 002: ID 0424:2742 Standard Microsystems Corp. 
Bus 001 Device 006: ID 138a:0091 Validity Sensors, Inc. 
Bus 001 Device 009: ID 0cf3:e301 Atheros Communications, Inc. 
Bus 001 Device 010: ID 04f3:2234 Elan Microelectronics Corp. 
Bus 001 Device 011: ID 0bda:568b Realtek Semiconductor Corp. 
Bus 002 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub
Bus 002 Device 002: ID 0424:5742 Standard Microsystems Corp. 
Bus 003 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
Bus 003 Device 002: ID 2109:2817 VIA Labs, Inc. 
Bus 003 Device 003: ID 2109:8888 VIA Labs, Inc. 
Bus 004 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub
Bus 004 Device 002: ID 2109:0817 VIA Labs, Inc.

# With BBB plugged into right side USB 3.0 port
$ ls -l /sys/bus/usb/devices/
total 0
lrwxrwxrwx 1 root root 0 Mar 13 23:56 1-0:1.0 -> ../../../devices/pci0000:00/0000:00:14.0/usb1/1-0:1.0
lrwxrwxrwx 1 root root 0 Mar 13 23:56 1-1 -> ../../../devices/pci0000:00/0000:00:14.0/usb1/1-1
lrwxrwxrwx 1 root root 0 Mar 14 00:02 1-1.1 -> ../../../devices/pci0000:00/0000:00:14.0/usb1/1-1/1-1.1
lrwxrwxrwx 1 root root 0 Mar 13 23:56 1-1:1.0 -> ../../../devices/pci0000:00/0000:00:14.0/usb1/1-1/1-1:1.0
lrwxrwxrwx 1 root root 0 Mar 14 00:02 1-1.1:1.0 -> ../../../devices/pci0000:00/0000:00:14.0/usb1/1-1/1-1.1/1-1.1:1.0
lrwxrwxrwx 1 root root 0 Mar 14 00:02 1-1.1:1.1 -> ../../../devices/pci0000:00/0000:00:14.0/usb1/1-1/1-1.1/1-1.1:1.1
lrwxrwxrwx 1 root root 0 Mar 13 23:56 1-1.2 -> ../../../devices/pci0000:00/0000:00:14.0/usb1/1-1/1-1.2
lrwxrwxrwx 1 root root 0 Mar 13 23:56 1-1.2:1.0 -> ../../../devices/pci0000:00/0000:00:14.0/usb1/1-1/1-1.2/1-1.2:1.0
lrwxrwxrwx 1 root root 0 Mar 13 23:56 1-3 -> ../../../devices/pci0000:00/0000:00:14.0/usb1/1-3
lrwxrwxrwx 1 root root 0 Mar 13 23:56 1-3:1.0 -> ../../../devices/pci0000:00/0000:00:14.0/usb1/1-3/1-3:1.0
lrwxrwxrwx 1 root root 0 Mar 13 23:56 1-3:1.1 -> ../../../devices/pci0000:00/0000:00:14.0/usb1/1-3/1-3:1.1
lrwxrwxrwx 1 root root 0 Mar 13 23:56 1-4 -> ../../../devices/pci0000:00/0000:00:14.0/usb1/1-4
lrwxrwxrwx 1 root root 0 Mar 13 23:56 1-4:1.0 -> ../../../devices/pci0000:00/0000:00:14.0/usb1/1-4/1-4:1.0
lrwxrwxrwx 1 root root 0 Mar 13 23:56 1-5 -> ../../../devices/pci0000:00/0000:00:14.0/usb1/1-5
lrwxrwxrwx 1 root root 0 Mar 13 23:56 1-5:1.0 -> ../../../devices/pci0000:00/0000:00:14.0/usb1/1-5/1-5:1.0
lrwxrwxrwx 1 root root 0 Mar 13 23:56 1-5:1.1 -> ../../../devices/pci0000:00/0000:00:14.0/usb1/1-5/1-5:1.1
lrwxrwxrwx 1 root root 0 Mar 13 23:56 2-0:1.0 -> ../../../devices/pci0000:00/0000:00:14.0/usb2/2-0:1.0
lrwxrwxrwx 1 root root 0 Mar 13 23:56 2-1 -> ../../../devices/pci0000:00/0000:00:14.0/usb2/2-1
lrwxrwxrwx 1 root root 0 Mar 13 23:56 2-1:1.0 -> ../../../devices/pci0000:00/0000:00:14.0/usb2/2-1/2-1:1.0
lrwxrwxrwx 1 root root 0 Mar 13 23:56 3-0:1.0 -> ../../../devices/pci0000:00/0000:00:1c.0/0000:01:00.0/0000:02:02.0/0000:39:00.0/usb3/3-0:1.0
lrwxrwxrwx 1 root root 0 Mar 13 23:56 3-1 -> ../../../devices/pci0000:00/0000:00:1c.0/0000:01:00.0/0000:02:02.0/0000:39:00.0/usb3/3-1
lrwxrwxrwx 1 root root 0 Mar 13 23:56 3-1:1.0 -> ../../../devices/pci0000:00/0000:00:1c.0/0000:01:00.0/0000:02:02.0/0000:39:00.0/usb3/3-1/3-1:1.0
lrwxrwxrwx 1 root root 0 Mar 13 23:56 3-1.5 -> ../../../devices/pci0000:00/0000:00:1c.0/0000:01:00.0/0000:02:02.0/0000:39:00.0/usb3/3-1/3-1.5
lrwxrwxrwx 1 root root 0 Mar 13 23:56 3-1.5:1.0 -> ../../../devices/pci0000:00/0000:00:1c.0/0000:01:00.0/0000:02:02.0/0000:39:00.0/usb3/3-1/3-1.5/3-1.5:1.0
lrwxrwxrwx 1 root root 0 Mar 13 23:56 4-0:1.0 -> ../../../devices/pci0000:00/0000:00:1c.0/0000:01:00.0/0000:02:02.0/0000:39:00.0/usb4/4-0:1.0
lrwxrwxrwx 1 root root 0 Mar 13 23:56 4-1 -> ../../../devices/pci0000:00/0000:00:1c.0/0000:01:00.0/0000:02:02.0/0000:39:00.0/usb4/4-1
lrwxrwxrwx 1 root root 0 Mar 13 23:56 4-1:1.0 -> ../../../devices/pci0000:00/0000:00:1c.0/0000:01:00.0/0000:02:02.0/0000:39:00.0/usb4/4-1/4-1:1.0
lrwxrwxrwx 1 root root 0 Mar 13 23:56 usb1 -> ../../../devices/pci0000:00/0000:00:14.0/usb1
lrwxrwxrwx 1 root root 0 Mar 13 23:56 usb2 -> ../../../devices/pci0000:00/0000:00:14.0/usb2
lrwxrwxrwx 1 root root 0 Mar 13 23:56 usb3 -> ../../../devices/pci0000:00/0000:00:1c.0/0000:01:00.0/0000:02:02.0/0000:39:00.0/usb3
lrwxrwxrwx 1 root root 0 Mar 13 23:56 usb4 -> ../../../devices/pci0000:00/0000:00:1c.0/0000:01:00.0/0000:02:02.0/0000:39:00.0/usb4
```

The RBL implements [RNDIS](https://en.wikipedia.org/wiki/RNDIS), a Microsoft proprietary protocol used mostly on top of USB to provide a virtual Ethernet link.

It uses BOOTP and TFTP...

- [Using Dnswmasq as a Standalone TFTP Server](https://stelfox.net/blog/2013/12/using-dnsmasq-as-a-standalone-tftp-server/)
- [Ubuntu 12.04 Set Up to Network Boot an AM335x Based Platform](http://processors.wiki.ti.com/index.php/Ubuntu_12.04_Set_Up_to_Network_Boot_an_AM335x_Based_Platform)
- [Ubuntu 18.04 netplan on multiple interfaces](https://ppc64el.wordpress.com/2018/03/22/ubuntu-18-04-netplan/)
- [Predictable Network Interface Names](https://www.freedesktop.org/wiki/Software/systemd/PredictableNetworkInterfaceNames/)
- [SO: Need example netplan yaml for static ip](https://askubuntu.com/questions/961552/need-example-netplan-yaml-for-static-ip)

```bash
# References:
# - [Netplan configuration examples](https://netplan.io/examples)
# - [dnsmasq man page](http://www.thekelleys.org.uk/dnsmasq/docs/dnsmasq-man.html)
# - [netplan man page](http://manpages.ubuntu.com/manpages/artful/man5/netplan.5.html)

# netplan configuration for USB RNDIS adapter
# NOTE: It is important to set `renderer` to match your system. The two options are `NetworkManager` and `systemd-networkd`. For Ubuntu desktop environments it's likely to be `NetworkManager`. If you don't set this right then NetworkManager will fight with your netplan configuration and attempt to run DHCP on your static network, removing the static IPv4 configuration and assigning an IPv6 address. You can review NetowrkManager logs with `journalctl -u NetworkManager -f`
# NOTE: dhcp4/6 are disabled by default but I included them in the configuration for posterity
$ cat /etc/netplan/01-netcfg.yaml
network:
  version: 2
  renderer: NetworkManager
  ethernets:
    enp0s20f0u2:
      addresses: [192.168.2.1/24]
      dhcp4: false
      dhcp6: false

# Apply netplan configuration changes
$ sudo netplan apply

# dnsmasq configuration
$ tail /etc/dnsmasq.conf

# Disable DNS
# TODO: Review where these settings come from
# NOTE: The `tftp-secure` option requires that the file to be transferred be owned by the user running the dnsmasq process, usually "dnsmasq" unless a different option has been specified on the command line or configuration file (via the `-u,--user` option); i.e. you must run `chown dnsmasq <file>`.
port=0
interface=enp0s20f0u2
dhcp-range=192.168.2.50,192.168.2.150,12h
bootp-dynamic
enable-tftp
#tftp-secure
tftp-root=/var/lib/tftpboot
dhcp-boot=beagle.img

# Alternative configuration
dhcp-vendorclass=set:am335x_rom,AM335x ROM
dhcp-vendorclass=set:am335x_spl,AM335x U-Boot SPL

port=0
interface=enp0s20f0u2u4u3
interface=usb0
dhcp-range=192.168.2.50,192.168.2.150,12h
bootp-dynamic
enable-tftp
#tftp-secure
tftp-root=/var/lib/tftpboot
dhcp-boot=net:am335x_spl,u-boot.img
dhcp-boot=net:am335x_rom,u-boot-spl.bin


# Restart the dnsmasq service after changing the configuration
$ sudo systemctl restart dnsmasq

# Check the status of the service
$ systemctl status dnsmasq.service
● dnsmasq.service - dnsmasq - A lightweight DHCP and caching DNS server
   Loaded: loaded (/lib/systemd/system/dnsmasq.service; enabled; vendor preset: enable
   Active: active (running) since Tue 2019-03-12 22:36:07 PDT; 1min 36s ago
  Process: 22200 ExecStartPost=/etc/init.d/dnsmasq systemd-start-resolvconf (code=exit
  Process: 22173 ExecStart=/etc/init.d/dnsmasq systemd-exec (code=exited, status=0/SUC
  Process: 22172 ExecStartPre=/usr/sbin/dnsmasq --test (code=exited, status=0/SUCCESS)
 Main PID: 22197 (dnsmasq)
    Tasks: 1 (limit: 4915)
   Memory: 2.6M
   CGroup: /system.slice/dnsmasq.service
           └─22197 /usr/sbin/dnsmasq -x /run/dnsmasq/dnsmasq.pid -u dnsmasq -r /run/dn

Mar 12 22:36:20 pop-os dnsmasq-dhcp[22197]: DHCP packet received on enp0s20f0u2 which 

# Start journalctl in follow mode on the dnsmasq service to see the connection and file transfer happen
$ journalctl -u dnsmasq.service -f
Mar 12 22:44:44 pop-os dnsmasq-dhcp[22197]: DHCP packet received on enp0s20f0u2 which has no address
Mar 12 22:44:48 pop-os dnsmasq-dhcp[22197]: BOOTP(enp0s20f0u2) 192.168.2.102 78:a5:04:fd:f6:26
Mar 12 22:44:48 pop-os dnsmasq-tftp[22197]: sent /tmp/tftp/beagle.img to 192.168.2.102

# [Purpose of avahi](https://unix.stackexchange.com/questions/163124/what-is-the-purpose-of-avahi-on-a-rhel-7-server)
# NOTE: You can disable avahi for a specific interface using the modification below - this will remove the mDNS messages being broadcast (which you can see in Wireshark)
$ sudo vim /etc/avahi/avahi-daemon.conf
# ... snip ...
deny-interfaces=enp0s20f0u2
# ... snip ...

# Disable IPv6 on the interface (probably not necessary but now I know how)
$ sudo vim /etc/sysctl.conf
# ... snip ...
net.ipv6.conf.enp0s20f0u2.disable_ipv6 = 1
# ... snip ...
```
