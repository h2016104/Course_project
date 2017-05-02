#include "radio.h"
#include "nRF24L01.h"
#include "gpio.h"

static DEFINE_SPINLOCK(spilock);
static unsigned long spilockflags;

extern struct spi_device *spi_device;
extern struct GpioRegisters *GpioRegisters_set;


uint8_t payload_size;
/******************************
 Initializes radio
******************************/
int nrf_radio_initialize(void)
{
	int result;
	uint8_t addr[5] = {0x12,0x12,0x12,0x12,0x12};
	uint8_t addr1[5] = {0x15,0x15,0x15,0x15,0x15};

	GpioRegisters_set = (struct GpioRegisters *)__io_address(GPIO_BASE);
	
	//keep CE low
	Set_GPIO_Function(NRF_CE,1);
	Set_GPIO_Output_Value(NRF_CE,0);

	enable_AA(DATA_PIPE_0);
	enable_pipes(DATA_PIPE_0);

	result = set_addr_width(5);
	if(result){
		printk(KERN_ALERT "Error: setting addr width failed!\n");
		goto out;
	}

	result = set_channel(1);
	if(result){
		printk(KERN_ALERT "Error: setting RF channel freq!\n");
		goto out;
	}

	result = set_speed(SPEED_1Mbps);
	if(result){
		printk(KERN_ALERT "Error: setting transmission speed!\n");
		goto out;
	}

	result = set_power(POWER_0DB);
	if(result){
		printk(KERN_ALERT "Error: setting radio power!\n");
		goto out;
	}

	result = set_rx_addr(addr,0);
	if(result){
		printk(KERN_ALERT "Error: setting rx address!\n");
		goto out;
	}

	set_tx_addr(addr1);

	result = set_rx_width(1,0);
	if(result){
		printk(KERN_ALERT "Error: setting pipe width!\n");
		goto out;
	}

	power_up();
	//set_as_recv();
	set_as_trans();

	return 0;

	out:
	return result;

}

/******************************
 Set module in transmitter mode
******************************/
void set_as_trans(void)
{
	uint8_t *val;
	uint8_t temp;

	val = nrf_transfer(CONFIG,1,NULL,R);
	temp = val[0];

	temp &= ~(1<<PRIM_RX);

	nrf_transfer(CONFIG,1,&temp,W);
}


/******************************
 Set module in receiver mode
******************************/
void set_as_recv(void)
{
	uint8_t *val;
	uint8_t temp;

	val = nrf_transfer(CONFIG,1,NULL,R);
	temp = val[0];

	temp |= (1<<PRIM_RX);

	nrf_transfer(CONFIG,1,&temp,W);
}


/******************************
 Power down nrf module
******************************/
void power_down(void)
{
	uint8_t *val;
	uint8_t temp;

	val = nrf_transfer(CONFIG,1,NULL,R);
	temp = val[0];

	temp &= ~(1<<PWR_UP);

	nrf_transfer(CONFIG,1,&temp,W);

}

/******************************
 Power up nrf module
******************************/
void power_up(void)
{
	uint8_t *val;
	uint8_t temp;

	val = nrf_transfer(CONFIG,1,NULL,R);
	temp = val[0];

	temp |= (1<<PWR_UP);

	nrf_transfer(CONFIG,1,&temp,W);

}


/******************************
 Enables Dynamic payload length on data pipes.
******************************/
void enable_dynpd(uint8_t pipe)
{
	nrf_transfer(DYNPD,1,&pipe,W);
}

/******************************
 checks whether RX FIFO is empty or not
******************************/
int is_rx_empty(void)
{
	if(nrf_transfer(FIFO_STATUS,1,NULL,R) && RX_EMPTY)
		return 1;
	else
		return 0;
}

/******************************
 checks whether RX FIFO is full or not
******************************/
int is_rx_full(void)
{
	if(nrf_transfer(FIFO_STATUS,1,NULL,R) && RX_FULL)
		return 1;
	else
		return 0;
}


/******************************
 checks whether TX FIFO is empty or not
******************************/
int is_tx_empty(void)
{
	if(nrf_transfer(FIFO_STATUS,1,NULL,R) && TX_EMPTY)
		return 1;
	else
		return 0;
}


/******************************
 checks whether TX FIFO is full or not
******************************/
int is_tx_full(void)
{
	if(nrf_transfer(FIFO_STATUS,1,NULL,R) && TX_FULL)
		return 1;
	else
		return 0;
}

