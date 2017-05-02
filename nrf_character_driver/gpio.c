#include "gpio.h"

struct GpioRegisters *GpioRegisters_set;

/**********************************
 Sets GPIO pin function
***********************************/
void Set_GPIO_Function(int GPIO, int function_Code)
{
	int registerIndex = GPIO / 10;
	int bit = (GPIO % 10) * 3;
 
	unsigned oldValue = GpioRegisters_set-> GPFSEL[registerIndex];
	unsigned  mask = 0b111 << bit;
    	printk("Changing function of GPIO%d from %x to %x\n",GPIO,(oldValue >> bit) & 0b111,function_Code);

	GpioRegisters_set-> GPFSEL[registerIndex] = (oldValue & ~mask)|((function_Code << bit) & mask);
}

/******************************
 Set GPIO pin level
******************************/
void Set_GPIO_Output_Value(int GPIO, bool output_Value)
{
	if (output_Value)
		GpioRegisters_set-> GPSET[GPIO / 32] = (1 << (GPIO % 32));
	else
		GpioRegisters_set-> GPCLR[GPIO / 32] = (1 << (GPIO % 32));
}
