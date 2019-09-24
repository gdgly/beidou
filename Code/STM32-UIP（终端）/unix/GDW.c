#include "GDW.h"

u8 num = 0;
//376.1���Ľ���
//͸��ת�����Ľ���
//����ֵ��1.͸��ת�����ģ����ؽ�����ı��ĳ���;2.��¼���Ļ����������ģ�����ֵ��0��3.�������ķ���-1��
int GDW_read(u8* origin_data, u8* recieve_data)
{
    int len;
	  u8 check;
		u8 i;
    if(origin_data[0] == 0x68)
    {
       if(origin_data[12] == 0x10)
       {
           len = origin_data[22];
					 for(i=0; i<len; i++)
           {
              recieve_data[i] = origin_data[24+i];
           }
           return len;
       }
			 //��¼���Ļ�����������
			 if(origin_data[12] == 0x02)
			 {
				 recieve_data[0] = 0x68;
				 recieve_data[1] = 0x32;
				 recieve_data[2] = 0;
				 recieve_data[3] = 0x32;
				 recieve_data[4] = 0;
				 recieve_data[5] = 0x68;
				 recieve_data[6] = 0;
				 recieve_data[7] = origin_data[7];
				 recieve_data[8] = origin_data[8];
				 recieve_data[9] = origin_data[9];
				 recieve_data[10] = origin_data[10];
				 recieve_data[11] = origin_data[11];
				 recieve_data[12] = 0;
				 recieve_data[13] = 0x60+num;
				 recieve_data[14] = 0;
				 recieve_data[15] = 0;
				 recieve_data[16] = 0x01;
				 recieve_data[17] = 0;
				 for(i=0; i<18; i++)
				 {
					 check ^= recieve_data[i];
				 }
				 recieve_data[18] = check;
				 recieve_data[19] = 0x16;
				 num++;
				 if(num>15)
				 {
						num = 0;
				 }
				 return 0;
			 }
		}
		return -1;
}

//͸��ת������Ӧ�����
int GDW_answer_read(u8* origin_data, u8* recieve_data)
{
		int len;
		u8 i;
		if(origin_data[0] == 0x68)
    {
       if(origin_data[12] == 0x10)
       {
           len = origin_data[19];
       }
       else{
           return -1;
       }
       for(i=0; i<len; i++)
       {
           recieve_data[i] = origin_data[21+i];
       }
       return len;
    }
    else{
       return -1;
    }
}

//376.1͸��ת�����ķ�װ
void GDW_forward_write(u8* addr, u8* origin_data, u8* send_data, u16 len)
{
		u8 i;
    u8 check = 0;
    
    send_data[0] = 0x68;
    send_data[1] = ((((len+34)&0xff)<<2)|0x02);
    send_data[2] = 0;
    send_data[3] = send_data[1];
    send_data[4] = 0;
    send_data[5] = 0x68;
    send_data[6] = 0x4b;
    
    send_data[7] = addr[0];
    send_data[8] = addr[1];
    send_data[9] = addr[2];
    send_data[10] = addr[3];
    send_data[11] = addr[4];
    
    send_data[12] = 0x10;
    send_data[13] = 0x60+num;
    send_data[14] = 0;
    send_data[15] = 0;
    send_data[16] = 0x01;
    send_data[17] = 0;
    
    send_data[18] = 0x04;   //�ն�ͨ�Ŷ˿�
    send_data[19] = 0x6b;   //͸��ת��ͨ�ſ�����
    send_data[20] = 0x01;   //͸��ת�����յȴ����ĳ�ʱʱ��
    send_data[21] = 0x01;   //͸��ת�����յȴ��ֽڳ�ʱʱ��
    
    send_data[22] = len;    //͸��ת�������ֽ���
    send_data[23] = 0;
    
    for(i=0; i<len; i++)
    {
        send_data[24+i] = origin_data[i];
    }
    for(i=0; i<16; i++)
    {
        send_data[24+len+i] = 0;
    }
    for(i=0; i< 24+16+len; i++)
    {
        check ^= send_data[i];
    }
    send_data[24+16+len] = check;
    send_data[24+16+len+1] = 0x16;
    num++;
		if(num>15)
		{
			num = 0;
		}
}


