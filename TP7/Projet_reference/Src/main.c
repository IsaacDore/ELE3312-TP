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
#include "adc.h"
#include "dma.h"
#include "gpio.h"
#include "spi.h"
#include "stdio.h"
#include "tim.h"
#include "usart.h"

#define ARM_MATH_CM4
#include "arm_math.h"
#include "time.h"
#include <seed.h>
#include <stdio.h>
#include <stdlib.h>

#include "ili9341.h"
#include "ili9341_gfx.h"

ili9341_t *_screen;
ili9341_text_attr_t text_attr;
char buffer[15] = {0};
float tab_value[256];

// WARNING: positions might be wrong (flip y-axis or wrong screen resolution)
const int screen_width = 320;
const int screen_height = 240;
// pig position
int pig_x;
int pig_y;
// bird data
volatile int vitesse_init = 0;
volatile int lauch = 0;
volatile float angle_init = 0;
volatile float old_x = 0;
volatile float old_y = 0;
volatile float current_x = 10;
volatile float current_y = 220;
volatile float vel_x = 0;
volatile float vel_y = 0;
volatile float gravity = 700000;
// state & flag
volatile int end_flag = 0;
volatile int preparation_state = 0;

void SystemClock_Config(void);

// Timer 2 callback (each 100ms)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (lauch == 0 && end_flag != 0) {
    return;
  }

  // TODO: update and draw new position
  float delta_time = 0.00001;
  current_x += vel_x * delta_time;
  current_y += vel_y * delta_time;
  vel_y += gravity * delta_time;

  // checks defeat conditions
  if ((150 <= current_x && current_x <= 170 &&
       current_y >= 120) // hits the wall
      || current_x > 320 - 10 - 1 || current_y > 240 - 10 - 1 ||
      current_y < 0 + 10) {
    end_flag = 2;
    vitesse_init = 0;
  }
  // checks victory condition
  if ((current_x >= pig_x - 20 && current_x <= pig_x + 20) &&
      (current_y >= pig_y - 20 &&
       current_y <= pig_y + 20)) { // TODO: fix to circle hitbox
    end_flag = 1;
    vitesse_init = 0;
  }
}

void HAL_SYSTICK_Callback(void) {
  static int time_while_pressed = 0;

  switch (preparation_state) {
  case 0: // state: repos
    if (HAL_GPIO_ReadPin(GPIOC, C1_Pin) == GPIO_PIN_SET &&
        time_while_pressed == 0) {
      time_while_pressed = 1;
      preparation_state = 1;
    }
    break;
  case 1: // �tat: comptage
    if (HAL_GPIO_ReadPin(GPIOC, C1_Pin) == GPIO_PIN_RESET) {
      preparation_state = 2;
    }
    time_while_pressed++;
    break;
  case 2: // �tat: fin de comptage
    vitesse_init = time_while_pressed;
    break;
  }
}

