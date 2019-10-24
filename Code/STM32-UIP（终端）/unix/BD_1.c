#include "BD_1.h"
#include "uart.h"
#include "usart.h"
#include "stm32f10x_it.h"
//#include "delay.h"
#include "timer.h"
#include "delay.h"

//??
extern int ask_count;
extern struct timer BD_send_timer;
u8 ZJ[] = {'$', 'X', 'T', 'Z', 'J', 0x00, 0x0D, 0, 0, 0, 0, 0, 0x35};		   //????
u8 IC[] = {'$', 'I', 'C', 'J', 'C', 0,    0x0C, 0, 0, 0, 0, 0x2B};			   //IC??
u8 SD[228];
static unsigned char Origin_Data[256];
u8 ASK[10];
int ask_len = 0;
int momo_bd;
u8 NUM, LEN;
int Freq_commu=30;//�����̱��ķ���ʱ����
static unsigned char RxBuffer[100];
struct timer delay_timer0;
int BD_process_STA;
//??
Boolean BD_1_RX_FLAG = FALSE;
Boolean BD_1_DATA_FLAG = FALSE;
u8 data_0[100];

u8 BD_BUF_1[100];
int BD_BUF_1_LEN;
u8 BD_BUF_2[100];
int BD_BUF_2_LEN;
u8 BD_BUF_3[100];
int BD_BUF_3_LEN;
u8 BD_BUF_ASK[50];
int BD_BUF_ASK_LEN;


u8 BD_1_SIG;
u8 BD_1_ADDR[3] = {0x04, 0xE3, 0xC8};   //���������ն˵�ַ
//u8 BD_1_ADDR[3] = {0x02, 0x00, 0x87};
u8 BD_Count = 0; //??????

//�����Լ�
int BD_CMD_ZJ()
{
	int ix;
	
	UART_WRITE(13, ZJ);
	BD_1_SIG = 0;
	UART_DATA_GET();
	for (ix = 0; ix < 6; ix++)
	{
		BD_1_SIG += RxBuffer[14+ix];
	}

	if (BD_1_SIG >= 12)
	{
		return 1;
		printf("���ʼ��ϸ�\r\n");
	}
	else
	{
		return 0;
	}
	
}

//ϵͳ�Լ�
void BD_CMD_IC()
{
	int ix; 
	
	UART_WRITE(12, IC);
	UART_DATA_GET();
	
	for (ix = 0; ix < 3; ix++)
	{
		BD_1_ADDR[ix] = RxBuffer[ix+7];
	}
		printf("����IC��ַΪ:\r\n");
		for(momo_bd=0;momo_bd<3;momo_bd++)
		{
			printf("%02X ",BD_1_ADDR[momo_bd]);
		}
		printf("\r\n********************************************\r\n");
}


//BD1���ͱ��ķ�װ$TXSQ
/****
ADDR: Ŀ�ĵ�ַ
Data��ԭʼ����
len��ԭʼ���ݳ���
*****/
void BD_TXSQ_SEND(u8* ADDR, u8* Data, u16 len)
{
	u8 ix, sum;
	printf("���ڽ��б����̱��ķ�װ����\r\n");
	SD[0]='$';
	SD[1]='T';
	SD[2]='X';
	SD[3]='S';
	SD[4]='Q';
	
	SD[5]=((len + 19) & 0xFF00) >> 8;
	SD[6]=(len + 19) & 0x00FF;

	SD[7]=BD_1_ADDR[0];
	SD[8]=BD_1_ADDR[1];
	SD[9]=BD_1_ADDR[2];
	
	SD[10]=0x46;
	
	SD[11]=ADDR[0];
	SD[12]=ADDR[1];
	SD[13]=ADDR[2];

	SD[14]=(((len + 1) << 3) & 0xFF00) >> 8;
	SD[15]=((len + 1) << 3) & 0x00FF;
	
	SD[16]=0x00;
	SD[17]=0xA4;

	for (ix = 0; ix < len; ix++)
	{
		SD[ix + 18] = Data[ix];
	}
	
	sum = 0; 

	for (ix = 0; ix < len + 18; ix++)
	{
		sum ^= SD[ix]; 
	}

	SD[len + 18] = sum;
	printf("������װ��ɣ��������ͳ���Ϊ%d�ı���:\r\n",19+len);
	for(momo_bd=0;momo_bd<19+len;momo_bd++)
		{
			printf("%02X ",SD[momo_bd]);
		}
		printf("\r\n********************************************\r\n\r\n");
	UART_WRITE(19+len, SD);
}

