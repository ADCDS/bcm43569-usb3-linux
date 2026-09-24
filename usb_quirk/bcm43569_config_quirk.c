// SPDX-License-Identifier: GPL-2.0
/*
 * BCM43569 USB 3 configuration descriptor length workaround.
 *
 * This device returns 57 bytes of configuration descriptors if requested,
 * but claims wTotalLength == 39. Linux therefore requests too few bytes and
 * never sees bulk OUT endpoint 0x02. Fix only the first nine-byte read for
 * the exact runtime VID:PID at SuperSpeed; usbcore's second read can then
 * fetch and parse the complete 57-byte descriptor list.
 */

#include <linux/kprobes.h>
#include <linux/module.h>
#include <linux/ptrace.h>
#include <linux/usb.h>

#if !defined(CONFIG_X86_64)
#error "This experimental kretprobe uses x86-64 pt_regs argument registers"
#endif

#define BCM_VENDOR_ID 0x0a5c
#define BCM_RUNTIME_ID 0x0bdc
#define BCM_CONFIG_BYTES 57

struct descriptor_fixup_call {
	struct usb_config_descriptor *descriptor;
};

static int descriptor_entry(struct kretprobe_instance *instance,
			    struct pt_regs *regs)
{
	struct descriptor_fixup_call *call = (void *)instance->data;
	struct usb_device *dev = (void *)regs->di;
	unsigned int type = regs->si;
	unsigned int index = regs->dx;
	void *buffer = (void *)regs->cx;
	int size = regs->r8;

	call->descriptor = NULL;
	if (!dev || !buffer || type != USB_DT_CONFIG || index != 0 ||
	    size != USB_DT_CONFIG_SIZE)
		return 0;
	if (le16_to_cpu(dev->descriptor.idVendor) != BCM_VENDOR_ID ||
	    le16_to_cpu(dev->descriptor.idProduct) != BCM_RUNTIME_ID ||
	    dev->speed != USB_SPEED_SUPER)
		return 0;

	call->descriptor = buffer;
	return 0;
}

static int descriptor_return(struct kretprobe_instance *instance,
			     struct pt_regs *regs)
{
	struct descriptor_fixup_call *call = (void *)instance->data;

	if (call->descriptor && (long)regs->ax >= USB_DT_CONFIG_SIZE &&
	    call->descriptor->bLength == USB_DT_CONFIG_SIZE &&
	    call->descriptor->bDescriptorType == USB_DT_CONFIG &&
	    le16_to_cpu(call->descriptor->wTotalLength) == 39) {
		WRITE_ONCE(call->descriptor->wTotalLength,
			   cpu_to_le16(BCM_CONFIG_BYTES));
		pr_info_ratelimited("bcm43569-config-quirk: corrected first config length to %u\n",
				    BCM_CONFIG_BYTES);
	}
	return 0;
}

static struct kretprobe config_probe = {
	.kp.symbol_name = "usb_get_descriptor",
	.entry_handler = descriptor_entry,
	.handler = descriptor_return,
	.data_size = sizeof(struct descriptor_fixup_call),
	.maxactive = 32,
};

static int __init bcm_quirk_init(void)
{
	int err = register_kretprobe(&config_probe);

	if (err)
		return err;
	pr_info("bcm43569-config-quirk: active for 0a5c:0bdc at SuperSpeed\n");
	return 0;
}

static void __exit bcm_quirk_exit(void)
{
	unregister_kretprobe(&config_probe);
	pr_info("bcm43569-config-quirk: removed\n");
}

module_init(bcm_quirk_init);
module_exit(bcm_quirk_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Experimental BCM43569 USB 3 configuration length fixup");
