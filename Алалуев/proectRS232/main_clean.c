#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_it.h"

static PORT_InitTypeDef PortInit;    // ����������� ���������� ��� ������������� ������ ����� ������
static UART_InitTypeDef UART_InitStructure; // ����������� ���������� ��� ������������� UART
uint32_t uart1_IT_TX_flag = RESET; // ���� ��������������� ����� �������� ������ �����

uint32_t uart2_IT_TX_flag = RESET;  // ���� ��������������� ����� �������� ������ �����
uint32_t uart2_IT_RX_flag = RESET;  // ���� ��������������� ����� ������ ������ �����

volatile void Delay_Tick(volatile uint32_t Tick){ while (Tick--) }

int main (void) {
    
    uint8_t DataByte='d'; //������ ��� ��������
    static uint8_t ReciveByte=0x00; //������ ��� ������

	// ��������� HSE ����������� (�������� ���������� ����������)
    RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	
    if (RST_CLK_HSEstatus() == SUCCESS){    // ���� HSE ���������� ��������� � ������ �����
        // ����� HSE ����������� � �������� ��������� �������� ��������� ��� CPU_PLL 
		// � ��������� ���������� �������� ������� CPU_PLL ������� 7 
        RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, 7);

        // ��������� ����� PLL
        RST_CLK_CPU_PLLcmd(ENABLE);
        if (RST_CLK_HSEstatus() == SUCCESS){ //���� ��������� CPU_PLL ������ �������
            RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV2); // ��������� CPU_C3_prescaler = 2
            RST_CLK_CPU_PLLuse(ENABLE); // ��������� CPU_C2_SEL �� CPU_PLL ������ ������ CPU_C1 �����

            /* ����� CPU_C3 ����� �� �������������� �������� ��������� ��������������� (CPU clock MUX) */
            RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
        }
        else {      // ���� CPU_PLL �� ���������
            while(1);
        }
    }
    else {          // ��������� ��������� HSE �� ���������
        while(1);
    }

	RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF,ENABLE);  //���������� ������������ ����� F

	// ���������� ����� ���������� 	PortInit ��� ����������� ������ UART2
    PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;
    PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
    PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;
    PortInit.PORT_PD = PORT_PD_DRIVER;
    PortInit.PORT_GFEN = PORT_GFEN_OFF;
    PortInit.PORT_FUNC =     PORT_FUNC_OVERRID; 
	PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;
    PortInit.PORT_MODE = PORT_MODE_DIGITAL;


	// ������������ 1 ����� ����� PORTF ��� ������ (UART1_TX) 
    PortInit.PORT_OE = PORT_OE_OUT;
    PortInit.PORT_Pin = PORT_Pin_1;
    PORT_Init(MDR_PORTF, &PortInit);

	// ������������ 0 ����� ����� PORTF ��� ����� (UART1_RX)
    PortInit.PORT_OE = PORT_OE_IN;
    PortInit.PORT_Pin = PORT_Pin_0;
    PORT_Init(MDR_PORTF, &PortInit);

	//���������� ������������ UART2
    RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2, ENABLE);
    // ������������� �������� �������� ������� ��� UART1?????
    UART_BRGInit(MDR_UART2, UART_HCLKdiv1);
    // ���������� ���������� ��� UART2
    NVIC_EnableIRQ(UART2_IRQn);

    // ���������� ����� ��� ���������� UART_InitStructure 
    UART_InitStructure.UART_BaudRate   = 115200;    //�������� ������� �������� ������
    UART_InitStructure.UART_WordLength = UART_WordLength8b; //����� �������� 8 ���
    UART_InitStructure.UART_StopBits   = UART_StopBits1;    //1 ���� ���
    UART_InitStructure.UART_Parity     = UART_Parity_No;    // ��� �������� ��������
    UART_InitStructure.UART_FIFOMode   = UART_FIFO_OFF;     // ���������� FIFO ������
    /* ���������� �������� �� ��������� � ������� */
    UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;       

    UART_Init (MDR_UART2, &UART_InitStructure);           //������������� UART1

    UART_ITConfig (MDR_UART2, UART_IT_RX, ENABLE);        //���������� ���������� �� ������ 
    UART_ITConfig (MDR_UART2, UART_IT_TX, ENABLE);        //���������� ���������� �� �������� ��������

    UART_Cmd(MDR_UART2,ENABLE); //���������� ������ UART2 

        while (1) {
                while (uart2_IT_RX_flag != SET); //���� ���� �� �� ������������ ���� �� ������ �����
                uart2_IT_RX_flag = RESET; 				        //������� ���� ������
                ReciveByte = UART_ReceiveData (MDR_UART2);    //��������� �������� ����
                UART_SendData (MDR_UART2,ReciveByte);	        //���������� �������� ���� �������
                while (uart2_IT_TX_flag != SET);			//���� ���� ���� �����
                uart2_IT_TX_flag = RESET;					//������� ���� ��������
        }
}

void assert_failed(uint32_t file_id, uint32_t line){
        while (1){
        }
}