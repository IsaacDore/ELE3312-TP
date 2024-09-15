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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
enum Button {
  None = -1,
  B0,B1,B2,B3
};

enum State {
  AwaitingPress,
  Failure,
  Success,
};

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
  /* USER CODE BEGIN 2 */
  enum Button code[4] = {B1,B0,B2,B3};
  enum Button input[4] = {None,None,None,None};

  enum State state = AwaitingPress;
  int press_cnt = 0;

  GPIO_TypeDef* ports[4] = {B0_GPIO_Port, B1_GPIO_Port, B2_GPIO_Port, B3_GPIO_Port};
  int pins[4] = {B0_Pin, B1_Pin, B2_Pin, B3_Pin};
  enum Button keys[4] = {B0, B1, B2, B3};

  GPIO_TypeDef* del_ports[4] = {DEL0_GPIO_Port, DEL1_GPIO_Port, DEL2_GPIO_Port, DEL3_GPIO_Port};
  int del_pins[4] = {DEL0_Pin, DEL1_Pin, DEL2_Pin, DEL3_Pin};

  unsigned char button_state = 0;

  int last_press = HAL_GetTick();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    switch (state) {
      case AwaitingPress:
        for (int i = 0; i < 4; i++){
          if (HAL_GPIO_ReadPin(ports[i], pins[i]) == GPIO_PIN_SET){
            button_state|=1;
            if ((button_state & 0b11)==0b01){
              input[press_cnt] = keys[i];
              press_cnt++;
              last_press = HAL_GetTick();
            }
          }
        }

        button_state <<= 1;
        
        for (int i = 0; i < 4; i++){
          if (press_cnt == i){
            HAL_GPIO_WritePin(del_ports[i], del_pins[i], GPIO_PIN_SET);
          }else{
            HAL_GPIO_WritePin(del_ports[i], del_pins[i], GPIO_PIN_RESET);
          }
        }

        int now = HAL_GetTick();
        if ((now - last_press)>5000){
          state = Failure;
        }
       
        if(press_cnt==4){
          state = Success;
          for(int i =0;i<4;i++){
            if (code[i]!=input[i]){
              state = Failure;
            }
          }
        } 

        HAL_Delay(40);
        break;
      case Failure:
        press_cnt = 0;
        GPIOC->ODR &= 0x0F;
        state = AwaitingPress;
        last_press = HAL_GetTick();
        break;
      case Success:
        HAL_GPIO_WritePin(DEL0_GPIO_Port, DEL0_Pin, GPIO_PIN_SET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(DEL0_GPIO_Port, DEL0_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(DEL1_GPIO_Port, DEL1_Pin, GPIO_PIN_SET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(DEL1_GPIO_Port, DEL1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(DEL2_GPIO_Port, DEL2_Pin, GPIO_PIN_SET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(DEL2_GPIO_Port, DEL2_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(DEL3_GPIO_Port, DEL3_Pin, GPIO_PIN_SET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(DEL3_GPIO_Port, DEL3_Pin, GPIO_PIN_RESET);
        now = HAL_GetTick();
        if ((now - last_press)>10000){
          state = Failure;
        }
        break;
      default:
        break;
    }
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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

#ifdef  USE_FULL_ASSERT
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
