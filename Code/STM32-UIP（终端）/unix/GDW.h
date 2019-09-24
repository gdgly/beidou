#ifndef GDW_H
#define GDW_H

typedef unsigned char u8;
typedef unsigned short u16;



//���Ľ���,��ȷ����ֵ��͸��ת�����ĳ��ȣ����󷵻�ֵ��-1
int GDW_read(u8* origin_data, u8* recieve_data);

int GDW_answer_read(u8* origin_data, u8* recieve_data);
//͸��ת�����ķ�װ
void GDW_forward_write(u8* addr, u8* origin_data, u8* send_data, u16 len);
//͸��ת��Ӧ���ķ�װ
void GDW_answer_write(u8* addr, u8* origin_data, u8* send_data, u16 len);

void LOG_ON(u8* addr, u8* send_data);
	
void HEART(u8* addr, u8* send_data);

void CONFIRM(u8* addr, u8* send_data);

#endif
