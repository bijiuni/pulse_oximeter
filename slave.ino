#include <Wire.h>
#include <math.h>
#include <Filters.h>

#define SLAVE_ADDRESS 0x12

uint8_t dataMode = '0';
bool inputdata = false;
float old_max = 1;
float old_min = 0;
int SPaa=0;
int HRaa=0;

float old_freq = 0;
bool tremor = false;
bool tremor_display = false;
bool first = true;

float freq_per = 0;
float R_ratio = 0;

void setup() {
  Wire.begin(SLAVE_ADDRESS);    // join I2C bus as a slave with address 1

}

void loop() {float old_max = 1;
float old_min = 0;


float old_freq = 0;
bool tremor = false;
bool tremor_display = false;
bool first = true;

float freq_per = 0;
float R_ratio = 0;

float filterFreq_low_AC = 10;
float filterFreq_high = 0.5;
float filterFreq_low_DC = 1;
FilterOnePole lowpassFilter_AC( LOWPASS, filterFreq_low_AC );
FilterOnePole highpassFilter( HIGHPASS, filterFreq_high );
FilterOnePole lowpassFilter_DC( LOWPASS, filterFreq_low_DC);
  
  
  int i,j,k;


  float IR_AC[350]; 
  float R_AC[350];
  float IR_DC[350];
  float R_DC[350];
  


  
  for (j=0; j<3; j++)
  {
     
     for (i=0; i<350; i++)
     {

       IR_AC[i] = highpassFilter.input(lowpassFilter_AC.input(analogRead(5)*0.0049));    // read the input pin

       IR_DC[i] = highpassFilter.input(lowpassFilter_AC.input(analogRead(4)*0.0049));

       R_AC[i] = lowpassFilter_DC.input(analogRead(3)*0.0049);

       R_DC[i] = lowpassFilter_DC.input(analogRead(2)*0.0049);
        

         delay(12.4);
     }
     
     calvar (IR_AC, IR_DC, R_AC, R_DC);
     
     SPaa = (int)(freq_per*60+0.5);
     HRaa = (int)(R_ratio + 0.5);
     if (tremor_display == true)
     {HRaa = '01';}
     
     waitdata ();
   
} 
delay (4340);
  }



void waitdata() {
  while (inputdata == false){
  if (Wire.available()) {
    dataMode = Wire.read();
    while (Wire.available()) Wire.read();
    inputdata = true;
  }
}
  switch (dataMode) {
    inputdata = false;
    case '0':
      Wire.write(SPaa);
      break;
    case '1':
      Wire.write(HRaa);
      break;
    default:
      Wire.write("NA");
  }

}

void calvar(float IR_AC[], float IR_DC[], float R_AC[], float R_DC[]) {
  float sample_freq = 80; // Sample Frequency in Hz
  int i,k;
  float sum, sum_old;
  float thresh = 0;
  byte pd_state = 0; 
  
  float red_DC_sum = 0;
  float IR_DC_sum = 0;
  float red_AC_square_sum = 0;
  float IR_AC_square_sum = 0;
  float red_AC_rms;
  float IR_AC_rms;

  sum = 0;
  pd_state = 0;
  int period = 0;
  
  // Autocorrelation
  for (i=100; i < 350 && (pd_state != 3); i++)
  {
    sum_old = sum;
    sum = 0;

    for (k=100; k <350-i+100; k++)
    {
      sum += (IR_AC[k]) * (IR_AC[k+i-100]);
    }

    //Serial.println(sum);

    // Peak Detect State Machine
    // 0: initial, set threshold
    // 1: detect positive slope
    // 2: detect negative slope
    // 3: peak found
    if (pd_state == 0)
    {
      thresh = sum / 2;
      pd_state = 1;
    } 
    else if (pd_state == 1 && (sum > thresh) && (sum - sum_old) > 0) 
    {
      pd_state = 2;
    }
    else if (pd_state == 2 && (sum - sum_old) <= 0) 
    {
      period = i - 100;
      pd_state = 3;
    } 
  }
  
  if (IR_AC[period] < IR_AC[period-1])
    period--;
  
  freq_per = sample_freq/period;

  float current_freq;
  current_freq = freq_per;

  if ((tremor == true) && (freq_per <= 0 || freq_per >= 3)) {
    tremor = false;
    //Serial.println("Tremor is too severe");
  } else if ((freq_per > 0 || freq_per < 3) && (tremor == false)) {
    if ((current_freq - old_freq)/old_freq > 0.08 && first == false) {
      freq_per = (current_freq + old_freq)/2;
    } 
    //Serial.println(freq_per);
    tremor = false;
  }

  if ((freq_per <= 0 || freq_per >= 3) && (tremor == false)) {
    tremor = true;
    tremor_display = true;
    //Serial.println(old_freq);
  }

  old_freq = current_freq;

  first = false;

  //Calculation of R ratio
  for (int i=100; i<350 ; i++) 
  {
    red_AC_square_sum += R_AC[i]*R_AC[i];    
    IR_AC_square_sum += IR_AC[i]*IR_AC[i];

    red_DC_sum += R_DC[i];
    IR_DC_sum += IR_DC[i];
  }

  red_AC_rms = sqrt(red_AC_square_sum/250);
  IR_AC_rms = sqrt(IR_AC_square_sum/250);

  R_ratio = (red_AC_rms/(red_DC_sum/250))/(IR_AC_rms/(IR_DC_sum/250));


  //calculation of max and min of the previous data
  for (i=0 ; i<350 ; i++) 
  {
    if (IR_AC[i] > old_max)
       old_max = IR_AC[i]; 
  }

    for (i=0 ; i<350 ; i++) 
  {
    if (IR_AC[i] < old_min)
       old_min = IR_AC[i]; 
  }

  old_max = old_max + 0.3;

  old_min = old_min - 0.3;

}

