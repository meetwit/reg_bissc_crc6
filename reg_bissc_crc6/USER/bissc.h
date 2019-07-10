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
	uint8_t crc_input[6];
	uint8_t crc_ans;			//计算的CRC
	uint32_t crc_err;			//统计
	uint32_t crc_cor;
  };


void bissc_init(struct bissc * bissc_d);
void bissc_read(struct bissc * bissc_d);

#endif


