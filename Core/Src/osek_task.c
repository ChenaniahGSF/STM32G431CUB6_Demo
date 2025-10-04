/*
 * osek_task.c
 *
 *  Created on: Oct 4, 2025
 *      Author: Diamo
 */
#include"TCB.h"
#include"OsAPIs.h"
#include "main.h"
#include "lwshell/lwshell.h"
#include "lwrb/lwrb.h"

extern volatile size_t usart_tx_dma_current_len;
extern lwrb_t usart_tx_rb;

TASK(T1)
{
  const OsEventMaskType OsWaitEventMask = (OsEventMaskType) EVT_BLINK_BLUE_LED;

  (void)OS_SetRelAlarm(ALARM_BLUE_LED, 0, 997);

  for(;;)
  {
    OsEventMaskType Events = (OsEventMaskType) 0U;

    if(E_OK == OS_WaitEvent(OsWaitEventMask))
    {
      (void) OS_GetEvent((OsTaskType)T1, &Events);

      if((Events & EVT_BLINK_BLUE_LED) == EVT_BLINK_BLUE_LED)
      {
        OS_ClearEvent(EVT_BLINK_BLUE_LED);
        // task here
        HAL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);
      }
    }
    else
    {
      OS_TerminateTask(); /* In case of error we switch off the task */
    }
  }
}

TASK(Idle)
{
  const OsEventMaskType OsWaitEventMask = (OsEventMaskType) EVT_BLINK_GREEN_LED;

  (void)OS_SetRelAlarm(ALARM_GREEN_LED,0, 503);

  for(;;)
  {
    OsEventMaskType Events = (OsEventMaskType) 0U;

    if(E_OK == OS_WaitEvent(OsWaitEventMask))
    {
      (void) OS_GetEvent((OsTaskType)Idle, &Events);

      if((Events & EVT_BLINK_GREEN_LED) == EVT_BLINK_GREEN_LED)
      {
        OS_ClearEvent(EVT_BLINK_GREEN_LED);

        // task here
      }
    }
    else
    {
      OS_TerminateTask(); /* In case of error we switch off the task */
    }
  }
}

TASK(TASK_5MS)
{
  const OsEventMaskType OsWaitEventMask = (OsEventMaskType) EVT_TRIGGER_5MS_TASK;

  (void)OS_SetRelAlarm(ALARM_5MS_TASK, 0, 2);

  for(;;)
  {
    OsEventMaskType Events = (OsEventMaskType) 0U;

    if(E_OK == OS_WaitEvent(OsWaitEventMask))
    {
      (void) OS_GetEvent((OsTaskType)TASK_5MS, &Events);

      if((Events & EVT_TRIGGER_5MS_TASK) == EVT_TRIGGER_5MS_TASK)
      {
        OS_ClearEvent(EVT_TRIGGER_5MS_TASK);
        // task here
        if (usart_tx_dma_current_len > 0) {
          lwshell_input(lwrb_get_linear_block_read_address(&usart_tx_rb), usart_tx_dma_current_len);

          lwrb_skip(&usart_tx_rb, usart_tx_dma_current_len);
          usart_tx_dma_current_len = 0;
        }
      }
    }
    else
    {
      OS_TerminateTask(); /* In case of error we switch off the task */
    }
  }
}
