#include <stdint.h>

#define RCC 0x40023800  //rcc base address
#define RCC_APB1ENR  (*(volatile uint32_t *) (RCC + 0x40))	//APB1 bus address for i2c
#define RCC_AHB1ENR  (*(volatile uint32_t *) (RCC + 0x30))	//AHB1 bus address for gpio b

#define GPIOB_BASE   0x40020400	//GPIO B base address
#define GPIOB_MODER      (*(volatile uint32_t *) (GPIOB_BASE + 0x00))
#define GPIOB_OSPEEDR    (*(volatile uint32_t *) (GPIOB_BASE + 0x08))
#define GPIOB_AFRL       (*(volatile uint32_t *) (GPIOB_BASE + 0x20))
#define GPIOB_AFRH       (*(volatile uint32_t *) (GPIOB_BASE + 0x24))
#define GPIOB_OTYPER     (*(volatile uint32_t *) (GPIOB_BASE + 0x04))
#define GPIOB_PUPDR      (*(volatile uint32_t *) (GPIOB_BASE + 0x0C))
#define GPIOB_ODR        (*(volatile uint32_t *) (GPIOB_BASE + 0x14))

#define I2C1_BASE 0x40005400	//I2C base address
#define I2C1_CR1	(*(volatile uint32_t *) (I2C1_BASE + 0x00))
#define I2C1_CR2	(*(volatile uint32_t *) (I2C1_BASE + 0x04))
#define I2C1_OAR1	(*(volatile uint32_t *) (I2C1_BASE + 0x08))
#define I2C1_OAR2	(*(volatile uint32_t *) (I2C1_BASE + 0x0C))
#define I2C1_DR		(*(volatile uint32_t *) (I2C1_BASE + 0x10))
#define I2C1_SR1	(*(volatile uint32_t *) (I2C1_BASE + 0x14))
#define I2C1_SR2	(*(volatile uint32_t *) (I2C1_BASE + 0x18))
#define I2C1_CCR	(*(volatile uint32_t *) (I2C1_BASE + 0x1C))
#define I2C1_TRISE	(*(volatile uint32_t *) (I2C1_BASE + 0x20))

#define RTC_ADDRESS 0x68	//rtc device id
#define RTC_SECOND 	0x00
#define RTC_MINUTE 	0x01
#define RTC_HOUR  	0x02
#define RTC_DOW    	0x03
#define RTC_DATE   	0x04
#define RTC_MONTH  	0x05
#define RTC_YEAR   	0x06
#define RTC_CONTROL 0x07
#define RTC_ROM 	0x08

#define GPIOD_BASE 	(0x40020C00)
#define GPIOD_MODER    *((volatile unsigned int*)(GPIOD_BASE + 0x00))
#define GPIOD_ODR      *((volatile unsigned int*)(GPIOD_BASE + 0x14))
#define GPIOD_BSRR      *((volatile unsigned int*)(GPIOD_BASE + 0x18))
#define GPIOD_OTYPER   *((volatile unsigned int*)(GPIOD_BASE + 0x04))
#define GPIOD_OSPEEDR   *((volatile unsigned int*)(GPIOD_BASE + 0x08))
#define GPIOD_PUPDR   *((volatile unsigned int*)(GPIOD_BASE + 0x0C))

void gpiod_init() {
	RCC_AHB1ENR |= (1 << 3);

	    GPIOD_MODER &= ~(0x3 << (12 * 2)); // Clear bits for pin 12
	    GPIOD_MODER |= (0x1 << (12 * 2));  // Set pin 12 to output mode

	    GPIOD_OTYPER &= ~(1 << 12); //set pin 12 to 0:output push pull
	    GPIOD_OSPEEDR |= (0x3 << (12 * 2)); // set pin 12 to 11: Very high mode
	    GPIOD_PUPDR &= ~(0x3 << (12 * 2)); //set pin 12 to 00: No pull-up, pull-down

}

void gpio_init()
{
	RCC_AHB1ENR |= (1<<1); // port b clock enable

	GPIOB_MODER &=~ ((1<<12) | (1<<13) | (1<<14) | (1<<15)); // clear pin 6,7 mode
	GPIOB_MODER |= (1<<13) | (1<<15); // alt function at pb6 and pb7

	GPIOB_OTYPER |= ((1<<6) | (1<<7)); //set open drain
	GPIOB_OSPEEDR &=~ ((1<<12) | (1<<13) | (1<<14) | (1<<15)); // clear pin 6,7 speed
	GPIOB_OSPEEDR |= (1<<12) | (1<<13) | (1<<14) | (1<<15); // Very high speed
	GPIOB_PUPDR &=~ ((1<<12) | (1<<13) | (1<<14) | (1<<15)); // clear pin 6,7 pull mode
	GPIOB_PUPDR |= (1<<12) | (1<<14); // pull up on pb6 and pb7
	GPIOB_AFRL |= (4<<(4*6)) | (4<<(4*7)); ////alternat function 4 set for gpiob pin 6 i2c SCK and pin 7 i2c SDA
}

