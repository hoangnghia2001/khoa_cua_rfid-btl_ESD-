/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "lcd.h"
#include "rc522.h"
#include "flash.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define StartAddressUID 0x0800A000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

uint8_t CardID[MFRC522_MAX_LEN];
uint8_t exitmenu = 255;
CLCD_Name LCD1;;
uint32_t AddressUID = StartAddressUID;
uint8_t PassWord[16] = {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};
uint32_t delayloa = 100;
char str[20];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void seclectmenu(void);
void adduid(uint8_t key);
void removeuid(uint32_t address);
void checkthe(void);
void startadd(void);
void setaddress(void);
void password(void);
void resetflash(void);
uint8_t checkcountUID(void);
uint32_t CheckKey(uint8_t key);
uint8_t CheckUID(uint8_t *data, uint32_t address);
uint8_t CheckListUID(uint8_t *data);
uint8_t checkbuton(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	if(exitmenu > 0)
		exitmenu --;
	else exitmenu = 0;
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
  MX_SPI1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

	TM_MFRC522_Init();

	CLCD_4BIT_Init(&LCD1, 16, 2, RS_GPIO_Port, RS_Pin, EN_GPIO_Port, EN_Pin,
				   D4_GPIO_Port, D4_Pin, D5_GPIO_Port, D5_Pin,
				   D6_GPIO_Port, D6_Pin, D7_GPIO_Port, D7_Pin);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	HAL_TIM_Base_Start_IT(&htim2);
	if (checkcountUID() == 0)
	{
		startadd();
	}
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

		CLCD_SetCursor(&LCD1, 0, 0);
		CLCD_WriteString(&LCD1, " MOI QUET THE ");
		if (checkbuton(MENU_GPIO_Port, MENU_Pin) != 0)
		{
			exitmenu = 15;
			CLCD_SetCursor(&LCD1, 0, 1);
			CLCD_WriteString(&LCD1, "THE NGUOI LON");
			uint8_t key = 0;
			uint8_t stat ;
			while (exitmenu )
			{
				if (TM_MFRC522_Check(CardID) == MI_OK)
				{
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
					HAL_GPIO_WritePin(LOA_GPIO_Port, LOA_Pin, 1);
					HAL_Delay(delayloa);
					HAL_GPIO_WritePin(LOA_GPIO_Port, LOA_Pin, 0);
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
					key = CheckListUID(CardID);
					key = key >> 4;
					break;
				}
				stat = checkbuton(MENU_GPIO_Port, MENU_Pin);
				switch(stat){
				case 0:
					break;
				case 3:
					password();
					key = 99;
					exitmenu = 0;
					break;
				default :
					key = 2;
					exitmenu = 0;
					break;
				}
			}
			switch (key){
			case 1:
				seclectmenu();
				break;
			case 99:
				break;
			default:
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "      SAI ");
				CLCD_SetCursor(&LCD1, 0, 1);
				CLCD_WriteString(&LCD1, "KHONG CO QUYEN");
				HAL_Delay(1000);
				CLCD_Clear(&LCD1);
				break;
			}
		}
		if (TM_MFRC522_Check(CardID) == MI_OK)
		{
//			sprintf(str, "ID: 0x%02X%02X%02X%02X%02X", CardID[0], CardID[1], CardID[2], CardID[3], CardID[4]);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
			HAL_GPIO_WritePin(LOA_GPIO_Port, LOA_Pin, 1);
			HAL_Delay(delayloa);
			HAL_GPIO_WritePin(LOA_GPIO_Port, LOA_Pin, 0);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
			if (CheckListUID(CardID) != 0)
			{
				HAL_GPIO_WritePin(KHOA_GPIO_Port, KHOA_Pin, 1);
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "    WELCOME");
				CLCD_SetCursor(&LCD1, 0, 1);
				CLCD_WriteString(&LCD1,str);
				HAL_Delay(5000);
				HAL_GPIO_WritePin(KHOA_GPIO_Port, KHOA_Pin, 0);
			}
			else
			{
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "   THE SAI");
				CLCD_SetCursor(&LCD1, 0, 1);
				CLCD_WriteString(&LCD1, str);
				HAL_Delay(5000);
			}

			CLCD_Clear(&LCD1);
			CLCD_SetCursor(&LCD1, 0, 0);
			CLCD_WriteString(&LCD1, " MOI QUET THE ");
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  htim2.Init.Prescaler = 35999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CS_Pin|QR_Pin|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LOA_Pin|KHOA_Pin|LED_Pin|D7_Pin
                          |D6_Pin|D5_Pin|D4_Pin|EN_Pin
                          |RW_Pin|RS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : CS_Pin QR_Pin PA15 */
  GPIO_InitStruct.Pin = CS_Pin|QR_Pin|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LOA_Pin KHOA_Pin LED_Pin D7_Pin
                           D6_Pin D5_Pin D4_Pin EN_Pin
                           RW_Pin RS_Pin */
  GPIO_InitStruct.Pin = LOA_Pin|KHOA_Pin|LED_Pin|D7_Pin
                          |D6_Pin|D5_Pin|D4_Pin|EN_Pin
                          |RW_Pin|RS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : SELECCT_Pin */
  GPIO_InitStruct.Pin = SELECCT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SELECCT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MENU_Pin */
  GPIO_InitStruct.Pin = MENU_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(MENU_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
uint8_t checkbuton(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == 1)
		return 0;
	else
	{
		HAL_Delay(50);
		if(HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == 1)	return 0;
		uint8_t i = 50;
		while (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == 0)
		{
			HAL_Delay(10);
			i--;
			if (i == 0)
			{
				i = 250;
				while (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == 0)
				{
					HAL_Delay(10);
					i--;
					if(i == 0)	return 3;
				}
				return 2;
			}
		}
		return 1;
	}
}

