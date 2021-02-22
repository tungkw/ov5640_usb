#include "usb_app.h"
#define USB_PACK_SIZE 512

uint8_t _UsbSendData(uint8_t *pbuff, uint32_t buffsize)
{
	uint16_t retry = 0;
	USBD_HandleTypeDef *pdev = &hUsbDeviceFS;
	USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*) pdev->pClassData;

	hcdc->TxBuffer = pbuff;
	hcdc->TxLength = buffsize;

	if (pdev->pClassData != NULL) {
		if (hcdc->TxState == 0) {
			/* Tx Transfer in progress */
			hcdc->TxState = 1;
			/* Transmit next packet */
			USBD_StatusTypeDef usb_status = USBD_LL_Transmit(pdev, CDC_IN_EP,
					hcdc->TxBuffer, hcdc->TxLength);
			//return usb_status;
		} else {
			return USBD_BUSY;
		}
	}
	//??????
	while (hcdc->TxState == 1) {
		retry++;
		if (retry == 0xfff0) {
			return USBD_FAIL;
		}
	}
	return USBD_OK;
}


void USB2PC(uint8_t *str, uint32_t len)
{
	uint32_t j = 0;
	if(len < USB_PACK_SIZE)
	{
		_UsbSendData(str,len);
	}
	else
	{
		for (j = 0;j < len/USB_PACK_SIZE;j++)
		{
			_UsbSendData((str+USB_PACK_SIZE*j),USB_PACK_SIZE);
		}
		_UsbSendData((str+USB_PACK_SIZE*j),len%USB_PACK_SIZE);
	}
}
