#include "TLV320ADC6140.h"
#include <Wire.h>
#include <Audio.h>
//********************************  Constants  *******************************//

#define ADC6140_I2C_ADDR                                             0x4C


//#define ADC_BITS                                                        16
#define ADC_BITS                                                        32


bool AudioControlTLV320ADC6140::enable(void)
{
  delay(100);

  // Setup for Master mode, pins 18/19, external pullups, 400kHz, 200ms default timeout
  Wire.begin();
  delay(5);

  //hard reset the ADC
  Serial.println("Hardware reset of ADC...");
#define RESET_PIN (resetPinADC)
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, HIGH); 
  delay(50); //not reset
  digitalWrite(RESET_PIN, LOW); 
  delay(50); //reset
  digitalWrite(RESET_PIN, HIGH); 
  delay(50); //not reset

  ADC_reset(); 
  delay(100);  //soft reset
  ADC_init(); 
  delay(100);

  if (debugToSerial) Serial.println("TLV320 enable done");

  return true;

}

bool AudioControlTLV320ADC6140::disable(void) {
  return true;
}

//dummy function to keep compatible with Teensy Audio Library
bool AudioControlTLV320ADC6140::inputLevel(float volume) {
  return false;
}




void AudioControlTLV320ADC6140::ADC_reset()
{
  if (debugToSerial) Serial.println("INFO: Reseting ADC");
  ADC_writePage(0x00, 0x01, 0x01);
  delay(10);
}


bool AudioControlTLV320ADC6140:: inputSelect(int n)
{
  return true;
}

// set MICPGA volume, 0-47.5dB in 0.5dB setps
bool AudioControlTLV320ADC6140::setInputGain_dB(float volume) {
  if (volume < 0.0) {
    volume = 0.0; // 0.0 dB
    Serial.println("controlTLV320ADC6140: WARNING: Attempting to set MIC volume outside range");
  }
  if (volume > 47.5) {
    volume = 47.5; // 47.5 dB
    Serial.println("controlTLV320ADC6140: WARNING: Attempting to set MIC volume outside range");
  }

  volume = volume * 2.0; // convert to value map (0.5 dB steps)
  int8_t volume_int = (int8_t) (round(volume)); // round

  if (debugToSerial) {
    Serial.print("INFO: Setting MIC volume to ");
    Serial.print(volume, 1);
    Serial.print(".  Converted to volume map => ");
    Serial.println(volume_int);
  }

  //ADC_writeAddress(ADC6140_MICPGA_LEFT_VOLUME_REG, ADC6140_MICPGA_VOLUME_ENABLE | volume_int); // enable Left MicPGA, set gain to 0 dB
  //ADC_writeAddress(ADC6140_MICPGA_RIGHT_VOLUME_REG, ADC6140_MICPGA_VOLUME_ENABLE | volume_int); // enable Right MicPGA, set gain to 0 dB
  return true;
}

//volume control, similar to Teensy Audio Board
// value between 0.0 and 1.0.  Set to span -58 to +15 dB
bool AudioControlTLV320ADC6140::volume(float volume)
{
  return true;
}


// -63.6 to +24 dB in 0.5dB steps.  uses signed 8-bit
bool AudioControlTLV320ADC6140::volume_dB(float volume)
{
  return true;
}


