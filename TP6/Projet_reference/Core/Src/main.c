/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define ARM_MATH_CM4
#include "arm_math.h"

#include "ili9341.h"
#include "ili9341_gfx.h"
#include "stdio.h"
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
ili9341_t *_screen;
arm_fir_instance_f32 fir_inst;
float tab_value[256];
float FFT_value[256];
float abs_value[128];
float fir_tabs[16] = {
    1.0000,  0.0950,  -0.8422, -0.1897, 0.3686,  0.0856,  -0.0458, -0.0044,
    -0.0028, -0.0025, -0.0004, -0.0023, -0.0001, -0.0022, -0.0000, -0.0022,
};
float tab_fir_value[256];
float state_buf[16];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
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
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  _screen = ili9341_new(&hspi1, Void_Display_Reset_GPIO_Port,
                        Void_Display_Reset_Pin, TFT_CS_GPIO_Port, TFT_CS_Pin,
                        TFT_DC_GPIO_Port, TFT_DC_Pin, isoLandscape, NULL, NULL,
                        NULL, NULL, itsNotSupported, itnNormalized);
  ili9341_fill_screen(_screen, ILI9341_BLACK);
  ili9341_text_attr_t text_attr = {&ili9341_font_11x18, ILI9341_WHITE,
                                   ILI9341_BLACK, 0, 0};

  arm_fir_init_f32(&fir_inst, 16, fir_tabs, state_buf, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */
    float scale = 120.0 / 4095.0;
    for (int x = 0; x < 256; x++) {
      HAL_ADC_Start(&hadc1);
      HAL_ADC_PollForConversion(&hadc1, 100);
      float value = tab_value[x] = HAL_ADC_GetValue(&hadc1) * scale;
      char buffer[15] = {0};
      sprintf(buffer, "Value : %-6.2f", value);
      ili9341_draw_string(_screen, text_attr, buffer);
      // draw value
      ili9341_draw_pixel(_screen, ILI9341_BLUE, x, (int)(120 - value));

      arm_fir_f32(&fir_inst, tab_value + x, tab_fir_value + x, 1);

      // draw value through filter
      ili9341_draw_pixel(_screen, ILI9341_RED, x,
                         (int)(120 - tab_fir_value[x]));

      HAL_Delay(100);
    }
    arm_rfft_fast_instance_f32 fftInstance;
    arm_rfft_fast_init_f32(&fftInstance, 256);
    arm_rfft_fast_f32(&fftInstance, tab_value, FFT_value, 0);
    arm_cmplx_mag_f32(FFT_value, abs_value, 128);
    float max_value;
    unsigned int max_index;
    arm_max_f32(abs_value, 128, &max_value, &max_index);
    scale = 120.0 / max_value;
    ili9341_fill_screen(_screen, ILI9341_BLACK);
    for (int x = 0; x < 128; x++) {
      float value = abs_value[x] * scale;
      ili9341_draw_line(_screen, ILI9341_RED, 2 * x, (int)(240 - value), 2 * x,
                        239);
      ili9341_draw_line(_screen, ILI9341_RED, 2 * x + 1, (int)(240 - value),
                        2 * x + 1, 239);
      char bufferFFT[20] = {0};
      sprintf(bufferFFT, "FFT : %-6.2f ", value);
      ili9341_draw_string(_screen, text_attr, bufferFFT);
    }
    HAL_Delay(5000);
  }
  /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
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
