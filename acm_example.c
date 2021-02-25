/*************************************************************************************
  (C) Copyright (2012-2021) Hewlett PAckard Enterprise Development LP
\*************************************************************************************
File: acm_example.c


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



int child_proc(void) {
        sd_bus_error error = SD_BUS_ERROR_NULL;
        sd_bus_message *m = NULL;
        sd_bus *bus = NULL;
        const char data[512];
        long unsigned int size;
        int i, r;
        const void *data_ptr;

        /* Connect to the system bus */
        r = sd_bus_default(&bus);
        if (r < 0) {
                fprintf(stderr, "Failed to connect to system bus: %s\n", strerror(-r));
                goto finish;
        }

        /* Issue the method call and store the response message in m */
        /*r = sd_bus_call_method(bus,*/
//        r = sd_bus_call_method(bus, 
//                               "org.freedesktop.systemd1",           /* service to contact */
//                               "/org/freedesktop/systemd1",           /* object path */
//                               "org.freedesktop.systemd1.Manager",           /* interface name */
//                               "StartUnit",                         /*  method name */
//                               &error,                               /* object to return error in */
//                               &m,                                   /* return message on success */
//                               "ss" ,                                /* input signature */
//                               "cups.service",                           /* first argument */
//                               "replace"
//                               );                             
                         
        r = sd_bus_call_method(bus, 
                               "net.acm.blocks",           /* service to contact */
                               "/net/acm/blocks",           /* object path */
                               "net..blocks",           /* interface name */
                               "Read",                         /*  method name */
                               &error,                               /* object to return error in */
                               &m,                                   /* return message on success */
                               "x" ,                                /* input signature */
                               1                           /* first argument */
                               );                           

        if (r < 0) {
                fprintf(stderr, "Failed to issue method call: %s\n", error.message);
                goto finish;
        }

	data_ptr = (const void *)data;
        size = sizeof(data);
        /* Parse the response message */
        r = sd_bus_message_read_array(m, 'y', &data_ptr, &size);
        if (r < 0) {
                fprintf(stderr, "Failed to parse response message: %s\n", strerror(-r));
                goto finish;
        }
        printf("SD_BUS block read results: \n");
        for(i = 0; i < size; i++)
        {
            printf("%x ", data[i]);
            if(((i+1) % 16) == 0)
                 printf("\n");
        }
        printf("\n");

finish:
        sd_bus_error_free(&error);
        sd_bus_message_unref(m);
        sd_bus_unref(bus);

        return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