/******************************
 sets RX payload width
******************************/
int set_rx_width(uint8_t width, uint8_t pipe)
{
	uint8_t cmd;

	if(width > 32)
		return -1;
	if(pipe > 5)
		return -1;

	cmd = RX_PW_P0 + pipe;

	switch(pipe){

		case 0:
			nrf_transfer(cmd, 1, &width, W);
			break;
		case 1:
			nrf_transfer(cmd, 1, &width, W);
			break;
		case 2:
			nrf_transfer(cmd, 1, &width, W);
			break;
		case 3:
			nrf_transfer(cmd, 1, &width, W);
			break;
		case 4:
			nrf_transfer(cmd, 1, &width, W);
			break;
		case 5:
			nrf_transfer(cmd, 1, &width, W);
			break;
	}
	return 0;
}

/******************************
 sets TX address (5 bytes)
******************************/
void set_tx_addr(uint8_t *val)
{
	nrf_transfer(TX_ADDR, 5, val, W);

}


/******************************
 sets address of different RX pipes
******************************/

int set_rx_addr(uint8_t *val, uint8_t pipe)
{
	uint8_t cmd;
	
	if(pipe > 5)
		return -1;

	cmd = RX_ADDR_P0 + pipe;

	switch(pipe){
		
		case 0:
			nrf_transfer(cmd, 5, val, W);
			break;
		case 1:
			nrf_transfer(cmd, 5, val, W);
			break;
		case 2:
			nrf_transfer(cmd, 1, val, W);
			break;
		case 3:
			nrf_transfer(cmd, 1, val, W);
			break;
		case 4:
			nrf_transfer(cmd, 1, val, W);
			break;
		case 5:
			nrf_transfer(cmd, 1, val, W);
			break;
	}
	return 0;
}


/******************************
 sets RF output power

 Note:- value range (0-3)
******************************/

int set_power(uint8_t power)
{
	
	uint8_t *val;
	uint8_t temp;

	if(power > 3)
		return -1;
	
	val = nrf_transfer(RF_SETUP, 1, NULL, R);
	temp = val[0];
	
	switch(power){

	case 0:
		temp &= ~((1<<1)+(1<<2));
		break;
	case 1:
		temp |= (1<<1);
		temp &= ~(1<<2);
		break;
	case 2:
		temp &= ~(1<<1);
		temp |= (1<<2);
		break;
	case 3:
		temp |= (1<<1) + (1<<2);
		break;
	default:
		break;

	}


	nrf_transfer(RF_SETUP, 1, &temp, W);

	return 0;
}


/******************************
 Sets communication speed

 Note:- 0----> 1Mbps
 	1----> 2Mbps
	2----> 250Kbps
******************************/

int set_speed(uint8_t speed)
{
	
	uint8_t *val;
	uint8_t temp;

	if(speed > 2)
		return -1;
	
	val = nrf_transfer(RF_SETUP, 1, NULL, R);
	temp = val[0];

	switch(speed){

	case 0:
		temp &= ~((1<<5)+(1<<3));
		break;
	case 1:
		temp &= ~(1<<5);
		temp |= (1<<3);
		break;
	case 2:
		temp |= (1<<5);
		temp &= ~(1<<3);
		break;
	default:
		break;
	}


	nrf_transfer(RF_SETUP, 1, &temp, W);

	return 0;
}


/******************************
 Sets nrf channel frequency

 note:- total channels 0 to 127
******************************/

int set_channel(uint8_t channel)
{
	if(channel > 127)
		return -1;

	nrf_transfer(RF_CH, 1, &channel, W);
	return 0;
}


/******************************
 Sets retransmission counts

 Note:- 0 = disabled, max value = 15
******************************/
int set_retrans_count(uint8_t count)
{
	uint8_t *val;

	if(count > 15)
		return -1;
	
	val = nrf_transfer(SETUP_RETR, 1, NULL, R);
	
	count = count | val[0];

	nrf_transfer(SETUP_RETR, 1, &count, W);
	return 0;
}


/******************************
 Sets retransmission delay

 Note:- value is between 0 to 15. each level adds 250us delay

 0 -----> 250us
 1 -----> 500us
 2 -----> 750us
 .         .
 .         .
******************************/

int set_retrans_delay(uint8_t delay)
{
	uint8_t *val;

	if(delay > 15)
		return -1;
	
	delay = delay << 4;
	val = nrf_transfer(SETUP_RETR,1,NULL,R);
	
	delay = delay | val[0];

	nrf_transfer(SETUP_RETR,1,&delay,W);
	return 0;
}


/******************************
 Sets RX/TX address width

 Note:- max value is 5 bytes and 0 bytes not allowed. If provided greater than that,
 function will return immediate and won't write to register
******************************/

int set_addr_width(uint8_t width)
{
	if(!(width > 2 || width < 6))
		return -1;
	width = width - 2;
	nrf_transfer(SETUP_AW, 1, &width, W);
	return 0;
}


