/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "rng.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "logger.h"
#include "lwrb/lwrb.h"
#include "ee24.h"
#include "spif.h"
#include "multi_button_user.h"
#include "lwshell/lwshell.h"
#include "lwshell/lwshell_user.h"
#include "lwutil/lwutil.h"
#include "smarttimer_user.h"
//#include "ssd1306_tests.h"

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
uint8_t uart_rx_buf[1];

EE24_HandleTypeDef ee24;
uint8_t erase_data[4] = {0xff, 0xff, 0xff, 0xff};
uint8_t in_data1[4] = {0x3, 0x3, 0x3, 0x3};
uint8_t in_data2[4] = {0xc, 0xc, 0xc, 0xc};
uint8_t out_data[4];
uint8_t spi_read[256];
uint8_t spi_write[256];
SPIF_HandleTypeDef spif;

lwrb_t usart_tx_rb;
uint8_t usart_tx_rb_data[128];

volatile size_t usart_tx_dma_current_len = 0;
uint8_t usart_rx_dma_buffer[64];

uint8_t random_buffer[37];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void usart_rx_check(size_t pos) {
    static size_t old_pos;

    /* Calculate current position in buffer and check for new data available */
    if (pos != old_pos) {                       /* Check change in received data */
        if (pos > old_pos) {                    /* Current position is over previous one */
            /*
             * Processing is done in "linear" mode.
             *
             * Application processing is fast with single data block,
             * length is simply calculated by subtracting pointers
             *
             * [   0   ]
             * [   1   ] <- old_pos |------------------------------------|
             * [   2   ]            |                                    |
             * [   3   ]            | Single block (len = pos - old_pos) |
             * [   4   ]            |                                    |
             * [   5   ]            |------------------------------------|
             * [   6   ] <- pos
             * [   7   ]
             * [ N - 1 ]
             */
            lwrb_write(&usart_tx_rb, &usart_rx_dma_buffer[old_pos], pos - old_pos);
        } else {
            /*
             * Processing is done in "overflow" mode..
             *
             * Application must process data twice,
             * since there are 2 linear memory blocks to handle
             *
             * [   0   ]            |---------------------------------|
             * [   1   ]            | Second block (len = pos)        |
             * [   2   ]            |---------------------------------|
             * [   3   ] <- pos
             * [   4   ] <- old_pos |---------------------------------|
             * [   5   ]            |                                 |
             * [   6   ]            | First block (len = N - old_pos) |
             * [   7   ]            |                                 |
             * [ N - 1 ]            |---------------------------------|
             */
            lwrb_write(&usart_tx_rb, &usart_rx_dma_buffer[old_pos], LWUTIL_ARRAYSIZE(usart_rx_dma_buffer) - old_pos);
            if (pos > 0) {
                lwrb_write(&usart_tx_rb, &usart_rx_dma_buffer[0], pos);
            }
        }
        old_pos = pos;                          /* Save current position as old for next transfers */
    }
}

#if 0
//uart interrupt mode, without dma
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  lwshell_input(uart_rx_buf, 1);
  HAL_UART_Receive_IT(&huart1, uart_rx_buf, 1);
}
#endif

//uart HT TC IDLE event will trigger this callback, dma mode enable
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  if(huart == &huart1) {
    usart_rx_check(Size);

    if (usart_tx_dma_current_len == 0 && (usart_tx_dma_current_len = lwrb_get_linear_block_read_length(&usart_tx_rb)) > 0) {
      lwshell_input(lwrb_get_linear_block_read_address(&usart_tx_rb), usart_tx_dma_current_len);

      lwrb_skip(&usart_tx_rb, usart_tx_dma_current_len);
      usart_tx_dma_current_len = 0;
    }
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, usart_rx_dma_buffer, sizeof(usart_rx_dma_buffer));
  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  stim_init();
  //set_timetick();
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
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM7_Init();
  MX_RNG_Init();
  MX_I2C2_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  logger_init();
  lwrb_init(&usart_tx_rb, usart_tx_rb_data, sizeof(usart_tx_rb_data));

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //HAL_UART_Receive_IT(&huart1, uart_rx_buf, sizeof(uart_rx_buf));
  //HAL_UART_Receive_DMA(&huart1, usart_rx_dma_buffer, sizeof(usart_rx_dma_buffer));
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, usart_rx_dma_buffer, sizeof(usart_rx_dma_buffer));

	logger_info("System Start!!!");

	buttons_init();
	HAL_TIM_Base_Start_IT(&htim7);

	lwshell_user_init();

#if 0
	if(EE24_Init(&ee24, &hi2c1, EE24_ADDRESS_DEFAULT)){
		EE24_Write(&ee24, 0x0, erase_data, sizeof(erase_data), 1000);
		HAL_Delay(10);
		
		EE24_Write(&ee24, 0x0, in_data1, sizeof(in_data1), 1000);
		HAL_Delay(10);
		
		EE24_Read(&ee24, 0x0, out_data, sizeof(out_data), 1000);
		HAL_Delay(10);
		print_hex(out_data, sizeof(out_data));
		
		EE24_Write(&ee24, 0x0, in_data2, sizeof(in_data2), 1000);
		HAL_Delay(10);
		
		EE24_Read(&ee24, 0x0, out_data, sizeof(out_data), 1000);
		HAL_Delay(10);
		print_hex(out_data, sizeof(out_data));
		
	} else {
		logger_error("EE24_Init failed..");
	}
#endif
	
#if 0
	if(SPIF_Init(&spif, &hspi1, SPI1_CS_GPIO_Port, SPI1_CS_Pin)) {
		for(int i=0; i<256; i++) {
			spi_write[i] = i;
		}
		logger_info("spi_write:");
		logger_hex(spi_write, sizeof(spi_write));
		
		SPIF_EraseSector(&spif, 0);
		SPIF_WritePage(&spif, 0, spi_write, sizeof(spi_write), 0);
		SPIF_ReadPage(&spif, 0, spi_read, sizeof(spi_read), 0);
		logger_info("spi_read:");
		logger_hex(spi_read, sizeof(spi_read));

	} else {
		logger_error("SPIF_Init failed..");
	}
#endif

	//RNG_Init();
	//RNG_Enable_IRQ();
	//ssd1306_TestAll();
	//stim_loop(1000, print_data_time, STIM_LOOP_FOREVER);
	//stim_runlater(5000, print_data_time);
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    //HAL_Delay(5000);
    //RNG_Get_Random(random_buffer, sizeof(random_buffer));
    //logger_hex(random_buffer, sizeof(random_buffer));
    stim_mainloop();
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 36;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV6;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM17 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */
  if (htim->Instance == TIM17)
  {
    stim_tick();
  }
  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM17)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  if(htim->Instance == TIM7) {
    button_ticks();
  }
  /* USER CODE END Callback 1 */
}

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
#ifdef USE_FULL_ASSERT
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
