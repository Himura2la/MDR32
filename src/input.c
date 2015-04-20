#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_rst_clk.h>

int i;
#define delay(T) for(i = T; i > 0; i--)

PORT_InitTypeDef PORTDInit;
void LEDs_Init(){
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
}

PORT_InitTypeDef PORTCInit;
void JoystickInit(){
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTC, ENABLE);

  PORTCInit.PORT_Pin   = PORT_Pin_10 | 
												 PORT_Pin_11 | 
												 PORT_Pin_12 | 
												 PORT_Pin_13 | 
												 PORT_Pin_14;
	
  PORTCInit.PORT_OE    = PORT_OE_IN;
  PORTCInit.PORT_FUNC  = PORT_FUNC_PORT;
  PORTCInit.PORT_MODE  = PORT_MODE_DIGITAL;
  PORTCInit.PORT_SPEED = PORT_SPEED_SLOW;

  PORT_Init(MDR_PORTC, &PORTCInit);
}

uint32_t Pins[5] = {PORT_Pin_10, PORT_Pin_11, PORT_Pin_12, PORT_Pin_13, PORT_Pin_14 };
int main() {
	LEDs_Init();
	JoystickInit();
	while(1){
		for (i = 0; i < 5; i++){
			if (!PORT_ReadInputDataBit(MDR_PORTC, Pins[i]))
				PORT_SetBits(MDR_PORTD, Pins[i]);
			else
				PORT_ResetBits(MDR_PORTD, Pins[i]);
		}	
	}
}
