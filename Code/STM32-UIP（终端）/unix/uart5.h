#ifndef __USART2_H
#define __USART2_H	 
#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����2��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

#define UART5_MAX_RECV_LEN		1024				//�����ջ����ֽ���
#define UART5_MAX_SEND_LEN		1024				//����ͻ����ֽ���
#define UART5_RX_EN 			1					//0,������;1,����.

extern u8  UART5_RX_BUF[UART5_MAX_RECV_LEN]; 		//���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern u8  UART5_TX_BUF[UART5_MAX_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
extern u16 UART5_RX_STA;   						//��������״̬

void UART5_Init(u32 bound);				//����2��ʼ�� 
void TIM4_Set(u8 sta);
void TIM4_Init(u16 arr,u16 psc);
unsigned int UART5_WRITE(unsigned int len,unsigned char *data);
//void UART_DMA_Config(DMA_Channel_TypeDef*DMA_CHx,u32 cpar,u32 cmar);
//void UART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u16 len);
void u2_printf(char* fmt, ...);
#endif













