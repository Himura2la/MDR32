#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_it.h"

static PORT_InitTypeDef PortInit;  // ����������� ���������� ��� ������������� ������ ����� ������
static UART_InitTypeDef UART_InitStructure; // ����������� ���������� ��� ������������� UART
uint32_t uart1_IT_TX_flag = RESET; // ���� ��������������� ����� �������� ���� �����

uint32_t uart2_IT_TX_flag = RESET;
uint32_t uart2_IT_RX_flag = RESET;
volatile void Delay_Tick(volatile uint32_t Tick)
{
	while (Tick--);
}
int main (void)
{
	
	  uint8_t DataByte='d'; //������ ��� ��������
    static uint8_t ReciveByte=0x00; //������ ��� ������
		
	 /* ��������� HSE ����������� (�������� ���������� ����������)*/
    RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	
    if (RST_CLK_HSEstatus() == SUCCESS)  /* ���� HSE ���������� ��������� � ������ �����*/
    {
      // ����� HSE ����������� � �������� ��������� �������� ��������� ��� CPU_PLL 
			// � ��������� ���������� �������� ������� CPU_PLL ������� 7 
			
      RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, 7);
      /* ��������� ����� PLL*/
      RST_CLK_CPU_PLLcmd(ENABLE);
      if (RST_CLK_HSEstatus() == SUCCESS)       //���� ��������� CPU_PLL ������ �������              
      {
        /* ��������� CPU_C3_prescaler = 2 */
        RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV2);
        /* ��������� CPU_C2_SEL �� CPU_PLL ������ ������ CPU_C1 �����*/
        RST_CLK_CPU_PLLuse(ENABLE);
        /* ����� CPU_C3 ����� �� �������������� �������� ��������� ��������������� (CPU clock MUX) */
        RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
      }
      else        /* ���� CPU_PLL �� ���������*/
      {
        while(1);
      }
    }
    else          /* ��������� ��������� HSE �� ��������� */
    {
      while(1);
    }

	
	


  /* Enables the HSI clock on PORTB,PORTF */
  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB,ENABLE);//���������� ������������ ����� B

  /* Fill PortInit structure*/
	// ���������� ����� ���������� 	PortInit
  PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;
  PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
  PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;
  PortInit.PORT_PD = PORT_PD_DRIVER;
  PortInit.PORT_GFEN = PORT_GFEN_OFF;
  PortInit.PORT_FUNC = PORT_FUNC_ALTER;
  PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;
  PortInit.PORT_MODE = PORT_MODE_DIGITAL;

  /* Configure PORTB pins 5 (UART1_TX) as output */
		// ������������ 5 ����� ����� PORTB ��� ������ (UART1_TX) 
  PortInit.PORT_OE = PORT_OE_OUT;
  PortInit.PORT_Pin = PORT_Pin_5;
  PORT_Init(MDR_PORTB, &PortInit);

  /* Configure PORTB pins 6 (UART1_RX) as input */
	// ������������ 6 ����� ����� PORTB ��� ������ (UART1_RX) 
  PortInit.PORT_OE = PORT_OE_IN;
  PortInit.PORT_Pin = PORT_Pin_6;
  PORT_Init(MDR_PORTB, &PortInit);

  /* Select HSI/2 as CPU_CLK source*/
//  RST_CLK_CPU_PLLconfig (RST_CLK_CPU_PLLsrcHSIdiv2,0);

  /* Enables the CPU_CLK clock on UART1*/
	//���������� ������������ UART1
  RST_CLK_PCLKcmd(RST_CLK_PCLK_UART1, ENABLE);

  /* Set the HCLK division factor = 1 for UART1*/
	// ������������� �������� �������� ������� ��� UART1
  UART_BRGInit(MDR_UART1, UART_HCLKdiv1);
  // ���������� ���������� ��� UART1
  NVIC_EnableIRQ(UART1_IRQn);

  /* Initialize UART_InitStructure */
	// ���������� ����� ��� ���������� UART_InitStructure
  UART_InitStructure.UART_BaudRate                = 115200;//�������� ������� �������� ������
  UART_InitStructure.UART_WordLength              = UART_WordLength8b;//����� �������� 8 ���
  UART_InitStructure.UART_StopBits                = UART_StopBits1;//1 ���� ���
  UART_InitStructure.UART_Parity                  = UART_Parity_No;// ��� �������� ��������
  UART_InitStructure.UART_FIFOMode                = UART_FIFO_OFF;// ���������� FIFO ������
  UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;// ���������� �������� �� ��������� � �������

  /* Configure UART1 parameters*/
  UART_Init (MDR_UART1,&UART_InitStructure); //������������� UART1

  /* Enable transmitter interrupt (UARTTXINTR)*/
  UART_ITConfig (MDR_UART1, UART_IT_TX, ENABLE); //���������� ���������� �� �������� ��������

  /* Enables UART1 peripheral */
  UART_Cmd(MDR_UART1,ENABLE); //���������� ������ UART1 


  while (1)
  {
		
    /* Send Data from UART1 */
    UART_SendData (MDR_UART1,DataByte);//���������� ������ �� UART1

    /* Wait until uart1_IT_TX being SET */ 
    while (uart1_IT_TX_flag != SET)//���� ���� ������ �� �����
    {
		}
    /* Clear uart1_IT_TX_flag */
    uart1_IT_TX_flag = RESET;//������� �����
		
		Delay_Tick(200); //�������� 
	}
}

/**
  * @brief  Reports the source file ID, the source line number
  *         and expression text (if USE_ASSERT_INFO == 2) where
  *         the assert_param error has occurred.
  * @param  file_id: pointer to the source file name
  * @param  line: assert_param error line source number
  * @param  expr:
  * @retval None
  */
#if (USE_ASSERT_INFO == 1)
void assert_failed(uint32_t file_id, uint32_t line)
{
  /* User can add his own implementation to report the source file ID and line number.
     Ex: printf("Wrong parameters value: file Id %d on line %d\r\n", file_id, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#elif (USE_ASSERT_INFO == 2)
void assert_failed(uint32_t file_id, uint32_t line, const uint8_t* expr);
{
  /* User can add his own implementation to report the source file ID, line number and
     expression text.
     Ex: printf("Wrong parameters value (%s): file Id %d on line %d\r\n", expr, file_id, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_ASSERT_INFO */

/** @} */ /* End of group UART_Interrupt_91 */

/** @} */ /* End of group __MDR32F9Q1_EVAL */

/** @} */ /* End of group __MDR32F9Qx_StdPeriph_Examples */

/******************* (C) COPYRIGHT 2011 Milandr *********/

/* END OF FILE main.c */

