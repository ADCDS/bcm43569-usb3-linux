# BCM43569 USB 3.0 for Linux

An x86-64 kernel module that lets the Broadcom BCM43569 USB Wi-Fi adapter
enumerate at USB 3.0 speed on Linux. It fixes a configuration-descriptor
length reported by this device and then lets the existing `brcmfmac` driver
handle Wi-Fi. It is **not** a replacement Wi-Fi driver or a firmware image.

## Supported device

- USB ID `0a5c:bd27` while firmware is loading; `0a5c:0bdc` after boot.
- The fix applies only to `0a5c:0bdc` at SuperSpeed (USB 3.0).
- Tested on x86-64 Debian 13 with kernel `6.12.95+deb13-amd64` and Debian's
  `firmware-brcm80211` package. Other kernels and devices are unverified.

The module uses a kernel return probe, so kernel-internal changes can break
it. It has passed a fresh unplug/replug test but has **not** been tested
across a host reboot. See [compatibility and limits](COMPATIBILITY.md).

## Install

Follow the [DKMS installation guide](INSTALL.md). You need matching
kernel headers, `dkms`, `build-essential`, and the separately supplied
Broadcom firmware (`firmware-brcm80211` on Debian). No proprietary firmware
or Windows driver is included in this repository or its releases.

GitHub releases provide source downloads; the module is built locally for
your running kernel. There is no universal precompiled `.ko` binary.

Load the module before connecting the adapter. If it is already connected,
physically unplug and reconnect it after installation. The included
`brcmfmac` soft dependency arranges the load order for future driver loads;
reboot behavior remains unverified.

## Verify

With the adapter connected, check `lsusb -t` for `5000M`, `ip link` for a new
wireless interface, and the kernel log for
`bcm43569-config-quirk: corrected first config length to 57`. The connection
must also pass a network transfer test; `5000M` alone is only the USB bus
signalling rate.

This device's maximum advertised Wi-Fi PHY rate is 867 Mbit/s, **not** 1
Gbit/s of download throughput. On the test host, a 5 GHz local LAN test
measured 355 Mbit/s upload and 378 Mbit/s download over USB 3.0. Results
depend on the access point, radio conditions, and test path.

## License

The module source is GPL-2.0-only. Firmware remains separately licensed and
must be obtained through your distribution or device vendor.
