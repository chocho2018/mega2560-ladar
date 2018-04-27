/*
This code is to teach you how to make the LCD display string with the library.
Maybe you need to set the font size, you can call this function "setFontSize()",and you can pass the parameters :
    FONT_SIZE_SMALL 
    FONT_SIZE_MEDIUM
    FONT_SIZE_LARGE
    FONT_SIZE_XLARGE
and if you want to change the font color,you can call this function "setColor()" with the parameters:
    RGB16_RED-------->RED 
    RGB16_GREEN------>GREEN 
    RGB16_BLUE------->BLUE 
    RGB16_YELLOW----->YELLOW 
    RGB16_CYAN------->CYAN 
    RGB16_PINK------->PINK 
    RGB16_WHITE------>WHITE 
 Created 2016-4-8
 By Andy zhou <Andy.zhou@dfrobot.com>
 version:V1.0
*/
#include <Arduino.h>
#include <SPI.h>
#include <MultiLCD.h>
#include <SD.h>
#include "datalogger.h"

#define STATE_SD_READY 0x1
#define STATE_OBD_READY 0x2
#define STATE_GPS_CONNECTED 0x4
#define STATE_GPS_READY 0x8
#define STATE_MEMS_READY 0x10
#define STATE_GUI_ON 0x20

LCD_R61581 lcd;
CDataLogger logger;

byte state = 0;

bool checkSD()
{
    Sd2Card card;
    SdVolume volume;
    state &= ~STATE_SD_READY;
    pinMode(SS, OUTPUT);

    lcd.setFontSize(FONT_SIZE_MEDIUM);
    if (card.init(SPI_HALF_SPEED, SD_CS_PIN)) {
        const char* type;
        switch(card.type()) {
        case SD_CARD_TYPE_SD1:
            type = "SD1";
            break;
        case SD_CARD_TYPE_SD2:
            type = "SD2";
            break;
        case SD_CARD_TYPE_SDHC:
            type = "SDHC";
            break;
        default:
            type = "SDx";
        }

        lcd.print(type);
        lcd.write(' ');
        if (!volume.init(card)) {
            lcd.print("No FAT!");
            return false;
        }

        uint32_t volumesize = volume.blocksPerCluster();
        volumesize >>= 1; // 512 bytes per block
        volumesize *= volume.clusterCount();
        volumesize >>= 10;

        lcd.print((int)volumesize);
        lcd.print("MB");
    } else {
        lcd.println("No SD Card");
        return false;
    }

    if (!SD.begin(SD_CS_PIN)) {
        lcd.println("Bad SD");
        return false;
    }

    state |= STATE_SD_READY;
    return true;
}

void setup(){
  lcd.begin();
  lcd.setFontSize(FONT_SIZE_MEDIUM);  //set font size
  lcd.setColor(RGB16_RED);  //set strings color
  lcd.println();
  lcd.println();
  lcd.println("DFRobot!!!");
  lcd.println("TELEMATICS LCD SHIELD V1.0");
  lcd.println();
  lcd.setColor(RGB16_WHITE);
  if (checkSD()) {
        uint16_t index = logger.openFile();
        lcd.println();
        if (index > 0) {
            lcd.print("File ID:");
            lcd.println(index);
        } else {
            lcd.print("No File");
        }
    }
}

void loop(){
}
