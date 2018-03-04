void setPixel (uint16_t x, uint16_t y, uint8_t c) {
  if(x>319 && y > 239)return;
  uint16_t p = (x+y*320) / 8;
  uint8_t px = 0;
  
  if (x%8 == 7)  px = 1;
  if (x%8 == 6)  px = 2;
  if (x%8 == 5)  px = 4;
  if (x%8 == 4)  px = 8;
  if (x%8 == 3)  px = 16;
  if (x%8 == 2)  px = 32;
  if (x%8 == 1)  px = 64;
  if (x%8 == 0)  px = 128;
   
  if(c==0){
    lcdDataArray[p + 320*240*0/8]&=~px;lcdDataArray[p + 320*240*1/8]&=~px;lcdDataArray[p + 320*240*2/8]&=~px;
  }
  if(c==1){
    lcdDataArray[p + 320*240*0/8]|=px;lcdDataArray[p + 320*240*1/8]&=~px;lcdDataArray[p + 320*240*2/8]&=~px;
  }
  if(c==2){
    lcdDataArray[p + 320*240*0/8]|=~px;lcdDataArray[p + 320*240*1/8]|=px;lcdDataArray[p + 320*240*2/8]&=~px;
  }
  if(c==3){
    lcdDataArray[p + 320*240*0/8]|=px;lcdDataArray[p + 320*240*1/8]|=px;lcdDataArray[p + 320*240*2/8]|=px;
  }
};