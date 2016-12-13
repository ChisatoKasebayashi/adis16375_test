#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

static void pabort(const char *s)
{
	perror(s);
	abort();
}

static const char *device = "/dev/spidev1.0";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 15000000;
static uint8_t delay;

int ret = 0;
int fd;

int set_spi_mode(uint8_t mode)
{
	//printf("[mode]%d ", mode);
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if(ret == -1) pabort("cant set spi mode");
	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if(ret == -1) pabort("cnat get spi mode");

	return ret;
}

int set_spi_bit(uint8_t bit)
{
	printf("set_spi_bit: bit = %x\n", bit);
	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if(ret == -1) pabort("cant set bits per word");
	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if(ret == -1) pabort("cnat get bits per word");

	return ret;
}

int set_spi_speed(uint32_t speed)
{
	printf("set _spi_speed: speed = %d\n",speed);
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if(ret == -1) pabort("cnat set max speed hz");
	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if(ret == -1) pabort("cant get max speed hz");

	return ret;
}

static void transfer(int fd, uint8_t reg)
{
	uint8_t tx[] = {0x00, 0x00, reg, 0x00};
	uint8_t rx[ARRAY_SIZE(tx)] = {0, };
	struct spi_ioc_transfer tr = {
		.tx_buf = (unsigned long)tx,
		.rx_buf = (unsigned long)rx,
		.len = ARRAY_SIZE(tx),
		.delay_usecs = delay,
		.speed_hz = speed,
		.bits_per_word = bits,
	};
	//printf("[len]%d ", tr.len);

	ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
	if(ret == -1) pabort("cant send message");

	if(mode != 4)
	{
	for(ret = 0; ret < ARRAY_SIZE(tx); ret++)
	{
		if(!(ret % 6)) puts("");
		printf("%.2X ", rx[ret]);
	}
	printf("\n");
	}
}

int main()
{
	int i;
	fd = open(device, O_RDWR);
	if(fd < 0) pabort("cant open device");
	ret = set_spi_bit(bits);
	if(ret==-1){
		printf("set_spi_bit Error!\n");
		exit(1);
	}
	ret = set_spi_speed(speed);
	if(ret==-1){
		printf("set_spi_speed Error!\n");
		exit(1);
	}
	for(i = 0; 1; i++)
	{
	//	printf(" -------- Test -------- \n");
		mode =0;
		mode |= SPI_CS_HIGH;
		ret = set_spi_mode(mode);
		if(ret == -1){
			printf("set_spi_mode[CS_HIGH] Error!\n");
		}
		transfer(fd, 0x00);
		mode =0;
		mode |= SPI_CPOL;
		mode |= SPI_CPHA;
		ret = set_spi_mode(mode);
		//printf("writed mode is %d\n",mode);
		if(ret == -1){
			printf("set_spi_mode[MODE:4] Error!\n");
		}
		if(i%2==0)
		{
			printf("[Z_GYRO_OUT] ");
			transfer(fd, 0x1A);//3F F7
		}
		else
		{
			printf("[Z_GYRO_LOW] ");
			transfer(fd, 0x18);//3F F7
		}
	//	ret = set_spi_mode(mode);
	//	transfer(fd);
	//	mode = SPI_NO_CS;
	}
	close(fd);
//	set_spi_mode(mode);


	return ret;
}
