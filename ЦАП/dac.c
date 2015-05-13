#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_dac.h>
#include <math.h>

#define SCALE 1260
#define PI 3.14159265

PORT_InitTypeDef PORTEInit;
void DACPortInit(){
	PORT_StructInit(&PORTEInit); //Load defaults
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTE, ENABLE);
		PORTEInit.PORT_Pin  = PORT_Pin_9;
		PORTEInit.PORT_OE   = PORT_OE_OUT;
		PORTEInit.PORT_MODE = PORT_MODE_ANALOG;
  PORT_Init(MDR_PORTE, &PORTEInit);	
}

void DACInit(){
  RST_CLK_PCLKcmd(RST_CLK_PCLK_DAC, ENABLE);
	DAC1_Init(DAC1_AVCC);
	DAC1_Cmd(ENABLE);
}

float a;
int main() {
	RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK, ENABLE);
	DACPortInit();
	DACInit();
	
	while(1){
		for (a=0; a<360; a+=5){
			DAC1_SetData(sin(a*PI/180)*SCALE + SCALE); //Jumper to EXT_CON!!!
		}
	}
}
