/*************************************************************************************
  (C) Copyright (2012-2021) Hewlett PAckard Enterprise Development LP
\*************************************************************************************
File: acm_poll.c


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
#include "acm_extra.h"       

extern void *sd_thread(void *vargp);
extern void *pwm_thread(void *vargp);

ACM_BLOCKS acm_blks;

ACM_BLK_INDX acm_idx[] = 
{
	{MULTI_PACKET, &acm_blks.block_0, sizeof(ACM_BLOCK_0)},  //BLK 0
	{SINGLE_PACKET, &acm_blks.block_1, sizeof(ACM_BLOCK_1)}, //blk 1
	{MULTI_PACKET, &acm_blks.block_2, sizeof(ACM_BLOCK_2)}, //blk 2
	{SINGLE_PACKET, &acm_blks.block_3, sizeof(ACM_BLOCK_3)}, //blk 3
	{SINGLE_PACKET, &acm_blks.block_4, sizeof(ACM_BLOCK_4)}, //blk 4
	{SINGLE_PACKET, &acm_blks.block_5, sizeof(ACM_BLOCK_5)}, //blk 5
	{SINGLE_PACKET, &acm_blks.block_6, sizeof(ACM_BLOCK_6)}, //blk 6
	{MULTI_PACKET, &acm_blks.block_7, sizeof(ACM_BLOCK_7)}, //blk 7
	{MULTI_PACKET, &acm_blks.block_8, sizeof(ACM_BLOCK_8)}, //blk 8
	{SINGLE_PACKET, &acm_blks.block_9, sizeof(ACM_BLOCK_9)}, //blk 9
	{SINGLE_PACKET, &acm_blks.block_A, sizeof(ACM_BLOCK_A)}, //blk a
	{MULTI_PACKET, &acm_blks.block_B, sizeof(ACM_BLOCK_B)}, //blk b
	{MULTI_PACKET, &acm_blks.block_C, sizeof(ACM_BLOCK_C)}, //blk c
	{INVALID_PKT,NULL,0}, //blk d
	{INVALID_PKT,NULL,0}, //blk e
	{INVALID_PKT,NULL,0}, //blk f
	{INVALID_PKT,NULL,0},  //BLK 10
	{INVALID_PKT,NULL,0}, //blk 11
	{INVALID_PKT,NULL,0}, //blk 12
	{INVALID_PKT,NULL,0}, //blk 13
	{INVALID_PKT,NULL,0}, //blk 14
	{MULTI_PACKET, &acm_blks.block_15, sizeof(ACM_BLOCK_15)}, //blk 15
	{INVALID_PKT,NULL,0}, //blk 16
	{INVALID_PKT,NULL,0}, //blk 17
	{INVALID_PKT,NULL,0}, //blk 18
	{INVALID_PKT,NULL,0}, //blk 19
	{INVALID_PKT,NULL,0}, //blk 1a
	{INVALID_PKT,NULL,0}, //blk 1b
	{INVALID_PKT,NULL,0}, //blk 1c
	{INVALID_PKT,NULL,0}, //blk 1d
	{INVALID_PKT,NULL,0}, //blk 1e
	{INVALID_PKT,NULL,0}, //blk 1f
	{SINGLE_PACKET, &acm_blks.block_20, sizeof(ACM_BLOCK_20)},//blk 20
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{MULTI_PACKET, &acm_blks.block_23, sizeof(ACM_BLOCK_23)},
	{SINGLE_PACKET, &acm_blks.block_24, sizeof(ACM_BLOCK_24)},
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{MULTI_PACKET, &acm_blks.block_2F, sizeof(ACM_BLOCK_2F)},
	{SINGLE_PACKET, &acm_blks.block_30, sizeof(ACM_BLOCK_30)},//Blk 30
	{MULTI_PACKET, &acm_blks.block_31, sizeof(ACM_BLOCK_31)},
	{MULTI_PACKET, &acm_blks.block_32, sizeof(ACM_BLOCK_32)},
	{SINGLE_PACKET, &acm_blks.block_33, sizeof(ACM_BLOCK_33)},
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{SINGLE_PACKET, &acm_blks.block_36, sizeof(ACM_BLOCK_36)},
	{MULTI_PACKET, &acm_blks.block_37, sizeof(ACM_BLOCK_37)},
	{MULTI_PACKET, &acm_blks.block_38, sizeof(ACM_BLOCK_38)},
	{MULTI_PACKET, &acm_blks.block_39, sizeof(ACM_BLOCK_39)},
	{MULTI_PACKET, &acm_blks.block_3A, sizeof(ACM_BLOCK_3A)},
	{SINGLE_PACKET, &acm_blks.block_3B, sizeof(ACM_BLOCK_3B)},
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{SINGLE_PACKET, &acm_blks.block_40, sizeof(ACM_BLOCK_40)},//BLK 40
	{MULTI_PACKET, &acm_blks.block_41, sizeof(ACM_BLOCK_41)},
	{SINGLE_PACKET, &acm_blks.block_42, sizeof(ACM_BLOCK_42)},
	{MULTI_PACKET, &acm_blks.block_43, sizeof(ACM_BLOCK_43)},
	{SINGLE_PACKET, &acm_blks.block_44, sizeof(ACM_BLOCK_44)},
	{SINGLE_PACKET, &acm_blks.block_45, sizeof(ACM_BLOCK_45)},
	{SINGLE_PACKET, &acm_blks.block_46, sizeof(ACM_BLOCK_46)},
	{SINGLE_PACKET, &acm_blks.block_47, sizeof(ACM_BLOCK_47)},
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0},   //blk 50
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0},   //blk 60
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, //BLK 70
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0},  //blk 80  
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0},  //blk 90 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0},   //blk a0
	{MULTI_PACKET, &acm_blks.block_A1, sizeof(ACM_BLOCK_A1)},
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0},
	{MULTI_PACKET, &acm_blks.block_B0_BF[0], sizeof(ACM_BLOCK_B0_BF)}, //blk b0
	{MULTI_PACKET, &acm_blks.block_B0_BF[1], sizeof(ACM_BLOCK_B0_BF)},
	{MULTI_PACKET, &acm_blks.block_B0_BF[2], sizeof(ACM_BLOCK_B0_BF)},
	{MULTI_PACKET, &acm_blks.block_B0_BF[3], sizeof(ACM_BLOCK_B0_BF)},
	{MULTI_PACKET, &acm_blks.block_B0_BF[4], sizeof(ACM_BLOCK_B0_BF)},
	{MULTI_PACKET, &acm_blks.block_B0_BF[5], sizeof(ACM_BLOCK_B0_BF)},
	{MULTI_PACKET, &acm_blks.block_B0_BF[6], sizeof(ACM_BLOCK_B0_BF)},
	{MULTI_PACKET, &acm_blks.block_B0_BF[7], sizeof(ACM_BLOCK_B0_BF)},
	{MULTI_PACKET, &acm_blks.block_B0_BF[8], sizeof(ACM_BLOCK_B0_BF)},
	{MULTI_PACKET, &acm_blks.block_B0_BF[9], sizeof(ACM_BLOCK_B0_BF)},
	{MULTI_PACKET, &acm_blks.block_B0_BF[10], sizeof(ACM_BLOCK_B0_BF)},
	{MULTI_PACKET, &acm_blks.block_B0_BF[11], sizeof(ACM_BLOCK_B0_BF)},
	{MULTI_PACKET, &acm_blks.block_B0_BF[12], sizeof(ACM_BLOCK_B0_BF)},
	{MULTI_PACKET, &acm_blks.block_B0_BF[13], sizeof(ACM_BLOCK_B0_BF)}, 
	{MULTI_PACKET, &acm_blks.block_B0_BF[14], sizeof(ACM_BLOCK_B0_BF)}, 
	{MULTI_PACKET, &acm_blks.block_B0_BF[15], sizeof(ACM_BLOCK_B0_BF)},
	{MULTI_PACKET, &acm_blks.block_C0, sizeof(ACM_BLOCK_C0)}, //blk c0
	{MULTI_PACKET, &acm_blks.block_C1, sizeof(ACM_BLOCK_C1)},
	{MULTI_PACKET, &acm_blks.block_C2, sizeof(ACM_BLOCK_C2)}, 
	{MULTI_PACKET, &acm_blks.block_C3, sizeof(ACM_BLOCK_C3)}, 
	{MULTI_PACKET, &acm_blks.block_C4, sizeof(ACM_BLOCK_C4)}, 
	{MULTI_PACKET, &acm_blks.block_C5, sizeof(ACM_BLOCK_C5)}, 
	{SINGLE_PACKET, &acm_blks.block_C6, sizeof(ACM_BLOCK_C6)}, 
	{SINGLE_PACKET, &acm_blks.block_C7, sizeof(ACM_BLOCK_C7)}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0},  //blk d0
	{MULTI_PACKET, &acm_blks.block_D1, sizeof(ACM_BLOCK_D1)},
	{MULTI_PACKET, &acm_blks.block_D2, sizeof(ACM_BLOCK_D2)}, 
	{MULTI_PACKET, &acm_blks.block_D3, sizeof(ACM_BLOCK_D3)}, 
	{SINGLE_PACKET, &acm_blks.block_D4, sizeof(ACM_BLOCK_D4)}, 
	{INVALID_PKT,NULL,0}, 
	{SINGLE_PACKET, &acm_blks.block_D6, sizeof(ACM_BLOCK_D6)},
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{SINGLE_PACKET, &acm_blks.block_E0, sizeof(ACM_BLOCK_E0)},  //blk e0
	{SINGLE_PACKET, &acm_blks.block_E1, sizeof(ACM_BLOCK_E1)}, 
	{SINGLE_PACKET, &acm_blks.block_E2, sizeof(ACM_BLOCK_E2)}, 
	{SINGLE_PACKET, &acm_blks.block_E3, sizeof(ACM_BLOCK_E3)}, 
	{SINGLE_PACKET, &acm_blks.block_E4, sizeof(ACM_BLOCK_E4)}, 
	{SINGLE_PACKET, &acm_blks.block_E5, sizeof(ACM_BLOCK_E5)}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{SINGLE_PACKET, &acm_blks.block_F0, sizeof(ACM_BLOCK_F0)},  //blk f0
	{SINGLE_PACKET, &acm_blks.block_F1, sizeof(ACM_BLOCK_F1)}, 
	{SINGLE_PACKET, &acm_blks.block_F2, sizeof(ACM_BLOCK_F2)}, 
	{SINGLE_PACKET, &acm_blks.block_F3, sizeof(ACM_BLOCK_F3)}, 
	{SINGLE_PACKET, &acm_blks.block_F4, sizeof(ACM_BLOCK_F4)}, 
	{MULTI_PACKET, &acm_blks.block_F5, sizeof(ACM_BLOCK_F5)},
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{INVALID_PKT,NULL,0}, 
	{SINGLE_PACKET, &acm_blks.block_100, sizeof(ACM_BLOCK_100)}, 
	{MULTI_PACKET, &acm_blks.block_101, sizeof(ACM_BLOCK_101)} ,
	{SINGLE_PACKET, &acm_blks.block_102, sizeof(ACM_BLOCK_102)} ,
	{MULTI_PACKET, &acm_blks.block_103, sizeof(ACM_BLOCK_103)} ,
	{MULTI_PACKET, &acm_blks.block_104, sizeof(ACM_BLOCK_104)} ,
	{SINGLE_PACKET, &acm_blks.block_105, sizeof(ACM_BLOCK_105)} 
};


/*


int acm_req(unsigned char *data)
{
    CMD_REQ req;
    unsigned char tmp;
    char *ptr;
    long converted;
    
    converted = strtol(data, &ptr, 16); 
    printf("Converted: %lx\n",converted);
    memset(&req, 0, sizeof(req)); 
    memcpy(&req, &converted, sizeof(req)); 
    //Evaluate the incoming data for real request
    //First 16bits are big endian size 
    //byte swap sizes
    printf("req size big: %x\n", req.cmd_size.big_end_req_sz);
    tmp = req.cmd_size.req_size[0];
    req.cmd_size.req_size[0] = req.cmd_size.req_size[1];
    req.cmd_size.req_size[1] = tmp;
    printf("req size little: %x\n", req.cmd_size.big_end_req_sz);
    if((req.cmd_size.big_end_req_sz < 0)  || (req.cmd_size.big_end_req_sz > 512))
    {
	//bad cmd
        printf("Invalid command size: % x\n", req.cmd_size.big_end_req_sz);
    }
    
    //Next byte is the command instruction(1-rd, 2-wr)
    if((req.instruction < 1) || (req.instruction > 2))
    {
        printf("Bad req instruction: %x\n", req.instruction);
    }
    printf("Instruction: %x\n", req.instruction);

    printf("block number big: %x\n", req.block_num.big_end_blk_num);
    tmp = req.block_num.blk_num[0];
    req.block_num.blk_num[0] = req.block_num.blk_num[1];
    req.block_num.blk_num[1] = tmp;
    printf("block number little: %x\n", req.block_num.big_end_blk_num);
    if((req.block_num.big_end_blk_num < 0) || (req.block_num.big_end_blk_num > 105))
    {
	printf("block num error in request\n");
    }
    
    printf("Block data: %s\n", req.data);
}
*/

