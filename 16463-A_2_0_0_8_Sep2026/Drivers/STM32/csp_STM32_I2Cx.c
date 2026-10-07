/**********************************************************************************************************
 *  Marturion Electronics Ltd
 *
 *  Knockmore Hill Business Park
 *  9 Ferguson Drive
 *  Lisburn
 *  Co. Antrim
 *  Northern Ireland
 *  BT28 2EX
 *
 *  Copyright 2017, Marturion Electronics Ltd
 *  All Rights Reserved
 *
 * Filename    :  csp_STM32_I2Cx.c
 * Date Created:  Wed 04 Oct 2017 03:24:40 PM
 * Programmer  :  William Paul
 * Description :  This module is used for
 *
 **********************************************************************************************************/
#include <stdio.h>

#include "csp_STM32_I2Cx.h"

Status I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction);



/**********************************************************************************************************
 * Function Name : init_i2c1
 * Description   : This function is used to initalise I2C1
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 * Version	Date d/m/y      Programmer          Reason for Change
 * 1.0.0	03/10/2017      Aaron Lynn          Original Created
 **********************************************************************************************************/
void I2C1_init(void)
{
	GPIO_InitTypeDef 	GPIO_InitStruct;
	I2C_InitTypeDef 	I2C_InitStruct;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);	// enable APB1 peripheral clock for I2C1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	// enable clock for SCL and SDA pins

	GPIO_InitStruct.GPIO_Pin =
		GPIO_Pin_6 |
		GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode	= GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);	// Enable I2C1 reset state
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);	// Release I2C1 from reset state

	// configure I2C1
	I2C_InitStruct.I2C_ClockSpeed			= 100000; 		        		// clock speed
	I2C_InitStruct.I2C_Mode					= I2C_Mode_I2C;					// I2C mode
	I2C_InitStruct.I2C_DutyCycle			= I2C_DutyCycle_2;	        	// 50% duty cycle --> standard
	I2C_InitStruct.I2C_OwnAddress1			= 0x00;							// own address, not relevant in master mode
	I2C_InitStruct.I2C_Ack					= I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress	= I2C_AcknowledgedAddress_7bit; // set address length to 7 bit addresses

	I2C_Init(I2C1, &I2C_InitStruct);
	I2C_Cmd(I2C1, ENABLE);

	return;
}

/**********************************************************************************************************
 * Function Name : init_I2C2
 * Description   : This function is used to initalise I2C2
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 * Version	Date d/m/y      Programmer          Reason for Change
 * 1.0.0	03/10/2017      Aaron Lynn          Original Created
**********************************************************************************************************/
void I2C2_init(void)
{
	GPIO_InitTypeDef 	GPIO_InitStruct;
	I2C_InitTypeDef 	I2C_InitStruct;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);	// enable APB1 peripheral clock for I2C2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	// enable clock for SCL and SDA pins

	GPIO_InitStruct.GPIO_Pin =
		GPIO_Pin_10 |
		GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode 	= GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, ENABLE);	// Enable I2C2 reset state
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, DISABLE);	// Release I2C2 from reset state

	I2C_InitStruct.I2C_ClockSpeed 			= 10000; 		        		// clock speed
	I2C_InitStruct.I2C_Mode 				= I2C_Mode_I2C;					// I2C mode
	I2C_InitStruct.I2C_DutyCycle 			= I2C_DutyCycle_2;	       		// 50% duty cycle --> standard
	I2C_InitStruct.I2C_OwnAddress1 			= 0x00;							// own address, not relevant in master mode
	I2C_InitStruct.I2C_Ack 					= I2C_Ack_Enable;
	I2C_InitStruct.I2C_AcknowledgedAddress 	= I2C_AcknowledgedAddress_7bit; // set address length to 7 bit addresses

	I2C_Init(I2C2, &I2C_InitStruct);
	I2C_Cmd(I2C2, ENABLE);

	return;
}


/**********************************************************************************************************
 * Function Name : I2C_start
 * Description   : This function is used to start I2C2
 * Arguments     : None
 * Returns       : None
 * Notes         : None
 * Version	Date d/m/y      Programmer          Reason for Change
 * 1.0.0	03/10/2017      Aaron Lynn          Original Created
 **********************************************************************************************************/
