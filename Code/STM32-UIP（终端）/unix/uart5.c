#include "delay.h"
#include "uart5.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	   
//////////////////////////////////////////////////////////////////////////////////	 
							  
////////////////////////////////////////////////////////////////////////////////// 	   

//���ڷ��ͻ����� 	
__align(8) u8 UART5_TX_BUF[UART5_MAX_SEND_LEN]; 	//���ͻ���,���UART4_MAX_SEND_LEN�ֽ�
#ifdef UART5_RX_EN   								//���ʹ���˽���   	  
//���ڽ��ջ����� 	
u8 UART5_RX_BUF[UART5_MAX_RECV_LEN]; 				//���ջ���,���UART4_MAX_RECV_LEN���ֽ�.
unsigned char *TXP;
int TX_len=0;


//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
u16 UART5_RX_STA=0;   	 
void UART5_IRQHandler(void)
{
	u8 res;	    
if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)//���յ�����
	{	 
		res =USART_ReceiveData(UART5);				 
		if(UART5_RX_STA<UART5_MAX_RECV_LEN)		//�����Խ�������
		{
		  TIM_SetCounter(TIM4,0);//���������      
			if(UART5_RX_STA==0)TIM4_Set(1);	 	//ʹ�ܶ�ʱ��4���ж� 
			UART5_RX_BUF[UART5_RX_STA++]=res;		//��¼���յ���ֵ	 
		}else 
		{
			UART5_RX_STA|=1<<15;					//ǿ�Ʊ�ǽ������
		} 
	}  	

if(USART_GetITStatus(UART5,USART_IT_TXE)==SET)
	{		 
		 if(TX_len!=0)
		{
			USART_SendData(UART5,*TXP);
			TX_len--;
			TXP++;
		}
			else
		{
			USART_ClearITPendingBit(UART5,USART_IT_TXE);
			USART_ITConfig(UART5,USART_IT_TXE,DISABLE);
		}
	}	
}  
 
unsigned int UART5_WRITE(unsigned int len,unsigned char *data)
{
	if(TX_len!=0)
	{
		return -1;
	}
	TX_len=len;
	TXP=data;
	
	USART_ClearITPendingBit(UART5,USART_IT_TXE);
	USART_ITConfig(UART5,USART_IT_TXE,ENABLE);
	return 0;
}
//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void UART5_Init(u32 bound)
{ 	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);	// GPIOAʱ��
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	
 	USART_DeInit(UART5);  //��λ����1
		 //UART4_TX   PA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��PA2
   
    //UART4_RX	  PA.3
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOD, &GPIO_InitStructure);  //��ʼ��PA3
	
	USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  
	USART_Init(UART5, &USART_InitStructure); //��ʼ������	2
  
	//����������
 	//UART4->BRR=(pclk1*1000000)/(bound);// ����������	 
	//UART4->CR1|=0X200C;  	//1λֹͣ,��У��λ.
	//USART_DMACmd(UART4,USART_DMAReq_Tx,ENABLE);  	//ʹ�ܴ���2��DMA����
	//UART_DMA_Config(DMA1_Channel7,(u32)&UART4->DR,(u32)UART4_TX_BUF);//DMA1ͨ��7,����Ϊ����2,�洢��ΪUART4_TX_BUF 
	USART_Cmd(UART5, ENABLE);                    //ʹ�ܴ��� 
	
#ifdef UART5_RX_EN		  	//���ʹ���˽���
	//ʹ�ܽ����ж�
  USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//�����ж�   
	
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	TIM4_Init(99,7199);		//10ms�ж�
	UART5_RX_STA=0;		//����
	TIM4_Set(0);			//�رն�ʱ��4
#endif	 									  	
}

//////////////////////////////////////////////////////////////////////////
//����printf����
/*
 * ��������itoa
 * ����  ������������ת�����ַ���
 * ����  :-radix =10 ��ʾ10����,�������Ϊ0
 *         -value Ҫת����������
 *         -buf ת������ַ���
 *         -radix = 10
 * ���  :��
 * ����  :��
 * ����  :��USART_printf()����
 */
 static char *itoa(int value, char *string, int radix)
 {
  int  i, d;
  int  flag = 0;
  char *ptr = string;

 /* This implementation only works for decimal numbers. */
  if (radix != 10)
  {
      *ptr = 0;
      return string;
  }
  if (!value)
  {
      *ptr++ = 0x30;
      *ptr = 0;
      return string;
  }
  /* if this is a negative value insert the minus sign. */
  if (value < 0)
  {
      *ptr++ = '-';
      /* Make the value positive. */
      value *= -1;
  }
  for (i = 10000; i > 0; i /= 10)
  {
      d = value / i;
      if (d || flag)
      {
          *ptr++ = (char)(d + 0x30);
          value -= (d * i);
          flag = 1;
      }
  }
     /* Null terminate the string. */
  *ptr = 0;
  return string;
 } /* NCL_Itoa */

 /*
 * ������:USART_printf
 * ����  :��ʽ�����,����C���е�printf,������û���õ�C��
 * ����  :-USARTx ����ͨ��
 *                     -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *                           -...    ��������
 * ���  :��
 * ����  :�� 
 * ����  :�ⲿ����
 *         ����Ӧ��   USART_printf( USART1, "\r\n this is a demo \r\n" );
 *                             USART_printf( UART4, "\r\n %d \r\n", i );
 *                             USART_printf( USART3, "\r\n %s \r\n", j );
 */
void u2_printf(char* fmt,...)
 {
   const char *s;
   int d;   
   char buf[16];

   va_list ap;
   va_start(ap, fmt);

      while ( * fmt != 0)     // �ж��Ƿ�ﵽ�ַ���������
      {                                                          
       if ( * fmt == 0x5c )   //'\'
       {                                                                          
        switch ( *++ fmt )
        {
         case 'r':            //�س�
         USART_SendData(UART5, 0x0d);
          fmt ++;
         break;
         case 'n':             //����
         USART_SendData(UART5, 0x0a);        
          fmt ++;
         break;                      
         default:
          fmt ++;
         break;
        }                         
      }
       else if ( * fmt == '%')
        {                       //
         switch ( *++ fmt )
         {                                
          case 's':             //�ַ���
          s = va_arg(ap, const char *);
          for ( ; *s; s++) 
          {
					 UART5->SR;
           USART_SendData(UART5,*s);
           while( USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET );
           }
            fmt++;
           break;

          case 'd':               //ʮ����
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
           {
						UART5->SR;
            USART_SendData(UART5,*s);
            while( USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET );
           }
            fmt++;
           break;
           default:
           fmt++;
           break;
           }                 
       } /* end of else if */
        else UART5->SR;
			  USART_SendData(UART5, * fmt++);
        while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET );
      }
			
 }

//��ʱ��4�жϷ������		    
void TIM4_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{	 			   
		UART5_RX_STA|=1<<15;	//��ǽ������
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx�����жϱ�־    
		TIM4_Set(0);			//�ر�TIM4  
	}   
}
//����TIM4�Ŀ���
//sta:0���ر�;1,����;
void TIM4_Set(u8 sta)
{
	if(sta)
	{
       
		TIM_SetCounter(TIM4,0);//���������
		TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx	
	}else TIM_Cmd(TIM4, DISABLE);//�رն�ʱ��4
}
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʼ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��		 
void TIM4_Init(u16 arr,u16 psc)
{	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��//TIM4ʱ��ʹ��    
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM4�ж�,��������ж�

	 	  
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
}
#endif		 
									 





