/*
    Send block data to ACM.
    This can be used as part of a read request from the ACM or a new request to send information to them
*/
int acm_rd_req(unsigned short blk_num)
{
    int fp, sz;
    unsigned short size;
    CMD_REQ req;

    if(blk_num <= ACM_MAX_BLOCK_NUM)
    {
        //Get acm block data and send to acm.
        req.block_num.blk_num[0] = (blk_num & 0xff00) >> 8;
        req.block_num.blk_num[1] = blk_num & 0x00ff;
        req.cmd_size.req_size[0] = ((acm_idx[blk_num].blk_size + 2) & 0xff00) >> 8;  //plus block num(2)
        req.cmd_size.req_size[1] = (acm_idx[blk_num].blk_size + 2) & 0x00ff;
        req.instruction = 0x2;  //Write
        memcpy(req.data, acm_idx[blk_num].blk_ptr, acm_idx[blk_num].blk_size);
        //printf("We got a read request. Send data to acm. Block: %x, size(big_endian): %x, data: %x %x %x %x\n", blk_num, req.cmd_size.big_end_req_sz,
        //           req.data[0], req.data[1], req.data[2], req.data[3]);

        //Do file write to acm_out with requested data
        fp = open(ACM_OUT, O_RDWR);
        if(fp >= 0)
        {
            //We can send data
            sz = write(fp, (void *)&req, acm_idx[blk_num].blk_size + 2 + 3); //cmd hdr + block num + block data
            //printf("We wrote %x bytes to ACM driver to send out(read request)\n", sz);
            close(fp);
            return(0);
        }    
    }
    return(1);
}
/*
     Incoming data from ACM.
     Triggered by poll() on out file
*/
void acm_wr_req(unsigned short blk_num, unsigned short sz, unsigned char *buff)
{
    unsigned char out[512];

    //Update acm block data
    //printf("acm Write req to iLO from acm-blk: %x, sz: %x, data: %x %x %x %x\n", blk_num, sz, buff[0], buff[1], buff[2], buff[3]);
    if(blk_num <= ACM_MAX_BLOCK_NUM)
        memcpy(acm_idx[blk_num].blk_ptr, buff, (acm_idx[blk_num].blk_size > sz) ? sz : acm_idx[blk_num].blk_size);    
}