Status I2C_start(I2C_TypeDef* I2Cx, uint8_t address, uint8_t direction)
{
	uint16_t	tout;

	I2C_GenerateSTART(I2Cx, ENABLE);
	tout = 0xFFFF;
	while ((I2Cx->SR1&0x0001) != 0x0001){	// Wait until SB flag is set: EV5
		if (tout-- == 0){
			return(Error);
		}
	}

	I2C_Send7bitAddress(I2Cx, address, direction);
	tout = 0xFFFF;
	while ((I2Cx->SR1&0x0002) != 0x0002){
		if (tout-- == 0){
			return(Error);
		}
	}
	return(Success);
}




/*************************************************************************************************
* Function Name :
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		04/10/17	W. Paul			Created
*************************************************************************************************/
Status I2C_BufferRd(I2C_TypeDef* I2Cx, uint8_t* pBuffer, uint32_t NumByteToRead, uint8_t SlaveAdd_8bit)
{
	Status				stat;
	__IO uint32_t		temp;

    I2Cx->CR2 |= I2C_IT_ERR;					// Enable I2C errors interrupts

	if (NumByteToRead == 1){
		stat	=  I2C_start(I2Cx,SlaveAdd_8bit,I2C_Direction_Receiver);
		if(stat == Error){	return(Error);	}

		I2C_AcknowledgeConfig(I2Cx, DISABLE);
		I2C_AcknowledgeConfig(I2Cx, DISABLE);				// Clear ACK bit
		__disable_irq();						// Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3 software sequence must complete before the current byte end of transfer
		temp = I2Cx->SR2;						// Clear ADDR flag
		I2C_GenerateSTOP(I2Cx, ENABLE);				// Program the STOP
		__enable_irq();							// Re-enable IRQs

		while ((I2Cx->SR1 & 0x00040) != 0x000040);	// Wait until a data is received in DR register (RXNE = 1) EV7

		*pBuffer = I2Cx->DR;					// Read the data
		while ((I2Cx->CR1&0x200) == 0x200);		// Make sure that the STOP bit is cleared by Hardware before CR1 write access
		I2C_AcknowledgeConfig(I2Cx, ENABLE);					// Enable Acknowledgement to be ready for another reception
	}
	else if (NumByteToRead == 2){
		I2Cx->CR1 |= CR1_POS_Set;				// Set POS bit
		stat	=  I2C_start(I2Cx,SlaveAdd_8bit,I2C_Direction_Receiver);
		if(stat == Error){	return(Error);	}

		__disable_irq();				// EV6_1: The acknowledge disable should be done just after EV6,that is after ADDR is cleared, so disable all active IRQs around ADDR clearing and ACK clearing
		temp = I2Cx->SR2;				// Clear ADDR by reading SR2 register
		I2C_AcknowledgeConfig(I2Cx, DISABLE);			// Clear ACK
		__enable_irq();					// Re-enable IRQs

		while ((I2Cx->SR1 & 0x00004) != 0x000004);	// Wait until BTF is set

		__disable_irq();				// Disable IRQs around STOP programming and data reading because of the limitation ?

		I2C_GenerateSTOP(I2Cx, ENABLE);	// Program the STOP
		*pBuffer = I2Cx->DR;
		pBuffer++;
		__enable_irq();					// Re-enable IRQs

		*pBuffer = I2Cx->DR;
		pBuffer++;
		while ((I2Cx->CR1&0x200) == 0x200);	// Make sure that the STOP bit is cleared by Hardware before CR1 write access
		I2C_AcknowledgeConfig(I2Cx, ENABLE);			// Enable Acknowledgement to be ready for another reception
		I2Cx->CR1  &= CR1_POS_Reset;		// Clear POS bit
	}

	else{	//3 or more bytes to read
		stat	=  I2C_start(I2Cx,SlaveAdd_8bit,I2C_Direction_Receiver);
		if(stat == Error){	return(Error);	}

		temp = I2Cx->SR2;							//	Clear ADDR by reading SR2 status register
		while (NumByteToRead){
			if (NumByteToRead != 3){
				while ((I2Cx->SR1 & 0x00004) != 0x000004);	// Wait until BTF is set
				*pBuffer = I2Cx->DR;
				pBuffer++;
				NumByteToRead--;
			}
			if (NumByteToRead == 3){
				while ((I2Cx->SR1 & 0x00004) != 0x000004);	// Wait until BTF is set
				I2C_AcknowledgeConfig(I2Cx, DISABLE);						// Clear ACK

				__disable_irq();		// Disable IRQs around data reading and STOP programming because of the limitation ?
				*pBuffer = I2Cx->DR;
				pBuffer++;

				I2C_GenerateSTOP(I2Cx, ENABLE);					// Program the STOP
				*pBuffer = I2Cx->DR;
				pBuffer++;
				__enable_irq();								// Re-enable IRQs


				while ((I2Cx->SR1 & 0x00040) != 0x000040);	// Wait until RXNE is set (DR contains the last data)
				*pBuffer = I2Cx->DR;
				NumByteToRead = 0;
			}
		}

		while ((I2Cx->CR1&0x200) == 0x200);					// Make sure that the STOP bit is cleared by Hardware before CR1 write access
		I2C_AcknowledgeConfig(I2Cx, ENABLE);								// Enable Acknowledgement to be ready for another reception
	}
	return Success;
}



