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
CAN_HandleTypeDef hcan1;
CAN_FilterTypeDef sFilterConfig;
CAN_TxHeaderTypeDef TxHeader; //will be used to store the header information, like RTR, DLC
CAN_RxHeaderTypeDef RxHeader;
uint8_t TxData[8];
uint32_t TxMailbox;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_CAN1_Init(); //includes init and filter and start
  /* USER CODE BEGIN 2 */

  if(HAL_CAN_IsSleepActive(&hcan1))
  {
	  HAL_CAN_WakeUp(&hcan1);
	  HAL_CAN_Start(&hcan1);
  }

  /* USER CODE END 2 */

  // store the required values in the TxHeader, and in the TxData

  //TxHeader.ExtId =CAN_ID_EXT;
  TxHeader.IDE = CAN_ID_STD; //Standard ID or Extended ID
  TxHeader.StdId = 0x446;	//is the Identifier ( ID of the Transmitter, and it should be maximum 11 bit wide)
  TxHeader.RTR = CAN_RTR_DATA; //indicates that we are sending a data frame
  TxHeader.DLC = 8;	//8 Bytes of data Field
  //TxHeader.TransmitGlobalTime =DISABLE;

  TxData[0] = 0x01;	//Start of message
  TxData[1] = 0xAA; //Board ID to talk
  TxData[2] = 0x00; //Read / Write Command
  TxData[3] = 0x01; //Data length
  TxData[4] = 0xAA; //Data Byte 0
  TxData[5] = 0xAA; //Data Byte 1
  TxData[6] = 0xAA; //Data Byte 2
  TxData[7] = 0x04; // End Of message


  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

	 	 memset(CANRxData, 0x00, sizeof(CANRxData));
	 	 memset(CANTxData, 0x00, sizeof(CANTxData));

	 	  // test relay
	 	  HAL_GPIO_WritePin(GPIOG,GPIO_PIN_8,GPIO_PIN_SET);
	 	  HAL_Delay (100);
	 	  HAL_GPIO_WritePin(GPIOG,GPIO_PIN_8,GPIO_PIN_RESET);

	 	  //Sent Message to CAN network

	 	 HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox);
	 	//if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1)!=1)
	 	//if (HAL_CAN_IsTxMessagePending(&hcan1,TxMailbox))

	 	 //Listen to CAN network

	 	 uint8_t res=1;
	 	while(1){
	 	res = HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, CANRxData);
	 	if (res==HAL_OK) break;
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 1;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_3TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN CAN1_Init 2 */

	sFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
	sFilterConfig.FilterBank=13; 							// max is 28
	sFilterConfig.FilterFIFOAssignment=CAN_FILTER_FIFO0; 	//fifo0 or fifo1
	sFilterConfig.FilterIdLow = 0x10;
	sFilterConfig.FilterIdHigh = 0x446<<5; 					//The value set in this register will be compared to the incoming Identifier: higher 16 bits of ID register , to only compare the STD ID of the incoming message, shift the value by 5. The STD ID starts from 5th bit in the ID HIGH Register
	sFilterConfig.FilterMaskIdHigh=0x446<<5; 				//Higher 16 Bits of the MASK register. The value set in this register will enable the comparison of that particular bit in the ID register to that of the incoming ID.
	sFilterConfig.FilterMaskIdLow=0;
	sFilterConfig.FilterMode=CAN_FILTERMODE_IDMASK; 		//mask mode or list mode
	sFilterConfig.FilterScale=CAN_FILTERSCALE_32BIT; 		// 2,16 or 32 bit filter registers
	sFilterConfig.SlaveStartFilterBank=28;

	if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig)!=HAL_OK)
	{
		Error_Handler();
	}
	if(HAL_CAN_Start(&hcan1) != HAL_OK)
	{
		Error_Handler();
	}

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