void i2c_reset()
{
	I2C1_CR1 |= (1<<15);
	I2C1_CR1 &=~ (1<<15);
}

void i2c_init()
{
	RCC_APB1ENR |= (1<<21); // I2C1 clock enable
	i2c_reset();
	I2C1_CR2 |= 0x10; // 16MHz clock
	I2C1_CCR |= 138;//
	I2C1_TRISE |= 75;//
	I2C1_CR1 |= (1<<0); // enable I2C
}

int8_t ds1307_date_set(uint16_t Address,uint16_t data){

	 	I2C1_CR1 |= (1<<8) | (1<<10); // start and ack enable

		while(!(I2C1_SR1 & (1<<0)));// wait till start bit is generated
		I2C1_DR = ((RTC_ADDRESS << 1) | 0);// RTC address + write operation

		while(!(I2C1_SR1 & (1<<1))); // wait till address is sent
		uint16_t temp = (I2C1_SR1 | I2C1_SR2); // reset the Status registers

		while(!(I2C1_SR1 & (1<<7))); // wait till Tx buffer is empty

		if(Address == RTC_SECOND){
			I2C1_DR = Address&~(1<<7); 	// data write to data register with 7 clock halt (CH) bit oscillator enable.
		}else{
			I2C1_DR = Address; 	// data write to data register
		}

		while(!(I2C1_SR1 & (1<<2))); // wait till byte is transferred
		while(!(I2C1_SR1 & (1<<7))); // wait till Tx buffer is empty

		I2C1_DR = data;		//data write to register
		while(!(I2C1_SR1 & (1<<7))); // wait till Tx buffer is empty

		I2C1_CR1  |=(1<<9); //stop bit

		return 0;
}

uint16_t bcd(uint16_t val){

	return (val % 10 + ((val / 10) << 4));;
}

uint16_t decimal(uint16_t val)
{
    return ((val >> 4) * 10) + (val & 0x0F);
}

uint16_t i2c_read(uint8_t Address)
{

	I2C1_CR1 |= (1<<8) | (1<<10); // start and ack enable

	while(!(I2C1_SR1 & (1<<0)));// wait till start bit is generated
	I2C1_DR = ((RTC_ADDRESS << 1) | 0);// RTC address + write operation

	while(!(I2C1_SR1 & (1<<1))); // wait till address is sent
	uint16_t temp = (I2C1_SR1 | I2C1_SR2); // reset the Status registers

	while(!(I2C1_SR1 & (1<<7))); // wait till Tx buffer is empty

	I2C1_DR = Address; 	// data write to data register

	while(!(I2C1_SR1 & (1<<2))); // wait till byte is transferred

	I2C1_CR1 |= (1<<8); 	// again send the start
	while(!(I2C1_SR1 & (1<<0))); // wait till start bit is generated

	I2C1_DR = ((RTC_ADDRESS << 1) | 1);//RTC address+ read operation
	while(!(I2C1_SR1 & (1<<1))); // wait till address is sent

	uint16_t rs = (I2C1_SR1 | I2C1_SR2); // clear Status registers 1 and 2

	I2C1_CR1 &=~ (1<<10); // disable ack
	I2C1_CR1 |= (1<<9); // stop bit generated

	while(!(I2C1_SR1 & (1<<6))); // wait till Rx is not empty

	return I2C1_DR; // send the received data
}

void led_on(){
	 GPIOD_BSRR = (1 << 12);
}
void led_off(){
	GPIOD_BSRR = (1 << (12+16));
}
int main(void)
{
	gpio_init();
	i2c_init();
	gpiod_init();


	ds1307_date_set(RTC_SECOND,bcd(1));//second set in rtc
	ds1307_date_set(RTC_MINUTE,bcd(0)); //set minute in rtc
	ds1307_date_set(RTC_HOUR,bcd(12));// set hour in rtc
	ds1307_date_set(RTC_DOW,bcd(7));//set day of week in rtc
	ds1307_date_set(RTC_DATE,bcd(10));//set date in rtc
	ds1307_date_set(RTC_MONTH,bcd(5));//set month in rtc
	ds1307_date_set(RTC_YEAR,bcd(25));//set year in rtc

	uint16_t year, month, date, hour, min, sec, dow;

	while(1){

		sec = decimal(i2c_read(RTC_SECOND));
		min = decimal(i2c_read(RTC_MINUTE));
		if(min % 5 == 0 && sec < 3 ){
			led_on();
		}else{
			led_off();
		}
		hour = decimal(i2c_read(RTC_HOUR));
		dow = decimal(i2c_read(RTC_DOW));
		date = decimal(i2c_read(RTC_DATE));
		month = decimal(i2c_read(RTC_MONTH));
		year = decimal(i2c_read(RTC_YEAR));

	}
}

