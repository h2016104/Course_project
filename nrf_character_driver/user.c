#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define DEVICE "/dev/nrf_test1"

int main()
{ int fd;
  int write_buf,read_buf;

  fd = open(DEVICE, O_RDWR);
  
  //read(fd, &read_buf, sizeof(read_buf));
  
  printf("nRF24L01+ module. \n");
  printf("enter the option\n");
  scanf("%d" , &write_buf);

  write(fd, &write_buf, sizeof(write_buf));
  
  read(fd, &read_buf, sizeof(read_buf));

  //printf("random number: %d \n",read_buf);
  close(fd);

  return 0;
}


