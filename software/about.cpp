#include <SSD_13XX.h>

#include "_images/hw_sm.c"
#include "_images/js_sm.c"
#include "core.h"

extern SSD_13XX mydisp;
extern volatile byte btnid;
extern byte appmode;
extern byte btncounter;

void SysInfo()
{
  mydisp.clearScreen();
  mydisp.setCursor(0, 0);
  mydisp.setTextColor(GREEN);
  mydisp.setTextScale(1);
  
  mydisp.println(F("HTH 2019 Badge"));
  mydisp.println(F("  ver 1"));
  mydisp.println(F("  rel date 2019-06-14"));
  mydisp.println(F("  src on github.com"));
  mydisp.println();
  mydisp.print(F("SDK ver: ")); mydisp.println(ESP.getSdkVersion());
}

void HWInfo()
{
  mydisp.clearScreen();
  mydisp.setCursor(0, 0);
  mydisp.setTextColor(GREEN);
  mydisp.setTextScale(1);

  mydisp.print(F("CPU: "));
  mydisp.print(ESP.getCpuFreqMHz());
  mydisp.println(F(" MHz"));
  
  mydisp.println(F("Flash Chip Size: "));
  mydisp.println(ESP.getFlashChipSize());

  mydisp.print(F("Free RAM: "));
  mydisp.println(ESP.getFreeHeap());
}

void Credits()
{
  mydisp.clearScreen();
  mydisp.setRotation(0);
  mydisp.setTextColor(GREEN);
  int h,w,buffidx,row,col;
  h = 16;
  w = 16;
  buffidx = 0;
  for (row=0; row<h; row++) {
    for (col=0; col<w; col++) { 
//      mydisp.drawPixel(col, row, pgm_read_word(image_data_hw_sm + buffidx));
      buffidx++;
    } 
  }
  mydisp.print("HTHackers");
  mydisp.setCursor(1, 20);
  mydisp.print("hthackers.com");
  mydisp.println("");
  mydisp.println("");
  mydisp.println("@syn_ack_zack");
  mydisp.println("@mcm3nac3");
}

void Shouts() {
  appmode=1;
  byte count=1;
  byte last=0;
  btnid = 0;
  mydisp.setTextColor(WHITE);
  while (btnid != 4)
  {
    if (btnid == 3){ 
      if (count == 1) {count = 3; } 
      else {count--;}
      btnid = 0;
    }
    else if (btnid == 2){ 
      if (count == 3) {count = 1; } 
      else {count++;}
      btnid = 0;
    }
    else if (btnid == 4) {break;}

    if (count != last)
    {
      switch(count)
      {
       case 1:
          mydisp.clearScreen();
          mydisp.setCursor(0, 0);
          mydisp.setTextScale(2);
          mydisp.println(F("Thanks To:"));
          mydisp.setTextScale(1);
          mydisp.println(F(""));
          mydisp.println(F("HackerWarehouse"));
          mydisp.println(F("#badgelife"));

          break;
      }
      last=count;
    }
    delay(150);
  }
  appmode=0;
  btncounter++;
}



