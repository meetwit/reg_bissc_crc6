#include "bissc.h"
//#include "GPIO.h"

extern void BISS_vSensorModeSingle(uint8_t uwDataLength);

//#ifndef  Control_P4_7(Mode, DriveStrength)
//#define Control_P4_7(Mode, DriveStrength)       PORT4->IOCR4 = (PORT4->IOCR4 & ~0xF8000000) | (Mode << 27); PORT4->PDR0 = (PORT4->PDR0 & ~0x70000000) | (DriveStrength << 28)
//#endif

unsigned char crc6_table[64]={
	0  ,3  ,6  ,5  ,12  ,15  ,10  ,
	9  ,24  ,27  ,30  ,29  ,20  ,23  ,
	18  ,17  ,48  ,51  ,54  ,53  ,60  ,
	63  ,58  ,57  ,40  ,43  ,46  ,45  ,
	36  ,39  ,34  ,33  ,35  ,32  ,37  ,
	38  ,47  ,44  ,41  ,42  ,59  ,56  ,
	61  ,62  ,55  ,52  ,49  ,50  ,19  ,
	16  ,21  ,22  ,31  ,28  ,25  ,26  ,
	11  ,8  ,13  ,14  ,7  ,4  ,1  ,2
};


/*input with 6zero,input 6bit*/
unsigned short fast_crc6(unsigned char *message, unsigned int len)
{
    unsigned char i,out=0;

	for(i=0;i<len;i++){
		out = message[i] ^ crc6_table[out];
	}

    return out^0x3f;

}

void BISS_vSensorModeSingle(uint8_t uwDataLength)
{
	int i=0;
	
	for(i=0;i<uwDataLength;i++){
		while((USIC0_CH0->TRBSR&0x00000800)!=0x00000800);		//等待发送缓冲器为空
		USIC0_CH0->IN[0]  = i;											//发送数据
	}
}


