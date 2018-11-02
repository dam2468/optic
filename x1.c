#include   <REGX51.H>
#include   <intrins.h>
#include   <STDIO.H>

#define  RX_R  P1_1
#define  TX_R  P1_0
#define  RX_M  P1_3
#define  TX_M  P1_2
#define  RX_L  P1_5
#define  TX_L  P1_4
float	SR2=0,SL2=0;
//-----------------------
#define  LEDR  P3_2
#define  LEDM  P3_3
#define  LEDL  P3_4

#define  MF0	P0_0
#define  MR0	P0_1
#define  MF1	P0_2
#define  MR1	P0_3
#define  CCNYR   P3_5
#define  CCNYM   P3_6
#define  CCNYL   P3_7
#define  LCNYL   P2_0
#define  LCNYM   P2_1
#define  LCNYR   P2_2
#define  RCNYL   P2_3
#define  RCNYM   P2_4
#define  RCNYR   P2_5
//------------------------
unsigned int  time=0;
unsigned int  timer=0;
float         SR=0,SM=0,SL=0;
bit           flag =0;
unsigned char stat=0,duty;
unsigned int temp=0;
#define  T    11059200/12/1000
bit onoff=0;
//計時超音波時間ms 為單位
void delay_1m(unsigned int dly)    
{
	int count;
	while(dly>0)
	{
		count=120;
		while(count>0)
		count--;
		dly--;
	}
}
//-----------------
void readUSR(void)
{
	 while(!RX_R);		//當RX為零時等待	
	 TR0=1;				//開啟計數			  
	 while(RX_R);		//當RX為1計數並等待		
	 TR0=0;				//關閉計數			
	 time=TH0*256+TL0;	
	 TH0=0;
	 TL0=0;
	 SR=(time*1.87)/100;    
	 if(flag==1)		    
	 {
	  	flag=0;
	 }
}
/********************************************************/
void readUSM(void)
{
	 while(!RX_M);		//當RX為零時等待	
	 TR0=1;				//開啟計數			    
	 while(RX_M);		//當RX為1計數並等待						
	 TR0=0;				//關閉計數			
	 time=TH0*256+TL0;
	 TH0=0;
	 TL0=0;
	 SM=(time*1.87)/100;     
	 if(flag==1)		   
	 {
	  	flag=0;
	 }
}
/********************************************************/
void readUSL(void)
{
	 while(!RX_L);	
	 TR0=1;			   
	 while(RX_L);		
	 TR0=0;			
	 time=TH0*256+TL0;
	 TH0=0;
	 TL0=0;
	 SL=(time*1.87)/100;    
	 if(flag==1)		
	 {
	  	flag=0;
	 }	
}
//---------------------------------
void zd0() interrupt 1 		 
{
    flag=1;							
}
//--------------------------------
void  StartUSR() 		        
{
	  TX_R=1;			               
	  _nop_(); 
	  _nop_(); 
	  _nop_();
	  TX_R=0;	  	  
}
/********************************************************/
void  StartUSM() 		       
{
	  TX_M=1;			               
	  _nop_(); 
	  _nop_(); 
	  _nop_();
	  TX_M=0;
}
/********************************************************/
void  StartUSL() 		        
{
	  TX_L=1;			                
	  _nop_(); 
	  _nop_(); 
	  _nop_();
	  TX_L=0;
}
/********************************************************/
void USR()
{
 	StartUSR();
  readUSR();			

}
/********************************************************/
void USM()
{
 	StartUSM();
  readUSM();		
	
}
/********************************************************/
void USL()
{
 	StartUSL();
  readUSL();		
	
}
/********************************************************/
void read_all_US()
{
	USL();
	USM();
	USR();
}
void delayms1(int x)
{
	char n1=0;
		
		while(n1<x)x--;
}
//-------------------------------
void Speed(unsigned char R,unsigned char L)
{
	unsigned	int n=0;
	while(n<=255)
	{
		if(R>n)
		{
			MF0=0;
			MR0=1;
		}
		else
		{
			MF0=0;
			MR0=0;
		}
		if(L>n)
		{
			MF1=0;
			MR1=1;
		}
		else
		{
			MF1=0;
			MR1=0;
		}
		n++;
		delay_1m(0);
	}
}
void Speed1(unsigned char R,unsigned char L)
{
	unsigned	int n=0;
	while(n<=255)
	{
		if(R>n)
		{
			MF0=0;
			MR0=1;
		}
		else
		{
			MF0=0;
			MR0=0;
		}
		if(L>n)
		{
			MF1=1;
			MR1=0;
		}
		else
		{
			MF1=0;
			MR1=0;
		}
		n++;
		delay_1m(0);
	}
}
void BREAK(void)
{
  TR1=0;	
  MF0=1;
  MR0=1;
  MF1=1;
  MR1=1;
}
void FORWARD(void)
{
  MF0=0;
  MR0=1;
  MF1=0;
  MR1=1;
}
void STOP(void)
{
  TR1=0;
  MF0=0;
  MR0=0;
  MF1=0;
  MR1=0;  
}
void SFORWARD(unsigned char duty1)
{
  TR1=1;
  duty=duty1%10; 
  if(duty==0)//將0-10的duty1指定給全域的duty
    duty=1;
  if(stat!=1)
  {
	onoff=0;
    temp=65536-(T*duty);
    TL1 = temp;        // 設定TL0與TH0
    TH1 = temp>>8;
    TR1=1;             // Timer0開始計時    
	stat=1;
  }
} 	
void T1_int(void) interrupt 3
{
  onoff=~onoff;
  if(!onoff)
  {
    temp=65536-(T*duty);
	if(stat==1)
	  FORWARD();
	else  
	  STOP();
  }
  else
  {
    temp=65535-(T*(10-duty));
	STOP();
  }
  TL1 = temp;         // 設定TL0與TH0
  TH1 = temp >>8;     // Timer0開始計時
  TR1=1;
}
//-----------------------------------------
void main(void)
{
	unsigned char a=0; 
	unsigned char o=1;
	unsigned int n1=0;
	unsigned char n2=0;
	unsigned char R=0;
	unsigned int R1=0;
	unsigned char L=0;
	unsigned int L1=0;
	unsigned char k=0;
	unsigned int k1=0;
	unsigned char Walls_R=0;
	unsigned char Walls_L=0;
	unsigned char change_R=0;
	unsigned char change_L=0;
	unsigned char Turn=0;
	TMOD=0x11;		   //設T0為方式1，GATE=1；
	TH1=0;
	TL1=0;
	TH0=0;
	TL0=0; 
	TR0=1;
	ET0=1;             //允許T0中斷
  ET1=1;
	TR0=1;
	TR1=1;			   //開啟計時器
	TI=1;
	EA=1;			   //開啟總中斷
	LEDR=1;
	LEDM=1;
	LEDL=1;
	

	while(1)
	{	

	
		if(CCNYR==1||CCNYM==1||CCNYL==1||LCNYL==1||LCNYM==1||LCNYR==1||RCNYL==1||RCNYM==1||RCNYR==1)
		{
			while(CCNYR==1||CCNYM==1||CCNYL==1||LCNYL==1||LCNYM==1||LCNYR==1||RCNYL==1||RCNYM==1||RCNYR==1)	  ;
			k++;
		};
		if(k==0)
		{
			SFORWARD(7);
		}		
		else if(k==1||k==3)
			{
				read_all_US();		
				if(SL<40)//左邊距離5~50
				{	 	
					LEDL=0;//可以  			
					BREAK();//煞車
					Speed(150,70);//車子右轉速度調整，可調整數值範圍為：0~255。
						while(SL<20)//左邊超音波第二次感測距離調整，可調整數值範圍為：5~50公分。
							{
								USL();
								Speed(160,50);//車子右轉速度調整，可調整數值範圍為：0~255。
								L++;
							}
				
					Turn=1;
					Walls_L++;
					Walls_R=0;
					if(change_R==1)
					{
						change_R=2;
					}
				
					a=0;
					LEDL=1;			//可以
				}
				else if(SR<40)//右邊超音波第一次感測距離調整，可調整數值範圍為：5~50公分。
				{
					LEDR=0;//可以
					BREAK();//煞車
					Speed(70,150);//車子左轉速度調整，可調整數值範圍為：0~255。
					while(SR<20)//右邊超音波第二次感測距離調整，可調整數值範圍為：5~50公分。
					{
						USR();
						Speed(50,160);//車子左轉速度調整，可調整數值範圍為:0~255。
           	R++;
					}
					if(change_L==1)
					{
						change_L=2;
					}
					Turn=1;	
					Walls_R++;
					Walls_L=0;
				
					a=1;
					LEDR=1;//可以
				}
				else if(SM<50)//中央感測器有障礙物    (可改距離)
				{	 	
					LEDM=0;//可以
					BREAK();//煞車
					while(SM<30)//中央超音波第二次感測距離調整，可調整數值範圍為：5~50公分。
					{
						USM();	    
						if(a==0)//-----迴轉
						{  
							Speed(20,180);//車子左轉速度調整，可調整數值範圍為：0~255。
							L1++;
							if(change_L==0)change_L=1;
						}
						else if(a==1)
						{
							Speed(180,20);//車子右轉速度調整，可調整數值範圍為：0~255。
							R1++;
							if(change_R==0)change_R=1;
						}
					}
					if(Turn==0)
					{
						Turn=1;
						if(a==0)
						{
								a=1;
								LEDM=1;
						}
						else
						{
							a=0;
							LEDM=1;
						}
						LEDM=1;
					}
				}	
				else // 都沒障礙 直線衝刺
				{
						if(Walls_L>3)
					{
						for(k1=0;k1<10;k1++){Speed(150,0);}
						SFORWARD(7);
						delayms1(100);
						for(k1=0;k1<5;k1++){Speed(50,150);}
						Walls_L=0;
					}
						if(Walls_R>3)
					{
						for(k1=0;k1<10;k1++){Speed(0,150);}
						SFORWARD(7);
						delayms1(100);
						for(k1=0;k1<5;k1++){Speed(150,50);}
						Walls_R=0;
					}
//------------------------
					Turn=0;
					SFORWARD(7); 
					delayms1(2);
					for(k1=0;k1<L;k1++)
					{
						Speed(50,180);
					}
					L=0;
					for(k1=0;k1<R;k1++)
					{
						Speed(180,50);
					}
					R=0;
					if(change_L==2)
					{
						for(k1=0;k1<L1+3;k1++)
						{
							Speed(200,50);
						}
						change_L=0;
					}
					L1=0;
					if(change_R==2)
					{	
						for(k1=0;k1<R1+3;k1++)
						{
							Speed(50,200);
						}
						change_R=0;
					}			
					
					R1=0;					
					LEDR=1;
					LEDM=1;
					LEDL=1;
					SFORWARD(7); 
				}
				SFORWARD(7);
			}
			else if(k==2)
			{
				if(o==1)
				{
					for(n1=0;n1<30;n1++)
					{
						SFORWARD(7);
						delayms1(50);
						delayms1(50);
						delayms1(20);
						o++;
					}
				}
				for(n1=0;n1<12;n1++)
				{
					Speed1(110,110);
				}

			}
			else
			{
				if(n2==0)
				{
					for(n1=0;n1<60;n1++)
					{
						SFORWARD(7);
						delayms1(60);
						delayms1(60);
						delayms1(20);
						n2++;
					}
				}
				BREAK();
			}
	}    
}
