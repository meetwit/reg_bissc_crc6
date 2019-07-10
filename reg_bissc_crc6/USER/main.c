#include "xmc4800.h"
#include "GPIO.h"
#include "bissc.h"
	struct bissc bissc_test={0}; 

int main(){
	
	bissc_init(&bissc_test);
	
	while(1){
			bissc_read(&bissc_test);
		}
}