int main(void) {
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_TIM2_Init();

  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1, 100);
  uint32_t instance_seed = HAL_ADC_GetValue(&hadc1);
  srand(SEED ^ instance_seed);

  _screen = ili9341_new(&hspi1, Void_Display_Reset_GPIO_Port,
                        Void_Display_Reset_Pin, TFT_CS_GPIO_Port, TFT_CS_Pin,
                        TFT_DC_GPIO_Port, TFT_DC_Pin, isoLandscape, NULL, NULL,
                        NULL, NULL, itsNotSupported, itnNormalized);

  ili9341_fill_screen(_screen, ILI9341_BLACK);
  ili9341_text_attr_t text_attr_tmp = {&ili9341_font_11x18, ILI9341_WHITE,
                                       ILI9341_BLACK, 0, 0};

  text_attr = text_attr_tmp;

  HAL_TIM_Base_Start_IT(&htim2);

  while (1) {
    float scale = 90.0 / 4095.0;
    pig_x = rand() % (300 - 180 + 1) +
            180; // random int in range(min,max): % (max - min + 1) + min
    pig_y = 240 -
            (rand() % (140 - 20 + 1) + 20); // knowing the pig has r=20 and the
                                            // zone is x:[160;320] & y:[0;240]

    ili9341_draw_line(_screen, ILI9341_ORANGE, 160, 120 - 1, 160, 0);
    ili9341_fill_circle(_screen, ILI9341_WHITE, (int)current_x, (int)current_y,
                        10);
    ili9341_fill_circle(_screen, ILI9341_PINK, pig_x, pig_y, 20);

    // waits for user to set a speed value before continuing
    while (vitesse_init == 0)
      ;

    sprintf(buffer, "%i", vitesse_init);
    ili9341_draw_string(_screen, text_attr, buffer);
    HAL_ADC_Start(&hadc2);
    HAL_ADC_PollForConversion(&hadc2, 100);
    // calculate the angle of the bird's trajectory depending on user input
    angle_init = HAL_ADC_GetValue(&hadc2) * scale;

    sprintf(buffer, "%.2f", angle_init);
    ili9341_draw_string(_screen, text_attr, buffer);

    HAL_Delay(1000);
    // TODO: all the trajectory
    vel_x = fabs(cos(angle_init * PI / 180.0)) * vitesse_init * 30;
    vel_y = -fabs(sin(angle_init * PI / 180.0)) * vitesse_init * 30;

    current_y = 220;
    lauch = 1;
    old_x = current_x;
    old_y = current_y;
    while (end_flag == 0) {
      ili9341_fill_circle(_screen, ILI9341_BLACK, (int)old_x, (int)old_y, 10);
      old_x = current_x;
      old_y = current_y;
      sprintf(buffer, "%.2f;%.2f", vel_x, vel_y);
      ili9341_draw_string(_screen, text_attr, buffer);
      // ili9341_fill_screen(_screen, ILI9341_BLACK);
      // ili9341_fill_circle(_screen, ILI9341_PINK, pig_x, pig_y, 20);
      // ili9341_draw_line(_screen, ILI9341_ORANGE, 160, 120 - 1, 160, 0);
      ili9341_fill_circle(_screen, ILI9341_WHITE, (int)current_x,
                          (int)current_y, 10);
    }

    ili9341_fill_screen(_screen, ILI9341_BLACK);
    // char buffer[15] = {0};
    if (end_flag == 1) {
      sprintf(buffer, "Victoire :)");
      ili9341_draw_string(_screen, text_attr, buffer);
    } else {
      sprintf(buffer, "Skill issue.");
      ili9341_draw_string(_screen, text_attr, buffer);
    }

    while (1)
      ;
    // Prelab stuff
    // Experience 1
    //		while(flag_timer_2 == 0);
    //		flag_timer_2 = 0;
    //		ili9341_fill_circle(_screen, ILI9341_WHITE, 160, 120, 20);
    //
    //		while(flag_timer_2 == 0);
    //		flag_timer_2 = 0;
    //		ili9341_fill_circle(_screen, ILI9341_BLACK, 160, 120, 20);

    // Experience 2
    //		int x;
    //		float scale = 120.0/4095.0;
    //		for (int x=0;x<256;x++) {
    //			while(flag_timer_2 == 0);
    //			flag_timer_2 = 0;
    //
    //			HAL_ADC_Start(&hadc2);
    //			HAL_ADC_PollForConversion(&hadc2,100);
    //			float value = tab_value[x] =
    // HAL_ADC_GetValue(&hadc2)*scale;
    //
    //			char buffer[15] = {0};
    //			sprintf(buffer, "Value : %-6.2f",value);
    //
    //			ili9341_draw_string(_screen, text_attr, buffer);
    //			ili9341_draw_pixel(_screen, ILI9341_BLUE, x,(int)
    //(120-value));
    //		}

    // Experience 3
    //		while(flag_systick_timer == 0);
    //		flag_systick_timer = 0;
    //
    //		char buffer[15] = {0};
    //		sprintf(buffer, "State : %3i", current_state);
    //		ili9341_draw_string(_screen, text_attr,buffer);
  }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
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
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
