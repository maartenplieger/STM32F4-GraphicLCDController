#include <stm32f4xx_spi.h>


#include <stm32f4xx_dma.h>


void DMAconfig(void)
{
RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_DMA2, ENABLE);

NVIC_InitTypeDef NVIC_InitStructure;

/* Enable the DMA Stream IRQ Channel */
NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream2_IRQn;
NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
NVIC_Init(&NVIC_InitStructure);

DMA_InitTypeDef DMA_InitStructure;

DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;

DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(SPI1->DR));
DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
DMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
DMA_InitStructure.DMA_Priority = DMA_Priority_High;
DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

/* Configure Rx DMA */
DMA_InitStructure.DMA_Channel = DMA_Channel_3;
DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &lcdDataArray[0];
DMA_InitStructure.DMA_BufferSize = DMAbufferSizeRx;
DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
DMA_Cmd(DMA2_Stream2, DISABLE);
while (DMA2_Stream2->CR & DMA_SxCR_EN);
DMA_Init(DMA2_Stream2, &DMA_InitStructure);



DMA_ITConfig(DMA2_Stream2, DMA_IT_TC , ENABLE); //| DMA_IT_HT

/* Enable the DMA channel */

DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_FEIF2|DMA_FLAG_DMEIF2|DMA_FLAG_TEIF2|DMA_FLAG_HTIF2|DMA_FLAG_TCIF2);
//DMA_ClearFlag(DMA2_Stream5, DMA_FLAG_FEIF5|DMA_FLAG_DMEIF5|DMA_FLAG_TEIF5|DMA_FLAG_HTIF5|DMA_FLAG_TCIF5);


DMA_Cmd(DMA2_Stream2, ENABLE); // Enable the DMA SPI TX Stream
//DMA_Cmd(DMA2_Stream5, ENABLE); // Enable the DMA SPI RX Stream


// Enable the SPI Rx/Tx DMA request
SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
// SPI_I2S_DMACmd(SPI3, SPI_I2S_DMAReq_Tx, ENABLE);

SPI_Cmd(SPI1, ENABLE);

}




uint16_t dmap = 0;

void DMA2_Stream2_IRQHandler(void)  
{
  /* Test on DMA Stream Transfer Complete interrupt */
  if(DMA_GetITStatus(DMA2_Stream2, DMA_IT_TCIF2))
  {
    /* Clear DMA Stream Transfer Complete interrupt pending bit */
    DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);
    dmaisactive=1;
    

  }
}

// this function initializes the SPI1 peripheral
void init_SPI1(void){
	
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;
	
	// enable clock for used IO pins
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	/* configure pins used by SPI1
	 * PA5 = SCK
	 * PA6 = MISO
	 * PA7 = MOSI
	 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// connect SPI1 pins to SPI alternate function
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
	
	// enable clock for used IO pins
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
// 	/* Configure the chip select pin
// 	   in this case we will use PE7 */
// 	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
// 	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
// 	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
// 	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
// 	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
// 	GPIO_Init(GPIOE, &GPIO_InitStruct);
// 	
// 	GPIOE->BSRRL |= GPIO_Pin_7; // set PE7 high
	
	// enable peripheral clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	/* configure SPI1 in Mode 0 
	 * CPOL = 0 --> clock is low when idle
	 * CPHA = 0 --> data is sampled at the first edge
	 */
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // set to full duplex mode, seperate MOSI and MISO lines
	SPI_InitStruct.SPI_Mode = SPI_Mode_Slave;     // Receive in slave mode
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b; // one packet of data is 8 bits wide
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;        // clock is low when idle
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;      // data sampled at first edge
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft ;//| SPI_NSSInternalSoft_Set; // set the NSS management to internal and pull internal NSS high
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // SPI frequency is APB2 frequency / 4
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;// data is transmitted MSB first
	SPI_Init(SPI1, &SPI_InitStruct); 
        
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
        SPI1->CR1 |= SPI_NSSInternalSoft_Set;
        SPI1->CR1 &= SPI_NSSInternalSoft_Reset;
        SPI_Cmd(SPI1, ENABLE);
}

