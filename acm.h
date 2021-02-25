/* acm.h
 *
 * Copyright 2016 Hewlett Packard Enterprise Development, LP
 *
 * Hewlett-Packard and the Hewlett-Packard logo are trademarks of
 * Hewlett-Packard Development Company, L.P. in the U.S. and/or other countries.
 *
 * Confidential computer software. Valid license from Hewlett Packard Enterprise is required for
 * possession, use or copying. Consistent with FAR 12.211 and 12.212,
 * Commercial Computer Software, Computer Software Documentation, and Technical
 * Data for Commercial Items are licensed to the U.S. Government under
 * vendor's standard commercial license.
 */
#ifndef ACM_H
#define ACM_H

#define TIMEOUT 5
#define ACM_IN "/sys/bus/i2c/devices/3-1008/gxp_acm_in"
#define ACM_OUT "/sys/bus/i2c/devices/3-1008/gxp_acm_out"
#define ACM_PWM_IN "/sys/class/hwmon/hwmon"  //This will be the base search for x/pwm0
#define ACM_PWM_FILE "/pwm0"
#define ACM_FAN_PWM_BLK 1
#define ACM_MAX_BLOCK_NUM 0x105



//extern void *pwm_thread(void *vargp);

typedef struct
{
    unsigned int valid_blks[34];         //bit map of updated blocks.  Up to block 0x40f in bits
}ACM_BLOCK_0;  //This block is for iLO to track stats on block usage


typedef struct
{
    unsigned char fan_pwms[8];
}ACM_BLOCK_1;  //fan pwms to acm

typedef struct
{
    unsigned char fan_cnt;
    unsigned char fan_status[16];
    unsigned char fan_speed[16]; //0-100%
    unsigned char fan_rotors[16];  //rev 1.11
    unsigned char fan_rotor_status[16];
}ACM_BLOCK_2;  //fan status

typedef struct
{
    unsigned char fan_override; 
}ACM_BLOCK_3;  //fan speed override  pwm to acm

typedef struct
{
    unsigned int chassis_total_pwr; 
}ACM_BLOCK_4;  //chassis pwr in 32 bit big endian value

typedef struct
{
    unsigned short present; //1bit per pwr supply
    unsigned short status;  //failed
    unsigned short mismatch;
    unsigned short supply_ready;
    unsigned short supply_input_ok;
    unsigned short gen_fail_status;
    unsigned short supply_enable_status;
    unsigned short input_voltage[16];
}ACM_BLOCK_5;  

typedef struct
{
    unsigned short chassis_12v_rail_mv;
    unsigned short total_input_pwr_avg; 
    unsigned short total_output_pwr_avg;
    unsigned short rolling_avg_input_pwr;
    unsigned short rolling_avg_output_pwr;
    unsigned short input_peak_pwr;
    unsigned short output_peak_pwr;
}ACM_BLOCK_6;  

typedef struct
{
    unsigned char iPDU[128];
}ACM_BLOCK_7; 


typedef struct
{
    unsigned char type0;
    unsigned char type0_len;
    unsigned char type0_ver[4];
    unsigned char type1;
    unsigned char type1_len;
    unsigned char type1_ver[17];
    unsigned char type2;
    unsigned char type2_len;
    unsigned char type2_name[11];
}PING_DATA;

typedef struct
{
/*    union
    {
        unsigned char data[128];
        ACM_PING_DATA acm_ping;
    }acm_ping_data;
*/
    union
    {
        PING_DATA ilo_ping;
        unsigned char data[48];
    }ilo_ping_data;
}ACM_BLOCK_8;  //ping

typedef struct
{
    unsigned char fw_update;           //fw update pending    
}ACM_BLOCK_9;  

typedef struct
{
    unsigned char chassis_asset_tag[32]; 
}ACM_BLOCK_A;  

typedef struct
{
    unsigned char  chassis_fru[256];
}ACM_BLOCK_B;  

typedef struct
{
    unsigned char url[256];           //APM SSO url   
}ACM_BLOCK_C;  






//These are not in current acm spec, through 0x1A
typedef struct
{
    unsigned short pic_family;
    unsigned short tray_id;
    unsigned short bay_id;
    unsigned short fw_rev;
    unsigned char  tray_fru[256];
}ACM_BLOCK_15; 