/*
�������file_deal
origin��ԭʼ����
lenth��ԭʼ���ݳ���
n���������Ӱ�����
�Ӱ���Ŵ�01��ʼ

*/
void file_deal(u8 *origin, u8 lenth, u8 n)
{
/*****************?????********************************/
	u8 i=0;     //????
	u8 k, l, j;  //??
	u8 m = n;
	u8 ADDR[3] = {0x04, 0xE3, 0xC9};	
	struct timer delay_timer;
	printf("���ڲ�ֳ���Ϊ%d�ı��ġ���\r\n",lenth);
	
	NUM = n;    //�Ӱ�����
	LEN = lenth;
	i = lenth/n + 1;
  printf("���α��Ľ������Ϊ%d���Ӱ�,�Ӱ���Ŵ�01��ʼ����\r\n",i);
	
	for(j=0; j<lenth; j++)
	{
		Origin_Data[j] = origin[j];
	}
	
	//�ְ�
	for(k=0;k<i;k++)	//????
	{
		printf("���ڴ����%d���Ӱ�����\r\n",k+1);
		data_0[0] = 0xff;
		data_0[1] = 0xff;
		data_0[2]=k+1;		//�Ӱ���
		data_0[3]=i;		//�Ӱ�����
		if (lenth-k*m<m)
		{
			m=lenth-k*l;
		}

		for(l=0;l<m;l++)		//????
		{
			data_0[4+l]=origin[n*k+l];
		}
		if(k==0)
		{
			for(momo_bd=0;momo_bd<l+4;momo_bd++)
			BD_BUF_1[momo_bd]=data_0[momo_bd];
			BD_BUF_1_LEN=l+4;
		}
		else if(k==1)
		{
			for(momo_bd=0;momo_bd<l+4;momo_bd++)
			BD_BUF_2[momo_bd]=data_0[momo_bd];
			BD_BUF_2_LEN=l+4;
		}
		else if (k==2)
		{
			for(momo_bd=0;momo_bd<l+4;momo_bd++)
			BD_BUF_3[momo_bd]=data_0[momo_bd];
			BD_BUF_3_LEN=l+4;
		}
		else
			printf("�Ӱ���Ŀ����3���������д�����\r\n");
	}
	
	//���Ͳ�ѯ��ASK
	printf("���ڷ�װASK���ġ���\r\n");
	ASK[0] = 0xff;
	ASK[1] = 0xff;
	ASK[2] = 0;   
  ASK[3] = i;      	
	for(k=4; k<4+i; k++) 
	{
		ASK[k]=0x00;
	}
	ask_len = k;
	
	for(momo_bd=0;momo_bd<ask_len;momo_bd++)
	BD_BUF_ASK[momo_bd]=ASK[momo_bd];
	BD_BUF_ASK_LEN=ask_len;
  timer_restart(&BD_send_timer);	
	
}



