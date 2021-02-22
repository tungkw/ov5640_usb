/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dcmi.h"
#include "dma.h"
#include "usb_device.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include "hardware.h"
//#include "sdram.h"
#include "usb_app.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define BUFFER_SIZE 2*1024
#define DATA_SIZE 30*1024

uint32_t jpeg_data[DATA_SIZE]__attribute__((at(0XC0000000+1280*800*2)));
uint32_t jpeg_buffer[2][BUFFER_SIZE];
uint32_t jpeg_data_len = 0;
uint8_t jpeg_process_state = 0;
uint8_t current_buffer = 0;
uint8_t buf[4];


void save_buffer(uint8_t buffer_num)
{
	uint32_t *p = jpeg_data + jpeg_data_len;
	for(uint32_t i=0; i<BUFFER_SIZE; i++)
	{
		p[i] = jpeg_buffer[buffer_num][i];
	}
	jpeg_data_len = jpeg_data_len+BUFFER_SIZE;
}


void data_process()
{
	if(jpeg_process_state == 0)
	{
		save_buffer(current_buffer);
		jpeg_process_state = 1;
	}
	else if(jpeg_process_state == 2)
	{
		jpeg_process_state = 0;
		jpeg_data_len = 0;
	}
}


//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
//{
//	if(jpeg_process_state == 0)
//	{
//		save_buffer(1);
//	}
//	current_buffer = 0;
//}

//void HAL_TIM_IC_CaptureHalfCpltCallback(TIM_HandleTypeDef *htim)
//{
//	if(jpeg_process_state == 0)
//	{
//		save_buffer(0);
//	}
//	current_buffer = 1;
//}

//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//	if(GPIO_Pin == GPIO_PIN_4)
//	{
//		if(jpeg_process_state)
//		{
//			jpeg_data[jpeg_data_len++] = GPIOI->IDR;
//		}
//	}
//	else if(GPIO_Pin == GPIO_PIN_7)
//	{
//		data_process();
//	}
//}


void HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	data_process();//jpeg数据处理
  __HAL_DCMI_ENABLE_IT(hdcmi, DCMI_FLAG_VSYNCRI);
}

void DCMI_DMAXferCplt(DMA_HandleTypeDef *hdma)
{
	if(jpeg_process_state==0)
	{
		HW_LED_turn(&led0);
		save_buffer(0);
	}
	current_buffer = 1;
}

void DCMI_DMAXferM1Cplt(DMA_HandleTypeDef *hdma)
{
	if(jpeg_process_state==0)
	{
		HW_LED_turn(&led1);
		save_buffer(1);
	}
	current_buffer = 0;
}

void DCMI_DMAXferError(DMA_HandleTypeDef *hdma)
{
	UNUSED(hdma);
}

HAL_StatusTypeDef dcmi_start(DCMI_HandleTypeDef* hdcmi, uint32_t DCMI_Mode, uint32_t pData, uint32_t pData2, uint32_t Length)
{
  /* Process Locked */
  __HAL_LOCK(hdcmi);

  /* Lock the DCMI peripheral state */
  hdcmi->State = HAL_DCMI_STATE_BUSY;
  
  /* Enable DCMI by setting DCMIEN bit */
  __HAL_DCMI_ENABLE(hdcmi);

  /* Configure the DCMI Mode */
  hdcmi->Instance->CR &= ~(DCMI_CR_CM);
  hdcmi->Instance->CR |=  (uint32_t)(DCMI_Mode);
  
  hdcmi->XferCount = 0U;
  hdcmi->XferTransferNumber = 0U;
	
//	__HAL_UNLOCK(&hdma_dcmi);
	hdcmi->DMA_Handle->XferCpltCallback = DCMI_DMAXferCplt;
	hdcmi->DMA_Handle->XferM1CpltCallback = DCMI_DMAXferM1Cplt;
	hdcmi->DMA_Handle->XferErrorCallback = DCMI_DMAXferError;
	HAL_DMAEx_MultiBufferStart_IT(hdcmi->DMA_Handle, (uint32_t)&hdcmi->Instance->DR, (uint32_t)pData, (uint32_t)pData2, Length);

  /* Enable Capture */
  hdcmi->Instance->CR |= DCMI_CR_CAPTURE;

  /* Release Lock */
  __HAL_UNLOCK(hdcmi);
  return HAL_OK;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USB_DEVICE_Init();
  MX_DCMI_Init();
  MX_FMC_Init();
  /* USER CODE BEGIN 2 */
	
	// sdram
//	SDRAM_Init();
	
	// led
	led_init();
	HW_LED_set(&led0, HW_LED_OFF);
	HW_LED_set(&led1, HW_LED_OFF);
	delay_ms(1000);
	
	// ov5640
	ov5640_init();
 	HW_OV5640_set_rgb565(&ov5640);
////	OV5640_Focus_Init();
	HW_OV5640_set_jpeg(&ov5640);
////	OV5640_Light_Mode(0);	//自动模式
////	OV5640_Color_Saturation(3);//色彩饱和度0
////	OV5640_Brightness(4);	//亮度0
////	OV5640_Contrast(3);		//对比度0
////	OV5640_Sharpness(33);	//自动锐度
////	OV5640_Focus_Constant();//启动持续对焦
 	HW_OV5640_set_resolution(&ov5640, 4,0,320,240);//设置输出尺寸 

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//	jpeg_process_state = 2;
//  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
//	HAL_TIM_IC_Start_DMA(&htim8, TIM_CHANNEL_1, (uint32_t*)jpeg_buffer, BUFFER_SIZE*2);
//	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

//	HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)jpeg_buffer, BUFFER_SIZE*2);
	dcmi_start(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)&jpeg_buffer[0], (uint32_t)&jpeg_buffer[1], BUFFER_SIZE);
	
	while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if(jpeg_process_state==1)
		{
			uint8_t *p = (uint8_t*)jpeg_data;
			uint8_t find_head = 0;
			uint32_t head_idx = 0;
			uint32_t jpeg_len = 0;
			if(jpeg_data_len)
			{
				for(uint32_t i = 0 ; i < jpeg_data_len*4-1; i++)
				{
					if(p[i]==0xff && p[i+1]==0xd8)
					{
						head_idx = i;
						find_head = 1;
					}
					else if(p[i]==0xff && p[i+1]==0xd9 && find_head)
					{
						jpeg_len = i+2-head_idx;
						break;
					}
				}
			}
			// if data correct transmit jpeg via usb
			if(jpeg_len)
			{
//				USB2PC((uint8_t*)jpeg_data+head_idx, jpeg_len);
				CDC_Transmit_FS((uint8_t*)jpeg_data + head_idx, jpeg_len);
			}
			// process finish;
			jpeg_process_state = 2;
		}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 15;
  RCC_OscInitStruct.PLL.PLLN = 144;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