//block 20 is the firmware upgrade ping to acm and the only thing in the request is block number 0x20 no data.
//The request from the flash engine will include the file number of the buffer we will open it and get a handle
typedef struct
{
    int file_num; //int
    int file_handle; //DLBUF handle
}ACM_BLOCK_20; 

typedef struct
{
    unsigned short bytes;
    unsigned short offset;
    unsigned char percent;
    unsigned char resv;
    unsigned char data[1024];
}ACM_BLOCK_23; 


typedef struct
{
    unsigned char status; //0 ok
}ACM_BLOCK_24; 


typedef struct
{
    unsigned char data[256]; //Free format data for Moonshot team to do whatever they like
}ACM_BLOCK_2F; 




//0x30 -0x3f Undefined power regulation
typedef struct
{
    unsigned char pwr_reg_enable;
    unsigned char eeprom_enable;
    unsigned char pwr_reg_mode;
}ACM_BLOCK_30;
typedef struct
{
    unsigned char pwr_reg_zone_cfg[256]; 
}ACM_BLOCK_31; 
typedef struct
{
    unsigned char pwr_req_set_point_cfg[160]; 
}ACM_BLOCK_32; 
typedef struct
{
    unsigned char cal_start;
    unsigned char eeprom_save;
    unsigned char msb_num_secs;
    unsigned char lsb_num_secs;
    unsigned char cal_scope;
    unsigned char cal_all_zone;
    unsigned char cal_time[8];
}ACM_BLOCK_33; 
typedef struct
{
    unsigned char cal_scope;
}ACM_BLOCK_36;
typedef struct
{
    unsigned char cal_status_resp[64];
}ACM_BLOCK_37;
typedef struct
{
    unsigned char node_pwm_status[160];
}ACM_BLOCK_38; 
typedef struct
{
    unsigned char setpoint[256];
}ACM_BLOCK_39; 
typedef struct
{
    unsigned char setpoint_status[160];
}ACM_BLOCK_3A;
typedef struct
{
    unsigned char cal_status;
    unsigned char cal_scope;
}ACM_BLOCK_3B;

//0x40 - 0x47 
typedef struct
{
    unsigned char PXE_MAC_ADDR[3][6];             // to acm
}ACM_BLOCK_40; 
typedef struct
{
    unsigned char asset_tag[256];           //server asset tag    
}ACM_BLOCK_41; 
typedef struct
{
    unsigned char ilo_ip4v_addr[4];           //ilo ip addr    to acm
    unsigned char ilo_ip4v_mask[4];          //ip mask???    
    unsigned char mac_addr[6];                //iLO MAC addr
}ACM_BLOCK_42; 

typedef struct
{
    unsigned char  tray_fru[512];
}ACM_BLOCK_43; 

typedef struct
{
    unsigned short punative_cap;               
}ACM_BLOCK_44; 
typedef struct
{
    unsigned short dc_watts_avg_pwr_cnt;
    unsigned short dc_peak_pwr_rd;
    unsigned short dc_pwr_acc_msw; //big endian
    unsigned short dc_pwr_acc_lsw;  //big_endian
}ACM_BLOCK_45; 
typedef struct
{
    unsigned short dc_watts; //node power  big endian             
}ACM_BLOCK_46; 
typedef struct
{
    unsigned short avg_pwr;    //  avg power      big endian     
    unsigned short avg_peak;    // avg peak      big endian     
    unsigned short rolling_avg_pwr;    // 1 min rolling avg power      big endian     
}ACM_BLOCK_47; 


//0xA1 IML log
typedef struct
{
    unsigned char iml_entry[256];
}ACM_BLOCK_A1; 

//B0-CF power supply fru undefined
typedef struct
{
    unsigned char  pwr_fru[256];
}ACM_BLOCK_B0_BF; 

