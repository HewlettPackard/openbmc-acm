/*************************************************************************************
  (C) Copyright (2012-2021) Hewlett PAckard Enterprise Development LP
\*************************************************************************************
File: acm_service.c


Objective:
SD-BUS service

Provide support for ACM(Apollo Chassis Manager) interface from user application/deamon.
Polling for packets from ACM.
Polling for FAN PWM value changes
Handler for SD-Bus client
**************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <systemd/sd-bus.h>
#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <unistd.h>
#include "acm.h"
#include "acm_extra.h"

#define FAN_PWM_BLOCK_1 1

extern ACM_BLK_INDX acm_idx[];
extern int acm_rd_req(unsigned short blk_num);


typedef struct
{
        unsigned char block_len[2];
        unsigned char inst;
	unsigned char block_num[2];
        unsigned char data[512];   
} BLOCK_DATA;


/*
	Send a single packet block to acm

int acm_send_single_block(BLOCK_DATA *block, int size)
{
     int fd, r;

        fd = open(ACM_IN, O_WRONLY | O_TRUNC, 0664);
        if(fd < -1)
        {
            fprintf(stderr, "Failed to open file\n");
        }
        r = write(fd, (unsigned char *)block, size);
        close(fd);	
	return(0);
}
*/

static int method_dump(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) 
{
        int x, i;
        int r;
	unsigned char a[512], *ptr;

        /* Read the parameters */
        r = sd_bus_message_read(m, "x", &x); //block number read
        if (r < 0) {
                fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-r));
                return r;
        }
        if(x == 0xff)
        {
            //dump all block data
            for(i = 0; i < 0x105; i++)
            {
                if(acm_idx[i].type >= 0)
                {
                    printf("Block %x \n", i);
                    dumphex(acm_idx[i].blk_ptr, acm_idx[i].blk_size);
                }
            }
        }
        else
        {   i = 1;
            printf("Block %x \n", x);
            dumphex(acm_idx[x].blk_ptr, acm_idx[x].blk_size);

        }
        return sd_bus_reply_method_return(m, "x", i);
}

extern void acm_ping_init(void);

static int method_ping(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) 
{
        int x, i;
        int r;

        /* Read the parameters */
        r = sd_bus_message_read(m, "x", &x); //block number read
        if (r < 0) {
                fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-r));
                return r;
        }
        if(x == 8)
        {
            acm_ping_init();
            i = x;
        }
        else
        {
            i = 0xff;
        }
        return sd_bus_reply_method_return(m, "x", i);
}



static int method_read(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
        int x;
        int r=0;
	unsigned char a[512], *ptr;
       

        /* Read the parameters */
        r = sd_bus_message_read(m, "x", &x); //block number read
        if (r < 0) {
                fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-r));
        }
        else
        {
            if(acm_idx[x].type >= 0)
            {
                ptr = (unsigned char *)acm_idx[x].blk_ptr;
                printf("copying data for block: %x, size: %x, data: %x %x %x %x\n", x, acm_idx[x].blk_size,
                      ptr[0], ptr[1], ptr[2], ptr[3]);
                memcpy(a, acm_idx[x].blk_ptr, acm_idx[x].blk_size);
                /* Reply with the response */
                printf("Reading block: %x data: %x %x %x %x\n",x, a[0], a[1], a[2],a[3]);
            }
            else
            {
                printf("Invalid block number\n");
                r = 1;
            }
        }
        if(r == 0)
        {
               // return sd_bus_message_new_method_return(m, &reply); 
                return sd_bus_reply_method_return(m, "ay", acm_idx[x].blk_size, a[0], a[1], a[2], a[3], a[4], a[5],a[6], a[7], a[8], a[9],
                                                                                                                 a[10], a[11], a[12], a[13], a[14], a[15], a[16], a[17], a[18], a[19], 
                                                                                                                 a[20], a[21], a[22], a[23], a[24], a[25], a[26], a[27], a[28], a[29],
                                                                                                                 a[30], a[31], a[32], a[33], a[34], a[35], a[36], a[37], a[38], a[39],
                                                                                                                 a[40], a[41], a[42], a[43], a[44], a[45], a[46], a[47], a[48], a[49]);
        }
        else
        {
                return sd_bus_reply_method_return(m, "ay", 1, r);      
        }
}

