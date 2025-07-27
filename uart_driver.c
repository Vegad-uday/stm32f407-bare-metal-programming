#include <stdint.h>


#define RCC_BASE         (0x40023800)
#define RCC_AHB1ENR      (*(volatile uint32_t*)(RCC_BASE + 0x30))
#define RCC_APB1ENR      (*(volatile uint32_t*)(RCC_BASE + 0x40))

#define GPIOA_BASE            (0x40020000)
#define GPIOA_MODER           (*(volatile unsigned int*)(GPIOA_BASE + 0x00))
#define GPIOA_OSPEEDR		  (*(volatile unsigned int*)(GPIOA_BASE + 0x08))
#define GPIOA_OTYPER		  (*(volatile unsigned int*)(GPIOA_BASE + 0x04))
#define GPIOA_PUPDR           (*(volatile unsigned int*)(GPIOA_BASE + 0x0C))
#define GPIOA_AFRL			  (*(volatile unsigned int*)(GPIOA_BASE +0x20))

#define UART4_BASE (0x40004C00)
#define UART4_CR1   (*(volatile unsigned int*)(UART4_BASE +0x0C))
#define UART4_BRR	(*(volatile unsigned int*)(UART4_BASE +0x08))
#define UART4_CR2	(*(volatile unsigned int*)(UART4_BASE +0x10))
#define UART4_SR 	(*(volatile unsigned int*)(UART4_BASE +0x00))
#define UART4_DR	(*(volatile unsigned int*)(UART4_BASE +0x04))


void gpio_init() {
    RCC_AHB1ENR |= (1 << 0); // Enable GPIOA clock


    GPIOA_MODER &= ~((0x3 << 0) | (0x3 << 2));
    GPIOA_MODER |=  (0x2 << 0) | (0x2 << 2); // Set PA0 and PA1 to alternate function mode

    GPIOA_OTYPER &= ~(1 << 0); // Push-pull for PA0
    GPIOA_OTYPER &= ~(1 << 1); // Push-pull for PA1

    GPIOA_OSPEEDR |= (0x3 << 0) | (0x3 << 2); // High speed
    GPIOA_PUPDR &= ~((0x3 << 0) | (0x3 << 2)); // No pull-up/down


    GPIOA_AFRL &= ~((0xF << 0) | (0xF << 4));
    GPIOA_AFRL |=  (0x8 << 0) | (0x8 << 4);// Set alternate function 8 for PA0 and PA1
}
void uart4_init(){
	 RCC_APB1ENR |= (1<<19);//uart4 clock enable

	 UART4_BRR = 1667;//buad rate 9600
	 UART4_CR1 &=~(1<<12);//1 Start bit, 8 Data bits
	 UART4_CR1 |=(1<<2);//enabal Rx
	 UART4_CR1 |=(1<<3);//enabal Tx
	 UART4_CR1 |=(1<<13); //stop bit

}


void uart_Tx(char c){

	while (!(UART4_SR & (1 << 7)));
	UART4_DR = c;//Tx char c
}

char uart_Rx(){
	  while (!(UART4_SR & (1 << 5)));
	  return UART4_DR;

}

int main(void)
{
	gpio_init();
	uart4_init();

	while(1){
		char c ='u';
		uart_Tx(c);
		uart_Tx('\n');
	}


}

