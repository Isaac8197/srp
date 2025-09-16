/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : OLED SSD1306 + 3x MAX6675 (CS1=PA7, CS2=PA4, CS3=PA3)
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <math.h>

/* ======= Librerías de tu proyecto ======= */
#include "delays.h"
#include "max6675.h"
#include "fonts.h"
#include "ssd1306_oled.h"

/* ======= Compat: si tu main.h usa CS_MAX_* en vez de CS1_MAX_* ======= */
#ifndef CS1_MAX_Pin
#  ifdef CS_MAX_Pin
#    define CS1_MAX_Pin        CS_MAX_Pin
#    define CS1_MAX_GPIO_Port  CS_MAX_GPIO_Port
#  endif
#endif

/* ======= Handles globales que faltaban ======= */
I2C_HandleTypeDef hi2c1;    /* lo usa ssd1306_oled.c */
TIM_HandleTypeDef htim2;    /* dummy para resolver TIM2_IRQHandler de stm32f1xx_it.c */

/* ======= Prototipos locales ======= */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);

/* ======= AUX OLED ======= */
static void OLED_PrintLine(uint16_t x, uint16_t y, const char *txt)
{
  SSD1306_GotoXY(x, y);
  SSD1306_Puts((char*)txt, &Font_11x18, WHITE);
}

static void OLED_ShowTemps(float t1, float t2, float t3)
{
  char buf[24];

  SSD1306_Fill(BLACK);

  if (isnan(t1)) snprintf(buf, sizeof(buf), "T1: Err");
  else           snprintf(buf, sizeof(buf), "T1: %.1f C", t1);
  OLED_PrintLine(0, 0, buf);

  if (isnan(t2)) snprintf(buf, sizeof(buf), "T2: Err");
  else           snprintf(buf, sizeof(buf), "T2: %.1f C", t2);
  OLED_PrintLine(0, 22, buf);

  if (isnan(t3)) snprintf(buf, sizeof(buf), "T3: Err");
  else           snprintf(buf, sizeof(buf), "T3: %.1f C", t3);
  OLED_PrintLine(0, 44, buf);

  SSD1306_UpdateScreen();
}

/* ======= Variables de aplicación ======= */
float t1, t2, t3;

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  /* Evitá IRQs de TIM2 si no lo usás realmente */
  HAL_NVIC_DisableIRQ(TIM2_IRQn);

  MX_GPIO_Init();     /* I2C pins + MAX6675 pins */
  MX_I2C1_Init();     /* Define e inicializa hi2c1 */

  delay_us_dwt_init();
  MAX6675_init();

  /* OLED */
  if (!SSD1306_Init()) {
    /* Si el OLED/I2C no responde, quedate acá (o blink LED) */
    while (1) { }
  }
  SSD1306_Fill(BLACK);
  SSD1306_UpdateScreen();

  while (1)
  {
    /* Lee los 3 sensores: CK/DAT compartidos, cambia solo CS */
    t1 = MAX6675_lee_cs(CS1_MAX_GPIO_Port, CS1_MAX_Pin);  /* CS1 = PA7 */
    HAL_Delay(2);
    t2 = MAX6675_lee_cs(CS2_MAX_GPIO_Port, CS2_MAX_Pin);  /* CS2 = PA4 */
    HAL_Delay(2);
    t3 = MAX6675_lee_cs(CS3_MAX_GPIO_Port, CS3_MAX_Pin);  /* CS3 = PA3 */

    /* Mostrar en OLED */
    OLED_ShowTemps(t1, t2, t3);

    HAL_Delay(1000);
  }
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

/* ==== GPIO: I2C1 PB6/PB7 + MAX6675 (PA5/PA6/PA7/PA4/PA3) ==== */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_AFIO_CLK_ENABLE();

  /* I2C1: PB6=SCL, PB7=SDA (AF Open-Drain con pull-ups) */
  GPIO_InitStruct.Pin   = GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode  = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull  = GPIO_PULLUP;              /* usa resistencias externas si tenés */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* MAX6675: DAT (SO) como entrada pull-up; CK/CSx como salidas */
  GPIO_InitStruct.Pin  = DAT_MAX_Pin;               /* PA5 */
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(DAT_MAX_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin   = CK_MAX_Pin;               /* PA6 */
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(CK_MAX_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = CS1_MAX_Pin;  HAL_GPIO_Init(CS1_MAX_GPIO_Port, &GPIO_InitStruct); /* PA7 */
  GPIO_InitStruct.Pin = CS2_MAX_Pin;  HAL_GPIO_Init(CS2_MAX_GPIO_Port, &GPIO_InitStruct); /* PA4 */
  GPIO_InitStruct.Pin = CS3_MAX_Pin;  HAL_GPIO_Init(CS3_MAX_GPIO_Port, &GPIO_InitStruct); /* PA3 */

  /* Estados iniciales del bus MAX6675 */
  HAL_GPIO_WritePin(CK_MAX_GPIO_Port,  CK_MAX_Pin,  GPIO_PIN_RESET);
  HAL_GPIO_WritePin(CS1_MAX_GPIO_Port, CS1_MAX_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(CS2_MAX_GPIO_Port, CS2_MAX_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(CS3_MAX_GPIO_Port, CS3_MAX_Pin, GPIO_PIN_SET);
}

/* ==== I2C1 @400kHz (define hi2c1) ==== */
static void MX_I2C1_Init(void)
{
  __HAL_RCC_I2C1_CLK_ENABLE();

  hi2c1.Instance             = I2C1;
  hi2c1.Init.ClockSpeed      = 400000;
  hi2c1.Init.DutyCycle       = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1     = 0;
  hi2c1.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2     = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

  if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
    Error_Handler();
  }
}

void Error_Handler(void)
{
  __disable_irq();
  while (1) { }
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