void seclectmenu(void)
{
	exitmenu = 15;
	uint8_t status = -1;
	CLCD_Clear(&LCD1);
	CLCD_SetCursor(&LCD1, 0, 0);
	CLCD_WriteString(&LCD1, "  SELECT MENU");
	CLCD_SetCursor(&LCD1, 0, 1);
	CLCD_WriteString(&LCD1, "    THEM THE");
	while (exitmenu )
	{
		if (checkbuton(SELECCT_GPIO_Port, SELECCT_Pin) != 0)
		{
			exitmenu = 15;
			status++;
			status = (status > 3) ? 0 : status;
			switch (status)
			{
			case 0:
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "=>  THEM THE");
				CLCD_SetCursor(&LCD1, 0, 1);
				CLCD_WriteString(&LCD1, "    XOA THE");
				break;
			case 1:
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "    THEM THE");
				CLCD_SetCursor(&LCD1, 0, 1);
				CLCD_WriteString(&LCD1, "=>  XOA THE");
				break;
			case 2:
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "    XOA THE");
				CLCD_SetCursor(&LCD1, 0, 1);
				CLCD_WriteString(&LCD1, "=>  TRA THE");
				break;
			default:
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "    TRA THE");
				CLCD_SetCursor(&LCD1, 0, 1);
				CLCD_WriteString(&LCD1, "=>  THOAT");
				break;
			}
		}
		if (checkbuton(MENU_GPIO_Port, MENU_Pin) != 0)
		{
			exitmenu = 15;
			switch (status)
			{
			case 0:
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "    SELECT ");
				CLCD_SetCursor(&LCD1, 0, 1);
				CLCD_WriteString(&LCD1, "   THE NGUOI LON");
				uint8_t statusadd = 0;
				uint8_t back = 1;
				while (back == 1)
				{
					if (exitmenu == 0)
					{
						CLCD_Clear(&LCD1);
						HAL_Delay(1000);
						return;
					}
					if (checkbuton(SELECCT_GPIO_Port, SELECCT_Pin) != 0)
					{
						exitmenu = 15;
						statusadd++;
						statusadd = (statusadd > 2) ? 0 : statusadd;
						switch (statusadd)
						{
						case 1:
							CLCD_Clear(&LCD1);
							CLCD_SetCursor(&LCD1, 0, 0);
							CLCD_WriteString(&LCD1, "=> THE NGUOI LON");
							CLCD_SetCursor(&LCD1, 0, 1);
							CLCD_WriteString(&LCD1, "   THE TRE EM");
							break;
						case 2:
							CLCD_Clear(&LCD1);
							CLCD_SetCursor(&LCD1, 0, 0);
							CLCD_WriteString(&LCD1, "   THE NGUOI LON");
							CLCD_SetCursor(&LCD1, 0, 1);
							CLCD_WriteString(&LCD1, "=> THE TRE EM");
							break;
						default:
							CLCD_Clear(&LCD1);
							CLCD_SetCursor(&LCD1, 0, 0);
							CLCD_WriteString(&LCD1, "   THE TRE EM");
							CLCD_SetCursor(&LCD1, 0, 1);
							CLCD_WriteString(&LCD1, "=> BACK");
							break;
						}
					}
					if (checkbuton(MENU_GPIO_Port, MENU_Pin) != 0)
					{
						exitmenu = 15;
						switch (statusadd)
						{
						case 1:
							CLCD_Clear(&LCD1);
							CLCD_SetCursor(&LCD1, 0, 0);
							CLCD_WriteString(&LCD1, "=>  THE 1 ");
							CLCD_SetCursor(&LCD1, 0, 1);
							CLCD_WriteString(&LCD1, "    THE 2 ");
							uint8_t statusadd1 = 1;
							uint8_t back11 = 1;
							while (back11 == 1)
							{
								if (exitmenu == 0)
								{
									CLCD_Clear(&LCD1);
									HAL_Delay(1000);
									return;
								}
								if (checkbuton(SELECCT_GPIO_Port, SELECCT_Pin) != 0)
								{
									exitmenu = 15;
									statusadd1++;
									statusadd1 = (statusadd1 > 4) ? 0 : statusadd1;
									switch (statusadd1)
									{
									case 1:
										CLCD_Clear(&LCD1);
										CLCD_SetCursor(&LCD1, 0, 0);
										CLCD_WriteString(&LCD1, "=>  THE 1 ");
										CLCD_SetCursor(&LCD1, 0, 1);
										CLCD_WriteString(&LCD1, "    THE 2 ");
										break;
									case 2:
										CLCD_Clear(&LCD1);
										CLCD_SetCursor(&LCD1, 0, 0);
										CLCD_WriteString(&LCD1, "=>  THE 2 ");
										CLCD_SetCursor(&LCD1, 0, 1);
										CLCD_WriteString(&LCD1, "    THE 3 ");
										break;
									case 3:
										CLCD_Clear(&LCD1);
										CLCD_SetCursor(&LCD1, 0, 0);
										CLCD_WriteString(&LCD1, "=>  THE 3 ");
										CLCD_SetCursor(&LCD1, 0, 1);
										CLCD_WriteString(&LCD1, "    THE 4 ");
										break;
									case 4:
										CLCD_Clear(&LCD1);
										CLCD_SetCursor(&LCD1, 0, 0);
										CLCD_WriteString(&LCD1, "=>  THE 4 ");
										CLCD_SetCursor(&LCD1, 0, 1);
										CLCD_WriteString(&LCD1, "    BACK ");
										break;
									default:
										CLCD_Clear(&LCD1);
										CLCD_SetCursor(&LCD1, 0, 0);
										CLCD_WriteString(&LCD1, "    THE 4 ");
										CLCD_SetCursor(&LCD1, 0, 1);
										CLCD_WriteString(&LCD1, "=>  BACK");
										break;
									}
								}
								if (checkbuton(MENU_GPIO_Port, MENU_Pin) != 0)
								{
									exitmenu = 15;
									uint8_t keyadd1 = (statusadd << 4) + statusadd1;
									switch (statusadd1)
									{
									case 1:
										if (CheckKey(keyadd1) != 0)
										{
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "  DA CO THE 1 ");
											HAL_Delay(1000);
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "=>  THE 1 ");
											CLCD_SetCursor(&LCD1, 0, 1);
											CLCD_WriteString(&LCD1, "    THE 2 ");
										}
										else
										{
											adduid(keyadd1);
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "=>  THE 1 ");
											CLCD_SetCursor(&LCD1, 0, 1);
											CLCD_WriteString(&LCD1, "    THE 2 ");
										}
										break;
									case 2:
										if (CheckKey(keyadd1) != 0)
										{
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "  DA CO THE 2 ");
											HAL_Delay(1000);
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "=>  THE 2 ");
											CLCD_SetCursor(&LCD1, 0, 1);
											CLCD_WriteString(&LCD1, "    THE 3 ");
										}
										else
										{
											adduid(keyadd1);
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "=>  THE 2 ");
											CLCD_SetCursor(&LCD1, 0, 1);
											CLCD_WriteString(&LCD1, "    THE 3 ");
										}
										break;
									case 3:
										if (CheckKey(keyadd1) != 0)
										{
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "  DA CO THE 3 ");
											HAL_Delay(1000);
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "=>  THE 3 ");
											CLCD_SetCursor(&LCD1, 0, 1);
											CLCD_WriteString(&LCD1, "    THE 4 ");
										}
										else
										{
											adduid(keyadd1);
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "=>  THE 3 ");
											CLCD_SetCursor(&LCD1, 0, 1);
											CLCD_WriteString(&LCD1, "    THE 4 ");
										}
										break;
									case 4:
										if (CheckKey(keyadd1) != 0)
										{
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "  DA CO THE 4 ");
											HAL_Delay(1000);
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "=>  THE 4 ");
											CLCD_SetCursor(&LCD1, 0, 1);
											CLCD_WriteString(&LCD1, "    BACK ");
										}
										else
										{
											adduid(keyadd1);
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "=>  THE 4 ");
											CLCD_SetCursor(&LCD1, 0, 1);
											CLCD_WriteString(&LCD1, "    BACK ");
										}
										break;
									default:
										back11 = 0;
										break;
									}
								}
							}
							CLCD_Clear(&LCD1);
							CLCD_SetCursor(&LCD1, 0, 0);
							CLCD_WriteString(&LCD1, "=> THE NGUOI LON");
							CLCD_SetCursor(&LCD1, 0, 1);
							CLCD_WriteString(&LCD1, "   THE TRE EM");
							break;
						case 2:
							CLCD_Clear(&LCD1);
							CLCD_SetCursor(&LCD1, 0, 0);
							CLCD_WriteString(&LCD1, "=>  THE 1 ");
							CLCD_SetCursor(&LCD1, 0, 1);
							CLCD_WriteString(&LCD1, "    THE 2 ");
							uint8_t statusadd2 = 1;
							uint8_t back12 = 1;
							while (back12 == 1)
							{
								if (exitmenu == 0)
								{
									CLCD_Clear(&LCD1);
									HAL_Delay(1000);
									return;
								}
								if (checkbuton(SELECCT_GPIO_Port, SELECCT_Pin) != 0)
								{
									exitmenu = 15;
									statusadd2++;
									statusadd2 = (statusadd2 > 4) ? 0 : statusadd2;
									switch (statusadd2)
									{
									case 1:
										CLCD_Clear(&LCD1);
										CLCD_SetCursor(&LCD1, 0, 0);
										CLCD_WriteString(&LCD1, "=>  THE 1 ");
										CLCD_SetCursor(&LCD1, 0, 1);
										CLCD_WriteString(&LCD1, "    THE 2 ");
										break;
									case 2:
										CLCD_Clear(&LCD1);
										CLCD_SetCursor(&LCD1, 0, 0);
										CLCD_WriteString(&LCD1, "=>  THE 2 ");
										CLCD_SetCursor(&LCD1, 0, 1);
										CLCD_WriteString(&LCD1, "    THE 3 ");
										break;
									case 3:
										CLCD_Clear(&LCD1);
										CLCD_SetCursor(&LCD1, 0, 0);
										CLCD_WriteString(&LCD1, "=>  THE 3 ");
										CLCD_SetCursor(&LCD1, 0, 1);
										CLCD_WriteString(&LCD1, "    THE 4 ");
										break;
									case 4:
										CLCD_Clear(&LCD1);
										CLCD_SetCursor(&LCD1, 0, 0);
										CLCD_WriteString(&LCD1, "=>  THE 4 ");
										CLCD_SetCursor(&LCD1, 0, 1);
										CLCD_WriteString(&LCD1, "    BACK ");
										break;
									default:
										CLCD_Clear(&LCD1);
										CLCD_SetCursor(&LCD1, 0, 0);
										CLCD_WriteString(&LCD1, "    THE 4 ");
										CLCD_SetCursor(&LCD1, 0, 1);
										CLCD_WriteString(&LCD1, "=>  BACK");
										break;
									}
								}
								if (checkbuton(MENU_GPIO_Port, MENU_Pin) != 0)
								{
									exitmenu = 15;
									uint8_t keyadd2 = (statusadd << 4) + statusadd2;
									switch (statusadd2)
									{
									case 1:
										if (CheckKey(keyadd2) != 0)
										{
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "  DA CO THE 1 ");
											HAL_Delay(1000);
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "=>  THE 1 ");
											CLCD_SetCursor(&LCD1, 0, 1);
											CLCD_WriteString(&LCD1, "    THE 2 ");
										}
										else
										{
											adduid(keyadd2);
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "=>  THE 1 ");
											CLCD_SetCursor(&LCD1, 0, 1);
											CLCD_WriteString(&LCD1, "    THE 2 ");
										}
										break;
									case 2:
										if (CheckKey(keyadd2) != 0)
										{
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "  DA CO THE 2 ");
											HAL_Delay(1000);
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "=>  THE 2 ");
											CLCD_SetCursor(&LCD1, 0, 1);
											CLCD_WriteString(&LCD1, "    THE 3 ");
										}
										else
										{
											adduid(keyadd2);
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "=>  THE 2 ");
											CLCD_SetCursor(&LCD1, 0, 1);
											CLCD_WriteString(&LCD1, "    THE 3 ");
										}
										break;
									case 3:
										if (CheckKey(keyadd2) != 0)
										{
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "  DA CO THE 3 ");
											HAL_Delay(1000);
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "=>  THE 3 ");
											CLCD_SetCursor(&LCD1, 0, 1);
											CLCD_WriteString(&LCD1, "    THE 4 ");
										}
										else
										{
											adduid(keyadd2);
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "=>  THE 3 ");
											CLCD_SetCursor(&LCD1, 0, 1);
											CLCD_WriteString(&LCD1, "    THE 4 ");
										}
										break;
									case 4:
										if (CheckKey(keyadd2) != 0)
										{
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "  DA CO THE 4 ");
											HAL_Delay(1000);
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "=>  THE 4 ");
											CLCD_SetCursor(&LCD1, 0, 1);
											CLCD_WriteString(&LCD1, "    BACK ");
										}
										else
										{
											adduid(keyadd2);
											CLCD_Clear(&LCD1);
											CLCD_SetCursor(&LCD1, 0, 0);
											CLCD_WriteString(&LCD1, "=>  THE 4 ");
											CLCD_SetCursor(&LCD1, 0, 1);
											CLCD_WriteString(&LCD1, "    BACK ");
										}
										break;
									default:
										back12 = 0;
										break;
									}
								}
							}
							CLCD_Clear(&LCD1);
							CLCD_SetCursor(&LCD1, 0, 0);
							CLCD_WriteString(&LCD1, "   THE NGUOI LON");
							CLCD_SetCursor(&LCD1, 0, 1);
							CLCD_WriteString(&LCD1, "=> THE TRE EM");
							break;
						default:
							back = 0;
							break;
						}
					}
				}
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "=>  THEM THE");
				CLCD_SetCursor(&LCD1, 0, 1);
				CLCD_WriteString(&LCD1, "    XOA THE");
				break;
			case 1:
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "     SELECT ");
				CLCD_SetCursor(&LCD1, 0, 1);
				CLCD_WriteString(&LCD1, "   XOA 1 THE");
				uint8_t statusremove = -1;
				uint8_t backrm = 1;
				while (backrm == 1)
				{
					if (exitmenu == 0)
					{
						CLCD_Clear(&LCD1);
						HAL_Delay(1000);
						return;
					}
					if (checkbuton(SELECCT_GPIO_Port, SELECCT_Pin) != 0)
					{
						exitmenu = 15;
						statusremove++;
						statusremove = (statusremove > 2) ? 0 : statusremove;
						switch (statusremove)
						{
						case 0:
							CLCD_Clear(&LCD1);
							CLCD_SetCursor(&LCD1, 0, 0);
							CLCD_WriteString(&LCD1, "=> XOA 1 THE");
							CLCD_SetCursor(&LCD1, 0, 1);
							CLCD_WriteString(&LCD1, "   XOA TAT CA");
							break;
						case 1:
							CLCD_Clear(&LCD1);
							CLCD_SetCursor(&LCD1, 0, 0);
							CLCD_WriteString(&LCD1, "   XOA 1 THE");
							CLCD_SetCursor(&LCD1, 0, 1);
							CLCD_WriteString(&LCD1, "=> XOA TAT CA");
							break;
						default:
							CLCD_Clear(&LCD1);
							CLCD_SetCursor(&LCD1, 0, 0);
							CLCD_WriteString(&LCD1, "   XOA TAT CA");
							CLCD_SetCursor(&LCD1, 0, 1);
							CLCD_WriteString(&LCD1, "=> BACK");
							break;
						}
					}
					if (checkbuton(MENU_GPIO_Port, MENU_Pin) != 0)
					{
						exitmenu = 15;
						switch (statusremove)
						{
						case 0:
							CLCD_Clear(&LCD1);
							CLCD_SetCursor(&LCD1, 0, 0);
							CLCD_WriteString(&LCD1, "=> CHON THE");
							CLCD_SetCursor(&LCD1, 0, 1);
							CLCD_WriteString(&LCD1, "   QUET THE");
							uint8_t statusrm1 = 0;
							uint8_t backrm1 = 1;
							while (backrm1 == 1)
							{
								if (exitmenu == 0)
								{
									CLCD_Clear(&LCD1);
									HAL_Delay(1000);
									return;
								}
								if (checkbuton(SELECCT_GPIO_Port, SELECCT_Pin) != 0)
								{
									statusrm1++;
									statusrm1 = (statusrm1 > 2) ? 0 : statusrm1;
									switch (statusrm1)
									{
									case 0:
										CLCD_Clear(&LCD1);
										CLCD_SetCursor(&LCD1, 0, 0);
										CLCD_WriteString(&LCD1, "=> CHON THE");
										CLCD_SetCursor(&LCD1, 0, 1);
										CLCD_WriteString(&LCD1, "   QUET THE");
										break;
									case 1:
										CLCD_Clear(&LCD1);
										CLCD_SetCursor(&LCD1, 0, 0);
										CLCD_WriteString(&LCD1, "   CHON THE");
										CLCD_SetCursor(&LCD1, 0, 1);
										CLCD_WriteString(&LCD1, "=> QUET THE");
										;
										break;
									default:
										CLCD_Clear(&LCD1);
										CLCD_SetCursor(&LCD1, 0, 0);
										CLCD_WriteString(&LCD1, "   QUET THE");
										CLCD_SetCursor(&LCD1, 0, 1);
										CLCD_WriteString(&LCD1, "=> BACK");
										break;
									}
								}
								if (checkbuton(MENU_GPIO_Port, MENU_Pin) != 0)
								{
									exitmenu = 15;
									switch (statusrm1)
									{
									case 0:
										CLCD_Clear(&LCD1);
										CLCD_SetCursor(&LCD1, 0, 0);
										CLCD_WriteString(&LCD1, "=> THE NGUOI LON");
										CLCD_SetCursor(&LCD1, 0, 1);
										CLCD_WriteString(&LCD1, "   THE TRE EM");
										uint8_t statusadd = 1;
										uint8_t backrm10 = 1;
										while (backrm10 == 1)
										{
											if (exitmenu == 0)
											{
												CLCD_Clear(&LCD1);
												HAL_Delay(1000);
												return;
											}
											if (checkbuton(SELECCT_GPIO_Port, SELECCT_Pin) != 0)
											{
												exitmenu = 15;
												statusadd++;
												statusadd = (statusadd > 2) ? 0 : statusadd;
												switch (statusadd)
												{
												case 1:
													CLCD_Clear(&LCD1);
													CLCD_SetCursor(&LCD1, 0, 0);
													CLCD_WriteString(&LCD1, "=> THE NGUOI LON");
													CLCD_SetCursor(&LCD1, 0, 1);
													CLCD_WriteString(&LCD1, "   THE TRE EM");
													break;
												case 2:
													CLCD_Clear(&LCD1);
													CLCD_SetCursor(&LCD1, 0, 0);
													CLCD_WriteString(&LCD1, "   THE NGUOI LON");
													CLCD_SetCursor(&LCD1, 0, 1);
													CLCD_WriteString(&LCD1, "=> THE TRE EM");
													break;
												default:
													CLCD_Clear(&LCD1);
													CLCD_SetCursor(&LCD1, 0, 0);
													CLCD_WriteString(&LCD1, "   THE TRE EM");
													CLCD_SetCursor(&LCD1, 0, 1);
													CLCD_WriteString(&LCD1, "=> BACK");
													break;
												}
											}
											if (checkbuton(MENU_GPIO_Port, MENU_Pin) != 0)
											{
												exitmenu = 15;
												switch (statusadd)
												{
												case 1:
													CLCD_Clear(&LCD1);
													CLCD_SetCursor(&LCD1, 0, 0);
													CLCD_WriteString(&LCD1, "=>  THE 1 ");
													CLCD_SetCursor(&LCD1, 0, 1);
													CLCD_WriteString(&LCD1, "    THE 2 ");
													uint8_t statusadd1 = 1;
													uint8_t back11 = 1;
													while (back11 == 1)
													{
														if (exitmenu == 0)
														{
															CLCD_Clear(&LCD1);
															HAL_Delay(1000);
															return;
														}
														if (checkbuton(SELECCT_GPIO_Port, SELECCT_Pin) != 0)
														{
															exitmenu = 15;
															statusadd1++;
															statusadd1 = (statusadd1 > 4) ? 0 : statusadd1;
															switch (statusadd1)
															{
															case 1:
																CLCD_Clear(&LCD1);
																CLCD_SetCursor(&LCD1, 0, 0);
																CLCD_WriteString(&LCD1, "=>  THE 1 ");
																CLCD_SetCursor(&LCD1, 0, 1);
																CLCD_WriteString(&LCD1, "    THE 2 ");
																break;
															case 2:
																CLCD_Clear(&LCD1);
																CLCD_SetCursor(&LCD1, 0, 0);
																CLCD_WriteString(&LCD1, "=>  THE 2 ");
																CLCD_SetCursor(&LCD1, 0, 1);
																CLCD_WriteString(&LCD1, "    THE 3 ");
																break;
															case 3:
																CLCD_Clear(&LCD1);
																CLCD_SetCursor(&LCD1, 0, 0);
																CLCD_WriteString(&LCD1, "=>  THE 3 ");
																CLCD_SetCursor(&LCD1, 0, 1);
																CLCD_WriteString(&LCD1, "    THE 4 ");
																break;
															case 4:
																CLCD_Clear(&LCD1);
																CLCD_SetCursor(&LCD1, 0, 0);
																CLCD_WriteString(&LCD1, "=>  THE 4 ");
																CLCD_SetCursor(&LCD1, 0, 1);
																CLCD_WriteString(&LCD1, "    BACK ");
																break;
															default:
																CLCD_Clear(&LCD1);
																CLCD_SetCursor(&LCD1, 0, 0);
																CLCD_WriteString(&LCD1, "    THE 4 ");
																CLCD_SetCursor(&LCD1, 0, 1);
																CLCD_WriteString(&LCD1, "=>  BACK");
																break;
															}
														}
														if (checkbuton(MENU_GPIO_Port, MENU_Pin) != 0)
														{
															exitmenu = 15;
															uint8_t keyadd1 = (statusadd << 4) + statusadd1;
															switch (statusadd1)
															{
															case 1:
																if (CheckKey(keyadd1) == 0)
																{
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "CHUA CO THE 1 ");
																	HAL_Delay(1000);
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "=>  THE 1 ");
																	CLCD_SetCursor(&LCD1, 0, 1);
																	CLCD_WriteString(&LCD1, "    THE 2 ");
																}
																else
																{
																	removeuid(CheckKey(keyadd1));
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "XOA THANH CONG");
																	HAL_Delay(1000);
																	if (checkcountUID() == 0)
																	{
																		startadd();
																		exitmenu = 0;
																	}
																	else
																	{
																		CLCD_Clear(&LCD1);
																		CLCD_SetCursor(&LCD1, 0, 0);
																		CLCD_WriteString(&LCD1, "=>  THE 1 ");
																		CLCD_SetCursor(&LCD1, 0, 1);
																		CLCD_WriteString(&LCD1, "    THE 2 ");
																	}
																}
																break;
															case 2:
																if (CheckKey(keyadd1) == 0)
																{
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "CHUA CO THE 2 ");
																	HAL_Delay(1000);
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "=>  THE 2 ");
																	CLCD_SetCursor(&LCD1, 0, 1);
																	CLCD_WriteString(&LCD1, "    THE 3 ");
																}
																else
																{
																	removeuid(CheckKey(keyadd1));
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "XOA THANH CONG");
																	HAL_Delay(1000);
																	if (checkcountUID() == 0)
																	{
																		startadd();
																		exitmenu = 0;
																	}
																	else
																	{
																		CLCD_Clear(&LCD1);
																		CLCD_SetCursor(&LCD1, 0, 0);
																		CLCD_WriteString(&LCD1, "=>  THE 2 ");
																		CLCD_SetCursor(&LCD1, 0, 1);
																		CLCD_WriteString(&LCD1, "    THE 3 ");
																	}
																}
																break;
															case 3:
																if (CheckKey(keyadd1) == 0)
																{
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "CHUA CO THE 3 ");
																	HAL_Delay(1000);
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "=>  THE 3 ");
																	CLCD_SetCursor(&LCD1, 0, 1);
																	CLCD_WriteString(&LCD1, "    THE 4 ");
																}
																else
																{
																	removeuid(CheckKey(keyadd1));
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "XOA THANH CONG");
																	HAL_Delay(1000);
																	if (checkcountUID() == 0)
																	{
																		startadd();
																		exitmenu = 0;
																	}
																	else
																	{
																		CLCD_Clear(&LCD1);
																		CLCD_SetCursor(&LCD1, 0, 0);
																		CLCD_WriteString(&LCD1, "=>  THE 3 ");
																		CLCD_SetCursor(&LCD1, 0, 1);
																		CLCD_WriteString(&LCD1, "    THE 4 ");
																	}
																}
																break;
															case 4:
																if (CheckKey(keyadd1) == 0)
																{
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "CHUA CO THE 4 ");
																	HAL_Delay(1000);
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "=>  THE 4 ");
																	CLCD_SetCursor(&LCD1, 0, 1);
																	CLCD_WriteString(&LCD1, "    BACK ");
																}
																else
																{
																	removeuid(CheckKey(keyadd1));
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "XOA THANH CONG");
																	HAL_Delay(1000);
																	if (checkcountUID() == 0)
																	{
																		startadd();
																		exitmenu = 0;
																	}
																	else
																	{
																		CLCD_Clear(&LCD1);
																		CLCD_SetCursor(&LCD1, 0, 0);
																		CLCD_WriteString(&LCD1, "=>  THE 4 ");
																		CLCD_SetCursor(&LCD1, 0, 1);
																		CLCD_WriteString(&LCD1, "    BACK ");
																	}
																}
																break;
															default:
																back11 = 0;
																break;
															}
														}
													}
													CLCD_Clear(&LCD1);
													CLCD_SetCursor(&LCD1, 0, 0);
													CLCD_WriteString(&LCD1, "=> THE NGUOI LON");
													CLCD_SetCursor(&LCD1, 0, 1);
													CLCD_WriteString(&LCD1, "   THE TRE EM");
													break;
												case 2:
													CLCD_Clear(&LCD1);
													CLCD_SetCursor(&LCD1, 0, 0);
													CLCD_WriteString(&LCD1, "=>  THE 1 ");
													CLCD_SetCursor(&LCD1, 0, 1);
													CLCD_WriteString(&LCD1, "    THE 2 ");
													uint8_t statusadd2 = 1;
													uint8_t back12 = 1;
													while (back12 == 1)
													{
														if (exitmenu == 0)
														{
															CLCD_Clear(&LCD1);
															HAL_Delay(1000);
															return;
														}
														if (checkbuton(SELECCT_GPIO_Port, SELECCT_Pin) != 0)
														{
															exitmenu = 15;
															statusadd2++;
															statusadd2 = (statusadd2 > 4) ? 0 : statusadd2;
															switch (statusadd2)
															{
															case 1:
																CLCD_Clear(&LCD1);
																CLCD_SetCursor(&LCD1, 0, 0);
																CLCD_WriteString(&LCD1, "=>  THE 1 ");
																CLCD_SetCursor(&LCD1, 0, 1);
																CLCD_WriteString(&LCD1, "    THE 2 ");
																break;
															case 2:
																CLCD_Clear(&LCD1);
																CLCD_SetCursor(&LCD1, 0, 0);
																CLCD_WriteString(&LCD1, "=>  THE 2 ");
																CLCD_SetCursor(&LCD1, 0, 1);
																CLCD_WriteString(&LCD1, "    THE 3 ");
																break;
															case 3:
																CLCD_Clear(&LCD1);
																CLCD_SetCursor(&LCD1, 0, 0);
																CLCD_WriteString(&LCD1, "=>  THE 3 ");
																CLCD_SetCursor(&LCD1, 0, 1);
																CLCD_WriteString(&LCD1, "    THE 4 ");
																break;
															case 4:
																CLCD_Clear(&LCD1);
																CLCD_SetCursor(&LCD1, 0, 0);
																CLCD_WriteString(&LCD1, "=>  THE 4 ");
																CLCD_SetCursor(&LCD1, 0, 1);
																CLCD_WriteString(&LCD1, "    BACK ");
																break;
															default:
																CLCD_Clear(&LCD1);
																CLCD_SetCursor(&LCD1, 0, 0);
																CLCD_WriteString(&LCD1, "    THE 4 ");
																CLCD_SetCursor(&LCD1, 0, 1);
																CLCD_WriteString(&LCD1, "=>  BACK");
																break;
															}
														}
														if (checkbuton(MENU_GPIO_Port, MENU_Pin) != 0)
														{
															exitmenu = 15;
															uint8_t keyadd2 = (statusadd << 4) + statusadd2;
															switch (statusadd2)
															{
															case 1:
																if (CheckKey(keyadd2) == 0)
																{
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "CHUA CO THE 1 ");
																	HAL_Delay(1000);
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "=>  THE 1 ");
																	CLCD_SetCursor(&LCD1, 0, 1);
																	CLCD_WriteString(&LCD1, "    THE 2 ");
																}
																else
																{
																	removeuid(CheckKey(keyadd2));
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "XOA THANH CONG");
																	HAL_Delay(1000);
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "=>  THE 1 ");
																	CLCD_SetCursor(&LCD1, 0, 1);
																	CLCD_WriteString(&LCD1, "    THE 2 ");
																}
																break;
															case 2:
																if (CheckKey(keyadd2) == 0)
																{
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "CHUA CO THE 2 ");
																	HAL_Delay(1000);
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "=>  THE 2 ");
																	CLCD_SetCursor(&LCD1, 0, 1);
																	CLCD_WriteString(&LCD1, "    THE 3 ");
																}
																else
																{
																	removeuid(CheckKey(keyadd2));
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "XOA THANH CONG");
																	HAL_Delay(1000);
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "=>  THE 2 ");
																	CLCD_SetCursor(&LCD1, 0, 1);
																	CLCD_WriteString(&LCD1, "    THE 3 ");
																}
																break;
															case 3:
																if (CheckKey(keyadd2) == 0)
																{
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "CHUA CO THE 3 ");
																	HAL_Delay(1000);
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "=>  THE 3 ");
																	CLCD_SetCursor(&LCD1, 0, 1);
																	CLCD_WriteString(&LCD1, "    THE 4 ");
																}
																else
																{
																	removeuid(CheckKey(keyadd2));
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "XOA THANH CONG");
																	HAL_Delay(1000);
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "=>  THE 3 ");
																	CLCD_SetCursor(&LCD1, 0, 1);
																	CLCD_WriteString(&LCD1, "    THE 4 ");
																}
																break;
															case 4:
																if (CheckKey(keyadd2) == 0)
																{
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "CHUA CO THE 4 ");
																	HAL_Delay(1000);
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "=>  THE 4 ");
																	CLCD_SetCursor(&LCD1, 0, 1);
																	CLCD_WriteString(&LCD1, "    BACK ");
																}
																else
																{
																	removeuid(CheckKey(keyadd2));
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "XOA THANH CONG");
																	HAL_Delay(1000);
																	CLCD_Clear(&LCD1);
																	CLCD_SetCursor(&LCD1, 0, 0);
																	CLCD_WriteString(&LCD1, "=>  THE 4 ");
																	CLCD_SetCursor(&LCD1, 0, 1);
																	CLCD_WriteString(&LCD1, "    BACK ");
																}
																break;
															default:
																back12 = 0;
																break;
															}
														}
													}
													CLCD_Clear(&LCD1);
													CLCD_SetCursor(&LCD1, 0, 0);
													CLCD_WriteString(&LCD1, "   THE NGUOI LON");
													CLCD_SetCursor(&LCD1, 0, 1);
													CLCD_WriteString(&LCD1, "=> THE TRE EM");
													break;
												default:
													backrm10 = 0;
													break;
												}
											}
										}
										CLCD_Clear(&LCD1);
										CLCD_SetCursor(&LCD1, 0, 0);
										CLCD_WriteString(&LCD1, "=> CHON THE");
										CLCD_SetCursor(&LCD1, 0, 1);
										CLCD_WriteString(&LCD1, "   QUET THE");
										break;
									case 1:
										CLCD_Clear(&LCD1);
										CLCD_SetCursor(&LCD1, 0, 0);
										CLCD_WriteString(&LCD1, "QUET THE");
										CLCD_SetCursor(&LCD1, 0, 1);
										CLCD_WriteString(&LCD1, "=>  BACK");
										uint8_t rmquet = 1;
										while (rmquet)
										{
											if (TM_MFRC522_Check(CardID) == MI_OK)
											{
												HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
												HAL_GPIO_WritePin(LOA_GPIO_Port, LOA_Pin, 1);
												HAL_Delay(delayloa);
												HAL_GPIO_WritePin(LOA_GPIO_Port, LOA_Pin, 0);
												HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
												if (CheckListUID(CardID) != 0)
												{
													removeuid(CheckKey(CheckListUID(CardID)));
													CLCD_Clear(&LCD1);
													CLCD_SetCursor(&LCD1, 0, 0);
													CLCD_WriteString(&LCD1, "XOA THANH CONG");
													HAL_Delay(1000);
													if (checkcountUID() == 0)
													{
														startadd();
														rmquet = 1;
														exitmenu = 0;
														return;
													}else{
														CLCD_Clear(&LCD1);
														CLCD_SetCursor(&LCD1, 0, 0);
														CLCD_WriteString(&LCD1, "QUET THE");
														CLCD_SetCursor(&LCD1, 0, 1);
														CLCD_WriteString(&LCD1, "=>  BACK");
													}

												}
												else
												{
													CLCD_Clear(&LCD1);
													CLCD_SetCursor(&LCD1, 0, 0);
													CLCD_WriteString(&LCD1, "THE CHUA THEM");
													HAL_Delay(1000);
													CLCD_Clear(&LCD1);
													CLCD_SetCursor(&LCD1, 0, 0);
													CLCD_WriteString(&LCD1, "QUET THE");
													CLCD_SetCursor(&LCD1, 0, 1);
													CLCD_WriteString(&LCD1, "=>  BACK");
												}
											}
											if (checkbuton(MENU_GPIO_Port, MENU_Pin) == 1)
											{
												rmquet = 0;
											}
										}
										CLCD_Clear(&LCD1);
										CLCD_SetCursor(&LCD1, 0, 0);
										CLCD_WriteString(&LCD1, "   CHON THE");
										CLCD_SetCursor(&LCD1, 0, 1);
										CLCD_WriteString(&LCD1, "=> QUET THE");
										break;
									default:
										backrm1 = 0;
										break;
									}
								}
							}
							CLCD_Clear(&LCD1);
							CLCD_SetCursor(&LCD1, 0, 0);
							CLCD_WriteString(&LCD1, "=> XOA 1 THE");
							CLCD_SetCursor(&LCD1, 0, 1);
							CLCD_WriteString(&LCD1, "   XOA TAT CA");
							break;
						case 1:
							resetflash();
							startadd();
							exitmenu = 0;
							break;
						default:
							backrm = 0;
							break;
						}
					}
				}
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "    THEM THE");
				CLCD_SetCursor(&LCD1, 0, 1);
				CLCD_WriteString(&LCD1, "=>  XOA THE");
				break;
			case 2:
				checkthe();
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "    XOA THE");
				CLCD_SetCursor(&LCD1, 0, 1);
				CLCD_WriteString(&LCD1, "=>  TRA THE");
				break;
				break;
			default:
				exitmenu = 0;
				break;
			}
		}
	}
	CLCD_Clear(&LCD1);
	HAL_Delay(1000);
}
uint8_t CheckUID(uint8_t *data, uint32_t address)
{
	uint8_t arr[8];
	Flash_Read_Array(address, arr, 8);
	if (arr[6] != 0xFF)
		return 0;
	for (uint8_t i = 0; i < 5; i++)
	{
		if (data[i] != arr[i])
			return 0;
	}
	return 1;
}

