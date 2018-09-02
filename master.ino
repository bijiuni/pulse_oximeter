#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Filters.h>

#define SLAVE_ADDRESS 0x12

#define LCD_CS 0 // Chip Select goes to Analog 3
#define LCD_CD 1 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define SD_CS 10 

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
 
int SPaa=0;
int HRaa=0; 
 
bool tremor_display = false; 
 
void setup()
{
  Wire.begin();
  
  tft.reset();

  uint16_t identifier = tft.readID();


 pinMode(10, OUTPUT);
 digitalWrite(10, HIGH);
  

  tft.begin(identifier);

  progmemPrint(PSTR("Initializing SD card..."));
  if (!SD.begin(SD_CS)) {
    progmemPrintln(PSTR("failed!"));
    return;
  }
  progmemPrintln(PSTR("OK!"));

  tft.setRotation(1); 

  testText();

}
 
 
void loop()
{
  
  int i,j,k;

float filterFreq_low_AC = 10;
float filterFreq_high = 0.5;
float filterFreq_low_DC = 1;
FilterOnePole lowpassFilter_AC( LOWPASS, filterFreq_low_AC );
FilterOnePole highpassFilter( HIGHPASS, filterFreq_high );
FilterOnePole lowpassFilter_DC( LOWPASS, filterFreq_low_DC);


  float IR_AC; 

  
  float sIR_AC, sIR_DC, sR_AC, sR_DC;
  
  tft.setRotation(1);
  testText();
  File dataFile = SD.open ("IR.txt",FILE_WRITE);
  
  for (j=0; j<3; j++)
  {
  Deleteppg ();
  
     int time =0;
     
     for (i=0; i<350; i++)
     {

       IR_AC= highpassFilter.input(lowpassFilter_AC.input(analogRead(5)*0.0049));    // read the input pin
        
       Displayppg (IR_AC,time);
         time = time +1;

         if (dataFile)
         {dataFile.println (IR_AC);}
         else{tft.setCursor(5, 5);
              tft.setTextColor(WHITE);  tft.setTextSize(5);
              tft.println("NO");}

         delay(12.4);
     }
     
     SPaa = Request ('0');
     HRaa = Request ('1');
     
     if (HRaa == '01')
     {
       tremor_display == true;
     }
     
     Displayvar ();}
     
     tft.fillScreen(BLACK);
  
  for (k=0; k<350; k++)
  {
       sIR_AC = highpassFilter.input(lowpassFilter_AC.input(analogRead(5)*0.0049));

       sIR_DC = highpassFilter.input(lowpassFilter_AC.input(analogRead(4)*0.0049));

       sR_AC = lowpassFilter_DC.input(analogRead(3)*0.0049);

       sR_DC = lowpassFilter_DC.input(analogRead(2)*0.0049);
       
       Displayfour ( sIR_AC, sIR_DC, sR_AC, sR_DC, k);
       
       if (dataFile)
         {dataFile.println (sIR_AC);}
         else{tft.setCursor(5, 5);
              tft.setTextColor(WHITE);  tft.setTextSize(5);
              tft.println("NO");}
       
       delay(12.4);
    
  }
      dataFile.close ();
}

int Request (int dataMode)
{  
    Wire.beginTransmission(SLAVE_ADDRESS);
    Wire.write(dataMode);
    delay(1);
    Wire.endTransmission();

    Wire.beginTransmission(SLAVE_ADDRESS);
    Wire.requestFrom(SLAVE_ADDRESS, 2);
    if (Wire.available()) {
      
      return Wire.read();
     
    }
    Wire.endTransmission();
  
}

void Displayppg (float dataa, int ttime){
  
  
  
  float xco = (240-((dataa/(2))*120))+0.5;
  int yco = (int) xco;
  tft.fillCircle (ttime, yco-1, 1, WHITE);
  
} 

void testText() {
  tft.fillScreen(BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  tft.setTextSize(1);
  tft.println("MEDE3010 Integrated Project      GZD Pulse Oximeter");
  tft.setTextColor(RED);    tft.setTextSize(3);
  tft.println();
  tft.setTextColor(CYAN);
  tft.setTextSize(3);
  tft.println(" SpO2 %  HR (bpm)");
 

}

void Displayvar() {
  String stra = String (SPaa);
  String strb = String (HRaa);
  String vardisplay;  
  
  if (tremor_display == true)
  {
  vardisplay = " " + stra + "   " + strb;
  }
  else
  {vardisplay = "tremor";}
  Deletevar ();
  
  tft.setCursor (10,70);
  tft.setTextColor(YELLOW);
  tft.setTextSize(5);
  tft.println(vardisplay);


}


void Deletevar() {
  
  int           x, y, w = tft.width();


  for(y=65; y<110; y++) tft.drawFastHLine(0, y, w, BLACK);
 
}

void Deleteppg() {
  
  int           x, y, w = tft.width();


  for(y=120; y<240; y++) tft.drawFastHLine(0, y, w, BLACK);
 
}

void progmemPrint(const char *str) {
  char c;
  while(c = pgm_read_byte(str++)) Serial.print(c);
}

// Same as above, with trailing newline
void progmemPrintln(const char *str) {
  progmemPrint(str);
  Serial.println();
}



void Displayfour ( float IRA, float IRD, float RA, float RD, int ttt)
{  
  
float IR_AC_max = 1;
float IR_AC_min = -1;
float R_AC_max = 1;
float R_AC_min = -1;
float IR_DC_max = 2;
float IR_DC_min = 0;
float R_DC_max = 2;
float R_DC_min = 0;

  float xa = (60-(IRA/(IR_AC_max-IR_AC_min))*60)+0.5;
  int ya = (int) xa;
  tft.fillCircle (ttt, ya-1, 1, WHITE);
  
  float xb = (120-(IRD/(IR_DC_max-IR_DC_min))*60)+0.5;
  int yb = (int) xb;
  tft.fillCircle (ttt, yb-1, 1, WHITE);
  
  float xc = (180-(RA/(R_AC_max-R_AC_min))*60)+0.5;
  int yc = (int) xc;
  tft.fillCircle (ttt, yc-1, 1, WHITE);
  
  float xd = (240-(RD/(R_DC_max-R_DC_min))*60)+0.5;
  int yd = (int) xd;
  tft.fillCircle (ttt, yd-1, 1, WHITE);
}
