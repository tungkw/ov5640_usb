#ifndef __HW_OV5640_H
#define __HW_OV5640_H

#include "main.h"

#define HW_OV5640_CHIP_ID_H_ADDR	((uint16_t)0X300A)
#define HW_OV5640_CHIP_ID_L_ADDR	((uint16_t)0X300B)
#define HW_OV5640_CHIP_ID					((uint16_t)0x5640)
#define HW_OV5640_SCCB_ADDR				((uint8_t)0X78)

typedef struct{
	uint8_t addr;
	void(*RST)(uint8_t);
	void(*PWDN)(uint8_t);
	void(*SCL)(uint8_t);
	void(*SDA_out)(uint8_t);
	uint8_t(*SDA_in)(void);
	void(*SDA_switch)(uint8_t); // 0 for SDA_in, 1 for SDA_out
	void(*delay_us)(uint32_t);
}HW_OV5640_HandleTypeDef;

void HW_OV5640_delay_ms(HW_OV5640_HandleTypeDef*, uint32_t);

/* =====================================================================================================
// SCCB
*/
void HW_OV5640_SCCB_Start(HW_OV5640_HandleTypeDef*);
void HW_OV5640_SCCB_Stop(HW_OV5640_HandleTypeDef*);
void HW_OV5640_SCCB_No_Ack(HW_OV5640_HandleTypeDef*);
uint8_t HW_OV5640_SCCB_write_byte(HW_OV5640_HandleTypeDef*, uint8_t);
uint8_t HW_OV5640_SCCB_read_byte(HW_OV5640_HandleTypeDef*);
uint8_t HW_OV5640_SCCB_write_reg(HW_OV5640_HandleTypeDef*, uint16_t, uint8_t);
uint8_t HW_OV5640_SCCB_read_reg(HW_OV5640_HandleTypeDef*, uint16_t);

/* =====================================================================================================
// initialization and configuration
*/
void HW_OV5640_init(HW_OV5640_HandleTypeDef*);
void HW_OV5640_soft_reset(HW_OV5640_HandleTypeDef*);
uint16_t HW_OV5640_get_chip_id(HW_OV5640_HandleTypeDef*);
void HW_OV5640_flash(HW_OV5640_HandleTypeDef*);
void HW_OV5640_set_jpeg(HW_OV5640_HandleTypeDef*);
void HW_OV5640_set_rgb565(HW_OV5640_HandleTypeDef*);
void HW_OV5640_set_resolution(HW_OV5640_HandleTypeDef* ov5640_handle, uint16_t offset_x, uint16_t offset_y, uint16_t width, uint16_t height);


#endif