#define PING_BLK 8
void acm_ping_init(void)
{
    PING_DATA ping={0,4,{5,1,0xc8,0x22},1,17,{'V','e','r','s','i','o','n',' ','1','.','0','.','0','0','0','0','0'},2,11,{'H','P','E',' ','o','p','e','n','B','M','C'}};
        
    memcpy(acm_idx[PING_BLK].blk_ptr, &ping, sizeof(PING_DATA));
    acm_rd_req(PING_BLK);
}


/*
    Poll() the sysfs file waiting for new block data to show up.
    If valid command found process request.
    Block writes will update the local database.
    Reads will cause a separate write of the requested data. 
*/
void *poll_thread(void *vargp)
{
       CMD_REQ req;
       int input_fd, sz;
       int ret;
       unsigned char buff[512];
       char *ptr;
       int indx;
       struct thread_info *tinfo = vargp;
       char data[4]={0,0,0,0};


	printf("Poll thread starting!!!!!!!!!!!!!!!!!!!!!!!!\n");

       
       do
       {
            input_fd = open(ACM_IN, O_RDWR);
            if(input_fd < 0)
            {
                sleep(2);
            }
            printf("Opening sysfs input interface for polling: %s, r: %x\n",ACM_IN, input_fd);
       }while(input_fd < 0); //wait forever for the file to show up.  Driver provides it

       //wait until we get a file so we know the driver is loaded
       acm_ping_init();   
       
       for(;;)
       {
                memset(buff, 0, sizeof(buff));


                sz = read(input_fd, buff, sizeof(buff));  //read whole buffer in to process
                indx = 0;

                //printf("Poll Received ACM request for %x bytes\n", sz);
                if(sz >= 5)
                {   
                    //Take first two bytes out of buffer as the block size + block num(2 bytes)
                    //This data is also big endian we are converting it here since we need little
                    req.cmd_size.req_size[1] = buff[indx++];
                    req.cmd_size.req_size[0] = buff[indx++];
                    if(req.cmd_size.big_end_req_sz + 3 == sz)
                    { //do size check on data read compared to size in command header + 3 for header
                        req.instruction = buff[indx++];
                        //printf("Read %x bytes, inst: %x, req size: %x\n", sz, req.instruction, req.cmd_size.big_end_req_sz);
                        //We have inst and block size.  Check if valid
                        switch(req.instruction)
                        {
                                case 1:
                                    //Read
                                    req.block_num.blk_num[1] = buff[indx++];  //endian swap
                                    req.block_num.blk_num[0] = buff[indx++];

                                    //printf("Read request for block_num: %x\n",req.block_num.big_end_blk_num);
                                    //process read request
                                    acm_rd_req(req.block_num.big_end_blk_num);
//                                    sz = read(input_fd, data, 1);  //read 1 extra byte
//                                    lseek(input_fd, 0, SEEK_SET);
//                                    write(input_fd, (void *)data, sizeof(data)); //clear out old cmd
                                    break;
                                case 2:
                                    //Write
                                    req.block_num.blk_num[1] = buff[indx++];  //endian swap
                                    req.block_num.blk_num[0] = buff[indx++];

                                    //printf("Write request for blk_num: %x, size %x\n",
                                    //       req.block_num.big_end_blk_num, req.cmd_size.big_end_req_sz);
                                    ptr = &buff[indx];
                                    
                                    acm_wr_req(req.block_num.big_end_blk_num, req.cmd_size.big_end_req_sz-2, ptr);
    //                                sz = read(input_fd, data, 1);  //read 1 extra byte
     //                               lseek(input_fd, 0, SEEK_SET);
      //                              write(input_fd, (void *)data, sizeof(data));
                                    break;
                                default:
                                    printf("Invalid instruction: %x\n",req.instruction);
                                    break;               
                        }
                    }
                    else
                    {   
                        //Error in size.
                        printf("Size error reading block.  Read %x bytes, size: %x\n", sz, req.cmd_size.big_end_req_sz);
                        if(input_fd >= 0)
                        {
                           close(input_fd);
                        }
                        
                        do
                        {
                             input_fd = open(ACM_IN, O_RDWR);
                             if(input_fd < 0)
                             {
                                 sleep(2);
                             }
                             printf("Opening sysfs input interface for polling: %s, r: %x\n",ACM_IN, input_fd);
                        }while(input_fd < 0); //wait forever for the file to show up.  Driver provides it
                        sleep(1);
                    }
                }
                sz = read(input_fd, data, sizeof(data));  //read  4 extra bytes to cause interrupt to driver saying we are ready for more data.
                lseek(input_fd, 0, SEEK_SET);
                if(sz < 5)  //a command should be atleast 5 bytes
                {
                    sleep(1);
                }
       }

    if(input_fd)
       close(input_fd);
    return(NULL);
}

