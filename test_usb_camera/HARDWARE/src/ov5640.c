#include "ov5640.h"
#include "ov5640_conf.h"


void HW_OV5640_delay_ms(HW_OV5640_HandleTypeDef* ov5640, uint32_t nms)
{
	uint32_t cnt;
	while(cnt < nms){
		ov5640->delay_us(1000);
		cnt++;
	}
}


/* =====================================================================================================
// SCCB
*/

// SDA keep high/low when SCL if high
void HW_OV5640_SCCB_write_bit(HW_OV5640_HandleTypeDef* ov5640_handle, uint8_t bit)
{
	ov5640_handle->SDA_out(bit);
	ov5640_handle->delay_us(5);
	ov5640_handle->SCL(1);
	ov5640_handle->delay_us(5);
	ov5640_handle->SCL(0);
}

// SDA read when SCL if high
uint8_t HW_OV5640_SCCB_read_bit(HW_OV5640_HandleTypeDef* ov5640_handle)
{
	ov5640_handle->SDA_switch(0);
	uint8_t bit;
	ov5640_handle->delay_us(5);
	ov5640_handle->SCL(1);
	ov5640_handle->delay_us(5);
	bit = ov5640_handle->SDA_in();
	ov5640_handle->SCL(0);
	ov5640_handle->SDA_switch(1);
	return bit;
}

// SDA falling edge when SCL if high
void HW_OV5640_SCCB_Start(HW_OV5640_HandleTypeDef* ov5640_handle)
{
	ov5640_handle->SDA_out(1);
	ov5640_handle->SCL(1);
	ov5640_handle->delay_us(5);
	ov5640_handle->SDA_out(0);
	ov5640_handle->delay_us(5);
	ov5640_handle->SCL(0);  
}

// SDA rising edge when SCL if high
void HW_OV5640_SCCB_Stop(HW_OV5640_HandleTypeDef* ov5640_handle)
{
	ov5640_handle->SDA_out(0);
	ov5640_handle->delay_us(5);
	ov5640_handle->SCL(1);
	ov5640_handle->delay_us(5);
	ov5640_handle->SDA_out(1);
	ov5640_handle->delay_us(5);
}  

void HW_OV5640_SCCB_No_Ack(HW_OV5640_HandleTypeDef* ov5640_handle)
{
	HW_OV5640_SCCB_write_bit(ov5640_handle, 1);
}

uint8_t HW_OV5640_SCCB_write_byte(HW_OV5640_HandleTypeDef* ov5640_handle, uint8_t data)
{
	uint8_t j;	 
	for(j=0; j<8; j++)
	{
		HW_OV5640_SCCB_write_bit(ov5640_handle, (data&0x80)>>7);
		data <<= 1;  
	}
	return HW_OV5640_SCCB_read_bit(ov5640_handle);
}

uint8_t HW_OV5640_SCCB_read_byte(HW_OV5640_HandleTypeDef* ov5640_handle)
{
	uint8_t j, ret=0;
	ov5640_handle->SDA_switch(0);
	for(j=0; j<8; j++)
	{  
		ret = ret<<1;
		ret += HW_OV5640_SCCB_read_bit(ov5640_handle); 
	}
	ov5640_handle->SDA_switch(1);
	return ret;
}

uint8_t HW_OV5640_SCCB_write_reg(HW_OV5640_HandleTypeDef* ov5640_handle, uint16_t addr, uint8_t data)
{
	uint8_t res=0;
	HW_OV5640_SCCB_Start(ov5640_handle);
	if(HW_OV5640_SCCB_write_byte(ov5640_handle, ov5640_handle->addr|0x00)) res=1; // SCCB address + I/O(0/1)
	if(HW_OV5640_SCCB_write_byte(ov5640_handle, addr>>8)) res=2; // high address
	if(HW_OV5640_SCCB_write_byte(ov5640_handle, addr)) res=3; // low address
	if(HW_OV5640_SCCB_write_byte(ov5640_handle, data)) res=4; // data
	HW_OV5640_SCCB_Stop(ov5640_handle);
	return res;
}

uint8_t HW_OV5640_SCCB_read_reg(HW_OV5640_HandleTypeDef* ov5640_handle, uint16_t addr)
{
	uint8_t res=0;
	HW_OV5640_SCCB_Start(ov5640_handle);
	if(HW_OV5640_SCCB_write_byte(ov5640_handle, ov5640_handle->addr|0x00)) res=1; // SCCB address + I/O(0/1)
	if(HW_OV5640_SCCB_write_byte(ov5640_handle, addr>>8)) res=2; // high address
	if(HW_OV5640_SCCB_write_byte(ov5640_handle, addr)) res=3; // low address
	HW_OV5640_SCCB_Stop(ov5640_handle);
	
	HW_OV5640_SCCB_Start(ov5640_handle);
	if(HW_OV5640_SCCB_write_byte(ov5640_handle, ov5640_handle->addr|0x01)) res=4; // SCCB address + I/O(0/1)
	res = HW_OV5640_SCCB_read_byte(ov5640_handle); // data
	HW_OV5640_SCCB_No_Ack(ov5640_handle);
	HW_OV5640_SCCB_Stop(ov5640_handle);
	return res;
}
	
