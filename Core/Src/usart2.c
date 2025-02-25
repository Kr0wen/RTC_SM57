/*
 * usart2.c
 *
 *  Created on: 13 sept. 2019
 *      Author: aravey
 */

#include "stm32f4xx_hal.h"
#include "usart2.h"


// USART2 Initialization
void init_usart(void){

	//Initialize PA2 as USART2_TX and PA3 as USART2_RX
	//Activate A port
	SET_BIT(RCC->AHB1ENR,RCC_AHB1ENR_GPIOAEN);
    /* Setup PA2 and PA3 as Alternate Function */
	//MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER2, 0b01);
	//MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER3, 0b01);
	GPIOA->MODER |= 0x000000A0;
	GPIOA->MODER &= 0xFFFFFFAF;
	/* Setup Alternate function as USART2 */
	GPIOA->AFR[0] &= 0xFFFF77FF;
	GPIOA->AFR[0] |= 0x00007700;
	/* Push pull output */
	GPIOA->OTYPER &= 0xFFFFFFF3;
	/* Pull up resistor on */
	GPIOA->PUPDR &= 0xFFFFFF5F;
	GPIOA->PUPDR |= 0x00000050;
	/* Output speed set to VeryHigh */
	GPIOA->OSPEEDR |= 0x000000F0;


	//Activate USART2 Clock
	SET_BIT(RCC->APB1ENR,RCC_APB1ENR_USART2EN);
	//Enable USART, no TE no RE yet, Oversampling = 8, 8bit mode, no parity
	//Enable Tx and Rx
	USART2->CR1 = 0x0000800C;
	// No LIN mode, No clock output (synchronous mode)
	USART2->CR2 = 0x00000000;
	// No control mode, 3 sample point,
	USART2->CR3 = 0x00000000;
	// 19200bauds -> USARTDIV = 273.4375 -> Mantissa = 273d=0x111 , Fraction = 0.4375*16 = 7d = 0x7
	USART2->BRR = 0x00001117;
	//Enable UART
	USART2->CR1 = 0x0000A00C;
	//SET_BIT(USART2->CR1, USART_CR1_UE );

	HAL_Delay(1);
}

// USART2 Initialization with Rx interrupt
void init_usart_int(void){

	//Initialize PA2 as USART2_TX and PA3 as USART2_RX
	//Activate A port
	SET_BIT(RCC->AHB1ENR,RCC_AHB1ENR_GPIOAEN);
    /* Setup PA2 and PA3 as Alternate Function */
	GPIOA->MODER |= 0x000000A0;
	GPIOA->MODER &= 0xFFFFFFAF;
	/* Setup Alternate function as USART2 */
	GPIOA->AFR[0] &= 0xFFFF77FF;
	GPIOA->AFR[0] |= 0x00007700;
	/* Push pull output */
	GPIOA->OTYPER &= 0xFFFFFFF3;
	/* Pull up resistor on */
	GPIOA->PUPDR &= 0xFFFFFF5F;
	GPIOA->PUPDR |= 0x00000050;
	/* Output speed set to VeryHigh */
	GPIOA->OSPEEDR |= 0x000000F0;


	//Activate USART2 Clock
	SET_BIT(RCC->APB1ENR,RCC_APB1ENR_USART2EN);
	//Enable USART, no TE no RE yet, Oversampling = 8, 8bit mode, no parity, Rx interrupt enable
	//Enable Tx and Rx
	USART2->CR1 = 0x0000802C;
	// No LIN mode, No clock output (synchronous mode)
	USART2->CR2 = 0x00000000;
	// No control mode, 3 sample point,
	USART2->CR3 = 0x00000000;
	// 19200bauds -> USARTDIV = 273.4375 -> Mantissa = 273d=0x111 , Fraction = 0.4375*16 = 7d = 0x7
	USART2->BRR = 0x00001117;
	//Enable UART
	USART2->CR1 = 0x0000A02C;
	//SET_BIT(USART2->CR1, USART_CR1_UE );

	HAL_Delay(1);

	NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
	NVIC_EnableIRQ(USART2_IRQn);
}

void serial_putc(char c)
{
	//Load data to register
	USART2->DR = c;
	while(!(USART2->SR & 0x00000080));
	while(!(USART2->SR & 0x00000040));
}

void serial_puts( char *msg)
{
	int cnt = 0;;
	while(msg[cnt] != '\0'){
		serial_putc(msg[cnt]);
		cnt++;
	}
}

void newLine()
{
	serial_putc(10); // nouvelle ligne
	serial_putc(13); // retour chariot
}


signed int string2int(char *s)
{
   signed int result;
   unsigned int sign, index;
   char c;
   index = 0;
   sign = 0;
   result = 0;
   c = s[index];
   index++;
   if (c == '-')
   {
      sign = 1;
      c = s[index];
		index++;
   }
   else if (c == '+')
   {
      c = s[index];
	  index++;
   }
   if (c >= '0' && c <= '9')
   {
	 while (c >= '0' && c <= '9')
	 {
		result = 10*result + (c - '0');
		c = s[index];
		index++;
	 }
   }
   if (sign == 1) result = -result;
   return(result);
}


char * int2string(signed int num, char * s)
{
     unsigned int temp=1;
     unsigned int i,sign=0,cnt=0;
     char c;

     if(num<0) {  									// Traitement du signe
         sign=1;
         num*=-1;
     }
     while(temp>0) {

         s[cnt]=(num%10)+'0';    					// Conversion

         cnt++;

		 temp=(num/10);
         num=temp;
     }
     if(sign==1) {
         s[cnt]=0x2D;      							// caract�re '-'
         cnt++;
     }
     for(i = 0;i<(int)(cnt/2);i++) {

         c=s[i];
         s[i]=s[cnt-i-1];        					// inverse le nombre
         s[cnt-i-1]=c;
     }
     s[cnt]='\0';     								// met un terme � la cha�ne de caract�res
     return s;
}

char * float2string(float nombre,char *chaine)
{
	double nbf=nombre;
	int inc=0,i=0,end=0;
	int nb;

	if(nbf<0)
	{
		chaine[0]='-';
		nbf=-nbf;
		++i;
	}

	while(nbf>=1)
	{
		nbf/=10;
		++inc;
	}

	while(!end)
	{
		nbf*=10;
		nb=(int)nbf;

		if(inc == -2) end=1; // convertie jusqu'� 2 chiffres apr�s la virgule

		nb%=10;

		if(inc==0)
		{
			chaine[i]='.';
			++i;
		}
		chaine[i]=nb+'0';
		inc--;
		i++;
	}

	chaine[i]='\0';
	return(chaine);
}
