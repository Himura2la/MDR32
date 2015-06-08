/**
  ******************************************************************************
  * @file    Examples/MDR32F9Q1_EVAL/UART/Interrupt/main.c
  * @author  Milandr Application Team
  * @version V1.2.0
  * @date    04/07/2011
  * @brief   Main program body.
  ******************************************************************************
  * <br><br>
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, MILANDR SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 Milandr</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "MDR32F9Qx_board.h"
#include "MDR32F9Qx_config.h"
#include "MDR32Fx.h"
#include "MDR32F9Qx_uart.h"
#include "MDR32F9Qx_port.h"
#include "MDR32F9Qx_rst_clk.h"
#include "MDR32F9Qx_it.h"



/** @addtogroup __MDR32F9Qx_StdPeriph_Examples MDR32F9Qx StdPeriph Examples
  * @{
  */

/** @addtogroup __MDR32F9Q1_EVAL MDR32F9Q1 Evaluation Board
  * @{
  */

/** @addtogroup UART_Interrupt_91 UART_Interrupt
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static PORT_InitTypeDef PortInit;  // определение переменной для инициализации портов ввода вывода
static UART_InitTypeDef UART_InitStructure; // определение переменной для инициализации UART
uint32_t uart1_IT_TX_flag = RESET; // Флаг устанавливается после передачи одного байта

uint32_t uart2_IT_TX_flag = RESET;// Флаг устанавливается после передачи одного байта
uint32_t uart2_IT_RX_flag = RESET;// Флаг устанавливается после приема одного байта
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
	
volatile void Delay_Tick(volatile uint32_t Tick)
{
	while (Tick--);
}

int main (void)
{
  uint8_t DataByte='d'; //данные для передачи
    static uint8_t ReciveByte=0x00; //данные для приема
		
	 /* Включение HSE осциллятора (внешнего кварцевого резонатора)*/
    RST_CLK_HSEconfig(RST_CLK_HSE_ON);
	
    if (RST_CLK_HSEstatus() == SUCCESS)  /* Если HSE осциллятор включился и прошел текст*/
    {
      // Выбор HSE осциллятора в качестве источника тактовых импульсов для CPU_PLL 
			// и установка умножителя тактовой частоты CPU_PLL равного 7 
			
      RST_CLK_CPU_PLLconfig(RST_CLK_CPU_PLLsrcHSEdiv1, 7);
      /* Включение схемы PLL*/
      RST_CLK_CPU_PLLcmd(ENABLE);
      if (RST_CLK_HSEstatus() == SUCCESS)       //Если включение CPU_PLL прошло успешно              
      {
        /* Установка CPU_C3_prescaler = 2 */
        RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV2);
        /* Установка CPU_C2_SEL от CPU_PLL выхода вместо CPU_C1 такта*/
        RST_CLK_CPU_PLLuse(ENABLE);
        /* Выбор CPU_C3 такта на мультиплексоре тактовых импульсов микропроцессора (CPU clock MUX) */
        RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
      }
      else        /* блок CPU_PLL не включился*/
      {
        while(1);
      }
    }
    else          /* кварцевый резонатор HSE не включился */
    {
      while(1);
    }

	
	
  /* Enables the HSI clock on ,PORTF */

  RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTF,ENABLE);//Разрешение тактирования порта F

  /* Fill PortInit structure*/
	// заполнение полей переменной 	PortInit для обеспечения работы UART2
  PortInit.PORT_PULL_UP = PORT_PULL_UP_OFF;
  PortInit.PORT_PULL_DOWN = PORT_PULL_DOWN_OFF;
  PortInit.PORT_PD_SHM = PORT_PD_SHM_OFF;
  PortInit.PORT_PD = PORT_PD_DRIVER;
  PortInit.PORT_GFEN = PORT_GFEN_OFF;
  PortInit.PORT_FUNC =   PORT_FUNC_OVERRID; 
	PortInit.PORT_SPEED = PORT_SPEED_MAXFAST;
  PortInit.PORT_MODE = PORT_MODE_DIGITAL;


	
  /* Configure PORTF pins 1 (UART2_TX) as output */
			// Конфигурация 1 ножки порта PORTF как выхода (UART1_TX) 
  PortInit.PORT_OE = PORT_OE_OUT;
  PortInit.PORT_Pin = PORT_Pin_1;
  PORT_Init(MDR_PORTF, &PortInit);

  /* Configure PORTF pins 0 (UART2_RX) as input */
		// Конфигурация 0 ножки порта PORTF как входа (UART1_RX)
  PortInit.PORT_OE = PORT_OE_IN;
  PortInit.PORT_Pin = PORT_Pin_0;
  PORT_Init(MDR_PORTF, &PortInit);


  /* Enables the CPU_CLK clock on UART1,UART2 */
	//Разрешение тактирования UART2
  RST_CLK_PCLKcmd(RST_CLK_PCLK_UART2, ENABLE);

  /* Set the HCLK division factor = 1 for UART1,UART2*/
	// Инициализация делителя тактовой частоты для UART1?????
  UART_BRGInit(MDR_UART2, UART_HCLKdiv1);
  // Разрешение прерывания для UART2
  NVIC_EnableIRQ(UART2_IRQn);

  /* Initialize UART_InitStructure */
	// Заполнение полей для переменной UART_InitStructure 
  UART_InitStructure.UART_BaudRate                = 115200;//тактовая частота передачи данных
  UART_InitStructure.UART_WordLength              = UART_WordLength8b;//длина символов 8 бит
  UART_InitStructure.UART_StopBits                = UART_StopBits1;//1 стоп бит
  UART_InitStructure.UART_Parity                  = UART_Parity_No;// нет контроля четности
  UART_InitStructure.UART_FIFOMode                = UART_FIFO_OFF;// выключение FIFO буфера
  UART_InitStructure.UART_HardwareFlowControl     = UART_HardwareFlowControl_RXE | UART_HardwareFlowControl_TXE;// аппаратный контроль за передачей и приемом


   /* Configure UART2 parameters*/
	 
  UART_Init (MDR_UART2,&UART_InitStructure);//Инициализация UART1

  /* Enable Receiver interrupt*/
  UART_ITConfig (MDR_UART2, UART_IT_RX, ENABLE);//Разрешение прерывания по приему 
  UART_ITConfig (MDR_UART2, UART_IT_TX, ENABLE);//Разрешение прерывания по окончани передачи

  /* Enables UART2 peripheral */
  UART_Cmd(MDR_UART2,ENABLE); //Разрешение работы UART2 

  while (1)
  {

    /* Wait until uart2_IT_RX being SET */
    while (uart2_IT_RX_flag != SET)//ждем пока не не установиться флаг по приему байта
    {
    }
    /* Clear uart2_IT_RX_flag */
    uart2_IT_RX_flag = RESET; 								//очищаем флаг приема
    /* Recive data*/
    ReciveByte = UART_ReceiveData (MDR_UART2);//считываем принятый байт
		UART_SendData (MDR_UART2,ReciveByte);			//отправляем принятый байт обратно
    /* Wait until uart2_IT_TX being SET */
    while (uart2_IT_TX_flag != SET)						//ждем пока байт уйдет
    {
    }
    /* Clear uart2_IT_TX_flag */
    uart2_IT_TX_flag = RESET;									//очищаем флаг передачи
	
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

