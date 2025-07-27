#include <stdint.h>

#define RCC_BASE         0x40023800
#define RCC_AHB1ENR  *((volatile unsigned int*)(RCC_BASE + 0x30))
#define RCC_APB1ENR      (*(volatile uint32_t *) (RCC_BASE + 0x40))
#define RCC_CFGR         (*(volatile uint32_t *) (RCC_BASE + 0x08))

#define TIM4_BASE        0x40000800
#define TIM4_CR1         (*(volatile uint32_t *) (TIM4_BASE + 0x00))
#define TIM4_CCMR1       (*(volatile uint32_t *) (TIM4_BASE + 0x18))
#define TIM4_CCER        (*(volatile uint32_t *) (TIM4_BASE + 0x20))
#define TIM4_CNT         (*(volatile uint32_t *) (TIM4_BASE + 0x24))
#define TIM4_PSC         (*(volatile uint32_t *) (TIM4_BASE + 0x28))
#define TIM4_ARR         (*(volatile uint32_t *) (TIM4_BASE + 0x2C))
#define TIM4_CCR1        (*(volatile uint32_t *) (TIM4_BASE + 0x34)) // Use CH1

#define GPIOD_BASE_ADD   0x40020C00
#define GPIOD_MODER      (*(volatile uint32_t *) (GPIOD_BASE_ADD + 0x00))
#define GPIOD_OSPEEDR    (*(volatile uint32_t *) (GPIOD_BASE_ADD + 0x08))
#define GPIOD_AFRH       (*(volatile uint32_t *) (GPIOD_BASE_ADD + 0x24))

int main(void)
{
    // Set system clock to HSI
    RCC_CFGR &= ~((1 << 0) | (1 << 1));

    // Set APB1 prescaler to 1
    RCC_CFGR &= ~(1 << 12);

    // Enable clock for TIM4
    RCC_APB1ENR |= (1 << 2);

    // Configure TIM4 before enabling
    TIM4_CNT = 0;               // Reset counter
    TIM4_PSC = 16000 - 1;       // Prescaler for 1ms
    TIM4_ARR = 5000 - 1;        // Auto-reload for 1 sec

    // Configure PWM mode 1 on Channel 1
    TIM4_CCMR1 &= ~((1 << 0) | (1 << 1));   // Clear CC1S bits (output mode)
    TIM4_CCMR1 |= (1 << 3);       // Output compare preload enable
    TIM4_CCMR1 |= (0b110 << 4);   // PWM mode 1

    TIM4_CCER |= (1 << 0);      // Enable output for CH1

    TIM4_CCR1 = 2500;     // Initial duty cycle 50%

    TIM4_CR1 |= (1 << 0);    // Enable the counter

    // Enable clock for GPIOD
    RCC_AHB1ENR |= (1 << 3);


    // Set PD12 as Alternate Function mode
    GPIOD_MODER &= ~(0x3 << (12 * 2));   // Clear mode bits
    GPIOD_MODER |= (0x2 << (12 * 2));    // Set alternate function mode

    // Set PD12 speed to high
    GPIOD_OSPEEDR |= (0x3 << (12 * 2));

    // Set Alternate Function to AF2 for timer 4
    GPIOD_AFRH &= ~(0xF << ((12 - 8) * 4)); // Clear AFRH bits
    GPIOD_AFRH |= (0x2 << ((12 - 8) * 4)); // Set AF2 for TIM4

    while (1)
    {

    };
}

