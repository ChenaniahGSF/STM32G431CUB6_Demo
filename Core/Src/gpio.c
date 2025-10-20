/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "logger.h"

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_BUTTON_Pin */
  GPIO_InitStruct.Pin = USER_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(USER_BUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI1_CS_Pin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : EC11_A_Pin EC11_B_Pin */
  GPIO_InitStruct.Pin = EC11_A_Pin|EC11_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : EC11_KEY_Pin */
  GPIO_InitStruct.Pin = EC11_KEY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(EC11_KEY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USER_LED_Pin */
  GPIO_InitStruct.Pin = USER_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USER_LED_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 12, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 12, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

}

/* USER CODE BEGIN 2 */
int32_t rolling_absolute = 0;
int32_t rolling_counter = 0;  //Clockwise +, counterclockwise -
uint8_t rolling_flag = 2; //Clockwise:0, counterclockwise:1, initial:2

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  static uint8_t count = 0;
  static uint8_t b_flag;
  GPIO_PinState a_value = HAL_GPIO_ReadPin(EC11_A_GPIO_Port, EC11_A_Pin);
  GPIO_PinState b_value = HAL_GPIO_ReadPin(EC11_B_GPIO_Port, EC11_B_Pin);

  if (GPIO_Pin == EC11_A_Pin)
  {
    if (a_value == GPIO_PIN_RESET && count == 0)
    {
      b_flag = 0;
      if (b_value)
        b_flag = 1;
      count = 1;
    }

    if (a_value == GPIO_PIN_SET && count == 1)
    {
      if (b_value == GPIO_PIN_RESET && b_flag == 1)
      {
    	// Clockwise
    	rolling_counter++;
    	rolling_absolute++;
        rolling_flag = 0;
        logger_info("rolling_flag = %d, rolling_counter = %d, rolling_absolute = %d", rolling_flag, rolling_counter, rolling_absolute);
      }
      if (b_value && b_flag == 0)
      {
      	// counterclockwise
      	rolling_counter--;
      	rolling_absolute = (rolling_absolute > 0) ? (rolling_absolute - 1) : 0;
        rolling_flag = 1;
        logger_info("rolling_flag = %d, rolling_counter = %d, rolling_absolute = %d", rolling_flag, rolling_counter, rolling_absolute);
      }
      count = 0;
    }
  }
}
/* USER CODE END 2 */