//C0-C7 Megacell Undefined
typedef struct
{
    unsigned char id;
    unsigned char bay;
    unsigned char fru[256];
}ACM_BLOCK_C0; 
typedef struct
{
    unsigned char id;
    unsigned char bay;
    unsigned char megacell_regs[256];
}ACM_BLOCK_C1; 
typedef struct
{
    unsigned char megacell_regs_req[64];
}ACM_BLOCK_C2; 
typedef struct
{
    unsigned char megacell_regs_resp[256];
}ACM_BLOCK_C3; 
typedef struct
{
    unsigned char id;
    unsigned char bay;
    unsigned char megacell_data_req[62];
}ACM_BLOCK_C4; 
typedef struct
{
    unsigned char megacell_data_resp[256];
}ACM_BLOCK_C5; 
typedef struct
{
    unsigned char id;
    unsigned char bay;
    unsigned char data_lsb;
    unsigned char data_msb;
}ACM_BLOCK_C6; 

typedef struct
{
    unsigned char id;
    unsigned char bay;
    unsigned char temp_lsb;
    unsigned char temp_msb;
}ACM_BLOCK_C7; 

//D0-D5 HD Zoning not snap 1
typedef struct
{
    unsigned char backplane_info[256];    // hard drive backplane info.     
}ACM_BLOCK_D1; 
typedef struct
{
    unsigned char zone_table[66];    // hard drive current zone table   
}ACM_BLOCK_D2; 
typedef struct
{
    unsigned char zone_table[66];    // hard drive pending zone table (Type ID + SPE Node ID + Bays Data)
}ACM_BLOCK_D3; 
typedef struct
{
    unsigned char reset_defaults[4];    // reset to factory defaults     
}ACM_BLOCK_D4; 
typedef struct
{
    unsigned char hdbp_id;    // hard drive backplane ID.     
    unsigned char resv[3];
}ACM_BLOCK_D6; 


//E0 - E2 power supply temps
typedef struct
{
    unsigned char count;    //number of power supply entries to follow
    unsigned short inlet_temp[16];
}ACM_BLOCK_E0;

typedef struct
{
    unsigned char count;  //num of supply entries to follow
    unsigned short temp[16];  //internal temp
}ACM_BLOCK_E1; 

typedef struct
{
    unsigned char count;
    unsigned short watts[16];  //power supply total output power of 12V main.  reg 0x22 in power supply
}ACM_BLOCK_E2; 

typedef struct
{
    unsigned short input_pwr;  //total input pwr supply capacity ac watts
    unsigned short output_pwr;  //total power supply output power dc watts
}ACM_BLOCK_E3; 

typedef struct
{
    unsigned char pwr_supply_cnt;  //total input pwr supplys
    unsigned short inlet_temp[16];  
}ACM_BLOCK_E4; 

typedef struct
{
    unsigned char pwr_supply_cnt;  //total input pwr supplys
    unsigned short highest_temp[16];  
}ACM_BLOCK_E5; 

typedef struct
{
    unsigned char acm_temp; 
    unsigned char resv[3];
}ACM_BLOCK_F0; 

typedef struct
{
    unsigned char ambient_temp;    
    unsigned char resv[3];
}ACM_BLOCK_F1; 

typedef struct
{
    unsigned char hdd_backplane_temp;    
    unsigned char resv[3];
}ACM_BLOCK_F2; 

typedef struct
{
    unsigned char SEP_temp;    
    unsigned char resv[3];
}ACM_BLOCK_F3; 

typedef struct
{
    unsigned char hdd_efuse_temp;    
    unsigned char resv[3];
}ACM_BLOCK_F4; 

typedef struct
{
    unsigned char data[256];
}ACM_BLOCK_F5; 
typedef struct
{
    unsigned int nodes;
}ACM_BLOCK_100; 
typedef struct
{
    unsigned char fw_component_change[300];
}ACM_BLOCK_101; 
typedef struct
{
    unsigned char chassis_sn[10];
    unsigned char chassis_pn[10];
}ACM_BLOCK_102; 
typedef struct
{
    unsigned char CM_Creds[512];
}ACM_BLOCK_103; 
typedef struct
{
    unsigned char CM_Network[287];
}ACM_BLOCK_104; 
typedef struct
{
    unsigned int request_id;
    unsigned char priv_bit_maks[4];//Big endian order from acm for some reasons.
}ACM_BLOCK_105; 
#pragma pack()

