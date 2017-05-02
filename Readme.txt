Readme 
This repository contains two programs.
nrf_character_driver folder contains a kernel module program that can be used to activate nRF24L01+
rfcomm module in kernel space. this module provide functions that configure the nRF24L01+ in kernel.
User can write its own transfer and recieve API's to complete the module.

nrf_final folder contains the user program that uses inbuilt spi driver to configure the nRF24L01+ and perform 
the required transmission and reception.

How to build nrf_character_driver:

$ sudo git clone https://github.com/msperl/spi-config.git
$ cd spi-config
$ sudo depmode –a (just for the first time)
$ modprobe spi-config devices=bus=0:cs=1:force_release (whenever you want to run your module)
$ cd
$ cd  nrf_character_driver
$ make 
$ sudo insmod ./nrf_1.ko
$ sudo gcc -g -o user user.c
$ sudo ./user

How to build nrf_user_program:

$ cd nrf_final
$ ./configure --driver=SPIDEV
$ sudo make install –B
$ cd nrf_user_program
$ cd examples_linux
$ make
$ sudo gcc -g -o nrf_testing nrf_testing.c
$ sudo ./nrf_testing

To find Arduino code:

Path: nrf_final/examples/nrf_testing