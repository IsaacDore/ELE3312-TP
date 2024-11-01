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

#define EXP2

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "gpio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include "stdio.h"

#include "ili9341.h"
#include "ili9341_gfx.h"
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

const float XX = 350.0;
const float YY = 1450.0;
const uint32_t STEPS = 6;
const uint32_t ZZ = 500;
const float k = 0.5;
volatile int step = 0;
int prev_step = 0;
float step_freq = 350;
float step_mul = 1.0;

#define TABLE_LENGTH 20000
uint32_t tab_value[TABLE_LENGTH];
volatile int flag_done = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void fill_table_sqr_wave(float freq) {
  for (int i = 0; i < TABLE_LENGTH; i++) {
    int time_ms = i;
    int wl = (1.0 / freq) * 40000;
    float value = (time_ms % wl) < (wl / 2) ? 2047.0 : 0.0;
    value *= 1.0 + k * sinf(2 * 3.1415 * time_ms / 20000);
    tab_value[i] = (int)value;
  }
}

void HAL_SYSTICK_Callback(void) {
  static uint32_t ms_elapsed = 0;
  static uint8_t rev = 0;
  ms_elapsed++;
  if (ms_elapsed > ZZ) {
    ms_elapsed = 0;
    if (rev) {
      step--;
      if (step == 0) {
        rev = 0;
      }
    } else {
      step++;
      if (step >= (STEPS)) {
        rev = 1;
      }
    }
  }
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac) {
  HAL_DAC_Start_DMA(hdac, DAC_CHANNEL_1, tab_value, TABLE_LENGTH,
                    DAC_ALIGN_12B_R);
}

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
  MX_TIM2_Init();
  MX_DAC_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  _screen = ili9341_new(&hspi1, Void_Display_Reset_GPIO_Port,
                        Void_Display_Reset_Pin, TFT_CS_GPIO_Port, TFT_CS_Pin,
                        TFT_DC_GPIO_Port, TFT_DC_Pin, isoLandscape, NULL, NULL,
                        NULL, NULL, itsNotSupported, itnNormalized);

  ili9341_fill_screen(_screen, ILI9341_BLACK);
  ili9341_text_attr_t text_attr = {&ili9341_font_11x18, ILI9341_WHITE,
                                   ILI9341_BLACK, 0, 0};

  step_mul = powf(YY / XX, 1.0 / 6.0);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

// Experience 1
#ifdef EXP1
  HAL_TIM_Base_Start(&htim2);
  HAL_ADC_Start_DMA(&hadc1, tab_value, TABLE_LENGTH);
#endif
// END Experience 1

// Experience 2
#ifdef EXP2
  for (int i = 0; i < TABLE_LENGTH; i++) {
    float value = (i % 100) * (4097.0 / 100);
    tab_value[i] = (int)value;
  }

  HAL_TIM_Base_Start(&htim2);
  HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, tab_value, TABLE_LENGTH,
                    DAC_ALIGN_12B_R);
#endif
  // END Experience 2

  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
// Experience 1
#ifdef EXP1
    static int count = 0;
    while (flag_done == 0) {
    };             // Wait for DMA
    flag_done = 0; // Reset flag_done

    int max_value;
    int min_value;
    float rms = 0;
    float mean = 0;
    min_value = max_value = tab_value[0];
    for (int i = 1; i < TABLE_LENGTH; i++) {
      if (tab_value[i] > max_value)
        max_value = tab_value[i];
      if (tab_value[i] < min_value)
        min_value = tab_value[i];
      mean += tab_value[i];
    }
    mean /= TABLE_LENGTH;
    for (int i = 0; i < TABLE_LENGTH; i++) {
      float ac_value = tab_value[i] - mean;
      rms += ac_value * ac_value;
    }
    rms /= TABLE_LENGTH;
    int db_value = 10 * log(rms);
    ili9341_fill_screen(_screen, ILI9341_BLACK);

    char buf[80];
    sprintf(buf, "Min:%i\r\n", min_value);
    ili9341_text_attr_t text_attr = {&ili9341_font_11x18, ILI9341_WHITE,
                                     ILI9341_BLACK, 0, 40};
    ili9341_draw_string(_screen, text_attr, buf);

    sprintf(buf, "Max:%i\r\n", max_value);
    ili9341_text_attr_t text_attr2 = {&ili9341_font_11x18, ILI9341_WHITE,
                                      ILI9341_BLACK, 0, 80};
    ili9341_draw_string(_screen, text_attr2, buf);

    sprintf(buf, "dB:%i\r\n", db_value);
    ili9341_text_attr_t text_attr3 = {&ili9341_font_11x18, ILI9341_WHITE,
                                      ILI9341_BLACK, 0, 120};
    ili9341_draw_string(_screen, text_attr3, buf);
    printf("Done : %i\r\n", count);

    count++;
#endif
// END Experience 1

// Experience 2
#ifdef EXP2
// Tout se fait tout seul par DMA
#endif

    if (prev_step != step) {
      char buf[80];
      step_freq = XX * powf(step_mul, step);
      prev_step = step;
      sprintf(buf, "freq:%.2f\r\n", step_freq);
      ili9341_text_attr_t text_attr = {&ili9341_font_11x18, ILI9341_WHITE,
                                       ILI9341_BLACK, 0, 40};
      ili9341_draw_string(_screen, text_attr, buf);
      sprintf(buf, "step:%i\r\n", step);
      ili9341_text_attr_t text_attr3 = {&ili9341_font_11x18, ILI9341_WHITE,
                                        ILI9341_BLACK, 0, 120};
      ili9341_draw_string(_screen, text_attr3, buf);
      fill_table_sqr_wave(step_freq);
    }

    // END Experience 2
  }
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
int fputc(int ch, FILE *f) {
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART2 and Loop until the end of transmission
   */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
  if (hadc->Instance == ADC1) {
    flag_done = 1;
  }
}

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
