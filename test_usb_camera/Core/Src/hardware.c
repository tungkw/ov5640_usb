#include "hardware.h"

void delay_us(uint32_t nus)
{
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;				 
	ticks = SystemCoreClock/1000000 * nus; 					
	told = SysTick->VAL;        				
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			
		}  
	};
}

void delay_ms(uint32_t nms)
{
	uint32_t i=0;
	while(i < nms){
		delay_us(1000);
		i++;
	}
}


HW_LED_HandleTypeDef led0;
HW_LED_HandleTypeDef led1;
void led_init()
{
	led0.GPIO = GPIOB;
	led0.Pin = GPIO_PIN_1;
	
	led1.GPIO = GPIOB;
	led1.Pin = GPIO_PIN_0;
}



HW_OV5640_HandleTypeDef ov5640;
void ov5640_rst(uint8_t state){HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, (GPIO_PinState)state);}
void ov5640_pwdn(uint8_t state){HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5, (GPIO_PinState)state);}
void ov5640_scl(uint8_t state){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, (GPIO_PinState)state);}
void ov5640_sda_out(uint8_t state){HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, (GPIO_PinState)state);}
uint8_t ov5640_sda_in(){return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3);}
void ov5640_sda_switch(uint8_t state)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Mode = state == 0 ? GPIO_MODE_INPUT : GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
void ov5640_init()
{
	ov5640.RST = ov5640_rst;
	ov5640.PWDN = ov5640_pwdn;
	ov5640.SCL = ov5640_scl;
	ov5640.SDA_out = ov5640_sda_out;
	ov5640.SDA_in = ov5640_sda_in;
	ov5640.SDA_switch = ov5640_sda_switch;
	ov5640.delay_us = delay_us;
	HW_OV5640_init(&ov5640);
	
	// check chip id
	while(HW_OV5640_get_chip_id(&ov5640)!=HW_OV5640_CHIP_ID);
	
	// flash light
	HW_OV5640_SCCB_write_reg(&ov5640, 0x3016, 0X02);
	HW_OV5640_SCCB_write_reg(&ov5640, 0x301C, 0X02);
	HW_OV5640_SCCB_write_reg(&ov5640, 0X3019, 0X02);
	delay_ms(50);
	HW_OV5640_SCCB_write_reg(&ov5640, 0X3019, 0X00);
}




