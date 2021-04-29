
#ifndef control_TLV320ADC6140_h_
#define control_TLV320ADC6140_h_

#include "AudioControl.h"
#include <Arduino.h>


class AudioControlTLV320ADC6140: public AudioControl
{
  public:
    AudioControlTLV320ADC6140(int _samplerate) 
    {  
      samplingrate = _samplerate;
    };
    
    bool enable(void);
    bool disable(void);
    bool volume(float n);
    bool volume_dB(float n);
    bool inputLevel(float n);  //dummy to be compatible with Teensy Audio Library
    bool setInputGain_dB(float n);
    //bool setMicBias(int n);  //use ADC6140_MIC_BIAS_OFF or ADC6140_MIC_BIAS_2_5 or one of other choices defined earlier
    bool debugToSerial;
    unsigned int ADC_readPage(uint8_t page, uint8_t reg);
    bool ADC_writePage(uint8_t page, uint8_t reg, uint8_t val);
    virtual bool inputSelect(int n);
  
  private:
    void ADC_reset(void);
    void ADC_init(void);

    bool ADC_writeAddress(uint16_t address, uint8_t val);
    bool ADC_goToPage(uint8_t page);
    int prevMicDetVal = -1;
    int resetPinADC = 17;  //ADC reset pin, ADC3206_RESET_PIN
    int samplingrate=0;
    float HP_cutoff_Hz = 0.0f;
    float sample_rate_Hz = 44100; //only used with HP_cutoff_Hz to design HP filter on ADC, if used
    void setIIRCoeffOnADC_Left(uint32_t *coeff);
    void setIIRCoeffOnADC_Right(uint32_t *coeff);
};


#endif
