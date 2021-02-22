
#ifndef __USB_APP_H
#define __USB_APP_H

#include "main.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

#define USB_PACK_SIZE 512
extern USBD_HandleTypeDef hUsbDeviceFS;

void USB2PC(uint8_t *str, uint32_t len);



#endif

