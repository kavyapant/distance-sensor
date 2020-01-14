/*											   TEAM  11
 										    	
						        	DISTANCE SENSOR INTERFACING USING LCD

			                        MADE BY-
								 	NAME			REG NO.		ROLL NO.

								1.	KAVYA PANT		170911148		40
								

*/

#include<LPC17xx.H>
#include<stdio.h>
#define TRIG (1<<4)		//P0.4
#define ECHO (1<<5)		//P0.5
void delayUS(unsigned int);
void starttimer(void);
unsigned int stoptimer(void);
#define rs 27			// Register Select Pin for LCD
#define e 28			// Enable Pin for LCD
#define dt 23			// Data Lines for LCD
unsigned int y;
unsigned char commands[]={0x30,0x30,0x30,0x20,0x28,0x0C,0x06,0x01,0x80};
int echotime=0;
float dist_cm=0;
char strdist[50];
unsigned char Msg[20] = {"Distance:"};  
unsigned int flag,flag2,i,j,temp1,temp2;
void lcd_write(void);
void port_write(void);
void delay(int);

int main()
{
	SystemInit();
	SystemCoreClockUpdate();
	
	LPC_PINCON->PINSEL0 = 0xfffff0ff;	 //P0.4 and P0.5 as GPIO
	LPC_GPIO0->FIODIR |= TRIG; 			//p0.4 as output
	LPC_GPIO0->FIODIR &=~ECHO;			//echo is set as input at p0.5
	LPC_GPIO0->FIOCLR |=TRIG;			//TRIG pin is cleared
	LPC_PINCON->PINSEL1&=0xFC003FFF;	//P0.23 to P0.28 as GPIO for LCD
	LPC_GPIO0->FIODIR|=(1<<rs|1<<e|0xF<<dt);
	flag=0;
	for(i=0;i<9;i++){
	temp1=commands[i];				    //start and configure LCD
	lcd_write();}
	flag=1;
	while(1)
	{
		LPC_GPIO0->FIOPIN |=TRIG;		//give a HIGH pulse  to trig pin
		delayUS(10);				    // 10us delay
		LPC_GPIO0->FIOCLR |=TRIG;		//clear TRIG pin
		while(!(LPC_GPIO0->FIOPIN & ECHO)); //wait till echo pin becomes high
		starttimer();
		while(LPC_GPIO0->FIOPIN & ECHO);	//wait till echo pin becomes low
		echotime = stoptimer();
		dist_cm = (float)(0.0343 * echotime)/(float)(2.0);		//0.0343 cm/us		
		sprintf(strdist,"%3.2fcm", dist_cm);
		for(i=0;i<100000;i++);
		flag=1;
		for(i=0;i<9;i++){
		temp1=Msg[i];
		lcd_write();}	   //write "Distance:" to LCD
		for(i=0;i<6;i++)
		{
			temp1=strdist[i];
			lcd_write();			//write the calculated distance to LCD
		}
		flag=0;
		temp1=0x01;
		lcd_write();				 //clear the LCD
	}
}

void delayUS(unsigned int us)
{
	LPC_TIM0->TCR = 0x2;			 //reset timer
	LPC_TIM0->TCR = 0x1;			//enable timer
	while(LPC_TIM0->TC < us );		//timer runs till TC is less than delay needed
	LPC_TIM0->TCR = 0;				//disable timer
	LPC_TIM0->PR = 2;
	
}

void starttimer(void)
{
	LPC_TIM0->TCR = 0x2;
	LPC_TIM0->PR = 2;
	LPC_TIM0->TCR = 0x1;			//start timer	
}

unsigned int stoptimer(void)
{
	LPC_TIM0->TCR = 0;				//disable timer
	return LPC_TIM0->TC;		    //return current value of timer 
}

void lcd_write()
{
	flag2=(flag==1)?0:((temp1==0x30)||(temp1==0x20))?1:0;	  //check if 4-bit command
	temp2=temp1&0xF0;
	temp2=temp2>>4;
	port_write();
	if(!flag2){
		temp2=temp1&0xF;
		port_write();
		}
	delay(15000);
}

void port_write()
{
	LPC_GPIO0->FIOPIN=0;
	LPC_GPIO0->FIOPIN=temp2<<dt;
	if(flag==0)
		LPC_GPIO0->FIOCLR=1<<rs;		   //command
	else
		LPC_GPIO0->FIOSET=1<<rs;		   //data
	LPC_GPIO0->FIOSET=1<<e;
	delay(1500);
	LPC_GPIO0->FIOCLR=1<<e;
	delay(1500);
}

void delay(int a){
	for(j=0;j<a;j++);
}
	  
	
