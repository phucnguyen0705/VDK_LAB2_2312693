
#include "main.h"


TIM_HandleTypeDef htim2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

const int MAX_LED = 4;
int index_led = 0;
int led_buffer[4] = {0,0,0,0};
const uint8_t seg_code[10] = {
		0b1000000, // 0
		0b1111001, // 1
		0b0100100, // 2
		0b0110000, // 3
		0b0011001, // 4
		0b0010010, // 5
		0b0000010, // 6
		0b1111000, // 7
		0b0000000, // 8
		0b0010000 // 9
};
void display7SEG(int num) {
	uint8_t code = seg_code[num];
	for(int i = 0; i < 7; i++) {
		HAL_GPIO_WritePin(GPIOB, (1 << i), (code & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	}
}

void update7SEG(int index) {
	 switch (index){
	     case 0:
	         HAL_GPIO_WritePin(GPIOA,  GPIO_PIN_6, GPIO_PIN_RESET);
	         HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
	         HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	         HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
	         break;
	     case 1:
	    	 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	         HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7, GPIO_PIN_RESET);
	         HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	         HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
	         break;
	     case 2:
 	 	     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
	         HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
	         HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
	         break;
	     case 3:
	    	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	    	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);
	    	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	         HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
	         break;
	     default:
	         break;
	     }
}
int timer0_counter = 0;
int timer0_flag = 0;
int timer1_counter = 0;
int timer1_flag = 0;
int timer2_counter = 0;
int timer2_flag = 0;
int TIMER_CYCLE = 10;
void setTimer0(int duration) {
    timer0_counter = duration / TIMER_CYCLE;
    timer0_flag = 0;
}
void setTimer1(int duration) {
    timer1_counter = duration / TIMER_CYCLE;
    timer1_flag = 0;
}
void setTimer2(int duration) {
    timer2_counter = duration / TIMER_CYCLE;
    timer2_flag = 0;
}
void timer_run() {
    if (timer0_counter > 0) {
        timer0_counter--;
        if (timer0_counter == 0) timer0_flag = 1;
    }
    if (timer1_counter > 0) {
        timer1_counter--;
        if (timer1_counter == 0) timer1_flag = 1;
    }
    if (timer2_counter > 0) {
        timer2_counter--;
        if (timer2_counter == 0) timer2_flag = 1;
    }
}

int hour = 15, minute = 8, second = 50;
void updateClockBuffer() {
    led_buffer[0] = hour / 10;
    led_buffer[1] = hour % 10;
    led_buffer[2] = minute / 10;
    led_buffer[3] = minute % 10;
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2) {
        timer_run();
    }
}
int main(void)
{ int hour = 15, minute = 8, second = 50;
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM2_Init();
  HAL_TIM_Base_Start_IT(&htim2);
  updateClockBuffer();
  setTimer0(1000);
  setTimer1(1000);
  setTimer2(250);
  int dot_status = 0;
      while (1)
      {
          if (timer0_flag == 1) {
              second++;
              if (second >= 60) { second = 0; minute++; }
              if (minute >= 60) { minute = 0; hour++; }
              if (hour >= 24) { hour = 0; }
              updateClockBuffer();
              setTimer0(1000);
          }

          if (timer1_flag == 1) {
              dot_status = !dot_status;
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, dot_status ? GPIO_PIN_RESET : GPIO_PIN_SET);
              HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, dot_status ? GPIO_PIN_RESET : GPIO_PIN_SET);
              setTimer1(1000);
          }

          if (timer2_flag == 1) {
        	  update7SEG(index_led);
        	  display7SEG(led_buffer[index_led]);
        	  index_led++;
              if(index_led >= 4) index_led = 0;
              setTimer2(250);
  }
}
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

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

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, dot_Pin|l_Pin|en1_Pin|en2_Pin
                          |en3_Pin|en4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, a_Pin|b_Pin|c_Pin|d_Pin
                          |e_Pin|f_Pin|g_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : dot_Pin l_Pin en1_Pin en2_Pin
                           en3_Pin en4_Pin */
  GPIO_InitStruct.Pin = dot_Pin|l_Pin|en1_Pin|en2_Pin
                          |en3_Pin|en4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : a_Pin b_Pin c_Pin d_Pin
                           e_Pin f_Pin g_Pin */
  GPIO_InitStruct.Pin = a_Pin|b_Pin|c_Pin|d_Pin
                          |e_Pin|f_Pin|g_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
