# Compatibility and limits

## Device match

The module targets Broadcom USB vendor `0a5c`, runtime product `0bdc`, at
SuperSpeed. The bootloader presents as `0a5c:bd27` and uses the existing
Linux `brcmfmac` firmware-loading path; this module does not modify that
stage. USB 2.0 devices and other vendor/product IDs are not changed.

At USB 3.0 speed, this adapter advertises a 39-byte configuration descriptor
but returns a complete 57-byte configuration when asked. The final bytes
contain the bulk OUT endpoint and SuperSpeed companion descriptors. Without
the correction, Linux asks for 39 bytes and `brcmfmac` cannot find its bulk
OUT endpoint. This module changes the initial nine-byte configuration read
to request 57 bytes on the subsequent read.

## Tested environment

| Item | Verified result |
| --- | --- |
| Host | x86-64, Debian 13, kernel `6.12.95+deb13-amd64` |
| Firmware | Debian `firmware-brcm80211` 20250410-2 |
| USB | SuperSpeed `5000M`; all three endpoints available |
| Wi-Fi | `brcmfmac` connected on 5 GHz, 80 MHz |
| Local LAN TCP | 355 Mbit/s upload; 378 Mbit/s download |
| Fresh unplug/replug | Passed with the DKMS module loaded |
| Host reboot | Not tested |

The 867 Mbit/s figure is the radio's maximum PHY rate, before Wi-Fi and TCP
overhead ([BCM43569 datasheet](https://media.digikey.com/pdf/Data%20Sheets/Cypress%20PDFs/BCM43569_RevI_Jul1%2C2016.pdf)).
It is not a throughput promise. The USB `5000M` figure is bus signalling
speed, not Wi-Fi speed.

## Known limits

- Only the x86-64 register calling convention is implemented.
- The return probe depends on the kernel's `usb_get_descriptor` implementation
  and may need updates for other kernel versions.
- Kernel Secure Boot policies may block locally built modules unless they
  are signed and trusted.
- This is a device-specific compatibility module, not a general USB or Wi-Fi
  driver.
- The module does not provide firmware, improve the radio's PHY limit, or
  guarantee a particular transfer speed.
