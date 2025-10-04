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
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lwprintf/lwprintf.h"
#include "lwrb/lwrb.h"
#include "ee24.h"
#include "spif.h"
#include "multi_button.h"
#include "lwshell/lwshell.h"
#include "lwshell/lwshell_user.h"
#include "OsAPIs.h"
#include "SysTickTimer.h"
#include "TCB.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define OUTPUT_BUFFER_SIZE (64)
#define ARRAY_LEN(x)            (sizeof(x) / sizeof((x)[0]))
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
struct output_buffer {
    uint8_t buffer[OUTPUT_BUFFER_SIZE];
    int size;
};
struct output_buffer uart_buffer;
uint8_t uart_rx_buf[1];

EE24_HandleTypeDef ee24;
uint8_t erase_data[4] = {0xff, 0xff, 0xff, 0xff};
uint8_t in_data1[4] = {0x3, 0x3, 0x3, 0x3};
uint8_t in_data2[4] = {0xc, 0xc, 0xc, 0xc};
uint8_t out_data[4];
uint8_t spi_read[256];
uint8_t spi_write[256];
SPIF_HandleTypeDef spif;

static Button btn1;

lwrb_t usart_tx_rb;
uint8_t usart_tx_rb_data[128];

volatile size_t usart_tx_dma_current_len = 0;
uint8_t usart_rx_dma_buffer[64];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int lwprintf_self_func(int ch, lwprintf_t* p) {
    uint8_t c = (uint8_t)ch;

    /* Don't print zero */
    if (c == '\0') {
        HAL_UART_Transmit(&huart1, uart_buffer.buffer, uart_buffer.size, 100);
        uart_buffer.size = 0;
        return ch;
    }
    if(uart_buffer.size >= (OUTPUT_BUFFER_SIZE - 1)) {
        uart_buffer.buffer[uart_buffer.size++] = ch;
        HAL_UART_Transmit(&huart1, uart_buffer.buffer, uart_buffer.size, 100);
        uart_buffer.size = 0;
    } else {
      uart_buffer.buffer[uart_buffer.size++] = ch;
    }
    
    return ch;
}

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
            lwrb_write(&usart_tx_rb, &usart_rx_dma_buffer[old_pos], ARRAY_LEN(usart_rx_dma_buffer) - old_pos);
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
  __disable_irq();
  if(huart == &huart1) {
    usart_rx_check(Size);

    if (usart_tx_dma_current_len == 0 && (usart_tx_dma_current_len = lwrb_get_linear_block_read_length(&usart_tx_rb)) > 0) {
      //OS_SetEvent(TASK_5MS, EVT_TRIGGER_5MS_TASK);
      //lwshell_input(lwrb_get_linear_block_read_address(&usart_tx_rb), usart_tx_dma_current_len);

      //lwrb_skip(&usart_tx_rb, usart_tx_dma_current_len);
      //usart_tx_dma_current_len = 0;
    }
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, usart_rx_dma_buffer, sizeof(usart_rx_dma_buffer));
  }
  __enable_irq();
}

uint8_t read_button_gpio(uint8_t button_id) {
	uint8_t result;
	switch (button_id) {
		case 1:
			result = HAL_GPIO_ReadPin(USER_BUTTON_GPIO_Port, USER_BUTTON_Pin);
			break;
		default:
			return 0;
	}
	return result;
}

void btn1_single_click_handler(Button* btn) {
	(void)btn;
	lwprintf("Button 1: Single Click\r\n");
}

void btn1_double_click_handler(Button* btn) {
  (void)btn;
	lwprintf("Button 1: Double Click\r\n");
	HAL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);
}

void btn1_long_press_start_handler(Button* btn) {
  (void)btn;
  lwprintf("Button 1: Long Press Start\r\n");
}

void btn1_long_press_hold_handler(Button* btn) {
  (void)btn;
  lwprintf("Button 1: Long Press Hold...\r\n");
}

void btn1_press_repeat_handler(Button* btn) {
  lwprintf("Button 1: Press Repeat (count: %d)\n", button_get_repeat_count(btn));
}

void button_init_process(void) {
	// Initialize button 1 (active high for simulation)
    button_init(&btn1, read_button_gpio, 1, 1);
    
	// Attach event handlers for button 1
    button_attach(&btn1, BTN_SINGLE_CLICK, btn1_single_click_handler);
    button_attach(&btn1, BTN_DOUBLE_CLICK, btn1_double_click_handler);
    button_attach(&btn1, BTN_LONG_PRESS_START, btn1_long_press_start_handler);
    //button_attach(&btn1, BTN_LONG_PRESS_HOLD, btn1_long_press_hold_handler);
    //button_attach(&btn1, BTN_PRESS_REPEAT, btn1_press_repeat_handler);
	
	button_start(&btn1);
}

#if 0
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == &htim7) {
    button_ticks();
  }
}
#endif

void print_hex(uint8_t* in, uint32_t in_len) {
	uint32_t i;
	for(i=0; i<in_len; i++) {
		lwprintf("%02x", in[i]);
		if((i+1)%16 == 0) {
			lwprintf("\r\n");
		}
	}
	if(i%16 != 0) {
		lwprintf("\r\n");
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
  /* USER CODE BEGIN 2 */

  lwprintf_init(lwprintf_self_func);
  lwrb_init(&usart_tx_rb, usart_tx_rb_data, sizeof(usart_tx_rb_data));

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //HAL_UART_Receive_IT(&huart1, uart_rx_buf, sizeof(uart_rx_buf));
  //HAL_UART_Receive_DMA(&huart1, usart_rx_dma_buffer, sizeof(usart_rx_dma_buffer));
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, usart_rx_dma_buffer, sizeof(usart_rx_dma_buffer));

	lwprintf("System Start!!\r\n");
	
	//button_init_process();
	//HAL_TIM_Base_Start_IT(&htim7);

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
		lwprintf("EE24_Init failed..\r\n");
	}
#endif
	
#if 0
	if(SPIF_Init(&spif, &hspi1, SPI1_CS_GPIO_Port, SPI1_CS_Pin)) {
		for(int i=0; i<256; i++) {
			spi_write[i] = i;
		}
		lwprintf("spi_write:\r\n");
		print_hex(spi_write, sizeof(spi_write));
		
		SPIF_EraseSector(&spif, 0);
		SPIF_WritePage(&spif, 0, spi_write, sizeof(spi_write), 0);
		SPIF_ReadPage(&spif, 0, spi_read, sizeof(spi_read), 0);
		lwprintf("spi_read:\r\n");
		print_hex(spi_read, sizeof(spi_read));

	} else {
		lwprintf("SPIF_Init failed..\r\n");
	}
#endif
	
	OS_StartOS(APP_MODE_DEFAULT);

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // will never reach here
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
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

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM17)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  //if(htim == &htim7) {
  //  button_ticks();
  //}

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
