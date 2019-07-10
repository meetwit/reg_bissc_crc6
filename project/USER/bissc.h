#ifndef __BISSC_MW_H
#define __BISSC_MW_H

#include <stdio.h> 
#include <stdlib.h>
#include "xmc4800.h"

struct bissc{
	uint8_t head;			//  xbit 
	uint8_t start;		//  1bit
	uint8_t csd;			//  1bit
	uint32_t data;		// 20bit
	uint8_t zero;			//  6bit
	uint8_t err;			//  1bit
	uint8_t war;			//  1bit
	uint8_t crc;			//  6bit
	uint8_t foot;			//  ybit
	
	uint8_t RxPacket[5];		//数据接收
	uint8_t crc_input[6];		//CRC计算处理
	uint8_t crc_ans;				//计算的CRC
	uint8_t CRC_result;			//当前帧传输正确=1，传输错误=0
	
	/*仅给出接口部分，没有赋值处理*/
	uint32_t cor_1_count;		//连续1正确统计，仅给出接口，根据CRC_result，自行实现
	uint32_t err_1_count;		//连续1错误统计，仅给出接口，根据CRC_result，自行实现
	uint32_t err_2_count;		//连续2错误统计，仅给出接口，根据CRC_result，自行实现
	uint32_t err_3_count;		//连续3错误统计，仅给出接口，根据CRC_result，自行实现
	uint32_t err_4_count;		//连续4错误统计，仅给出接口，根据CRC_result，自行实现
	uint32_t err_5_count;		//连续5错误统计，仅给出接口，根据CRC_result，自行实现
	
	uint8_t init_state;		//0正常，1等待接收超时，2读取超时
	uint8_t read_state;		//0正常，1等待接收超时，2读取超时
	
	uint32_t read_timeout_count;	//读取超时计数，仅给出接口，根据CRC_result，自行实现
  };



uint8_t bissc_init(struct bissc * bissc_d);
uint8_t bissc_read(struct bissc * bissc_d);

#endif


