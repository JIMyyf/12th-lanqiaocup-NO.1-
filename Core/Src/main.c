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
#include "led.h"
#include "lcd.h"
#include "stdbool.h"
#include "stdio.h"
#include "string.h"
#include "time.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
    float cnbr_spend, vnbr_spend;
    int cnbr_num, vnbr_num;
} car_num;
car_num car_msg; // 停车坪车辆数量管理
typedef struct
{
    char car_type[5], car_num[5];
    int year, month, day, hour, minute, second;
    bool isempty;
} car_stop;
car_stop stop[8];
car_stop receive;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define all 8 // 停车坪数量总数
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

int differ_time(int i)
{
    struct tm time1 = {0}, time2 = {0};
    time_t time_diff;

    time1.tm_year = receive.year + 100; // 从1900开始算
    time1.tm_mon = receive.month - 1;
    time1.tm_mday = receive.day;
    time1.tm_hour = receive.hour;
    time1.tm_min = receive.minute;
    time1.tm_sec = receive.second;

    time2.tm_year = stop[i].year - 1900;
    time2.tm_mon = stop[i].month - 1;
    time2.tm_mday = stop[i].day;
    time2.tm_hour = stop[i].hour;
    time2.tm_min = stop[i].minute;
    time2.tm_sec = stop[i].second;

    time_t t1 = mktime(&time1);
    time_t t2 = mktime(&time2);

    time_diff = difftime(t1, t2);

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

<<<<<<< HEAD
void usart_callback(void)
{
    if (strlen(USART1_RECEIVE_ARRAY) == 22)
    {
        sscanf(USART1_RECEIVE_ARRAY, "%[^:]:%[^:]:%2d%2d%2d%2d%2d%2d", receive.car_type, receive.car_num,
               &receive.year, &receive.month, &receive.day, &receive.hour, &receive.minute, &receive.second);
        if ((strcmp(receive.car_type, "CNBR") == 0 || strcmp(receive.car_type, "VNBR") == 0) && date_legality(
                                                                                                            receive.year, receive.month, receive.day, receive.hour, receive.minute, receive.second))
        {
            for (int i = 0; i < 8; i++)
            {
                if (stop[i].isempty == true && strcmp(stop[i].car_num, receive.car_num) == 0)
                { // 车出库
                    stop[i].isempty = false;
                    int ans = differ_time(i);
                    if (ans != -1)
                    {
                        if (strcmp(receive.car_type, "CNBR") == 0)
                        {
                            printf("%s:%s:%d:%.2f\r\n", receive.car_type, receive.car_num, ans, ans * car_msg.cnbr_spend);
                        }
                        else
                        {
                            printf("%s:%s:%d:%.2f\r\n", receive.car_type, receive.car_num, ans, ans * car_msg.vnbr_spend);
                        }
                        if (strcmp(receive.car_type, "CNBR") == 0)
                        {
                            car_msg.cnbr_num--;
                        }
                        else
                        {
                            car_msg.vnbr_num--;
                        }
                    }
                    else
                    {
                        printf("\r\nError\r\n");
                    }
                    return;
                }
=======
void usart_callback(void){
  if(strlen(USART1_RECEIVE_ARRAY)==22){
    sscanf(USART1_RECEIVE_ARRAY,"%[^:]:%[^:]:%2d%2d%2d%2d%2d%2d",receive.car_type,receive.car_num,&receive.year,&receive.month,&receive.day,&receive.hour,&receive.minute,&receive.second);
    if((check_type(receive.car_type,"CNBR")==0||check_type(receive.car_type,"VNBR")==0)&&date_legality(
      receive.year,receive.month,receive.day,receive.hour,receive.minute,receive.second)){
      for(int i=0;i<8;i++){
        if(stop[i].isempty==true&&check_type(stop[i].car_num,receive.car_num)==0){//车出库
          stop[i].isempty=false;
          int ans=differ_time(i);
          if(ans!=-1){
            if(check_type(receive.car_type,"CNBR")==0){
              printf("%s:%s:%d:%.2f\r\n",receive.car_type,receive.car_num,ans,ans*car_msg.cnbr_spend);
            }else{
              printf("%s:%s:%d:%.2f\r\n",receive.car_type,receive.car_num,ans,ans*car_msg.vnbr_spend);
>>>>>>> c7264c071236c971ae452c6b4d55495f1513a9c4
            }
            // 车入库
            int j = 0;
            while (stop[j].isempty == true)
                j++;
            if (j > 7)
                printf("\r\nError\r\n"); // 车库满了
            strcpy(stop[j].car_num, receive.car_num);
            strcpy(stop[j].car_type, receive.car_type);
            stop[j].year = receive.year + 2000;
            stop[j].month = receive.month;
            stop[j].day = receive.day;
            stop[j].hour = receive.hour;
            stop[j].minute = receive.minute;
            stop[j].second = receive.second;
            stop[j].isempty = true;
            if (strcmp(receive.car_type, "CNBR") == 0)
            {
                car_msg.cnbr_num++;
            }
            else
            {
                car_msg.vnbr_num++;
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
int mode = 0;     // LCD显示选择
bool type = true; // B4按键选择
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
    MX_TIM3_Init();
    MX_TIM6_Init();
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
    HAL_TIM_PWM_Start_IT(&htim3, TIM_CHANNEL_2);
    HAL_TIM_Base_Start_IT(&htim6);
    HAL_UART_Receive_IT(&huart1, &USART1_RECEIVE_DATA, USART1_RECEIVE_BUUFER_SIZE);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
    car_msg.cnbr_num = 0;
    car_msg.vnbr_num = 0;
    car_msg.cnbr_spend = 3.5;
    car_msg.vnbr_spend = 2;
    stop[0].isempty = false;
    stop[1].isempty = false;
    stop[2].isempty = false;
    stop[3].isempty = false;
    stop[4].isempty = false;
    stop[5].isempty = false;
    stop[6].isempty = false;
    stop[7].isempty = false;
    while (1)
    {
        if (USART1_RECEIVE_STATE & 0x8000)
        {
            usart_callback();
            USART1_RECEIVE_STATE = 0;
        }
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
                car_msg.cnbr_spend += 0.5;
                car_msg.vnbr_spend += 0.5;
            }
        }
        else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == 0)
        {
            while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == 0)
                ;
            if (mode == 1)
            {
                car_msg.cnbr_spend -= 0.5;
                car_msg.vnbr_spend -= 0.5;
            }
        }
        else if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == 0)
        {
            while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == 0)
                ;
            if (type == true)
            {
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 20);
                led_Toggle(2);
                type = false;
            }
            else
            {
                __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
                led_Toggle(2);
                type = true;
            }
        }
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
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
            LCD_DisplayStringLine(Line2, (u8 *)"        Data  ");
            sprintf(text, "   CNBR:%d    ", car_msg.cnbr_num);
            LCD_DisplayStringLine(Line4, (u8 *)text);
            sprintf(text, "   VNBR:%d    ", car_msg.vnbr_num);
            LCD_DisplayStringLine(Line6, (u8 *)text);
            sprintf(text, "   IDLE:%d    ", all - car_msg.cnbr_num - car_msg.vnbr_num);
            LCD_DisplayStringLine(Line8, (u8 *)text);
        }
        else if (mode == 1)
        {
            LCD_DisplayStringLine(Line2, (u8 *)"        Para  ");
            sprintf(text, "   CNBR:%.2f    ", car_msg.cnbr_spend);
            LCD_DisplayStringLine(Line4, (u8 *)text);
            sprintf(text, "   VNBR:%.2f    ", car_msg.vnbr_spend);
            LCD_DisplayStringLine(Line6, (u8 *)text);
            LCD_DisplayStringLine(Line8, (u8 *)"                ");
        }
        HAL_TIM_Base_Start_IT(&htim6);
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
