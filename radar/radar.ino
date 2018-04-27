#include<Servo.h>
#include "MultiLCD.h"
#include <SPI.h>
#include <Arduino.h>
#include <FlexiTimer2.h>
#define header_H    0x55 //帧头
#define header_L    0xAA //帧头
#define device_Addr 0x11 //模块地址
#define data_Length 0x00 //数据长度
#define get_Dis_CMD 0x02 //距离测量命令
#define checksum    (header_H+header_L+device_Addr+data_Length+get_Dis_CMD) //校验和
unsigned char Rx_DATA[8];
unsigned char CMD[6]={header_H,header_L,device_Addr,data_Length,get_Dis_CMD,checksum};

bool clockwise1=false,clockwise2=false,sendFlag=true,angleFlag=true,Serial1Flag=false,dataFlag=false,drawFlag=false,clearFlag=false;
int degree_L=30,degree_S=1,angle=1;
int x[181],y[181],Dis[181],num=0,Plflag=0;
int x0=239,y0=0,r=239,r1=5;
int flag=0;
Servo myServo;
LCD_R61581 lcd;

void flash()
{
    if(Serial1.available())
    {
       Serial1Flag=true;
      }
     else
     {
         Serial1Flag=false;
      }
     /* if(flag>0)
      {
          myServo.write(degree_S);
          Sjudge();
        }*/
      // moveFan(degree_L,1);
      // Tjudge();
  }

void setup() {
  // put your setup code here, to run once:
     memset(Dis,0,sizeof(Dis));
     Serial.begin(19200);
     Serial1.begin(19200);
     lcd.begin();
     myServo.attach(9);
     //计算圆心为（239，0），半径为239的半圆每度对应圆周上的坐标
      for(int i=0;i<181;i++)
    {
         double d=(3.14/180)*i;
         x[i]=int(x0-r*cos(d));
         y[i]=int(r*sin(d));
      }
       for(int i=0;i<180;i++)
       {
         lcd.setFontSize(FONT_SIZE_MEDIUM);  //set font size
         lcd.setColor(RGB16_GREEN);
         lcd.drawLine(x[i],y[i],x[i+1],y[i+1]);
       }
      //drawMap();
      drawFan(31);
      FlexiTimer2::set(50,flash);
      FlexiTimer2::start();
}
void drawMap()
{
       lcd.setColor(RGB16_GREEN);
       lcd.drawCircle(x0,y0,50);
       lcd.drawCircle(x0,y0,100);
       lcd.drawCircle(x0,y0,150);
       lcd.drawCircle(x0,y0,200);
       for(int i=299;i<319;i++)
       {
          lcd.clearLine(i);
        }
       for(int i=0;i<=180;i+=30)
       {
           lcd.setColor(RGB16_GREEN);
          lcd.drawLine(x0,y0,x[i],y[i]);
        }
  }
  void clearMap()
{
       lcd.setColor(0);
       lcd.drawCircle(x0,y0,50);
       lcd.drawCircle(x0,y0,100);
       lcd.drawCircle(x0,y0,150);
       lcd.drawCircle(x0,y0,200);
       for(int i=0;i<=180;i+=30)
       {
           lcd.setColor(RGB16_GREEN);
          lcd.drawLine(x0,y0,x[i],y[i]);
        }
  }
 void clearFan(int i)
 {
      if(i<30||i>150)
       return;
     for(int j=i+30;j>=i-30;j--)
     {
          lcd.setColor(0);
          lcd.drawLine(x0,y0,x[j],y[j]);
      }
  }
void drawFan(int i)
{
     if(i<30||i>150)
       return;
     for(int j=i-30;j<=i+30;j++)
     {
          lcd.setColor(RGB16_GREEN);
          lcd.drawLine(x0,y0,x[j],y[j]);
      }
  }
  void moveFan(int d)
  {
       if((d<30)||(d>150))
         return;
       if(clockwise2)
       {
                 lcd.setColor(RGB16_GREEN);
                 lcd.drawLine(x0,y0,x[d-30],y[d-30]);
                 lcd.setColor(0);
                 lcd.drawLine(x0,y0,x[d+30],y[d+30]);             
        }
        else
        {
                 lcd.setColor(RGB16_GREEN);
                 lcd.drawLine(x0,y0,x[d+30],y[d+30]);
                 lcd.setColor(0);
                 lcd.drawLine(x0,y0,x[d-30],y[d-30]);
          }
        /*   lcd.setColor(RGB16_GREEN);
           lcd.drawCircle(x0,y0,50);
           lcd.drawCircle(x0,y0,100);
           lcd.drawCircle(x0,y0,150);
           lcd.drawCircle(x0,y0,200);*/
         
    }
 void drawPoint(int d)//取值范围d:[1,180]
  {
            if(Dis[d]==0)
                return;
            unsigned int dis=Dis[d];
            double dd=(3.14/180)*d;
            double d1=(3.14/180)*(d-1);
            int xd1=int(x0-dis*cos(dd));
            int yd1=int(dis*sin(dd));
            int xd2=int(x0-dis*cos(d1));
            int yd2=int(dis*sin(d1));
             
            int xxx=int((xd1+xd2)/2);
            int yyy=int((yd1+yd2)/2);
            lcd.setColor(RGB16_RED);
            lcd.fillCircle(xxx,yyy,r1);
    }
   void clearPoint(int d)//d:[1,180]
{
    if(Dis[d]!=0)
    {
            int dis=Dis[d];
            double dd=(3.14/180)*d;
            double d1=(3.14/180)*(d-1);
            int xd1=int(x0-dis*cos(dd));
            int yd1=int(dis*sin(d));
            int xd2=int(x0-dis*cos(d1));
            int yd2=int(dis*sin(d1));
             
            int xxx=int((xd1+xd2)/2);
            int yyy=int((yd1+yd2)/2);
            lcd.setColor(0);
            lcd.fillCircle(xxx,yyy,r1);
      }
  }
 void sendCmd()
 {
          for(int i=0;i<6;i++)
          {
             Serial1.write(CMD[i]);
           }
  }   
