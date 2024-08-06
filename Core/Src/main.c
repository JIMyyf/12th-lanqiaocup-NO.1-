/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "led.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "time.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
  char car_type[5], car_num[5];
  int year, month, day, hour, min, sec;
  bool isempty;
} car_stop;
car_stop car[8];      // 停车场每个车的信息
car_stop car_receive; // 串口发送过来的数据

typedef struct
{
  float cnbr_spend, vnbr_spend;
  int cnbr_num, vnbr_num;
} stop_msg;

stop_msg stop_message; // 停车场的整体信息
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
int mode = 0;
bool B4 = true;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
bool date_legality(int year, int month, int day, int hour, int minute, int second)
{
  if (month < 1 || month > 12)
  {
    return false;
  }
  else
  {
    if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12)
    {
      if (day < 1 && day > 31)
      {
        return false;
      }
    }
    else if (month == 4 || month == 6 || month == 9 || month == 11)
    {
      if (day < 1 && day > 30)
      {
        return false;
      }
    }
    else
    {
      if ((year % 100 != 0 && year % 4 == 0) || year % 400 == 0)
      { // 闰年
        if (day < 1 && day > 29)
        {
          return false;
        }
      }
      else
      {
        if (day < 1 && day > 28)
        {
          return false;
        }
      }
    }
  }
  if ((hour > 24 || hour < 0) && (minute > 59 || minute < 0) && (second > 59 || second < 0))
  {
    return false;
  }
  return true;
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
bool check_car_type(char car_type[], char standard_type[])
{
  if (strcmp(car_type, standard_type) == 0)
  {
    return true;
  }
  return false;
}
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
int differ_time(int i)
{
  struct tm time_start = {0}, time_end = {0};
  time_t time_diff;

  time_start.tm_year = car_receive.year + 100; // 从1900年开始算
  time_start.tm_mon = car_receive.month - 1;
  time_start.tm_mday = car_receive.day;
  time_start.tm_hour = car_receive.hour;
  time_start.tm_min = car_receive.min;
  time_start.tm_sec = car_receive.sec;

  time_end.tm_year = car[i].year - 1900;
  time_end.tm_mon = car[i].month - 1;
  time_end.tm_mday = car[i].day;
  time_end.tm_hour = car[i].hour;
  time_end.tm_min = car[i].min;
  time_end.tm_sec = car[i].sec;

  time_t ts = mktime(&time_start);
  time_t te = mktime(&time_end);

  time_diff = difftime(ts, te);

  int days = time_diff / (24 * 3600);
  int hours = (time_diff % (24 * 3600)) / 3600;
  int minutes = (time_diff % 3600) / 60;
  int seconds = time_diff % 60;

  if (days >= 0 && hours >= 0 && minutes >= 0 && seconds >= 0)
  {
    return (days * 24 + hours + (minutes > 0 ? 1 : 0));
  }
  return -1;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void car_check(void)
{
  if (strlen(USART1_RECEIVE_ARRAY) == 22) // 字符长度22
  {
    sscanf(USART1_RECEIVE_ARRAY, "%[^:]:%[^:]:%2d%2d%2d%2d%2d%2d",
           car_receive.car_type, car_receive.car_num, &car_receive.year, &car_receive.month,
           &car_receive.day, &car_receive.hour, &car_receive.min, &car_receive.sec);
    if (date_legality(car_receive.year, car_receive.month, car_receive.day,
                      car_receive.hour, car_receive.min, car_receive.sec) &&
        (check_car_type(car_receive.car_type, (char *)"VNBR\0") || check_car_type(car_receive.car_type, (char *)"CNBR\0")))
    {
      for (int i = 0; i < 8; i++)
      {
        if (car[i].isempty == false && check_car_type(car[i].car_num, car_receive.car_num))
        {
          car[i].isempty = true;
          int ans = differ_time(i);
          if (ans != -1)
          {
            if (check_car_type(car_receive.car_type, (char *)"CNBR\0"))
            {
              printf("%s:%s:%d:%.2f\r\n", car_receive.car_type, car_receive.car_num, ans,
                     ans * stop_message.cnbr_spend);
              stop_message.cnbr_num--;
            }
            else
            {
              printf("%s:%s:%d:%.2f\r\n", car_receive.car_type, car_receive.car_num, ans,
                     ans * stop_message.vnbr_spend);
              stop_message.vnbr_num--;
            }
          }
          else
          {
            printf("\r\nError\r\n");
          }
          return;
        }
      }
      int j = 0;
      while (car[j].isempty == false)
        j++;
      if (j > 7)
      {
        printf("\r\nError\r\n");
        return;
      }
      strcpy(car[j].car_num, car_receive.car_num);
      strcpy(car[j].car_type, car_receive.car_type);
      car[j].year = car_receive.year + 2000;
      car[j].month = car_receive.month;
      car[j].day = car_receive.day;
      car[j].hour = car_receive.hour;
      car[j].min = car_receive.min;
      car[j].sec = car_receive.sec;
      car[j].isempty = false;
      if (strcmp(car_receive.car_type, "CNBR\0") == 0)
      {
        stop_message.cnbr_num++;
      }
      else
      {
        stop_message.vnbr_num++;
      }
    }
    else
    {
      printf("\r\nError\r\n");
    }
  }
  else
  {
    printf("\r\nError\r\n");
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
  MX_TIM6_Init();
  MX_TIM17_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  led_init();
  LCD_Init();
  LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_PWM_Start_IT(&htim17, TIM_CHANNEL_1);
  HAL_UART_Receive_IT(&huart1, (uint8_t *)USART1_RECEIVE_DATA, USART1_RECEIVE_BUUFER_SIZE);
  __HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 0);

  car[0].isempty = true;
  car[1].isempty = true;
  car[2].isempty = true;
  car[3].isempty = true;
  car[4].isempty = true;
  car[5].isempty = true;
  car[6].isempty = true;
  car[7].isempty = true;

  stop_message.cnbr_num = 0;
  stop_message.vnbr_num = 0;
  stop_message.cnbr_spend = 3.5;
  stop_message.vnbr_spend = 2;
  while (1)
  {
    if (USART1_RECEIVE_STATE & 0x8000)
    {
      USART1_RECEIVE_STATE = 0;
      car_check();
    }
    if (8 - stop_message.cnbr_num - stop_message.vnbr_num > 0)
    {
      led_ON(1);
    }
    else
    {
      led_OFF(1);
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == 0)
    {
      while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == 0)
        ;
      mode++;
      if (mode == 2)
        mode = 0;
    }
    else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == 0)
    {
      while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == 0)
        ;
      if (mode == 1)
      {
        stop_message.cnbr_spend += 0.5;
        stop_message.vnbr_spend += 0.5;
      }
    }
    else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == 0)
    {
      while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == 0)
        ;
      if (mode == 1)
      {
        stop_message.cnbr_spend -= 0.5;
        stop_message.vnbr_spend -= 0.5;
      }
    }
    else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == 0)
    {
      while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == 0)
        ;
      if (B4)
      {
        B4 = false;
        __HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 20);
        led_ON(2);
      }
      else
      {
        B4 = true;
        __HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 0);
        led_OFF(2);
      }
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
char text[30];
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6)
  {
    if (mode == 0)
    {
      LCD_DisplayStringLine(Line2, (u8 *)"        Data    ");
      sprintf(text, "   CNBR:%d    ", stop_message.cnbr_num);
      LCD_DisplayStringLine(Line4, (u8 *)text);
      sprintf(text, "   VNBR:%d    ", stop_message.vnbr_num);
      LCD_DisplayStringLine(Line6, (u8 *)text);
      sprintf(text, "   IDLE:%d    ", 8 - stop_message.cnbr_num - stop_message.vnbr_num);
      LCD_DisplayStringLine(Line8, (u8 *)text);
    }
    else if (mode == 1)
    {
      LCD_DisplayStringLine(Line2, (u8 *)"        Para    ");
      sprintf(text, "   CNBR:%.2f    ", stop_message.cnbr_spend);
      LCD_DisplayStringLine(Line4, (u8 *)text);
      sprintf(text, "   VNBR:%.2f    ", stop_message.vnbr_spend);
      LCD_DisplayStringLine(Line6, (u8 *)text);
      LCD_DisplayStringLine(Line8, (u8 *)"              ");
    }
  }
}
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
