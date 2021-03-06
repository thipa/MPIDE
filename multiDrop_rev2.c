#include <24FJ48GA002.h>
#include "BL_Support.h"
#use delay (internal = 8 MHz, clock = 32MHz)
#PIN_SELECT U1RX = PIN_B12 //PIN_B14 //
#PIN_SELECT U1TX = PIN_B13 //PIN_B15 //
#use rs232 (UART1, BAUD = 115200, XMIT = PIN_B13, RCV = PIN_B12)


char arrayData[6] = {};
char arrayDataXI[2] = {};
char arrayDataXII[2] = {};
char arrayDataYI[2] = {};
char arrayDataYII[2] = {};
char arrayAng[2] = {};
char arrayAngGrip[2] = {};

char SM_id = 1;
int getPackage = 0;
void SM_RxD(int c){
	if (SM_id <= 2){
		if (c ==  0xFF){
			SM_id++;
		}else{
			SM_id = 1;
		}
	}else if (SM_id <= 3){
		if (c == 0x02){
			SM_id++;
		}
	}else if (SM_id <= 4){
		if ( c == 0x06){
			SM_id++;
		}
	}else if (SM_id > 4 && SM_id <= 6){
		arrayDataXI[SM_id - 5] = c; //PosX
		SM_id++;
	}else if (SM_id == 7){
		arrayData[SM_id - 7] = c;	//DirPosX[0]
		SM_id++;
	}else if (SM_id > 7 && SM_id <= 9){
		arrayDataYI[SM_id - 8] = c;	//PosY
		SM_id++;
	}else if (SM_id == 10){
		arrayData[SM_id - 9] = c;	//DirPosY[1]
		SM_id++;
	}else if (SM_id > 10 && SM_id <= 12){
		arrayAng[SM_id - 11] = c;	//Ang
		SM_id++;
	}else if (SM_id > 12 && SM_id <= 14){
		arrayDataXII[SM_id - 13] = c;//goX
		SM_id++;
	}else if (SM_id == 15){
		arrayData[SM_id - 13] = c;	//goDir[2]
		SM_id++;
	}else if (SM_id > 15 && SM_id <= 17){
		arrayDataYII[SM_id - 16] = c;	//goY
		SM_id++;
	}else if (SM_id == 18){
		arrayData[SM_id - 15] = c;	//goDir[3]
		SM_id++;
	}else if (SM_id > 18 && SM_id <= 20){
		arrayAngGrip[SM_id - 19] = c;//Anggrip
		SM_id++;
		if(SM_id >= 21){
			printf("%d\n",SM_id);
			getPackage = 1;
			SM_id = 1;
		}
	}
}

#INT_RDA               // receive data interrupt one time per one 
void UART1_Isr() {
    int c = getc();
    SM_RxD(c);
}
void main(){
	disable_interrupts(GLOBAL);
	
    clear_interrupt(INT_RDA);   // recommend style coding to confirm everything clear before use
    enable_interrupts(INT_RDA);
	enable_interrupts(GLOBAL);
	while(TRUE){
		int bagPosX, bagPosY, angle, goPosX, goPosY, angleGrip;
		if (getPackage >= 1){
			getPackage = 0;
			memcpy(&bagPosX, arrayDataXI, sizeof(bagPosX));
			memcpy(&bagPosY, arrayDataYI, sizeof(bagPosY));
			memcpy(&angle, arrayAng, sizeof(angle));
			memcpy(&goPosX, arrayDataXII, sizeof(goPosX));
			memcpy(&goPosY, arrayDataYII, sizeof(goPosY));
			memcpy(&angleGrip, arrayAngGrip, sizeof(angleGrip));

			move_posZ(7680, 1);
			moveXYZ(bagPosX, arrayData[0], bagPosY, arrayData[1], 3000, 1);
		}
	}
}

