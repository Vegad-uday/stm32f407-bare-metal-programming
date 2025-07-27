#define GPIOA_BASE            (0x40020000)
#define GPIOA_MODER           (*(volatile unsigned int*)(GPIOA_BASE + 0x00))
#define GPIOA_PUPDR           (*(volatile unsigned int*)(GPIOA_BASE + 0x0C))

#define GPIOD_BASE            (0x40020C00)
#define GPIOD_MODER           (*(volatile unsigned int*)(GPIOD_BASE + 0x00))
#define GPIOD_LED             (*(volatile unsigned int*)(GPIOD_BASE + 0x14))

#define RCC_BASE              (0x40023800)
#define RCC_AHB1ENR           (*(volatile unsigned int*)(RCC_BASE + 0x30))
#define RCC_APB2ENR           (*(volatile unsigned int*)(RCC_BASE + 0x44))

#define SYSCFG_BASE           (0x40013800)
#define SYSCFG_EXTICR1        (*(volatile unsigned int*)(SYSCFG_BASE + 0x08))

#define EXTI_BASE             (0x40013C00)
#define EXTI_IMR              (*(volatile unsigned int*)(EXTI_BASE + 0x00))
#define EXTI_RTSR             (*(volatile unsigned int*)(EXTI_BASE + 0x08))
#define EXTI_FTSR             (*(volatile unsigned int*)(EXTI_BASE + 0x0C))
#define EXTI_PR               (*(volatile unsigned int*)(EXTI_BASE + 0x14))

#define NVIC_ISER0            (*(volatile unsigned int*)0xE000E100)

void EXTI0(void)
{
    RCC_APB2ENR |= (1 << 14);       // enable SYSCFG clock

    SYSCFG_EXTICR1 &= ~(0xF << 0);  // map EXTI0 to PA0

    EXTI_IMR |= (1 << 0);           // unmask EXTI0
    EXTI_RTSR |= (1 << 0);          // rising edge
    EXTI_FTSR &= ~(1 << 0);         // no falling edge

    NVIC_ISER0 |= (1 << 6);         // enable EXTI0 interrupt
}

void delay(unsigned int n){
        for(volatile int i=0; i<4000 * n; i++);
}

void EXTI0_IRQHandler(void)
{
    if (EXTI_PR & (1 << 0))
    {
        EXTI_PR |= (1 << 0);         // clear pending bit
        GPIOD_LED |= (1<<15);
        delay(1);
        GPIOD_LED |= (1<<(15+16));
    }
}


int main(void)
{
         RCC_AHB1ENR |= (1 << 0);  // enable GPIOA
         RCC_AHB1ENR |= (1 << 3);  // enable GPIOD

         GPIOD_MODER &= ~(3 << (15 * 2));
         GPIOD_MODER |= (1 << (15 * 2));  // PD15 output

         GPIOA_MODER &= ~(3 << (0 * 2));  // PA0 input
         GPIOA_PUPDR &= ~(3 << (0 * 2));  // No pull-up/pull-down

     EXTI0();

    while (1)
    {

    }
         
