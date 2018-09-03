# Developing a Pulse Oximeter Prototype

Pulse oximetry is a non-invasive approach for real-time in vivo measurement of blood
oxygen level and heart rate of a person. In many occasions, blood oxygenation information
should be available to clinicians or patients on a continuous basis. For patients under risk of
respiratory failure, such as airway obstruction, it is essential to monitor the arterial oxygen
saturation in blood, which reflects the efficiency of gas exchange in the lungs. As an optical device, pulse oximeter can be used to
detect blood oxygen saturation based on localized volume changes of arterial blood.

This report introduces a simple design of pulse oximeter: GZD pulse oximeter.
It consists of a series of electronic components, such as capacitors, integrated chips (ICs)
and light emitting diodes (LEDs) that are planted onto a printed circuit board (PCB). A
microcontroller device, Arduino is also used to process signals digitally. Coupled with an
TFT screen, the resulting device is used for calculating and displaying the measurement
results, including photoplethysmogram (PPG) waveform, arterial oxygen saturation and heart
rate reading.

## Getting Started

To build the prototype, you would need two Arduino Uno, a soldered PCB board, a TFT screen, and 3D printed components.

This file will focus on the software part of the oximeter. The _slave.ino_ and _master.ino_ are run on two Arduino Uno respectively. One is mainly for the display while the other is mainly for processing signals.

### Software 

After feeding the filtered analog signal into the digital Arduino device, a digital low-
pass filter and a digital high-pass filter is used to eliminate the high-frequency noise induced
by the ADC , and to ensure a stable AC signal respectively. The AC signal of IR is then
utilised to calculate the heart beat rate. By performing autocorrelation on the IR_AC, a peak-
detection algorithm can be used to find the position of the first peak. The heart rate is then
calculated by dividing 1 by the time length between the origin and the first peak. For
calculation of the SpO2, a parameter called R_ratio is computed. By conducting an empirical

calibration using finger Sims together with a commercial pulse oximeter, a calibrated
equation (SpO2 against R_ratio) is obtained. Inputting an R-ratio value gives the SpO2 value.
Apart from the Arduino, a TFT screen is also implemented as an added feature. After
calculating the heart rate and SpO2, their values as well as the obtained PPG signal can be
displayed on the screen,. After three cycles of waveform display, it will switch to another
mode and four waveforms, including the IR_AC, IR_DC, R_AC and R_DC are shown
instead. It then changes back into the original mode.

### Installing

Three Arduino libraries are used in this project. Details can be found in the _arduino_lib_ folder.

In the master Arduino, you needs to include libraries as follows
```
#include <SPFD5408_Adafruit_GFX.h>    // Core graphics library
#include <SPFD5408_Adafruit_TFTLCD.h> // Hardware-specific library
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Filters.h>
```


In the slave Arduino:
```
#include <Wire.h>
#include <math.h>
#include <Filters.h>
```



## Deployment

###PCB Development

The PCB file is named PCB board.pcb
The result are shown below
![PCB file](https://github.com/bijiuni/pulse_oximeter/blob/master/img/pcb1.PNG)
![PCB after soldering](https://github.com/bijiuni/pulse_oximeter/blob/master/img/pcb2.PNG)


###Display

The screen display are in the following format

![Screen Display](https://github.com/bijiuni/pulse_oximeter/blob/master/img/display.PNG)


###Arduino

The arduino uno used in this project

![Arduino Uno](https://github.com/bijiuni/pulse_oximeter/blob/master/img/arduino.jpg)

## Built With

* [Arduino](https://www.arduino.cc/)


## Authors

* **Zach Lyu** *
* **Gwinky Yip** *

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* Other Group mate: Darren Chu
