#include "stm32f4xx.h"
#include "tx_api.h"

static void GPIO_EXTI_Init(void);

int main(void)
{
    GPIO_EXTI_Init();

    tx_kernel_enter();

    while (1)
    {
    }
}

static void GPIO_EXTI_Init(void)
{
    /* Enable GPIOA, GPIOB, SYSCFG clocks */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    for (volatile int i = 0; i < 1000; i++);

    /* PA4 input */
    GPIOA->MODER &= ~(3U << (4 * 2));

    /* PA3 output */
    GPIOA->MODER &= ~(3U << (3 * 2));
    GPIOA->MODER |=  (1U << (3 * 2));

    /* PB0, PB7, PB14 output LEDs */
    GPIOB->MODER &= ~((3U << (0 * 2)) |
                      (3U << (7 * 2)) |
                      (3U << (14 * 2)));

    GPIOB->MODER |=  ((1U << (0 * 2)) |
                      (1U << (7 * 2)) |
                      (1U << (14 * 2)));

    /* PA3 LOW, LEDs OFF */
    GPIOA->BSRR = GPIO_BSRR_BR3;
    GPIOB->BSRR = GPIO_BSRR_BR0 |
                  GPIO_BSRR_BR7 |
                  GPIO_BSRR_BR14;

    /* EXTI4 source = PA4 */
    SYSCFG->EXTICR[1] &= ~(0xFU << 0);

    /* Rising edge interrupt on EXTI4 */
    EXTI->IMR  |= EXTI_IMR_IM4;
    EXTI->RTSR |= EXTI_RTSR_TR4;
    EXTI->FTSR &= ~EXTI_FTSR_TR4;

    /* Clear pending */
    EXTI->PR = EXTI_PR_PR4;

    /* Enable NVIC */
    NVIC_SetPriority(EXTI4_IRQn, 5);
    NVIC_EnableIRQ(EXTI4_IRQn);
}