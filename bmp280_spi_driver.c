#include <stdint.h>

// RCC
#define RCC_BASE         (0x40023800)
#define RCC_AHB1ENR      (*(volatile uint32_t*)(RCC_BASE + 0x30))
#define RCC_APB1ENR      (*(volatile uint32_t*)(RCC_BASE + 0x40))

// SPI2
#define SPI2_BASE        (0x40003800)
#define SPI2_CR1         (*(volatile uint32_t*)(SPI2_BASE + 0x00))
#define SPI2_SR          (*(volatile uint32_t*)(SPI2_BASE + 0x08))
#define SPI2_DR          (*(volatile uint32_t*)(SPI2_BASE + 0x0C))

// GPIOB
#define GPIOB_BASE       (0x40020400)
#define GPIOB_MODER      (*(volatile uint32_t*)(GPIOB_BASE + 0x00))
#define GPIOB_OSPEEDR    (*(volatile uint32_t*)(GPIOB_BASE + 0x08))
#define GPIOB_AFRH       (*(volatile uint32_t*)(GPIOB_BASE + 0x24))
#define GPIOB_ODR        (*(volatile uint32_t*)(GPIOB_BASE + 0x14))

// BMP280
#define BMP280_ID_REG           0xD0
#define BMP280_CTRL_MEAS        0xF4
#define BMP280_TEMP_MSB         0xFA
#define BMP280_TEMP_LSB         0xFB
#define BMP280_TEMP_XLSB        0xFC
#define BMP280_CALIB_TEMP_START 0x88

uint16_t dig_T1;
int16_t dig_T2, dig_T3;
int32_t t_fine;

// SPI
void spi_tx(uint8_t *data, int size) {
	for (int i = 0; i < size; i++) {
		while (!(SPI2_SR & (1 << 1))); // TXE
		SPI2_DR = data[i];
	}
	while (SPI2_SR & (1 << 7)); // wait BSY=0
	uint8_t tmp = SPI2_DR;
	tmp = SPI2_SR;
}

void spi_rx(uint8_t *data, int size) {
	while (size--) {
		while (SPI2_SR & (1 << 7)); // BSY
		SPI2_DR = 0xFF; // dummy
		while (!(SPI2_SR & (1 << 0))); // RXNE
		*data++ = SPI2_DR;
	}
}

// SPI single byte read
uint8_t bmp280_read_reg(uint8_t reg) {
	uint8_t tx[2] = { reg | 0x80, 0xFF };
	uint8_t rx[2];

	GPIOB_ODR &= ~(1 << 9); // CSB low
	spi_tx(&tx[0], 1);//data tx
	spi_rx(&rx[1], 1);// data tx
	GPIOB_ODR |= (1 << 9); // CSB high

	return rx[1];
}


void bmp280_read_bytes(uint8_t reg, uint8_t* buffer, uint8_t len) {
	reg |= 0x80;
	GPIOB_ODR &= ~(1 << 9);
	spi_tx(&reg, 1);
	spi_rx(buffer, len);
	GPIOB_ODR |= (1 << 9);
}

// SPI single byte write
void bmp280_write_reg(uint8_t reg, uint8_t value) {
	uint8_t tx[2] = { reg & 0x7F, value };
	GPIOB_ODR &= ~(1 << 9);
	spi_tx(tx, 2);
	GPIOB_ODR |= (1 << 9);
}

void bmp280_read_calib() {
	uint8_t calib[6];
	bmp280_read_bytes(BMP280_CALIB_TEMP_START, calib, 6); // read address of BMP280_CALIB_TEMP_START and store in calib[6] array
	dig_T1 = (uint16_t)(calib[1] << 8 | calib[0]); //0x89
	dig_T2 = (int16_t)(calib[3] << 8 | calib[2]);//0x8B
	dig_T3 = (int16_t)(calib[5] << 8 | calib[4]);//0x 8D
}

int32_t bmp280_compensate_T(int32_t adc_T) {
	int32_t var1, var2, T;
	var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
	var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}

void gpio() {
	RCC_AHB1ENR |= (1 << 1); // GPIOB clock

	// Set PB13, PB14, PB15 (SPI2) alternate function
	GPIOB_MODER &= ~((3 << 26) | (3 << 28) | (3 << 30));
	GPIOB_MODER |= (2 << 26) | (2 << 28) | (2 << 30);

	// PB9 as NSS (manual software control)
	GPIOB_MODER |= (1 << 18);

	// Speed
	GPIOB_OSPEEDR |= (3 << 26) | (3 << 28) | (3 << 30) | (3 << 18);

	// AF5 for SPI2
	GPIOB_AFRH &= ~(0xFFF << 20);
	GPIOB_AFRH |= (5 << 20) | (5 << 24) | (5 << 28);
}

void spi_config() {
	RCC_APB1ENR |= (1 << 14); // SPI2 clock enable

	SPI2_CR1 = 0;
	SPI2_CR1 |= (1 << 2); // Master
	SPI2_CR1 |= (1 << 1); // CPOL = 1
	SPI2_CR1 |= (1 << 0); // CPHA = 1
	SPI2_CR1 |= (1 << 9) | (1 << 8); // SSM, SSI
	SPI2_CR1 |= (1 << 6); // SPE
}

// Read raw temperature
int32_t bmp280_read_raw_temp() {
	uint8_t buf[3];
	bmp280_read_bytes(BMP280_TEMP_MSB, buf, 3);
	return (int32_t)(((uint32_t)buf[0] << 12) | ((uint32_t)buf[1] << 4) | ((uint32_t)buf[2] >> 4));
}

int main(void) {
	gpio();
	spi_config();

	uint8_t chip_id = bmp280_read_reg(BMP280_ID_REG);
	while (chip_id == 0x58); // BMP280 ID

	bmp280_read_calib();
	bmp280_write_reg(BMP280_CTRL_MEAS, 0x27); // normal mode

	while (1) {
		int32_t raw_temp = bmp280_read_raw_temp();
		int32_t temp = bmp280_compensate_T(raw_temp);

	}
}