void angleParse()
{
     if(angle>180)
     {
         angle=180;
         angleFlag=false;
      }
      if(angle<1)
      {
          angle=1;
          sendFlag=false;
          angleFlag=true;
          dataFlag=true;
        }
  }
 void Anglechange(unsigned int dis)
 {
       if(angleFlag)
       {
           Dis[angle]=dis;
           angle+=2;
           angleParse();
        }
        else
        {
           Dis[angle]=dis;
           angle-=2;
           angleParse();
       }
  }
void dataGet()
{
    if(dataFlag==true)
    {
         return;
      }
     while(Serial1.available())
   {
         Rx_DATA[num++]=Serial1.read();
         if(num==1&&Rx_DATA[num-1]!=0x55)
         {
             num=0;
             Plflag++;
         }
         if(num==2&&Rx_DATA[num-1]!=0xAA)
         {
             num=0;
             Plflag++;
         }
         if(num==2&&Rx_DATA[num-1]==0xAA&&Rx_DATA[num-2]==0x55)
         {
             int v=((Plflag%8>0)?1:0);
             v+=Plflag/8;
             for(int j=0;j<v;j++)
             {
                 Anglechange(0);
             }
        
         }
              if(num==8)
              {
                  num=0;
                  unsigned char t=Rx_DATA[0]+Rx_DATA[1]+Rx_DATA[2]+Rx_DATA[3]+Rx_DATA[4]+Rx_DATA[5]+Rx_DATA[6];
                  if(t==Rx_DATA[7])
                  {
                         unsigned int dis=((Rx_DATA[5]<<8)|Rx_DATA[6]);
                         if(dis>220||dis<0)
                            dis=0;
                         Anglechange(dis);
                    }
                    else
                    {
                        Anglechange(0);
                      }
                }
     }
  }

  void Sjudge()
  {
      if(clockwise1)
      {
          degree_S-=2;
          if(degree_S<1)
          {
             degree_S=1;
             clockwise1=false;
            }
        }
       else
       {
           degree_S+=2;
           if(degree_S>180)
           {
              degree_S=180;
              clockwise1=true;
            }
        }
    }

void Tjudge()
{
     if(clockwise2)
      {
          degree_L--;
          if(degree_L<30)
          {
             degree_L=30;
             clockwise2=false;
            }
        }
       else
       {
           degree_L++;
           if(degree_L>150)
           {
              degree_L=150;
              clockwise2=true;
            }
        }
  }
void loop() {
  // put your main code here, to run repeatedly:
       switch(flag)
       {
          case 0:
                 sendCmd();
                 myServo.write(degree_S);
                 Sjudge();
                 moveFan(degree_L);
                 Tjudge();
                 break;
          case 1:
                if(degree_S!=1)
                {
                     myServo.write(degree_S);
                     Sjudge();
                  }
                 moveFan(degree_L);
                 Tjudge();
                 break;
          case 2:
                 clearFan(30);
                 drawMap();
                 for(int i=1;i<181;i++)
                 {
                    drawPoint(i);
                  }
                  delay(1000);
                  drawFlag=true;
                 break;
          case 3:
                  for(int i=1;i<181;i++)
                  {
                       Dis[i]=0;
                    }
                  lcd.clear(0,0,479,319);
                   for(int i=0;i<180;i++)
                   {
                        lcd.setColor(RGB16_GREEN);
                        lcd.drawLine(x[i],y[i],x[i+1],y[i+1]);
                      }
                  drawFan(30);
                  clearFlag=true;
                 /*moveFan(degree_L,1)
                 clearPoint(degree_L-30+1)
                 clearPoint(degree_L+30)
                  Tjudge();*/
                  
                 break;
          default:
                 break;
        
        }
       if(sendFlag==false)
       {
           flag=1;
        }
        if(degree_S==1&&degree_L==30&&clockwise2==false&&dataFlag==true)
        {
              flag=2;
         }
         if(drawFlag==true)
         {
             flag=3;
          }
          if(clearFlag)
          {
              sendFlag=true;
              drawFlag=false;
              dataFlag=false;
              clearFlag=false;
              flag=0;
            }
            if(Serial1Flag)
            {
                dataGet();
              }
}
