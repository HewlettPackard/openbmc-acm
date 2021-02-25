/*************************************************************************************
  (C) Copyright (2012-2021) Hewlett PAckard Enterprise Development LP
\*************************************************************************************
File: acm_extra.c


Objective:
Support functions
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

void dumphex(const void *data, int size)
{
    char ascii[17];
    int i,j;

    ascii[16] = '\0';
    for(i = 0; i < size; ++i)
    {
        printf("%02X ", ((unsigned char *)data)[i]);
        if(((unsigned char *)data)[i] >= ' ' && ((unsigned char *)data)[i] <= '~')
        {
            ascii[i % 16] = ((unsigned char *)data)[i];
        }
        else
        {
            ascii[i % 16] = '.';
        }
        if((i+1) % 8 == 0 || i+1 == size)
        {
            printf(" ");
            if((i + 1) % 16 == 0)
            {
                 printf("|  %s \n", ascii);
            }
            else if(i+1 == size)
            {
                 ascii[(i+1) % 16] = '\0';
                 if((i + 1) % 16 <= 8)
                 {
                    printf(" ");
                 }
                 for(j = (i+1) % 16; j < 16; ++j)
                 {
                      printf("   ");
                 }
                 printf("|  %s \n", ascii);
            }
         }
      }
}



