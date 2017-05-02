#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <mach/platform.h>
#include "nRF24L01.h"
#include "gpio.h"
#include "radio.h"

#define NRF_CE 25 //module chip enable pin
#define DEVICE_NAME	"nrf_test1"	
#define CLASS_NAME	"nrf_t"
#define MAJOR_NUM 110


/* NRF registers*/
static const uint8_t  nrf_reg[]={

	TX_ADDR,
	RX_ADDR_P0,
	CONFIG,
	EN_AA,
	EN_RXADDR,
	SETUP_AW,
	SETUP_RETR,
	RF_CH,
	RF_SETUP,
	STATUS,
	OBSERVE_TX,
	RPD,
	FIFO_STATUS,	
	RX_PW_P0

};

static char *nrf_reg_name[]={
	"TX_ADDR",
	"RX_ADDR_P0",
	"CONFIG",
	"EN_AA",
	"EN_RXADDR",
	"SETUP_AW",
	"SETUP_RETR",
	"RF_CH",
	"RF_SETUP",
	"STATUS",
	"OBSERVE_TX",
	"RPD",
	"FIFO_STATUS",
	"RX_PW_P0"
};


unsigned int address[5]={0x00,0x00,0x00,0x00,0x00};


static int number_opens = 0;
static struct cdev nrf_cdev;				
static struct class* nrf_class = NULL;			
static struct device* nrf_device = NULL;		
struct spi_device *spi_device;

/******************************
 nrf module file operations
******************************/

static int nrf_open(struct inode *inode, struct file *fp)
{
	number_opens++;
	printk(KERN_INFO "NRF: Device successfully open\n");
	printk(KERN_INFO "NRF: device has been opened %d times",number_opens);
	return 0;
}

static int nrf_release(struct inode *inode,struct file *fp)
{
	printk(KERN_INFO "device successfully closed\n");
	return 0;
}

static ssize_t nrf_write(struct file *fp, const char *data, size_t len, loff_t *offset)
{
	int ret;
	int data_test;
	
	ret = copy_from_user(&data_test,data,len);
	printk(KERN_INFO "%d data written!!\n",data_test);
	
	printk(KERN_INFO "%d bytes written!!\n",len);
	return len;
}


/* Displays NRF module register info*/
static ssize_t nrf_read(struct file *fp, char *buf, size_t len, loff_t *offset){
	
	

	uint8_t *ret;
	char s[64];
	int loop;
	int count=0;
	printk(KERN_INFO "\nnRF24L01+ status display\n");
	

	if(*offset > 0)
		return 0;
	
	for(loop = 0; loop < 14; loop++){

		ret = nrf_transfer(nrf_reg[loop],1,NULL,R);
		mdelay(10);
		sprintf(s,"%s = 0x%02x\n",nrf_reg_name[loop],ret[0]);
		printk(KERN_INFO "%s\n",s);

		if(copy_to_user(buf+count,s,strlen(s))){
		//printk(KERN_INFO "error!!\n");
			return -EFAULT;
		}
		//printk(KERN_INFO "error2!!\n");
		*offset += strlen(s);	
		count+= strlen(s);
	}
	//printk(KERN_INFO "error3!!\n");
	printk(KERN_INFO "%d count!!\n",count);
	return count;

}

struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = nrf_open,
	.release = nrf_release,
	.read = nrf_read,
	.write = nrf_write,
};

/******************************
 register spi device and attach it to Master driver
******************************/
static int reg_spi_device(void)
{
	int retval = 0;
	
	struct spi_board_info spi_device_info = {
		.modalias = "nrf24l01+",
		.max_speed_hz = 5000000,
		.bus_num = 0,
		.chip_select = 0,
		.mode = 0,
	};

	struct spi_master *master;
	
	master = spi_busnum_to_master(spi_device_info.bus_num);
	if(!master){
		printk(KERN_ALERT "getting master device is failed!!\n");	
		retval = -ENODEV;
		return retval;
	}
	
	spi_device = spi_new_device(master,&spi_device_info);
	if(!spi_device){
		printk(KERN_ALERT "registering spi device is failed!!\n");	
		retval = -ENODEV;
		return retval;
	}
	
	spi_device->bits_per_word = 8;

	retval = spi_setup(spi_device);
	if(retval){
		spi_unregister_device(spi_device);
		return retval;
	}

	return 0;

}

/******************************
 Module initialization function
******************************/
static int __init nrf_init(void)
{
	int ret;
	int retval = 0;

	printk(KERN_INFO "hello from module ***!!\n");
	
	retval = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
	
	if(retval < 0)
	{
		printk(KERN_ALERT "device registration failed!!\n");
		return retval;
	}
	
	else
	{
		printk(KERN_INFO "device registration is successful!!\n");	
	}

	nrf_class = class_create(THIS_MODULE, CLASS_NAME);
	
	if(IS_ERR(nrf_class))
	{
		unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
		printk(KERN_ALERT "FAILED to register device class\n\n");
		return PTR_ERR(nrf_class);
	}
	printk(KERN_INFO "NRF: device class registerd correctly\n\n");

	nrf_device	= device_create(nrf_class, NULL, MKDEV(MAJOR_NUM, 0), NULL, DEVICE_NAME);

	if(IS_ERR(nrf_device))
	{
		class_destroy(nrf_class);
		unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
		printk(KERN_ALERT "FAILED to create device\n\n");
		return PTR_ERR(nrf_class);
	}
	printk(KERN_INFO "NRF:class device  registerd correctly\n\n");
	
	cdev_init(&nrf_cdev,&fops);
	if(cdev_add(&nrf_cdev, MAJOR_NUM, 1) == -1)
	{
		device_destroy(nrf_class, MKDEV(MAJOR_NUM, 0));                        
		class_destroy(nrf_class);                             
		unregister_chrdev(MAJOR_NUM, DEVICE_NAME);   
		return -1;
	}

	ret = reg_spi_device();
	if(ret < 0)
	{
		printk(KERN_ALERT "spi init device registration failed!!\n");
		return ret;
	}
	
	else
	{
		printk(KERN_INFO "spi init device registration is successful!!\n");	
	}

	nrf_radio_initialize();
	
	return 0;
}


/******************************
 Module exit function
******************************/
static void __exit nrf_exit(void)
{
	printk(KERN_ALERT "Bye from module!!\n");
	power_down();
	spi_unregister_device(spi_device);

	cdev_del(&nrf_cdev);
	device_destroy(nrf_class, MKDEV(MAJOR_NUM, 0));     
	class_unregister(nrf_class);                          
	class_destroy(nrf_class);                             
	unregister_chrdev(MAJOR_NUM, DEVICE_NAME);             
	printk(KERN_INFO "NRF: Goodbye from the everyone :) :)\n");
}


module_init(nrf_init);
module_exit(nrf_exit);
module_param_array(address,uint,NULL,0);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("DD project");
MODULE_DESCRIPTION("Driver for 2.4 Ghz RF module nrf24L01+");
