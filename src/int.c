#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_timer.h>

PORT_InitTypeDef PORTDInit;
void PortsInit(){
	PORT_StructInit(&PORTDInit); //Load defaults
		RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD, ENABLE);
		PORTDInit.PORT_Pin   = PORT_Pin_10 | 
													 PORT_Pin_11 | 
													 PORT_Pin_12 | 
													 PORT_Pin_13 | 
													 PORT_Pin_14;
		PORTDInit.PORT_OE    = PORT_OE_OUT;
		PORTDInit.PORT_MODE  = PORT_MODE_DIGITAL;
		PORTDInit.PORT_SPEED = PORT_SPEED_SLOW;
  PORT_Init(MDR_PORTD, &PORTDInit);	
}

uint8_t cur_i;
void NextLED(){
	switch(cur_i++ % 5) {
		case 0:
			PORT_ResetBits(MDR_PORTD, PORT_Pin_14);
			PORT_SetBits(MDR_PORTD, PORT_Pin_10);
			break;
		case 1:
			PORT_ResetBits(MDR_PORTD, PORT_Pin_10);
			PORT_SetBits(MDR_PORTD, PORT_Pin_11);
			break;
		case 2:
			PORT_ResetBits(MDR_PORTD, PORT_Pin_11);
			PORT_SetBits(MDR_PORTD, PORT_Pin_12);
			break;
		case 3:
			PORT_ResetBits(MDR_PORTD, PORT_Pin_12);
			PORT_SetBits(MDR_PORTD, PORT_Pin_13);
			break;
		case 4:
			PORT_ResetBits(MDR_PORTD, PORT_Pin_13);
			PORT_SetBits(MDR_PORTD, PORT_Pin_14);
			break;
	}
}

TIMER_CntInitTypeDef TIM1Init;

void TimerInit(){
	RST_CLK_PCLKcmd(RST_CLK_PCLK_TIMER1, ENABLE);	
	TIMER_BRGInit(MDR_TIMER1, TIMER_HCLKdiv1);
	
	TIMER_CntStructInit(&TIM1Init);	//Load defaults
		TIM1Init.TIMER_Prescaler        = 8000;
		TIM1Init.TIMER_Period           = 1000;
  TIMER_CntInit(MDR_TIMER1, &TIM1Init);	
	
	//TIntInit
	NVIC_EnableIRQ(Timer1_IRQn);
	NVIC_SetPriority(Timer1_IRQn, 0);
	TIMER_ITConfig(MDR_TIMER1, TIMER_STATUS_CNT_ZERO, ENABLE);
	
	TIMER_Cmd(MDR_TIMER1, ENABLE);
}

void Timer1_IRQHandler() {
	if(TIMER_GetITStatus(MDR_TIMER1, TIMER_STATUS_CNT_ZERO)){
		NextLED();
		TIMER_ClearITPendingBit(MDR_TIMER1, TIMER_STATUS_CNT_ZERO);
	}
}

int main() {
	PortsInit();
	TimerInit();
	cur_i = 0;
	
	while(1){
		// Silence is priceless
	}
}