uint8_t CheckListUID(uint8_t *data)
{
	uint32_t pt = StartAddressUID;
	while (Flash_Read_Byte(pt + 5) != 0xFF)
	{
		if(Flash_Read_2Byte(pt + 6) == 0xFFFF){
			if (CheckUID(data, pt) == 1)
				return *(uint8_t *)(pt + 5);
		}
		pt = pt + 8;
	}
	return 0;
}
uint8_t checkcountUID(void)
{
	uint32_t pt = StartAddressUID;
	uint8_t count = 0;
	while (Flash_Read_Byte(pt + 5) != 0xFF)
	{
		if(Flash_Read_2Byte(pt + 6) == 0xFFFF){
			if ((Flash_Read_Byte(pt + 5) >> 4) == 1)
			{
				count++;
			}
		}
		pt = pt + 8;
	}
	return count;
}
void adduid(uint8_t key)
{
	setaddress();
	CLCD_Clear(&LCD1);
	CLCD_SetCursor(&LCD1, 0, 0);
	CLCD_WriteString(&LCD1, "QUET THE");
	CLCD_SetCursor(&LCD1, 0, 1);
	CLCD_WriteString(&LCD1, "=>  BACK");
	while (exitmenu)
	{
		if (TM_MFRC522_Check(CardID) == MI_OK)
		{
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
			HAL_GPIO_WritePin(LOA_GPIO_Port, LOA_Pin, 1);
			HAL_Delay(delayloa);
			HAL_GPIO_WritePin(LOA_GPIO_Port, LOA_Pin, 0);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
			if (CheckListUID(CardID) == 0)
			{
				CardID[5] = key;
				Flash_Write_Array(AddressUID, CardID, 6);
				AddressUID += 8;
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "THANH CONG");
				HAL_Delay(1000);
				return;
			}
			else
			{
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "THE DA TON TAI");
				HAL_Delay(1000);
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "QUET THE");
				CLCD_SetCursor(&LCD1, 0, 1);
				CLCD_WriteString(&LCD1, "=>  BACK");
			}
		}
		if (checkbuton(MENU_GPIO_Port, MENU_Pin) == 1)
		{
			return;
		}
	}
}
void checkthe(void)
{
	exitmenu = 30;
	CLCD_Clear(&LCD1);
	CLCD_SetCursor(&LCD1, 0, 0);
	CLCD_WriteString(&LCD1, "QUET THE");
	CLCD_SetCursor(&LCD1, 0, 1);
	CLCD_WriteString(&LCD1, "=>  BACK");
	while (exitmenu )
	{
		if (TM_MFRC522_Check(CardID) == MI_OK)
		{
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
			HAL_GPIO_WritePin(LOA_GPIO_Port, LOA_Pin, 1);
			HAL_Delay(delayloa);
			HAL_GPIO_WritePin(LOA_GPIO_Port, LOA_Pin, 0);
			HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
			if (CheckListUID(CardID) == 0)
			{
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "THE CHUA THEM");
				HAL_Delay(1000);
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "QUET THE");
				CLCD_SetCursor(&LCD1, 0, 1);
				CLCD_WriteString(&LCD1, "=>  BACK");
				HAL_Delay(1000);
			}
			else
			{
				uint8_t key = CheckListUID(CardID);
				uint8_t key2 = key & 0x0f;
				uint8_t key1 = key >> 4;
				CLCD_Clear(&LCD1);
				switch (key1)
				{
				case 1:
					CLCD_SetCursor(&LCD1, 0, 0);
					CLCD_WriteString(&LCD1, "THE NGUOI LON");
					break;
				default:
					CLCD_SetCursor(&LCD1, 0, 0);
					CLCD_WriteString(&LCD1, "THE TRE EM");
					break;
				}
				switch (key2)
				{
				case 1:
					CLCD_SetCursor(&LCD1, 0, 1);
					CLCD_WriteString(&LCD1, "THE 1");
					break;
				case 2:
					CLCD_SetCursor(&LCD1, 0, 1);
					CLCD_WriteString(&LCD1, "THE 2");
					break;
				case 3:
					CLCD_SetCursor(&LCD1, 0, 1);
					CLCD_WriteString(&LCD1, "THE 3");
					break;
				default:
					CLCD_SetCursor(&LCD1, 0, 1);
					CLCD_WriteString(&LCD1, "THE 4");
					break;
				}
				HAL_Delay(1000);
				CLCD_Clear(&LCD1);
				CLCD_SetCursor(&LCD1, 0, 0);
				CLCD_WriteString(&LCD1, "QUET THE");
				CLCD_SetCursor(&LCD1, 0, 1);
				CLCD_WriteString(&LCD1, "=>  BACK");
			}
		}
		if (checkbuton(MENU_GPIO_Port, MENU_Pin) == 1)
		{
			return;
		}
	}
}
uint32_t CheckKey(uint8_t key)
{
	uint32_t pt = StartAddressUID;
	while (Flash_Read_Byte(pt + 5) != 0xFF)
	{
		if(Flash_Read_2Byte(pt + 6) == 0xFFFF){
			if (*(uint8_t *)(pt + 5) == key)
				return pt;
		}
		pt = pt + 8;
	}
	return 0;
}
void removeuid(uint32_t addressrm)
{
	Flash_Write_2Byte(addressrm + 6, 0x0000);
}
void startadd(void)
{
	CLCD_Clear(&LCD1);
	CLCD_SetCursor(&LCD1, 0, 0);
	CLCD_WriteString(&LCD1, "MOI QUET THE");
	CLCD_SetCursor(&LCD1, 0, 1);
	CLCD_WriteString(&LCD1, "THE NGUOI LON");
	setaddress();
	while (1)
		{
			if (TM_MFRC522_Check(CardID) == MI_OK)
			{
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
				HAL_GPIO_WritePin(LOA_GPIO_Port, LOA_Pin, 1);
				HAL_Delay(delayloa);
				HAL_GPIO_WritePin(LOA_GPIO_Port, LOA_Pin, 0);
				HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
				if (CheckListUID(CardID) == 0)
				{
					CardID[5] = 0x11;
					Flash_Write_Array(AddressUID, CardID, 6);
					AddressUID += 8;
					break;
				}
				else
				{
					CLCD_Clear(&LCD1);
					CLCD_SetCursor(&LCD1, 0, 0);
					CLCD_WriteString(&LCD1, "THE DA TON TAI");
					HAL_Delay(1000);
					CLCD_Clear(&LCD1);
					CLCD_SetCursor(&LCD1, 0, 0);
					CLCD_WriteString(&LCD1, "MOI QUET THE");
					CLCD_SetCursor(&LCD1, 0, 1);
					CLCD_WriteString(&LCD1, "THE NGUOI LON");
				}
			}
		}
	CLCD_Clear(&LCD1);
	CLCD_SetCursor(&LCD1, 0, 0);
	CLCD_WriteString(&LCD1, "THEM THANH CONG");
	CLCD_SetCursor(&LCD1, 0, 1);
	CLCD_WriteString(&LCD1, "THE NGUOI LON");
	HAL_Delay(1000);
	CLCD_Clear(&LCD1);
}
void setaddress(void){
	uint32_t pt = StartAddressUID;
	while (Flash_Read_Byte(pt + 5) != 0xFF)
	{
		pt = pt + 8;
	}
	AddressUID = pt;
}

