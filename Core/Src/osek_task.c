/*
 * osek_task.c
 *
 *  Created on: Oct 4, 2025
 *      Author: Diamo
 */
#include"TCB.h"
#include"OsAPIs.h"
#include "main.h"

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
