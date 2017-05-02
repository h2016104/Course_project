#ifndef _RADIO_H
#define _RADIO_Hi

#include "gpio.h"
#include <linux/types.h>
#include <asm/io.h>
#include <mach/platform.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/delay.h>
#include <linux/spi/spi.h>
#include <linux/random.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/spinlock.h>

#define R 0
#define W 1

#define DATA_PIPE_0 (1<<0)
#define DATA_PIPE_1 (1<<1)
#define DATA_PIPE_2 (1<<2)
#define DATA_PIPE_3 (1<<3)
#define DATA_PIPE_4 (1<<4)
#define DATA_PIPE_5 (1<<5)
#define DATA_PIPE_ALL 0xFF

#define SPEED_1Mbps 0
#define SPEED_2Mbps 1
#define SPEED_250Kbps 4

#define POWER_18DB 0
#define POWER_12DB 1
#define POWER_6DB 2
#define POWER_0DB 3


int nrf_radio_initialize(void);
uint8_t *nrf_transfer(uint8_t reg, size_t count, uint8_t *val, bool op);
void enable_AA(uint8_t pipe);
void enable_pipes(uint8_t pipe);
int set_retrans_delay(uint8_t delay);
int set_addr_width(uint8_t width);
int set_retrans_count(uint8_t count);
int set_channel(uint8_t channel);
int set_speed(uint8_t speed);
int set_power(uint8_t power);
int set_rx_addr(uint8_t *val, uint8_t pipe);
void set_tx_addr(uint8_t *val);
int set_rx_width(uint8_t width, uint8_t pipe);
int is_tx_full(void);
int is_tx_empty(void);
int is_rx_full(void);
int is_rx_empty(void);
void enable_dynpd(uint8_t pipe);
void power_up(void);
void power_down(void);
void set_as_recv(void);
void set_as_trans(void);
void setPayLoadSize(uint8_t size); 
uint8_t spi_write_reg(uint8_t reg, uint8_t val);
void spi_write_reg_burst(uint8_t reg, const uint8_t *buf, size_t count);
uint8_t spi_read_reg(uint8_t reg);
uint8_t spi_read_reg_burst(uint8_t reg, uint8_t *dst, size_t count);
void setRetries(uint8_t delay, uint8_t count);
void openWritingPipe(uint64_t value);


#endif
