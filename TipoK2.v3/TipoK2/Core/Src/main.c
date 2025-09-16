/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : 3× MAX6675 (CS1=PA7, CS2=PA4, CS3=PA3) – sin UART
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include <stdio.h>

#include "delays.h"
#include "max6675.h"

/* ================== UART opcional ==================
 * Si NO tenés habilitado HAL_UART_MODULE_ENABLED,
 * esta sección no se compila y evitás warnings.
 */
#ifdef HAL_UART_MODULE_ENABLED
UART_HandleTypeDef huart1;
static void MX_USART1_UART_Init(void);
#endif
/* =================================================== */

/* Prototipos locales */
static void MX_GPIO_Init(void);
void SystemClock_Config(void);

/* Variables afuera del while (como pediste) */
float t1, t2, t3;
char  texto[120];

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  /* Delays de µs por DWT y bus MAX6675 */
  delay_us_dwt_init();
  MAX6675_init();

  while (1)
  {
      /* Lee los tres sensores: comparten CK/DAT y cambian solo el CS */
      t1 = MAX6675_lee_cs(CS_MAX_GPIO_Port,  CS_MAX_Pin);   // CS1 = PA7
      t2 = MAX6675_lee_cs(CS2_MAX_GPIO_Port, CS2_MAX_Pin);  // CS2 = PA4
      t3 = MAX6675_lee_cs(CS3_MAX_GPIO_Port, CS3_MAX_Pin);  // CS3 = PA3

      /* (opcional) string para ver en debugger/semihost */
      sprintf(texto, "T1=%.1f  T2=%.1f  T3=%.1f\r\n", t1, t2, t3);

      HAL_Delay(1000);
  }
}

/* ==== GPIO: DAT entrada; CK/CSx salidas ==== */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* DAT (DO del MAX6675) como entrada con pull-up */
  GPIO_InitStruct.Pin  = DAT_MAX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DAT_MAX_GPIO_Port, &GPIO_InitStruct);

  /* CK y los 3 CS como salidas */
  GPIO_InitStruct.Pin   = CK_MAX_Pin | CS_MAX_Pin | CS2_MAX_Pin | CS3_MAX_Pin;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Estados iniciales: CK=0, CSx=alto (inactivo) */
  HAL_GPIO_WritePin(CK_MAX_GPIO_Port,  CK_MAX_Pin,  GPIO_PIN_RESET);
  HAL_GPIO_WritePin(CS_MAX_GPIO_Port,  CS_MAX_Pin,  GPIO_PIN_SET);
  HAL_GPIO_WritePin(CS2_MAX_GPIO_Port, CS2_MAX_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(CS3_MAX_GPIO_Port, CS3_MAX_Pin, GPIO_PIN_SET);
}

/* ==== Reloj simple por HSI ==== */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }

  RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                   | RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) { Error_Handler(); }
}

/* ==== UART opcional: solo se compila si está habilitado ==== */
#ifdef HAL_UART_MODULE_ENABLED
static void MX_USART1_UART_Init(void)
{
  huart1.Instance        = USART1;
  huart1.Init.BaudRate   = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits   = UART_STOPBITS_1;
  huart1.Init.Parity     = UART_PARITY_NONE;
  huart1.Init.Mode       = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK) { Error_Handler(); }
}
#endif

void Error_Handler(void)
{
  __disable_irq();
  while (1) { }
}


#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  (void)file; (void)line;
}
#endif /* USE_FULL_ASSERT */