void bissc_init(struct bissc * bissc_d){
	uint32_t Brg_PDiv = 0;
	
	 SCU_RESET->PRCLR0 |= SCU_RESET_PRCLR0_USIC0RS_Msk;					//clear PRCLR0 USIC0
	 while (SCU_RESET->PRSTAT0 &	SCU_RESET_PRSTAT0_USIC0RS_Msk);

	 USIC0_CH0->KSCFG    =  0x00000003;      // load U0C0 kernel state configuration
	                                         // register

	Brg_PDiv = 9;			//144/2/(pdiv+1)	9的话约等于7.2M

	USIC0_CH0->FDR |= 0x3ff43ff;

	USIC0_CH0->BRG = (((uint32_t)Brg_PDiv << USIC_CH_BRG_PDIV_Pos) & USIC_CH_BRG_PDIV_Msk)|0x00080;

	PORT4->IOCR4 = (PORT4->IOCR4 & ~0xF8000000) | (0 << 27); 
	PORT4->PDR0 = (PORT4->PDR0 & ~0x70000000) | (2 << 28);
	
	PORT1->IOCR4 = (PORT1->IOCR4 & ~0xF8000000) | (0x90 << 20); 
	PORT1->PDR0 = (PORT1->PDR0 & ~0x70000000) | (0x2 << 28);
	
	PORT1->IOCR0 = (PORT1->IOCR0 & ~0x0000F800) | (0x12 << 11); 
	PORT1->PDR0 = (PORT1->PDR0 & ~0x00000070) | (0x02 << 4);
	
	PORT1->IOCR8 = (PORT1->IOCR8 & ~0x000000F8) | (0x12 << 3); 
	PORT1->PDR1 = (PORT1->PDR1 & ~0x00000007) | (0x02);
	
//	Control_P4_7(INPUT, STRONG);							//MRST
//	Control_P1_3(OUTPUT_PP_AF2, STRONG);			//MTSR
//	Control_P1_1(OUTPUT_PP_AF2,STRONG);				//SCLK
//	Control_P1_8(OUTPUT_PP_AF2, STRONG);			//SELO


	WR_REG(USIC0_CH0->BRG,USIC_CH_BRG_SCLKCFG_Msk,USIC_CH_BRG_SCLKCFG_Pos,3);			//与clock同步，极性反转，为了空闲时输出为高 3

	//Configuration of USIC Input Stage
	WR_REG(USIC0_CH0->DX0CR, USIC_CH_DX0CR_DSEL_Msk, USIC_CH_DX0CR_DSEL_Pos, 2);		//000	DX0C	P4.7
	WR_REG(USIC0_CH0->DX0CR, USIC_CH_DX0CR_INSW_Msk, USIC_CH_DX0CR_INSW_Pos, 1);		//由自选择DXn决定输入

	WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_PDL_Msk, USIC_CH_SCTR_PDL_Pos, 1);		//The passive data level is . here is change 1
	WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_TRM_Msk, USIC_CH_SCTR_TRM_Pos, 1);		//The shift control signal is considered active if it is at 1-level. 
																																						//This is the setting to be programmed to allow data transfers.
	WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_SDIR_Msk, USIC_CH_SCTR_SDIR_Pos, 1);	//MSB first
	//Set Word Length (WLE) & Frame Length (FLE)
	WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_FLE_Msk, USIC_CH_SCTR_FLE_Pos, 39);	//frame length
	WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_WLE_Msk, USIC_CH_SCTR_WLE_Pos, 7);		//word length

	//TBUF Data Enable (TDEN) = 1, TBUF Data Single Shot Mode (TDSSM) = 1
	WR_REG(USIC0_CH0->TCSR, USIC_CH_TCSR_TDEN_Msk, USIC_CH_TCSR_TDEN_Pos, 1);			//配置为01 TDV等于1时传输开始
	//WR_REG(USIC0_CH0->TCSR, USIC_CH_TCSR_TDV_Msk, USIC_CH_TCSR_TDV_Pos, 1);				//add me
	WR_REG(USIC0_CH0->TCSR, USIC_CH_TCSR_TDSSM_Msk, USIC_CH_TCSR_TDSSM_Pos, 1);		//单次

	WR_REG(USIC0_CH0->PCR_SSCMode, USIC_CH_PCR_SSCMode_MSLSEN_Msk, USIC_CH_PCR_SSCMode_MSLSEN_Pos, 1);		//使能MSLSEN，主模式必须使能
	WR_REG(USIC0_CH0->PCR_SSCMode, USIC_CH_PCR_SSCMode_SELCTR_Msk, USIC_CH_PCR_SSCMode_SELCTR_Pos, 1);		//SELO输出方式 直接
	WR_REG(USIC0_CH0->PCR_SSCMode, USIC_CH_PCR_SSCMode_SELINV_Msk, USIC_CH_PCR_SSCMode_SELINV_Pos, 0);		//反转SELO输出
	WR_REG(USIC0_CH0->PCR_SSCMode, USIC_CH_PCR_SSCMode_SELO_Msk, USIC_CH_PCR_SSCMode_SELO_Pos, 1);				//SELOx active control by SELCTR

	WR_REG(USIC0_CH0->CCR, USIC_CH_CCR_MODE_Msk, USIC_CH_CCR_MODE_Pos, 1);
	WR_REG(USIC0_CH0->CCR, USIC_CH_CCR_AIEN_Msk, USIC_CH_CCR_AIEN_Pos, 1);

	USIC0_CH0->PCR_SSCMode = 0x000003B;         // disable tiwen   fmclk  FEM 1
	WR_REG(USIC0_CH0->PCR_SSCMode,USIC_CH_PCR_SSCMode_TIWEN_Msk, USIC_CH_PCR_SSCMode_TIWEN_Pos, 0);			//no delay between world ?
	USIC0_CH0->TCSR &= 0xfffffffE;							//The automatic update of SCTR.WLE and TCSR.EOF is disabled
	USIC0_CH0->TCSR &= 0xffffffBE;							//The data word in TBUF is not considered as last word of an SSC frame
	USIC0_CH0->TCSR |= 0x40;										//The data word in TBUF is valid for transmission and a transmission start is possible. 
																							//New data should not be written to a TBUFx input location while TDV = 1.
	USIC0_CH0->RBCTR=0;
	USIC0_CH0->RBCTR  =0x0002800;              // fifo size 64  limit 40   DP 0
	USIC0_CH0->RBCTR  =0x6002800;								//0x6003e00
	USIC0_CH0->TBCTR=0;
	USIC0_CH0->TBCTR  =0x0000800;              // fifo size 2  limit  2    DP 62
	USIC0_CH0->TBCTR  =0x3000828;								//0x100023e
	
	BISS_vSensorModeSingle(1);		//推空缓存
	while((USIC0_CH0->TRBSR&0x0008)==0x0008){;}	//等待接收缓存有数据
	bissc_d->RxPacket[0] = (USIC0_CH0->OUTR&0x000000ff);			//取数据低8位，根据配置的位数读取，最多16位
		
}

