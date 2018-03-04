/******************************************************************************
 * 
 * Project:  Graphic LCD Controller using ARM STM32F4
 * Purpose:  Drive monochrome LCD graphic screens of 320x240 pixels
 * Author:   Maarten Plieger
 * Date:     2018-03-04
 *
 ******************************************************************************
 *
 * Copyright 2013, Maarten Plieger
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 ******************************************************************************/

#include "stm32f4_discovery.h"
#include "stm32f4xx_tim.h"
#include "misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stm32f4xx_spi.h>
#include <time.h>
#include <stdlib.h>

/* Port mappings */
#define LCD_D0 GPIO_Pin_0
#define LCD_D1 GPIO_Pin_1
#define LCD_D2 GPIO_Pin_2
#define LCD_D3 GPIO_Pin_3
#define LCD_FLM GPIO_Pin_4
#define LCD_BACKLIGHT_LEDA GPIO_Pin_5 // Low is on, On the display resistor R27 is removed and replaced by transistor BC557 with a resistor of 470 Ohm to base
#define LCD_LP GPIO_Pin_6
#define LCD_CP GPIO_Pin_7

/* Define frame buffer */
#define DMAbufferSizeRx ((320*240*3)/8) + 1
uint8_t dmaisactive = 0;
__IO uint8_t lcdDataArray[DMAbufferSizeRx];

/* Helper functions */
#include "spi_dma.h"
#include "rcc_set_frequency.h"
#include "setup_ports.h"
#include "setup_timers.h"
#include "setPixel.h"

/* Global variables */
uint16_t dmatimer1 = 0;
uint16_t dmatimer2 = 0;
uint8_t dmatimehasreachedtreshold = 0;
uint8_t dmatimehastresholdtriggered = 0;
static uint16_t lcdYCounter = 0;
static uint8_t lcdCmd = LCD_BACKLIGHT_LEDA;

/* Set init image on screen, used to display something after reset */
void setInitDisplayImage () {
  int x, y;
  for(y=0;y<239;y++){
    for(x=0;x<320;x++){
      setPixel(x,y,x/80);
    }
  }
  for(y=10;y<200;y++){
    setPixel(y,y,3);
    setPixel(y,239-y,3);
    setPixel(319-y,y,3);
    setPixel(319-y,239-y,3);
  }
  for(x=0;x<320;x++){
    setPixel(x,0,3);
  }
}

/* Interrupt handler from timer */
void TIM3_IRQHandler(void) {
  uint16_t lcdDataPointer;
  uint8_t lcdXCounter;  
  uint8_t lcdDataTemp;
  
  if (TIM_GetITStatus(TIM3, TIM_IT_CC1) != RESET) {
    TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
  }

  if ((lcdYCounter % 240) == 0){
    /* Write scan start up pulse, Latch one vertical line in */
    GPIO_SetBits  (GPIOD, LCD_FLM); // FLM High
    GPIO_SetBits  (GPIOD, LCD_LP);  // Latch High
    GPIO_ResetBits(GPIOD, LCD_LP);  // Latch low
    GPIO_ResetBits(GPIOD, LCD_FLM); // FLM low
  }
  
  lcdDataPointer = lcdYCounter * (320/8);
  
  for(lcdXCounter=0;lcdXCounter<320/8;lcdXCounter++) {
    uint8_t memData = lcdDataArray[lcdDataPointer++];
    memData = ((memData << 4) | (memData >> 4));
    lcdDataTemp = (memData&15)     |lcdCmd|LCD_CP;
    GPIO_Write(GPIOD,  lcdDataTemp);
    GPIO_ResetBits(GPIOD,   LCD_CP); 
    lcdDataTemp = ((memData>>4)&15)|lcdCmd|LCD_CP;
    GPIO_Write(GPIOD,  lcdDataTemp);
    GPIO_ResetBits(GPIOD,   LCD_CP); 
  }
  GPIO_SetBits(GPIOD,   LCD_LP);  // Latch High
  GPIO_ResetBits(GPIOD, LCD_LP);  // Latch low
   
  lcdYCounter++; 
  if (lcdYCounter>=240*3) {
    lcdYCounter = 0;
    if(dmaisactive == 1) {
      dmaisactive = 0;
      dmatimer1 = 0;
      dmatimehasreachedtreshold = 0;
      dmatimehastresholdtriggered = 0;
    } else if(dmatimehasreachedtreshold == 0) {
      dmatimer1++;
      if(dmatimer1>20)dmatimehasreachedtreshold = 1;
    }
  }
}

/* Entry point of program */
int main(void) {
  uint8_t byteCounter = 0;
  rcc_set_frequency(SYSCLK_240_MHZ);
  setup_ports();
  setInitDisplayImage();
  DMAconfig();
  init_SPI1();
  setup_timers();    

  while(1){
    if (dmatimehasreachedtreshold == 1 && dmatimehastresholdtriggered == 0){
      dmatimehastresholdtriggered = 1; 
      DMAconfig();
      init_SPI1();
     
    } else {
      /* Last byte controlls backlight of screen (0 is off, 255 is on) */
      if(lcdDataArray[DMAbufferSizeRx - 1] == 0) {
        lcdCmd |= LCD_BACKLIGHT_LEDA;
      } else {
        lcdCmd &= ~LCD_BACKLIGHT_LEDA;
      }
    }
    if(dmatimehastresholdtriggered == 1) {
      if(byteCounter>128){
        lcdDataArray[0 + 320*240*0/8]=15 ;lcdDataArray[0 + 320*240*1/8]=15 ;lcdDataArray[0 + 320*240*2/8]=15 ;
      }else {
        lcdDataArray[0 + 320*240*0/8]=240 ;lcdDataArray[0 + 320*240*1/8]=240 ;lcdDataArray[0 + 320*240*2/8]=240 ;
      }
    }
    /* Blink status led on development board to indicate flashing went OK*/
    if(byteCounter>128)GPIO_ResetBits(GPIOE, 1); else GPIO_SetBits(GPIOE, 1); 
    byteCounter++;
  } 
  return 0;
}