/*************************************************************************************************
* Function Name :
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		04/10/17	W. Paul			Created
*************************************************************************************************/
Status I2C_BufferWr(I2C_TypeDef* I2Cx, uint8_t* pBuffer, uint32_t NumByteToWrite, uint8_t SlaveAdd_8bit )
{
	Status			stat;
    __IO uint32_t	temp	= 0;


    I2Cx->CR2 |= I2C_IT_ERR;			// Enable Error IT (used in all modes: DMA, Polling and Interrupts

	stat	=  I2C_start(I2Cx,SlaveAdd_8bit,I2C_Direction_Transmitter);
	if(stat == Error){			return(Error);	}
    temp = I2Cx->SR2;					// Clear ADDR flag by reading SR2 register

    do{
    	I2Cx->DR = *pBuffer;
    	pBuffer++;
    	while ((I2Cx->SR1 & 0x00004) != 0x000004);		// EV8_2: Wait until BTF is set before programming the STOP
    }while(--NumByteToWrite);

    I2C_GenerateSTOP(I2Cx, ENABLE);						// Send STOP condition
    while ((I2Cx->CR1&0x200) == 0x200);				// Make sure that the STOP bit is cleared by Hardware

	return Success;
}


/*************************************************************************************************
* Function Name :
* Description   : 	This Function
* Arguments     : 	void
* Returns       : 	void
* Notes         : 	None
* Version	Date d/m/y	Programmer		Reason for Change
* 0.1.0		04/10/17	W. Paul			Created
*************************************************************************************************/
Status I2C_BufferWrRd(I2C_TypeDef* I2Cx, uint8_t* pBuffer, uint32_t NumByteToWrite,uint32_t NumByteToRead, uint8_t SlaveAdd_8bit )
{
	Status			stat;
    __IO uint32_t	temp	= 0;
	uint8_t* 		pBuffer_orr;

	pBuffer_orr		=  pBuffer;
    I2Cx->CR2 |= I2C_IT_ERR;			// Enable Error IT (used in all modes: DMA, Polling and Interrupts

	stat	=  I2C_start(I2Cx,SlaveAdd_8bit,I2C_Direction_Transmitter);
	if(stat == Error){			return(Error);	}
    temp = I2Cx->SR2;					// Clear ADDR flag by reading SR2 register

    do{
    	I2Cx->DR = *pBuffer;
    	pBuffer++;
    	while ((I2Cx->SR1 & 0x00004) != 0x000004);		// EV8_2: Wait until BTF is set before programming the STOP
    }while(--NumByteToWrite);

//    I2C_GenerateSTOP(I2Cx, ENABLE);						// Send STOP condition
//    while ((I2Cx->CR1&0x200) == 0x200);				// Make sure that the STOP bit is cleared by Hardware
	pBuffer	=  pBuffer_orr;
	stat	=  I2C_BufferRd(I2Cx, pBuffer, NumByteToRead, SlaveAdd_8bit);
	return Success;
}

/**********************************************************************************************************
 **********************************************************************************************************/
//end of file