void AudioControlTLV320ADC6140::ADC_init()
{
  if (debugToSerial) Serial.println("controlTLV320ADC6140: Initializing ADC");

  
  //SLEEP_CFG
  ADC_writePage(0, 0x02, 0x81);
  
  //ASI_CFG
  ADC_writePage(0, 0x07, 0x70);//ASI_CFG0: I2S mode/32 bits/FSYNC POl Default/BCLK POl Default/TX EDGE Default/TX FILL Default/
  ADC_writePage(0, 0x08, 0x00);//ASI_CFG1: TX_LSB Transmit the LSB for a full cycle/Bus keeper is always disabled/
  ADC_writePage(0, 0x09, 0x00);//ASI_CFG1: All devices are connected in the common ASI bus

  //MST_CFG
  ADC_writePage(0, 0x13, 0x00);//Device is in slave mode/fS is a multiple (or submultiple) of 48 kHz/13MHz
  ADC_writePage(0, 0x14, 0x48);//48 kHz/ Ratio 256
  //ADC_writePage(0, 0x16, 0x10);//CLK_SRC
  //ADC_writePage(0, 0x1F, 0x40);//PDMCLK_CFG

  uint8_t CH_CFG  = 0x80;
  uint8_t GAIN    = 42;//42 << 2;//42
  uint8_t DIGITAL = 201;//0xC9

  //ASI_CH 
  ADC_writePage(0, 0x0B, 0x00);//ASI_CH1 is Left slot I2S
  ADC_writePage(0, 0x0C, 0x00);//ASI_CH2 is Left slot I2S
  ADC_writePage(0, 0x0D, 0x20);//ASI_CH3 is Right slot I2S
  ADC_writePage(0, 0x0B, 0x20);//ASI_CH4 is Right slot I2S
  //ADC_writePage(0, 0x0B, 0x00);//ASI_CH5
  //ADC_writePage(0, 0x0B, 0x00);//ASI_CH6
  //ADC_writePage(0, 0x0B, 0x00);//ASI_CH7
  //ADC_writePage(0, 0x0B, 0x00);//ASI_CH8
  
  //CH1_CFG
  ADC_writePage(0, 0x3C, CH_CFG);//Channel Differential/ Line Input /AGC disabled
  ADC_writePage(0, 0x3D, GAIN);//0dB Gain
  ADC_writePage(0, 0x3E, DIGITAL);//Digital volume control is set to 0 dB
  ADC_writePage(0, 0x3F, 0x80);//Gain calibration is set to 0 dB
  ADC_writePage(0, 0x40, 0x00);//No Phase Calibration
  //CH2_CFG
  ADC_writePage(0, 0x41, CH_CFG);//Channel Differential/ Line Input /AGC disabled
  ADC_writePage(0, 0x42, GAIN);//0dB Gain
  ADC_writePage(0, 0x43, DIGITAL);//Digital volume control is set to 0 dB
  ADC_writePage(0, 0x44, 0x80);//Gain calibration is set to 0 dB
  ADC_writePage(0, 0x45, 0x00);//No Phase Calibration
  //CH3_CFG
  ADC_writePage(0, 0x46, CH_CFG);//Channel Differential/ Line Input /AGC disabled
  ADC_writePage(0, 0x47, GAIN);//0dB Gain
  ADC_writePage(0, 0x48, DIGITAL);//Digital volume control is set to 0 dB
  ADC_writePage(0, 0x49, 0x80);//Gain calibration is set to 0 dB
  ADC_writePage(0, 0x4A, 0x00);//No Phase Calibration
  //CH4_CFG
  ADC_writePage(0, 0x4B, CH_CFG);//Channel Differential/ Line Input /AGC disabled
  ADC_writePage(0, 0x4C, GAIN);//0dB Gain
  ADC_writePage(0, 0x4D, DIGITAL);//Digital volume control is set to 0 dB
  ADC_writePage(0, 0x4E, 0x80);//Gain calibration is set to 0 dB
  ADC_writePage(0, 0x4F, 0x00);//No Phase Calibration
  //CH5_CFG
  ADC_writePage(0, 0x52, DIGITAL);//Digital volume control is set to 0 dB
  ADC_writePage(0, 0x53, 0x80);//Gain calibration is set to 0 dB
  ADC_writePage(0, 0x54, 0x00);//No Phase Calibration
  //CH6_CFG
  ADC_writePage(0, 0x57, DIGITAL);//Digital volume control is set to 0 dB
  ADC_writePage(0, 0x58, 0x80);//Gain calibration is set to 0 dB
  ADC_writePage(0, 0x59, 0x00);//No Phase Calibration
  //CH7_CFG
  ADC_writePage(0, 0x5C, DIGITAL);//Digital volume control is set to 0 dB
  ADC_writePage(0, 0x5D, 0x80);//Gain calibration is set to 0 dB
  ADC_writePage(0, 0x5E, 0x00);//No Phase Calibration
  //CH8_CFG
  ADC_writePage(0, 0x61, DIGITAL);//Digital volume control is set to 0 dB
  ADC_writePage(0, 0x62, 0x80);//Gain calibration is set to 0 dB
  ADC_writePage(0, 0x63, 0x00);//No Phase Calibration

  //DSP_CFG
  //ADC_writePage(0, 0x6B, 0x01);
  //ADC_writePage(0, 0x6C, 0x40);
  
  //IN_CH_EN
  ADC_writePage(0, 0x73, 0xF0);

  //ASI_OUT_CH_EN
  ADC_writePage(0, 0x74, 0xF0);
  
  //PWR_CFG
  ADC_writePage(0, 0x75, 0x68);//Power up the PLL/Power up all enabled ADC and PDM channels



}

