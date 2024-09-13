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
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mpu6050.h"
#include "dgh_esp8266.h"
#include "character_lcd.h"

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
MPU6050_t MPU6050;
double x, y, z;
double abs_x, abs_y, abs_z;
char temp, c_max, pn; //pn: +/-
double limit = 8;
uint8_t tcp_flag = 0, ma = 30;
//uint8_t cnt = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
char max3(double, double, double);
double my_abs(double);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	
  LCD_Config.Font = _FONT_5X7; 
  LCD_Config.Mode = _4BIT_INTERFACE; 
  LCD_Config.NumberOfLine = _2LINE_DISPLAY; 
  LCD_Init();
  HAL_Delay(1);
  LCD_GotoXY(0, 0);
  LCD_PutString("   WELCOME! :)");
  HAL_Delay(2000);
  LCD_Clear();
	
	if(ESP_IsDeviceReady() == HAL_OK)
	{
		LCD_PutString("ESP Ready");
		HAL_Delay(3000);
		LCD_Clear();
	}
	if(ESP_StartConnection("192.168.43.153", 333) == HAL_OK)
	{
		tcp_flag = 1;
		LCD_PutString("TCP Connected");
		HAL_Delay(2000);
		LCD_Clear();
	}
	else
	{
		tcp_flag = 0;
		LCD_PutString("TCP Failed");
		HAL_Delay(2000);
		LCD_Clear();
	}
	
	while(MPU6050_Init(&hi2c1) == 1){};
	LCD_PutString("GY-521 OK");
	HAL_Delay(2000);
	LCD_Clear();
		
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		x = 0; y = 0; z = 0;
		for(uint8_t i=0; i<ma; i+=1) 
		{
			MPU6050_Read_All(&hi2c1, &MPU6050);
			x += MPU6050.Gx;
			y += MPU6050.Gy;
			z += MPU6050.Gz;
		}
		abs_x = my_abs(x)/ma;
		abs_y = my_abs(y)/ma;
		abs_z = my_abs(z)/ma;
		temp = max3(abs_x, abs_y, abs_z);
		if(c_max == '-')
			pn = '-';
		else if(c_max == 'x')
		{
			if(x > 0)
				pn = 'p';
			else
				pn = 'n';
		}
		else if(c_max == 'y')
		{
			if(y > 0)
				pn = 'p';
			else
				pn = 'n';
		}
		else
		{
			if(z > 0)
				pn = 'p';
			else
				pn = 'n';
		}
		if(tcp_flag)
		{
			ESP_Transmit(c_max, pn);
		}
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
		LCD_GotoXY(0, 0);
		LCD_PutChar(c_max);
		LCD_GotoXY(1, 0);
		LCD_PutChar(pn);
		HAL_Delay(100);
  }
}

char max3(double x, double y, double z)
{
	if(x > y)
	{
		if(x > z)  //x>z,y
		{
			if(x > limit)  
				c_max = 'x';
			else
				c_max = '-';
			return 'x';
		}
		else  //z>x>y
		{
			if(z > limit)
				c_max = 'z';
			else
				c_max = '-';
			return 'z';
		}
	}
	else if(y > z)  //y>x,z
	{
		if(y > limit)
			c_max = 'y';
		else
			c_max = '-';
		return 'y';
	}
	if(z > limit)  //z>y>x
		c_max = 'z';
	else
		c_max = '-';
	return 'z';
}

double my_abs(double x)
{
	if(x > 0)
		return x;
	return -1*x;
}
  /* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