void bissc_read(struct bissc * bissc_d){
		uint8_t i=0;
		BISS_vSensorModeSingle(5);
		
		for(i=0;i<5;i++){
			while((USIC0_CH0->TRBSR&0x0008)==0x0008);		//等待接收缓存有数据
			bissc_d->RxPacket[i]= (USIC0_CH0->OUTR&0x000000ff);			//取数据低8位，根据配置的位数读取，最多16位
		}
		if(bissc_d->RxPacket[0]&0x10){
			//bissc_test.head 	= (RxPacket[0]&0xe0)>>5;		//3bit
			bissc_d->start 	= (bissc_d->RxPacket[0]&0x10)>>4;		//1bit	always value 1
			bissc_d->csd 		= (bissc_d->RxPacket[0]&0x08)>>3;		//1bit
			bissc_d->data 	= (((uint32_t)bissc_d->RxPacket[0]&0x07) << 17) | (((uint32_t)bissc_d->RxPacket[1])<<9) | (((uint32_t)bissc_d->RxPacket[2])<<1)|(((uint32_t)bissc_d->RxPacket[3]&0x80) >>7) ;		//20bit
			bissc_d->zero 	= (bissc_d->RxPacket[3]&0x7e)>>1;		//6bit
			bissc_d->err 		= bissc_d->RxPacket[3]&0x1;				//1bit
			bissc_d->war 		= (bissc_d->RxPacket[4]&0x80)>>7;		//1bit
			bissc_d->crc 		= (bissc_d->RxPacket[4]&0x7e)>>1;		//6bit
			//bissc_test.foot 	= RxPacket[4]&0x1;				//1bit
		}
		else if(bissc_d->RxPacket[0]&0x08){
			//bissc_test.head 	= (RxPacket[0]&0xe0)>>5;		//4bit
			bissc_d->start 	= (bissc_d->RxPacket[0]&0x08)>>3;		//1bit	always value 1
			bissc_d->csd 		= (bissc_d->RxPacket[0]&0x04)>>2;		//1bit
			bissc_d->data 	= (((uint32_t)bissc_d->RxPacket[0]&0x03) << 18) | (((uint32_t)bissc_d->RxPacket[1])<<10) | (((uint32_t)bissc_d->RxPacket[2])<<2)|(((uint32_t)bissc_d->RxPacket[3]&0xc0) >>6) ;		//20bit
			bissc_d->zero 	= (bissc_d->RxPacket[3]&0x3f)>>0;		//6bit
			bissc_d->err 		= (bissc_d->RxPacket[4]&0x80)>>7;		//1bit
			bissc_d->war 		= (bissc_d->RxPacket[4]&0x40)>>6;		//1bit
			bissc_d->crc 		= (bissc_d->RxPacket[4]&0x3f)>>0;		//6bit
			//bissc_test.foot 	= RxPacket[4]&0x1;				//0bit
		}

		bissc_d->crc_input[0]=(bissc_d->data&0xf0000)>>16;
		bissc_d->crc_input[1]=(bissc_d->data&0xfc00)>>10;
		bissc_d->crc_input[2]=(bissc_d->data&0x3f0)>>4;
		bissc_d->crc_input[3]=(bissc_d->data&0xf)<<2;
		bissc_d->crc_input[4]=(bissc_d->err<<1)|(bissc_d->war<<0);
		bissc_d->crc_input[5]=0;
		bissc_d->crc_ans = fast_crc6(bissc_d->crc_input,6);

		bissc_d->CRC_result = (bissc_d->crc_ans==bissc_d->crc) ? 1 : 0;	
}
	