static int method_write(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) 
{
        int x;
        const char *s;
        int r=0;
//        int fd;

        /* Read the parameters */
        r = sd_bus_message_read(m, "xs", &x, &s);
        if (r < 0) {
                fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-r));
        }
        else
        {
    	     fprintf(stderr,"Received block: %x, data: %s\n",x, s);
            if(acm_idx[x].type >= 0)
            {
                memcpy(acm_idx[x].blk_ptr, s, (acm_idx[x].blk_size < strlen(s)) ? acm_idx[x].blk_size : strlen(s));
                acm_rd_req(x);  //This will send data to acm
            }
            else
            {
                r = 1;
            }
        }
        /* Reply with the response */
        return sd_bus_reply_method_return(m, "x", r);
}


static int method_fan_pwm_write(sd_bus_message *m, void *userdata, sd_bus_error *ret_error) {
        int x,i;
        int pwm;
	 unsigned char *ptr;
        int r=0;
        BLOCK_DATA block;

        /* Read the parameters */
        r = sd_bus_message_read(m, "xx", &x, &pwm);
        if (r < 0) {
                fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-r));
        }
        else
        {
            fprintf(stderr,"Received PWM value: %02x\n", pwm);
            ptr = (unsigned char *)acm_idx[x].blk_ptr;
            for(i = 0; i < 8; i++)
            {
                ptr[i] = pwm;
            }
            acm_rd_req(x);  //This will send data to acm
            r = pwm;
        }
        /* Reply with the response */
        return sd_bus_reply_method_return(m, "x", r);
            
}


/* The vtable of our little object, implements the net.poettering.Calculator interface */
static const sd_bus_vtable blocks_vtable[] = {
        SD_BUS_VTABLE_START(0),
        SD_BUS_METHOD("Dump", "x", "x", method_dump, SD_BUS_VTABLE_UNPRIVILEGED),
        SD_BUS_METHOD("Read", "x", "ay", method_read, SD_BUS_VTABLE_UNPRIVILEGED),
        SD_BUS_METHOD("Write", "xs", 
                                  "x", method_write,
                                   SD_BUS_VTABLE_UNPRIVILEGED),
        SD_BUS_METHOD("FANPWM", "xx", "x", method_fan_pwm_write,
                                   SD_BUS_VTABLE_UNPRIVILEGED),
        SD_BUS_METHOD("Ping", "x", "x", method_ping, SD_BUS_VTABLE_UNPRIVILEGED),
        SD_BUS_VTABLE_END
};

void *sd_thread(void *vargp)
{
//int argc, char *argv[]) {
        ACM_BLK_INDX *sd_acm_idx;
        sd_bus_slot *slot = NULL;
        sd_bus *bus = NULL;
        int r;
        struct thread_info *tinfo = vargp;

        printf("SD_Bus service thread starting !!!!!!!!!!!!!!!!!!!!!!\n");

        sd_acm_idx = tinfo->blocks;

        /* Connect to the user bus this time */
        r = sd_bus_default(&bus);
        if (r < 0) {
                fprintf(stderr, "Failed to connect to system bus: %s\n", strerror(-r));
                goto finish;
        }

        /* Install the object */
        r = sd_bus_add_object_vtable(bus,
                                     &slot,
                                     "/net/acm/blocks",  /* object path */
                                     "net.acm.blocks",   /* interface name */
                                     blocks_vtable,
                                     NULL);
        if (r < 0) {
                fprintf(stderr, "Failed to issue method call: %s\n", strerror(-r));
                goto finish;
        }

        /* Take a well-known service name so that clients can find us */
        r = sd_bus_request_name(bus, "net.acm.blocks", 0);
        if (r < 0) {
                fprintf(stderr, "Failed to acquire service name: %s\n", strerror(-r));
                goto finish;
        }

        for (;;) {
                /* Process requests */
                r = sd_bus_process(bus, NULL);
                if (r < 0) {
                        fprintf(stderr, "Failed to process bus: %s\n", strerror(-r));
                        goto finish;
                }
                if (r > 0) /* we processed a request, try to process another one, right-away */
                        continue;

                /* Wait for the next request to process */
                r = sd_bus_wait(bus, (uint64_t) -1);
                if (r < 0) {
                        fprintf(stderr, "Failed to wait on bus: %s\n", strerror(-r));
                        goto finish;
                }
        }


finish:
        sd_bus_slot_unref(slot);
        sd_bus_unref(bus);

        return(NULL);
}


