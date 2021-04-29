#define SAMPLERATE      48000 
#define OVERSAMPLING    48000//96000 192000 384000

#include <Audio.h>
#include <Wire.h>
#include <SerialFlash.h>
#include <usb_names.h>
#include "TLV320ADC6140.h"
#include "utility/imxrt_hw.h"
#include "Filter_FFT.h"

int ADC3206_RESET_PIN=17;     
AudioControlTLV320ADC6140     tlv320adc6140(OVERSAMPLING);

AudioInputI2S           i2s;           
Filter_FFT              filter1(SAMPLERATE , OVERSAMPLING, true); 
Filter_FFT              filter2(SAMPLERATE , OVERSAMPLING, false); 
AudioOutputUSB          usb;   
        
AudioConnection          patchCord1(i2s, 0, filter1, 0);
AudioConnection          patchCord2(i2s, 1, filter2, 0);
AudioConnection          patchCord3(filter1, 0, usb, 0);
AudioConnection          patchCord4(filter2, 0, usb, 1);



void setup()
{
  setFrequency(OVERSAMPLING);
  pinMode(7, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(7, HIGH);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
  
  AudioMemory(40);
  tlv320adc6140.enable();
  Serial.print("Sample Rate ");
  Serial.println(AUDIO_SAMPLE_RATE_EXACT);

}

void loop()
{
   
}



void setFrequency(int frequency)
{       
  // PLL between 27*24 = 648MHz und 54*24=1296MHz
  int n1 = 4; //SAI prescaler 4 => (n1*n2) = multiple of 4
  int n2 = 1 + (24000000 * 27) / (frequency * 256 * n1);
  double C = ((double)frequency * 256 * n1 * n2) / 24000000;
  int c0 = C;
  int c2 = 10000;
  int c1 = C * c2 - (c0 * c2);
  set_audioClock(c0, c1, c2, true);
  CCM_CS1CDR = (CCM_CS1CDR & ~(CCM_CS1CDR_SAI1_CLK_PRED_MASK | CCM_CS1CDR_SAI1_CLK_PODF_MASK))
       | CCM_CS1CDR_SAI1_CLK_PRED(n1-1) // &0x07
       | CCM_CS1CDR_SAI1_CLK_PODF(n2-1); // &0x3f 

}