void password(void){
	CLCD_Clear(&LCD1);
	CLCD_SetCursor(&LCD1, 0, 0);
	CLCD_WriteString(&LCD1, "NHAP PASSWORD :");
	exitmenu =120;
	uint8_t pass[16] = {-1};
	uint8_t k=0;
	uint8_t pas = -1;
	HAL_Delay(1000);
	while(checkbuton(MENU_GPIO_Port, MENU_Pin) != 0){}
	CLCD_SetCursor(&LCD1, k, 1);
	CLCD_WriteChar(&LCD1, 95);
	while(exitmenu){
		if(checkbuton(SELECCT_GPIO_Port, SELECCT_Pin) == 1){
			pas++;
			if(pas == 10)	pas = 0;
			CLCD_SetCursor(&LCD1, k, 1);
			CLCD_WriteChar(&LCD1, pas + 0x30);
		}
		if(checkbuton(MENU_GPIO_Port, MENU_Pin) == 1){
			pass[k] = pas;
			k = k+1;
			pas = -1;
			if(k == 16){
				for(uint8_t i = 0; i<16; i++){
					if(pass[i] != PassWord[i]){
						return;
					}
				}
				HAL_GPIO_WritePin(KHOA_GPIO_Port, KHOA_Pin, 1);
				HAL_Delay(5000);
				HAL_GPIO_WritePin(KHOA_GPIO_Port, KHOA_Pin, 0);
				resetflash();
				startadd();
				exitmenu =0;
			}
			CLCD_SetCursor(&LCD1, k, 1);
			CLCD_WriteChar(&LCD1, 95);
		}

	}

}
void resetflash(){
	uint32_t pt = StartAddressUID;
	while(Flash_Read_8Byte(pt) != 0xFFFFFFFFFFFFFFFF){
		Flash_Erase(pt);
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
