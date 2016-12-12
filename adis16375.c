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

int main()
{
	fd = open(device, O_RDWR);
	if(fd < 0) pabort("cant open device");
	ret = set_spi_bit(8);
	close(fd);

	return ret;
}
