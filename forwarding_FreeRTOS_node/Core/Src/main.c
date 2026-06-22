#include "main.h"
#include "cmsis_os.h"

/* ================= RTOS ================= */

osThreadId_t defaultTaskHandle;

const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

/* ================= PROTOTYPES ================= */

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void StartDefaultTask(void *argument);

/* ================= MAIN ================= */

int main(void)
{
    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();

    HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);

    osKernelInitialize();

    defaultTaskHandle =
        osThreadNew(StartDefaultTask,
                    NULL,
                    &defaultTask_attributes);

    osKernelStart();

    while (1)
    {
    }
}

/* ================= GPIO ================= */

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* ---------- INPUT ---------- */

    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ---------- PA3 OUTPUT ---------- */
    /* PATH B -> Board3 */

    HAL_GPIO_WritePin(GPIOA,
                      GPIO_PIN_3,
                      GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* ---------- PB1 OUTPUT ---------- */
    /* PATH A -> Board1 */

    HAL_GPIO_WritePin(GPIOB,
                      GPIO_PIN_1,
                      GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* ---------- DEBUG LED PB0 ---------- */

    HAL_GPIO_WritePin(GPIOB,
                      GPIO_PIN_0,
                      GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* ================= EXTI ================= */

void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

/* ================= ISR CALLBACK ================= */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_4)
    {
        /* PATH A */
        HAL_GPIO_WritePin(GPIOB,
                          GPIO_PIN_1,
                          GPIO_PIN_SET);

        /* PATH B */
        HAL_GPIO_WritePin(GPIOA,
                          GPIO_PIN_3,
                          GPIO_PIN_SET);

        HAL_GPIO_WritePin(GPIOB,
                          GPIO_PIN_0,
                          GPIO_PIN_SET);

        for (volatile int i = 0; i < 10000; i++)
        {
        }

        HAL_GPIO_WritePin(GPIOB,
                          GPIO_PIN_1,
                          GPIO_PIN_RESET);

        HAL_GPIO_WritePin(GPIOA,
                          GPIO_PIN_3,
                          GPIO_PIN_RESET);

        HAL_GPIO_WritePin(GPIOB,
                          GPIO_PIN_0,
                          GPIO_PIN_RESET);
    }
}

/* ================= THREAD ================= */

void StartDefaultTask(void *argument)
{
    for (;;)
    {
        osDelay(1000);
    }
}

/* ================= CLOCK ================= */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(
        PWR_REGULATOR_VOLTAGE_SCALE3);

    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_HSI;

    RCC_OscInitStruct.HSIState =
        RCC_HSI_ON;

    RCC_OscInitStruct.HSICalibrationValue =
        RCC_HSICALIBRATION_DEFAULT;

    RCC_OscInitStruct.PLL.PLLState =
        RCC_PLL_NONE;

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
        RCC_SYSCLKSOURCE_HSI;

    RCC_ClkInitStruct.AHBCLKDivider =
        RCC_SYSCLK_DIV1;

    RCC_ClkInitStruct.APB1CLKDivider =
        RCC_HCLK_DIV1;

    RCC_ClkInitStruct.APB2CLKDivider =
        RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct,
            FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/* ================= ERROR ================= */

void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
    }
}
