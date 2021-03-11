/*************************************************************************************
  (C) Copyright (2012-2021) Hewlett PAckard Enterprise Development LP
\*************************************************************************************
File: acm_pwm.c


Objective:
Provide support for ACM(Apollo Chassis Manager) interface from user application/deamon.
Polling for packets from ACM.
Polling for FAN PWM value changes
Handler for SD-Bus client
**************************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>       
#include <systemd/sd-bus.h>
#include <errno.h>
#include "acm.h"
       
extern ACM_BLK_INDX acm_idx[];
extern int acm_rd_req(unsigned short blk_num);

void *pwm_thread(void *vargp)
{
       long int pwm_val=0x7f;
       char *out_ptr;
       unsigned char *ptr;
       int input_fd, cnt, sz,i;
       unsigned char buff[8];
       unsigned char temp[32];
       static long int last_pwm_val=0;
       struct thread_info *tinfo = vargp;
       printf("PWM thread starting!!!!!!!!!!!!!!!!!!!!!!!!\n");


       cnt = 0;
       do
       {
            for(i = 0; i < 3; i++)
            {
                memset(buff, 0, sizeof(buff));
                strncpy(temp, ACM_PWM_IN, sizeof(temp));
                buff[0] = '0' + i;  //hwmonx  x = 0-2
                strncat(&buff[1], ACM_PWM_FILE, 6); //add  /pwm0 file to the end
                strncat(temp, buff, 7);
                input_fd = open(temp, O_RDONLY);
                if(input_fd >= 0)
                   break;
                printf("Opening sysfs input interface for polling fan pwm: %s, r: %s\n",temp, strerror(errno));
                sleep(2);
            }
//            cnt = cnt++%7;
       }while(input_fd == -1); //wait forever


        memset(buff, 0, sizeof(buff));
        for(cnt=0;;)
        {
        
            sz = read(input_fd, buff, 3);
            if(sz)
            {
                pwm_val = strtol(buff, &out_ptr, 10); 
                 //timed out on poll but read anyway and look for new data
                 if(last_pwm_val != pwm_val)
                 {
                    printf("Found new pwm value: %lx\n", pwm_val);
                    ptr = (unsigned char *)acm_idx[ACM_FAN_PWM_BLK].blk_ptr;
                    *ptr++ = pwm_val; //primary
                    *ptr = pwm_val; //secondary
                    last_pwm_val = pwm_val;
                    acm_rd_req(ACM_FAN_PWM_BLK);  //This will send data to acm
                    cnt = 0;
                 }
                 else
                 {
                    cnt++;
                    if(cnt > 256)
                    {
                        printf("Force PWM update\n");
                        last_pwm_val = 0;
                    }
                 }
            }
            else
            {
                printf("Error reading PWM value. sz: %x\n", sz);
            }
            lseek(input_fd, 0, SEEK_SET);
            sleep(2);
        }
        
        if(input_fd)
           close(input_fd);
       return(NULL);
}