//BD1���ս�����$TXXX   
/*
origin�����յ�������
recieve_data�������������ĺ������
*/
int BD_read(u8* origin, u8* recieve_data)
{
	unsigned char hol_len[2], my_addr[3], info_cate, send_addr[3], from_addr[3];
	unsigned char send_time[2], len[2], crc, check;
	int lenth=0;
	int n;
	int flag=0;
  printf("���ڽ��б������Ľ����Եõ������������ݡ���\r\n");
		
		if(flag==0&&origin[0]=='$') 
		{
			flag++;
		}
		if(flag==1&&origin[1]=='T')
		{
			flag++;
		}
		else
		{
			flag=0;
		}
		if(flag==2&&origin[2]=='X')
		{
			flag++;
		}
		else
		{
			flag=0;
		}
		if(flag==3&&origin[3]=='X')
    {
      flag++;
    }
    else
    {
       flag=0;
		}
		if(flag==4&&origin[4]=='X')
		{
			flag=0;
			hol_len[0]=origin[5];          //??????
			hol_len[1]=origin[6];          //??????
			my_addr[0]=origin[7];          //????
			my_addr[1]=origin[8];
			my_addr[2]=origin[9];
			info_cate=origin[10];          //????
			send_addr[0]=origin[11];       //????
			send_addr[1]=origin[12];
			send_addr[2]=origin[13];
			send_time[0]=origin[14];       //????h
			send_time[1]=origin[15];       //????m
			len[0]=origin[16];             //??????
			len[1]=origin[17];             //??????
			lenth=((len[0]&0x00ff)<<8)|(len[1]&0x00ff);//????bit
			lenth=lenth/8;		//??????
			for(n=0;n<lenth;n++)
			{
				recieve_data[n]=origin[18+n];     //????
			}
			crc=origin[18+lenth];          //CRC??
			check=origin[18+lenth+1];      //???
			
			for(n=0;n<3;n++)		//???????
			{
				from_addr[n]=send_addr[n];
			}
			
			printf("���е������ݱ���Ϊ:\r\n");
			for(momo_bd=0;momo_bd<lenth;momo_bd++)
			{
				printf("%02X ",recieve_data[momo_bd]);
			}
			printf("\r\n********************************************\r\n");

			return lenth;
		}
		else if(origin[1]=='F'&&origin[2]=='K')
		{
			flag=0;
			printf("����������Ϣ\r\n");
			return -1;
		}
		else
		{
			flag=0;
			printf("��Ч��Ϣ\r\n");
			return -1;
		}
		
		
}
/*
�����ش����� BD_retransmission(unsigned char len,unsigned char *data)
len��data�����ݳ���
data��ACK���Ľ���������� A4 FF FF ��������
*/
void BD_retransmission(unsigned char len,unsigned char *data)
{
	unsigned char i, k, sum, j;
	struct timer delay_timer1;
	u8 ADDR[3] = {0x04, 0xE3, 0xC9};	  
	if((data[1]) == 0xff & data[2] == 0xff)//�ش�����
	{
		if(data[3] > 0)
		{
			printf("���ι����ش�%d���Ӱ�\r\n",data[3]);
			while(!timer_expired(&delay_timer0));  //
			for(i=4; i<(4+data[3]); i++)
			{
				printf("�����ش���%d���Ӱ�,�Ӱ���Ϊ%d\r\n",i-3,data[i]);
				if(data[i]*NUM > LEN)  //���һ���Ӱ�
				{
					sum = LEN - (data[i]-1)*NUM; //������
				}
				else
				{
					sum = NUM; // �������һ���Ӱ����������=�ְ�����
				}
				data_0[0] = data[1]; //FF
				data_0[1] = data[2]; //FF
				data_0[2] = data[i]; //�Ӱ���
				data_0[3] = LEN/NUM + 1; //�Ӱ�����
				for(k=0; k<sum; k++)
				{
					data_0[4+k] = Origin_Data[(data[i]-1)*NUM+k];
				}
				printf("���ķ�װ��ɡ���\r\n");
				
				if(data[i]==1)
					{
						for(momo_bd=0;momo_bd<sum+4;momo_bd++)
						BD_BUF_1[momo_bd]=data_0[momo_bd];
						BD_BUF_1_LEN=sum+4;
					}
				else if(data[i]==2)
					{
						for(momo_bd=0;momo_bd<sum+4;momo_bd++)
						BD_BUF_2[momo_bd]=data_0[momo_bd];
						BD_BUF_2_LEN=sum+4;
					}
				else if (data[i]==3)
					{
						for(momo_bd=0;momo_bd<sum+4;momo_bd++)
						BD_BUF_3[momo_bd]=data_0[momo_bd];
						BD_BUF_3_LEN=sum+4;
					}
				else
						printf("�Ӱ���Ŀ����3���������д�����\r\n");				
			}
				BD_BUF_ASK_LEN=ask_len;	
				ask_count=0;
		}
		//�޶������ش��������������½����ն�������
		else 
		{
			BD_process_STA=0;
			printf("�յ�ACK��ʾ�����ش����ݰ�,�����ݴ�����̽���,�����ٴη�������\r\n");
		}
	}
}

//�õ������жϽ��յõ������ݷ���Rx_Bufger�У�δʹ��
void UART_DATA_GET()
{
	//using a static temp,to prevent the variable dead before the UART_WRITE compoleted
	int i;	
	//get the used size of the fifo
	i=UART4_RX_STA;
	
	if(i >= 10)
	{
		//Get the data,this function will return 0 when success,
		//but will return 1 while recevie invaild len.
		//For savety ,before used the data of temp,a check must be taken,such as
		// if(fifo_gets(&RX_FIFO,i,temp)){
		//		return 1 for invaild length;
		//	}
		//	else{
		//		success ,let produce the data;
		//	}
		printf("�յ���������Ϊ:\r\n");
		for(momo_bd=0;momo_bd<i;momo_bd++)
		{
			RxBuffer[momo_bd]=UART4_RX_BUF[momo_bd];
			printf("%02X ",RxBuffer[momo_bd]);
		}
		printf("\r\n********************************************\r\n");
	}

}

//�õ�����Ӧ�����õ�IP��ַ��δʹ��
void IP_ADDR_GET(u8* origin, u8* ip_data)
{
	
}
