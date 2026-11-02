/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Automatic Rain Detection Smart Window
  * @board          : STM32F103C8T6 Blue Pill
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* Private variables ---------------------------------------------------------*/
uint32_t dryStartTime = 0;
uint8_t dryTiming = 0;
uint8_t windowIsOpen = 1;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
void Error_Handler(void);


/**
  * @brief  Main program
  */
int main(void)
{
    /* Initialize HAL */
    HAL_Init();

    /* Configure system clock */
    SystemClock_Config();

    /* Initialize GPIO and timer */
    MX_GPIO_Init();
    MX_TIM2_Init();

    /* Start PWM for servo on TIM2 Channel 2 */
    if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }

    /* Initial position: window OPEN */
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 1000);

    /* White LED ON, Red LED OFF */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

    /* Main loop */
    while (1)
    {
        /* Read digital output of rain sensor.
           Assumption: LOW = rain, HIGH = dry. */

        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
        {
            /* RAIN DETECTED */

            /* Reset dry timer */
            dryTiming = 0;

            /* Mark window as closed */
            windowIsOpen = 0;

            /* Red LED ON, White LED OFF */
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);

            /* Move servo to CLOSE position */
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 2000);
        }
        else
        {
            /* NO RAIN: SENSOR IS DRY */

            /* Start timing when dry condition begins */
            if (dryTiming == 0)
            {
                dryStartTime = HAL_GetTick();
                dryTiming = 1;
            }

            /* Wait for 10 continuous dry seconds */
            if ((HAL_GetTick() - dryStartTime) >= 10000U)
            {
                /* Open only if currently closed */
                if (windowIsOpen == 0)
                {
                    /* Move servo to OPEN position */
                    __HAL_TIM_SET_COMPARE(
                        &htim2, TIM_CHANNEL_2, 1000
                    );

                    /* White LED ON, Red LED OFF */
                    HAL_GPIO_WritePin(
                        GPIOB, GPIO_PIN_0, GPIO_PIN_SET
                    );

                    HAL_GPIO_WritePin(
                        GPIOB, GPIO_PIN_1, GPIO_PIN_RESET
                    );

                    /* Mark window as open */
                    windowIsOpen = 1;
                }
            }
        }

        /* Small delay between sensor readings */
        HAL_Delay(100);
    }
}


/**
  * @brief System Clock Configuration
  * @note  Assumes an 8 MHz external crystal and 72 MHz system clock.
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_HSE;

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

    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;

    RCC_ClkInitStruct.SYSCLKSource =
        RCC_SYSCLKSOURCE_PLLCLK;

    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}


/**
  * @brief TIM2 Initialization Function
  * @note  PWM frequency = 50 Hz with a 72 MHz timer clock.
  */
static void MX_TIM2_Init(void)
{
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim2.Instance = TIM2;

    htim2.Init.Prescaler = 71;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 19999;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload =
        TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode =
        TIM_MASTERSLAVEMODE_DISABLE;

    if (HAL_TIMEx_MasterConfigSynchronization(
            &htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 1000;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    if (HAL_TIM_PWM_ConfigChannel(
            &htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }

    /* Configure the timer output pin using generated MSP code */
    HAL_TIM_MspPostInit(&htim2);
}


/**
  * @brief GPIO Initialization Function
  * @note  PA0 = rain sensor DO
  *        PB0 = white LED
  *        PB1 = red LED
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable GPIO clocks */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* Initialize LEDs to OFF */
    HAL_GPIO_WritePin(
        GPIOB, GPIO_PIN_0 | GPIO_PIN_1, GPIO_PIN_RESET
    );

    /* PA0: Rain sensor digital input */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* PB0 and PB1: LED outputs */
    GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}


/**
  * @brief Error Handler
  */
void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
        /* Stop here if initialization fails */
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    /* Add error reporting here if needed */
}
#endif