//376.1͸��ת��Ӧ���ķ�װ
void GDW_answer_write(u8* addr, u8* origin_data, u8* send_data, u16 len)
{
		u8 i;
    u8 check = 0;
    
    send_data[0] = 0x68;
    send_data[1] = ((((len+17)&0xff)<<2)|0x02);
    send_data[2] = 0;
    send_data[3] = send_data[1];
    send_data[4] = 0;
    send_data[5] = 0x68;
    send_data[6] = 0x98;
    
    send_data[7] = addr[0];
    send_data[8] = addr[1];
    send_data[9] = addr[2];
    send_data[10] = addr[3];
    send_data[11] = addr[4];
    
    send_data[12] = 0x10;
    send_data[13] = 0x60+num;
    send_data[14] = 0;
    send_data[15] = 0;
    send_data[16] = 0x01;
    send_data[17] = 0;
    
    send_data[18] = 0x04;   //�ն�ͨ�Ŷ˿�
    
    send_data[19] = len;    //͸��ת�������ֽ���
    send_data[20] = 0;
    
    for(i=0; i<len; i++)
    {
        send_data[21+i] = origin_data[i];
    }
    
    send_data[21+len] = 0x18;
    send_data[22+len] = 0;
    
    for(i=0; i<23+len; i++)
    {
        check ^= send_data[i];
    }
    send_data[23+len] = check;
    send_data[23+len+1] = 0x16;
		
		num++;
		if(num>15)
		{
			num = 0;
		}
}


//��¼����
void LOG_ON(u8* addr, u8* send_data)
{
		u8 check = 0;
		u8 i;
		send_data[0] = 0x68;
    send_data[1] = 0x32;
    send_data[2] = 0;
    send_data[3] = 0x32;
    send_data[4] = 0;
    send_data[5] = 0x68;
    send_data[6] = 0xd9;
    
    send_data[7] = addr[0];
    send_data[8] = addr[1];
    send_data[9] = addr[2];
    send_data[10] = addr[3];
    send_data[11] = addr[4];
    
    send_data[12] = 0x02;
    send_data[13] = 0x70+num;
    send_data[14] = 0;
    send_data[15] = 0;
    send_data[16] = 0x01;
    send_data[17] = 0;
    for(i=0; i<18; i++)
		{
			check ^= send_data[i];
		}
		send_data[18] = check;
    
    send_data[19] = 0x16;
		num++;
		if(num>15)
		{
			num = 0;
		}
}

//��������
void HEART(u8* addr, u8* send_data)
{
		u8 check = 0;
		u8 i;
		send_data[0] = 0x68;
    send_data[1] = 0x32;
    send_data[2] = 0;
    send_data[3] = 0x32;
    send_data[4] = 0;
    send_data[5] = 0x68;
    send_data[6] = 0xd9;
    
    send_data[7] = addr[0];
    send_data[8] = addr[1];
    send_data[9] = addr[2];
    send_data[10] = addr[3];
    send_data[11] = addr[4];
    
    send_data[12] = 0x02;
    send_data[13] = 0x70+num;
    send_data[14] = 0;
    send_data[15] = 0;
    send_data[16] = 0x04;
    send_data[17] = 0;
    for(i=0; i<18; i++)
		{
			check ^= send_data[i];
		}
		send_data[18] = check;
    
    send_data[19] = 0x16;
		num++;
		if(num>15)
		{
			num = 0;
		}
}

void CONFIRM(u8* addr, u8* send_data)
{
	u8 check = 0;
	u8 i;
	send_data[0] = 0x68;
	send_data[1] = 0x32;
	send_data[2] = 0;
	send_data[3] = 0x32;
	send_data[4] = 0;
	send_data[5] = 0x68;
	send_data[6] = 0;
	send_data[7] = addr[0];
	send_data[8] = addr[1];
	send_data[9] = addr[2];
	send_data[10] = addr[3];
	send_data[11] = addr[4];
	send_data[12] = 0x02;
	send_data[13] = 0;
	send_data[14] = 0x65;
	send_data[15] = 0;
	send_data[16] = 0;
	send_data[17] = 0x01;
	send_data[18] = 0;
	for(i=0; i<19; i++)
	{
		check ^= send_data[i];
	}
	send_data[19] = check;
	
	send_data[20] = 0x16;
}
