# ov5640_usb

- 正点原子stm32f429iGt6开发板（阿波罗）
- 正点原子ov5640(+sccb) + dcmi + sdram
- stm32CubeMX 6.1.1
- MDK-ARM V5
- STM32Cube FW_F4 V1.25.2
- usb full speed device + common device class(virtual port com)


# Bug
- Minimum Heap Size
  - set to 0x600 solve usb connection problem
