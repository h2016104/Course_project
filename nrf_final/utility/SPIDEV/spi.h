
#ifndef SPI_H
#define	SPI_H

/**
 * @file spi.h
 * \cond HIDDEN_SYMBOLS
 * Class declaration for SPI helper files
 */

 /**
 * Example GPIO.h file
 *
 * @defgroup SPI SPI Example
 *
 * See RF24_arch_config.h for additional information
 * @{
 */

#include <inttypes.h>

#ifndef RF24_SPIDEV_SPEED
/* 8MHz as default */
#define RF24_SPIDEV_SPEED 8000000
#endif

class SPI {
public:

	/**
	* SPI constructor
	*/
	SPI();

	/**
	* Start SPI
	*/
	void begin(int busNo);

	/**
	* Transfer a single byte
	* @param tx Byte to send
	* @return Data returned via spi
	*/
	uint8_t transfer(uint8_t tx);

	/**
	* Transfer a buffer of data
	* @param tbuf Transmit buffer
	* @param rbuf Receive buffer
	* @param len Length of the data
	*/
	void transfernb(char* tbuf, char* rbuf, uint32_t len);

	/**
	* Transfer a buffer of data without an rx buffer
	* @param buf Pointer to a buffer of data
	* @param len Length of the data
	*/
	void transfern(char* buf, uint32_t len) {
	  transfernb(buf, buf, len);
	}

	~SPI();

private:

	int fd;

	void init();
};

/**
 * \endcond
 */
/*@}*/
#endif	/* SPI_H */

