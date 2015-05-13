#include <MDR32F9Qx_port.h>
#include <MDR32F9Qx_rst_clk.h>
#include <MDR32F9Qx_adc.h>
#include <MDR32F9Qx_dac.h>


uint16_t ADCConvertedValue[10];

ADC_InitTypeDef ADC;
ADCx_InitTypeDef ADC1;

void ADCInit(){
  RST_CLK_PCLKcmd(RST_CLK_PCLK_RST_CLK | RST_CLK_PCLK_ADC, ENABLE);
	
	ADC_StructInit(&ADC);
	ADC.ADC_TempSensor = ADC_TEMP_SENSOR_Enable;
  ADC.ADC_TempSensorConversion = ADC_TEMP_SENSOR_CONVERSION_Enable;
	ADC_Init(&ADC);
	
	ADCx_StructInit(&ADC1);
	ADC1.ADC_ChannelNumber = ADC_CH_TEMP_SENSOR;
	ADC1.ADC_Prescaler = ADC_CLK_div_512;
	ADC1_Init(&ADC1);
	
	//Int
	NVIC_EnableIRQ(ADC_IRQn);
	NVIC_SetPriority(ADC_IRQn, 0);
	ADC1_ITConfig(ADCx_IT_END_OF_CONVERSION, ENABLE);

	ADC1_Cmd(ENABLE);
}

uint8_t result;
void ADC_IRQHandler() {
	if(ADC_GetITStatus(ADC1_IT_END_OF_CONVERSION)){
		
		result = ADC1_GetResult();
		
		NVIC_ClearPendingIRQ(ADC_IRQn);
	}
}


int main() {
	ADCInit();
	
	ADC1_Start();
	
	while(1){
		
	}
}
