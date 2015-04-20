#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_rst_clk.h>

#define delay(T) for(i = T; i > 0; i--)

PORT_InitTypeDef PORTDInit;
int i;

int main() {
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
	
  PORTDInit.PORT_Pin   = PORT_Pin_10 | 
												 PORT_Pin_11 | 
												 PORT_Pin_12 | 
												 PORT_Pin_13 | 
												 PORT_Pin_14;
	
  PORTDInit.PORT_OE    = PORT_OE_OUT;
  PORTDInit.PORT_FUNC  = PORT_FUNC_PORT;
  PORTDInit.PORT_MODE  = PORT_MODE_DIGITAL;
  PORTDInit.PORT_SPEED = PORT_SPEED_SLOW;

  PORT_Init(MDR_PORTD, &PORTDInit);
	
	while(1){
		PORT_SetBits(MDR_PORTD, PORT_Pin_10);
		delay(0xFFFF);
		PORT_ResetBits(MDR_PORTD, PORT_Pin_10);
		PORT_SetBits(MDR_PORTD, PORT_Pin_11);
		delay(0xFFFF);
		PORT_ResetBits(MDR_PORTD, PORT_Pin_11);
		PORT_SetBits(MDR_PORTD, PORT_Pin_12);
		delay(0xFFFF);
		PORT_ResetBits(MDR_PORTD, PORT_Pin_12);
		PORT_SetBits(MDR_PORTD, PORT_Pin_13);
		delay(0xFFFF);
		PORT_ResetBits(MDR_PORTD, PORT_Pin_13);
		PORT_SetBits(MDR_PORTD, PORT_Pin_14);
		delay(0xFFFF);
		PORT_ResetBits(MDR_PORTD, PORT_Pin_14);
	}
}
