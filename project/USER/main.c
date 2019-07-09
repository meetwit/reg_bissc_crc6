#include "xmc4800.h"
#include "bissc.h"

//程序中的while等待替换掉，或者增加超时退出。2019年7月9日

//统计部分可以采用链表实现
//struct bisc_count{
//	uint8_t prev;
//	uint8_t next;
//};
//struct bisc_count err_c={0};

struct bissc bissc_test={0}; 			//step 1

/*test code A*/
/*
该部分定义的是测试代码，统计正确错误的个数
	uint32_t 数据[0,4294967296],读取和处理时间是8us，
	则一小时产生的计数和应该是24*60*60*1000000/8=450000000,所以单个数类型计数测试时间应该小于等于9.5小时
*/
//uint32_t cor_1_count=0,err_1_count=0,err_2_count=0,err_3_count=0,err_4_count=0,err_5_count=0;
uint8_t last_frame[5]={1,1,1,1,1};	//init all is correct


/*end of test code A*/

int main(){
	
	bissc_test.init_state = bissc_init(&bissc_test);		//step 2
	if(bissc_test.init_state){
		return 0;		//初始化超时
	}
	
	while(1){
		
			bissc_test.read_state = bissc_read(&bissc_test);		//step 3
	
/*test code B*/
		if(bissc_test.read_state){
			bissc_test.read_timeout_count++;
		}
		
			last_frame[4] = last_frame[3];
			last_frame[3] = last_frame[2];
			last_frame[2] = last_frame[1];
			last_frame[1] = last_frame[0];
			last_frame[0] = bissc_test.CRC_result;
		
			if(last_frame[0]==0){
				bissc_test.err_1_count++;
				if(last_frame[1]==0){
					bissc_test.err_2_count++;
					if(last_frame[2]==0){
						bissc_test.err_3_count++;
							if(last_frame[3]==0){
								bissc_test.err_4_count++;
									if(last_frame[4]==0){
										bissc_test.err_5_count++;
									}
							}
					}
				}
			}else{
				bissc_test.cor_1_count++;
			}
/*end of test code B*/
			
		}
}