/* =====================================================================================================
// initialization and configuration
*/
void HW_OV5640_init(HW_OV5640_HandleTypeDef* ov5640_handle)
{
	if(!ov5640_handle->addr) ov5640_handle->addr=HW_OV5640_SCCB_ADDR;
	ov5640_handle->SDA_switch(1);
	
	// power up
	ov5640_handle->RST(0);
	HW_OV5640_delay_ms(ov5640_handle, 20);
	ov5640_handle->PWDN(0);
	HW_OV5640_delay_ms(ov5640_handle, 5);
	ov5640_handle->RST(1);
	HW_OV5640_delay_ms(ov5640_handle, 20);
	
	HW_OV5640_soft_reset(ov5640_handle);
	
	//initialize camera and set SXGA 1600x1200
	int i;
	for( i=0; i<sizeof(ov5640_uxga_init_reg_tbl)/4; i++)
	{
		HW_OV5640_SCCB_write_reg(ov5640_handle, ov5640_uxga_init_reg_tbl[i][0], ov5640_uxga_init_reg_tbl[i][1]);
	}   
}

void HW_OV5640_soft_reset(HW_OV5640_HandleTypeDef* ov5640_handle)
{
	HW_OV5640_SCCB_write_reg(ov5640_handle, 0x3103, 0X11);	//system clock from pad, bit[1]
	HW_OV5640_SCCB_write_reg(ov5640_handle, 0X3008, 0X82);
	HW_OV5640_delay_ms(ov5640_handle, 10);
}

uint16_t HW_OV5640_get_chip_id(HW_OV5640_HandleTypeDef* ov5640_handle)
{
	uint16_t id;
	id = HW_OV5640_SCCB_read_reg(ov5640_handle, HW_OV5640_CHIP_ID_H_ADDR);
	id <<= 8;
	id += HW_OV5640_SCCB_read_reg(ov5640_handle, HW_OV5640_CHIP_ID_L_ADDR);
	return id;
}

void HW_OV5640_set_jpeg(HW_OV5640_HandleTypeDef* ov5640_handle)
{
	uint16_t i=0; 
	for(i=0;i<(sizeof(OV5640_jpeg_reg_tbl)/4);i++)
	{
		HW_OV5640_SCCB_write_reg(ov5640_handle, OV5640_jpeg_reg_tbl[i][0], OV5640_jpeg_reg_tbl[i][1]);
	}  
}

void HW_OV5640_set_rgb565(HW_OV5640_HandleTypeDef* ov5640_handle) 
{
	uint16_t i=0;
	for(i=0;i<(sizeof(ov5640_rgb565_reg_tbl)/4);i++)
	{
		HW_OV5640_SCCB_write_reg(ov5640_handle, ov5640_rgb565_reg_tbl[i][0], ov5640_rgb565_reg_tbl[i][1]);
	} 
} 

void HW_OV5640_set_resolution(HW_OV5640_HandleTypeDef* ov5640_handle, uint16_t offset_x, uint16_t offset_y, uint16_t width, uint16_t height)
{
	HW_OV5640_SCCB_write_reg(ov5640_handle, 0X3212, 0X03);//开始组3
	
	//以下设置决定实际输出尺寸(带缩放)
	HW_OV5640_SCCB_write_reg(ov5640_handle, 0x3808, width>>8);
	HW_OV5640_SCCB_write_reg(ov5640_handle, 0x3809, width&0xff);
	HW_OV5640_SCCB_write_reg(ov5640_handle, 0x380a, height>>8);
	HW_OV5640_SCCB_write_reg(ov5640_handle, 0x380b, height&0xff);
	
	//以下设置决定输出尺寸在ISP上面的取图范围
	//范围:xsize-2*offx,ysize-2*offy
	HW_OV5640_SCCB_write_reg(ov5640_handle, 0x3810, offset_x>>8);
	HW_OV5640_SCCB_write_reg(ov5640_handle, 0x3811, offset_x&0xff);
	HW_OV5640_SCCB_write_reg(ov5640_handle, 0x3812, offset_y>>8);
	HW_OV5640_SCCB_write_reg(ov5640_handle, 0x3813, offset_y&0xff);

	HW_OV5640_SCCB_write_reg(ov5640_handle, 0X3212, 0X13);//结束组3
	HW_OV5640_SCCB_write_reg(ov5640_handle, 0X3212, 0Xa3);//启用组3设置
}

//void HW_OV5640_flash();

