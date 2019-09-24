#include "sim900a.h"
#include "usart.h"		
#include "delay.h"		 
//#include "malloc.h"
#include "string.h"    		
#include "uart.h" 


//////////////////////////////////////////////////////////////////////////////////	
extern u8 ip[20];
extern u8 por[5];
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//usmart֧�ֲ��� 
//���յ���ATָ��Ӧ�����ݷ��ظ����Դ���
//mode:0,������UART4_RX_STA;
//     1,����UART4_RX_STA;
void sim_at_response(u8 mode)
{
	if(UART4_RX_STA&0X8000)		//���յ�һ��������
	{ 
		UART4_RX_BUF[UART4_RX_STA&0X7FFF]=0;//��ӽ�����
		printf("%s",UART4_RX_BUF);	//���͵�����
		if(mode)UART4_RX_STA=0;
	} 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//ATK-SIM900A �������(���Ų��ԡ����Ų��ԡ�GPRS����)���ô���

//sim900a���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
u8* sim900a_check_cmd(u8 *str)
{
	char *strx=0;
	if(UART4_RX_STA&0X8000)		//���յ�һ��������
	{ 
		UART4_RX_BUF[UART4_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strstr((const char*)UART4_RX_BUF,(const char*)str);
	} 
	return (u8*)strx;   
	
}
//��sim900a��������
//cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
u8 sim900a_send_cmd(u8 *cmd,u8 *ack,u16 waittime)
{
	u8 res=0; 
	UART4_RX_STA=0;
	if((u32)cmd<=0XFF)
	{
		while((UART4->SR&0X40)==0);	//�ȴ���һ�����ݴ������   
		UART4->DR=(u32)cmd;
	}else u2_printf("%s\r\n",cmd);//��������
	printf("%s\r\n",cmd);
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(10);
			if(UART4_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(sim900a_check_cmd(ack))break;//�õ���Ч���� 
				UART4_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
} 
//��1���ַ�ת��Ϊ16��������
//chr:�ַ�,0~9/A~F/a~F
//����ֵ:chr��Ӧ��16������ֵ
u8 sim900a_chr2hex(u8 chr)
{
	if(chr>='0'&&chr<='9')return chr-'0';
	if(chr>='A'&&chr<='F')return (chr-'A'+10);
	if(chr>='a'&&chr<='f')return (chr-'a'+10); 
	return 0;
}
//��1��16��������ת��Ϊ�ַ�
//hex:16��������,0~15;
//����ֵ:�ַ�
u8 sim900a_hex2chr(u8 hex)
{
	if(hex<=9)return hex+'0';
	if(hex>=10&&hex<=15)return (hex-10+'A'); 
	return '0';
}


int GPRS_init(const u8 *port,u8 * ipaddr,int ctr )
{
	//port="8086";	//�˿ڹ̶�Ϊ8086,����ĵ���8086�˿ڱ���������ռ�õ�ʱ��,���޸�Ϊ�������ж˿�
//	u8 mode=0;			//0,TCP����;1,UDP����
  u8 p[100];
//	u8 timex=0;
  int TstRslt=0;	//ͨ�Ų��Խ��
	int i;
	//u8 ipbuf[16]="114.212.112.36"; 		//IP����
  for(i=ctr;i>0;i--)
	{
	while(sim900a_send_cmd("AT","OK",100))//����Ƿ�Ӧ��ATָ�� 
	{
		sim_at_response(1);
		printf("δ��⵽ģ��!!!\n"); 
		delay_ms(1000);
		printf("��������ģ��...\n"); 	
	 	delay_ms(1000);  
	}

  printf("������IP��ַ:%s �˿�:%s �����˿ڣ�8888 \r\n",ipaddr,port);
	
//����GPRS����
	printf("GPRS�������ڽ�������...\r\n");
  sim900a_send_cmd("AT+CIPSHUT","SHUT OK",100);	sim_at_response(1);	//�ر��ƶ����� 
	if(sim900a_send_cmd("AT+CGCLASS=\"B\"","OK",1000))continue;	sim_at_response(1);		//����GPRS�ƶ�̨���ΪB,֧�ְ����������ݽ��� 
	if(sim900a_send_cmd("AT+CGDCONT=1,\"IP\",\"CMNET\"","OK",1000))continue; sim_at_response(1); //����PDP������,��������Э��,��������Ϣ
	if(sim900a_send_cmd("AT+CGATT=1","OK",1000))continue;sim_at_response(1);					//����GPRSҵ��
	if(sim900a_send_cmd("AT+CIPCSGP=1,\"CMNET\"","OK",1000))continue;sim_at_response(1);//����ΪGPRS����ģʽ
	if(sim900a_send_cmd("AT+CIPHEAD=1","OK",1000))continue;sim_at_response(1); 				//���ý���������ʾIPͷ(�����ж�������Դ)
  if(sim900a_send_cmd("AT+CLPORT=\"TCP\",\"8888\"","OK",1000))continue;sim_at_response(1);//����TCP���ӱ��ض˿ں�Ϊ8888
////��������������
	printf("�������������������...\r\n");
	sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",\"%s\"",ipaddr,port);
	//printf("%s\r\n",p);
	while (sim900a_send_cmd(p,"OK",1000)) sim_at_response(1);		//��������
  sim_at_response(1);
  printf("���ӳɹ������Ͳ�����....\r\n");      
	while (sim900a_send_cmd("AT+CIPSEND",">",1000));		//��������
	
	sim_at_response(1);
	u2_printf("Hello,I'm from GPRS!");
	delay_ms(10);
	if(sim900a_send_cmd((u8*)0X1A,"SEND OK",1000)==0)
		TstRslt=1;//��ȴ�5s
	else
		TstRslt=0; 
	sim_at_response(1);
	delay_ms(1000); 
	
//	  sim900a_send_cmd((u8*)0X1B,0,0);	//ESC,ȡ������ 
	if(TstRslt)
	{
		printf("init ok!\r\n");
		ctr=0;
//		sim900a_send_cmd("AT+CIPSHUT","SHUT OK",100);	sim_at_response(1);	//�ر��ƶ����� 
		return 1;

	}
	UART4_RX_STA=0;	
	}	
	printf("init failed!\r\n");
	return 0;	
}

int GPRS_send(u8 *data,u16 len )
{
	
	u8 *p;
	u16 timex=0;
	u8 count=0;
	u8 connectsta=0;			//0,��������;1,���ӳɹ�;2,���ӹر�; 
	u8 hbeaterrcnt=0;			//�������������,����5�������ź���Ӧ��,����������

	UART4_RX_STA=0;

	printf("���ݷ�����....\r\n");
		  
			if(sim900a_send_cmd("AT+CIPSEND",">",500)==0)		//��������
			{ 
 				printf("CIPSEND DATA:%s\r\n",data);	 			//�������ݴ�ӡ������
				u2_printf("%s\r\n",data);
				printf("UART4_TX_BUF:%s\r\n",UART4_TX_BUF);
				delay_ms(10);
				if(sim900a_send_cmd((u8*)0X1A,"SEND OK",5000)==0)
				{
					printf("���ݷ��ͳɹ�\r\n");//��ȴ�10s
				  return 1;
				}
				else printf("���ݷ���ʧ��\r\n"); 
				delay_ms(500); 
			}else sim900a_send_cmd((u8*)0X1B,0,0);	//ESC,ȡ������ 

	
}

/////////////////////////////
//�����Ҫ��һ��fifo���������տ��԰ѽ��յ������ݴ浽Rx_GPRS��
void GPRS_rec()
{
	
}





void GPRS_close( )
{
	sim900a_send_cmd("AT+CIPCLOSE=1","CLOSE OK",500);	//�ر�����
	sim900a_send_cmd("AT+CIPSHUT","SHUT OK",500);		//�ر��ƶ����� 
	if(strstr((const char*)UART4_RX_BUF,"SHUT OK"))printf("GPRS closed!");
}


