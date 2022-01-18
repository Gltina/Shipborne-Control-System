#include "usart.h"

//extern char USART_ReceData[USART_DATASIZE];
//extern uint32_t USART_ReceIndex;
//extern char USART_LastRece;

// LORA模块最大512字节, 待达到512后需要等待
int USART_CACHE_INDEX = 0;

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	//嵌套向量中断控制器组选择
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	//配置USART为中断源
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	//抢断优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	//子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	//使能中断
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//初始化配置NVIC
	NVIC_Init(&NVIC_InitStructure);

	// 使能串口接收中断
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
}

void USART_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// 打开串口GPIO的时钟
	RCC_APB2PeriphClockCmd(USART, ENABLE);
	// 打开串口外设的时钟
	RCC_APB2PeriphClockCmd(USART_TX, ENABLE);

	// 将USART Tx的GPIO配置为推挽复用模式	
	GPIO_InitStructure.GPIO_Pin = USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART_TX_GPIO_PORT, &GPIO_InitStructure);

	// 将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART_RX_GPIO_PORT, &GPIO_InitStructure);
    
    // AUX
    GPIO_InitStructure.GPIO_Pin = USART_AUX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART_AUX_GPIO_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(USART_AUX_GPIO_PORT, USART_AUX_GPIO_PIN);
    
	// 配置串口的工作参数	
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USARTx, &USART_InitStructure);

	// 配置嵌套向量中断控制器NVIC
	NVIC_Configuration();

	// 使能串口
	USART_Cmd(USARTx, ENABLE);
}

uint32_t Usart_SendByte(USART_TypeDef *pUSARTx, uint8_t c)
{
    /*等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET) {}
        
	/*发送一个字节数据到USART */
	USART_SendData(pUSARTx, (uint16_t) c);
    
	/*等待发送完成 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET) {}
    
    USART_CACHE_INDEX ++;
    
    if(USART_CACHE_INDEX == 512)
    {
        USART_CACHE_INDEX = 0;
        
        // 直到为1, 即lora缓冲区为空
        while(GPIO_ReadInputDataBit(USART_AUX_GPIO_PORT, USART_AUX_GPIO_PIN) == 0){}
    }
	
	return 0;
}

uint32_t Usart_SendHalfWord(USART_TypeDef *pUSARTx, uint16_t c)
{
	uint8_t temp_h, temp_l;

	/*取出高八位 */
	temp_h = (c & 0XFF00) >> 8;
	/*取出低八位 */
	temp_l = c &0XFF;

	/*发送高八位 */
	USART_SendData(pUSARTx, temp_h);
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET) {}

	/*发送低八位 */
	USART_SendData(pUSARTx, temp_l);
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET) {}
    
    // 直到为1
    //while(GPIO_ReadInputDataBit(USART_AUX_GPIO_PORT, USART_AUX_GPIO_PIN) == 0){}
	
	return 0;
}

uint32_t Usart_SendString(USART_TypeDef *pUSARTx, char *str)
{
	uint32_t i = 0;
	for (i = 0; str[i] != '\0'; ++i)
	{
		Usart_SendByte(pUSARTx, *(str + i));
	}

	/*等待发送完成 */
	//while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET) {}
    
    // 直到为1
    //while(GPIO_ReadInputDataBit(USART_AUX_GPIO_PORT, USART_AUX_GPIO_PIN) == 0){}
	
    return i;
}

uint32_t Usart_SendByLength(USART_TypeDef *pUSARTx, char *str, uint32_t length)
{
    uint32_t i = 0;
	for (i = 0; i < length; ++i)
	{
		Usart_SendByte(pUSARTx, *(str + i));
	}

	/*等待发送完成 */
	//while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET) {}
    
    return i;
}

int fputc(int ch, FILE *f)
{
    /*等待发送完毕 */
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET) {}
    
	/*发送一个字节数据到串口 */
	USART_SendData(USARTx, (uint8_t) ch);
    
    /*等待发送完成 */
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET) {}
         
    return ch;
}

int fgetc(FILE *f)
{
	/*等待串口输入数据 */
	while (USART_GetFlagStatus(USARTx, USART_FLAG_RXNE) == RESET) {}

	int result = USART_ReceiveData(USARTx);
        
    //Usart_SendByte(USARTx, result);
    
    return result;
}