/******************************
 Enables RX data pipes 
 
 e.g :
 enable_pipes(DATA_PIPE_0 | DATA_PIPE_1);
 turns on data AA on pipe 0 & pipe 1
******************************/

void enable_pipes(uint8_t pipe)
{
	nrf_transfer(EN_RXADDR, 1, &pipe, W);
}


/******************************
 Sets auto acknowledgement to given pipe numbers
turns on data AA on pipe 0 & pipe 1
******************************/

void enable_AA(uint8_t pipe)
{
	nrf_transfer(EN_AA, 1, &pipe, W);
}


/******************************
 This function performs Read/Write to nrf chip.
******************************/

uint8_t *nrf_transfer(uint8_t reg, size_t count, uint8_t *val, bool op)
{
	int retval;
	int loop;
	static uint8_t retarr[32];
	uint8_t len;
	
	len = count + 1;

	if(op == W){
		if(reg != W_TX_PAYLOAD)
			reg = reg + W_REGISTER;
	
		retarr[0] = reg;
		for(loop=1;loop < len;loop++){
			retarr[loop] = val[loop-1];
		}
		retval = spi_write(spi_device,retarr,len);
		//printk(KERN_INFO "printing retval: %d", retval);
		if(retval != 0)
			printk(KERN_INFO "failed to write register to device\n");
		
	}

	if(op == R){

		retval = spi_write_then_read(spi_device,&reg,1,retarr,count);
                //printk(KERN_INFO "printing retarr: %02", retarr[0]);
		if(retval != 0)
			printk(KERN_INFO "failed to read from device\n");
	}

	return retarr;
}

/******************************
 Functions for accessing registers and
 performs transmission reception.
******************************/

/*
uint8_t spi_write_reg(uint8_t reg, uint8_t val)
{
	struct spi_transfer t[3];
	struct spi_message m;
	uint8_t rxbuf;

	spi_message_init(&m);

	memset(t, 0, sizeof(t));

	t[0].tx_buf = ( W_REGISTER | ( REGISTER_MASK & reg ) );
	t[0].len = 1;
	spi_message_add_tail(&t[0], &m);

	
	t[1].tx_buf = &val;
	t[1].len = 1;
	spi_message_add_tail(&t[1], &m);
	

	t[2].rx_buf = &rxbuf;
	t[2].len = 1;
	spi_message_add_tail(&t[2], &m);

	spin_lock_irqsave(&spilock, spilockflags);
	spi_sync(spi_device, &m);
	spin_unlock_irqrestore(&spilock, spilockflags);

	return rxbuf;
}

void spi_write_reg_burst(uint8_t reg, const uint8_t *buf, size_t count)
{
	struct spi_transfer t[2];
	struct spi_message m;

	spi_message_init(&m);

	memset(t, 0, sizeof(t));

	t[0].tx_buf = &reg;
	t[0].len = 1;
	spi_message_add_tail(&t[0], &m);

	t[1].tx_buf = buf;
	t[1].len = count;
	spi_message_add_tail(&t[1], &m);

	spin_lock_irqsave(&spilock, spilockflags);
	spi_sync(spi_device, &m);
	spin_unlock_irqrestore(&spilock, spilockflags);
}

uint8_t spi_read_reg_burst(uint8_t reg, uint8_t *dst, size_t count)
{
	struct spi_transfer t[2];
	struct spi_message m;

	spi_message_init(&m);

	memset(t, 0, sizeof(t));

	t[0].tx_buf = ( R_REGISTER | ( REGISTER_MASK & reg ) );
	t[0].len = 1;
	spi_message_add_tail(&t[0], &m);

	t[1].rx_buf = (uint8_t *)dst;
	t[1].len = count;
	spi_message_add_tail(&t[1], &m);

	spin_lock_irqsave(&spilock, spilockflags);
	spi_sync(spi_device, &m);
	spin_unlock_irqrestore(&spilock, spilockflags);

	return 0;
}


uint8_t spi_read_reg(uint8_t reg)
{
	uint8_t res;

	if (spi_read_reg_burst(reg, &res, 1) == 0)
		return res;

	return 0;
}


void setRetries(uint8_t delay, uint8_t count)
{
	spi_write_reg(SETUP_RETR,(delay&0xf)<<ARD | (count&0xf)<<ARC);
}

void openWritingPipe(uint64_t value)
{

	spi_write_reg_burst(RX_ADDR_P0, value, 5);
	spi_write_reg_burst(TX_ADDR, value, 5);


 	spi_write_reg(RX_PW_P0,payload_size);
}
*/