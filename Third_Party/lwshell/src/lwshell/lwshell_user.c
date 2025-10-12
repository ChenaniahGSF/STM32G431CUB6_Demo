/*
 * lwshell_user.c
 *
 *  Created on: Sep 29, 2025
 *      Author: Diamo
 */
#include "lwshell/lwshell.h"
#include "lwprintf/lwprintf.h"
#include "usart.h"
#include "rtc.h"

void lwshell_user_output(const char* str, struct lwshell* lwobj) {
  (void)lwobj;
  //lwprintf("%s", str);
  HAL_UART_Transmit(&huart1, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}

static int32_t hello_cmd(int32_t argc, char** argv) {
  lwshell_user_output("Hello, LwSHELL!\r\n", NULL);
  for (int32_t i = 0; i < argc; i++) {
    lwshell_user_output("Arg ", NULL);
    lwshell_user_output(argv[i], NULL);
    lwshell_user_output("\r\n", NULL);
  }
  return 0;
}

static int get_hal_rtc_weekday(int year, int month, int day) {
    int q, m, K, J, h;

    // 调整1月和2月为上一年的13月和14月
    if (month < 3) {
        month += 12;
        year--;
    }

    q = day;
    m = month;
    K = year % 100;    // 年份后两位
    J = year / 100;    // 年份前两位

    // 基姆拉尔森公式
    h = (q + (13 * (m + 1)) / 5 + K + K / 4 + J / 4 + 5 * J) % 7;

    // 转换结果：公式h=0→周六，1→周日，2→周一...6→周五
    // 目标输出：1=周一，...，6=周六，7=周日
    switch(h) {
        case 0: return 6;   // 周六
        case 1: return 7;   // 周日
        case 2: return 1;   // 周一
        case 3: return 2;   // 周二
        case 4: return 3;   // 周三
        case 5: return 4;   // 周四
        case 6: return 5;   // 周五
        default: return -1; // 错误
    }
}

static int32_t set_time_cmd(int32_t argc, char** argv) {
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  const char* weekday_str[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

  if(argc == 1) {
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    printf("Date: 20%02d-%02d-%02d %02d:%02d:%02d %s\r\n",
             sDate.Year,
             sDate.Month,
             sDate.Date,
             sTime.Hours,
             sTime.Minutes,
             sTime.Seconds,
             weekday_str[sDate.WeekDay - 1]);
  }
  return 0;
}

void lwshell_user_init(void) {
  lwshell_init();
  lwshell_set_output_fn(lwshell_user_output);
  lwshell_register_cmd("hello", hello_cmd, "Print hello message with args");
  lwshell_register_cmd("date", set_time_cmd, "simulate date command");
}