int main(void)
{
    pthread_t poll_tid, sd_tid, pwm_tid;
    int ret;
    void *res;
    struct thread_info *tinfo = calloc(2, sizeof(*tinfo));

    if(tinfo == NULL)
    {
      printf("Failed to alloc tinfo\n");
      exit(0);
    }
    
    tinfo[0].thread_num = 1;
    tinfo[0].blocks = NULL;
    ret = pthread_create(&poll_tid, NULL, poll_thread, &tinfo[0]);
    if(ret != 0)
        printf("Failed to create polling thread\n");
  

    printf("Creating Thread for sd_bus polling.  \n");
    tinfo[1].thread_num = 2;
    tinfo[1].blocks = &acm_idx[0];
    ret = pthread_create(&sd_tid, NULL, sd_thread, &tinfo[1]);
    if(ret != 0)
        printf("Failed to create sd_bus thread\n");
    

    printf("Creating Thread for pwm polling.\n");
    tinfo[1].thread_num = 3;
    tinfo[1].blocks = NULL;
    ret = pthread_create(&pwm_tid, NULL, pwm_thread, &tinfo[1]);
    if(ret != 0)
        printf("Failed to create pwm thread\n");
    
    ret = pthread_join(poll_tid, &res);
    if(ret != 0)
      printf("Failed join of polling thread\n");
    ret = pthread_join(sd_tid, &res);
    if(ret != 0)
       printf("Failed join of sd_bus thread\n");
    ret = pthread_join(pwm_tid, &res);
    if(ret != 0)
       printf("Failed join of pwm thread\n");

    free(tinfo);

    exit(0);
}