/*

    NOTICE - If you add or remove any blocks then you also have to update the table(acm_idx) in acm.c

*/
typedef struct
{
    ACM_BLOCK_0     block_0;
    ACM_BLOCK_1     block_1;
    ACM_BLOCK_2     block_2;
    ACM_BLOCK_3     block_3;
    ACM_BLOCK_4     block_4;
    ACM_BLOCK_5     block_5;
    ACM_BLOCK_6     block_6;
    ACM_BLOCK_7     block_7;
    ACM_BLOCK_8     block_8;
    ACM_BLOCK_9     block_9;
    ACM_BLOCK_A     block_A;
    ACM_BLOCK_B     block_B;
    ACM_BLOCK_C     block_C;
    
    ACM_BLOCK_15   block_15;
    
    ACM_BLOCK_20   block_20;
    ACM_BLOCK_23   block_23;
    ACM_BLOCK_24   block_24;
    ACM_BLOCK_2F   block_2F;  //special for moonshot
    ACM_BLOCK_30   block_30;
    ACM_BLOCK_31   block_31;
    ACM_BLOCK_32   block_32;
    ACM_BLOCK_33   block_33;
    ACM_BLOCK_36   block_36;
    ACM_BLOCK_37   block_37;
    ACM_BLOCK_38   block_38;
    ACM_BLOCK_39   block_39;
    ACM_BLOCK_3A   block_3A;
    ACM_BLOCK_3B   block_3B;
    ACM_BLOCK_40   block_40;
    ACM_BLOCK_41   block_41;
    ACM_BLOCK_42   block_42;
    ACM_BLOCK_43   block_43;
    ACM_BLOCK_44   block_44;
    ACM_BLOCK_45   block_45;
    ACM_BLOCK_46   block_46;
    ACM_BLOCK_47   block_47;
    ACM_BLOCK_A1   block_A1;
    ACM_BLOCK_B0_BF   block_B0_BF[16]; //16 pwr supply frus
    ACM_BLOCK_C0   block_C0;
    ACM_BLOCK_C1   block_C1;
    ACM_BLOCK_C2   block_C2;
    ACM_BLOCK_C3   block_C3;
    ACM_BLOCK_C4   block_C4;
    ACM_BLOCK_C5   block_C5;
    ACM_BLOCK_C6   block_C6;
    ACM_BLOCK_C7   block_C7;
    ACM_BLOCK_D1   block_D1;
    ACM_BLOCK_D2   block_D2;
    ACM_BLOCK_D3   block_D3;
    ACM_BLOCK_D4   block_D4;
    ACM_BLOCK_D6   block_D6;
    ACM_BLOCK_E0   block_E0;
    ACM_BLOCK_E1   block_E1;
    ACM_BLOCK_E2   block_E2;
    ACM_BLOCK_E3   block_E3;
    ACM_BLOCK_E4   block_E4;
    ACM_BLOCK_E5   block_E5;
    ACM_BLOCK_F0   block_F0;
    ACM_BLOCK_F1   block_F1;
    ACM_BLOCK_F2   block_F2;
    ACM_BLOCK_F3   block_F3;
    ACM_BLOCK_F4   block_F4;
    ACM_BLOCK_F5   block_F5;
    ACM_BLOCK_100   block_100;
    ACM_BLOCK_101   block_101;
    ACM_BLOCK_102   block_102;
    ACM_BLOCK_103   block_103;
    ACM_BLOCK_104   block_104;
    ACM_BLOCK_105   block_105;
}ACM_BLOCKS;



//Block types
#define SINGLE_PACKET      0   //Single packet
#define MULTI_PACKET        1    //Multi-packet
#define INVALID_PKT   -1  //Unsupported

typedef struct
{
	int type;
	void *blk_ptr;
        unsigned int blk_size;
}ACM_BLK_INDX;

struct thread_info {
    pthread_t thread_id;
    int       thread_num;
    ACM_BLK_INDX  *blocks;
};


typedef enum
{
   ACM_BLOCK_RD = 1,
   ACM_BLOCK_WR = 2
}ACM_CMD;

#pragma pack(1)
typedef struct
{
    union
    {
	unsigned char req_size[2];
        unsigned short int big_end_req_sz;
    }cmd_size;
    unsigned char instruction;
    union
    {
	unsigned char blk_num[2];
        unsigned short int big_end_blk_num;
    }block_num;
    unsigned char data[512];  // rounds out to 8 bytes/long long
}CMD_REQ;
#pragma pack()



#endif