unsigned int AudioControlTLV320ADC6140::ADC_readPage(uint8_t page, uint8_t reg)
{
  unsigned int val;
  if (ADC_goToPage(page)) {
    Wire.beginTransmission(ADC6140_I2C_ADDR);
    Wire.write(reg);
    unsigned int result = Wire.endTransmission();
    if (result != 0) {
      Serial.print("controlTLV320ADC6140: ERROR: Read Page.  Page: "); Serial.print(page);
      Serial.print(" Reg: "); Serial.print(reg);
      Serial.print(".  Received Error During Read Page: ");
      Serial.println(result);
      val = 300 + result;
      return val;
    }
    if (Wire.requestFrom(ADC6140_I2C_ADDR, 1) < 1) {
      Serial.print("controlTLV320ADC6140: ERROR: Read Page.  Page: "); Serial.print(page);
      Serial.print(" Reg: "); Serial.print(reg);
      Serial.println(".  Nothing to return");
      val = 400;
      return val;
    }
    if (Wire.available() >= 1) {
      uint16_t val = Wire.read();
      if (debugToSerial) {
        Serial.print("controlTLV320ADC6140: Read Page.  Page: "); Serial.print(page);
        Serial.print(" Reg: "); Serial.print(reg);
        Serial.print(".  Received: ");
        Serial.println(val, HEX);
      }
      return val;
    }
  } else {
    Serial.print("controlTLV320ADC6140: INFO: Read Page.  Page: "); Serial.print(page);
    Serial.print(" Reg: "); Serial.print(reg);
    Serial.println(".  Failed to go to read page.  Could not go there.");
    val = 500;
    return val;
  }
  val = 600;
  return val;
}

bool AudioControlTLV320ADC6140::ADC_writeAddress(uint16_t address, uint8_t val)
{
  uint8_t reg = (uint8_t) (address & 0xFF);
  uint8_t page = (uint8_t) ((address >> 8) & 0xFF);

  return ADC_writePage(page, reg, val);
}

bool AudioControlTLV320ADC6140::ADC_writePage(uint8_t page, uint8_t reg, uint8_t val)
{
  if (debugToSerial) {
    Serial.print("controlTLV320ADC6140: Write Page.  Page: "); Serial.print(page);
    Serial.print(" Reg: "); Serial.print(reg);
    Serial.print(" Val: "); Serial.println(val);
  }
  if (ADC_goToPage(page)) {
    Wire.beginTransmission(ADC6140_I2C_ADDR);
    Wire.write(reg); delay(10);
    Wire.write(val); delay(10);
    uint8_t result = Wire.endTransmission();
    if (result == 0) 
      return true;
    else 
    {
      Serial.print("controlTLV320ADC6140: Received Error During writePage(): Error = ");
      Serial.println(result);
    }
  }
  return false;
}

bool AudioControlTLV320ADC6140::ADC_goToPage(byte page)
{
  Wire.beginTransmission(ADC6140_I2C_ADDR);
  Wire.write(0x00); delay(10);// page register  //was delay(10) from BPF
  Wire.write(page); delay(10);// go to page   //was delay(10) from BPF

  byte result = Wire.endTransmission();
  if (result != 0)
  {
    Serial.print("controlTLV320ADC6140: Received Error During goToPage(): Error = ");
    Serial.println(result);
    if (result == 2)
    {
      // failed to transmit address
      //return ADC_goToPage(page);
    }
    else if (result == 3)
    {
      // failed to transmit data
      //return ADC_goToPage(page);
    }
    return false;
  }
  return true;
}
