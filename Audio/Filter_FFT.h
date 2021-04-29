
#ifndef Filter_FFT_h_
#define Filter_FFT_h_

#include "AudioStream.h"

#include <sstream>
#include <iostream>
#include <iomanip>

#include <arm_math.h>
#include <arm_const_structs.h>

#define FFT_SIZE           256//1024//256
#define forward_fft   0
#define inverse_fft   1


typedef struct SR_Descriptor
{
  const uint8_t SR_n;
  const uint32_t rate;
  const char* const text;
  const char* const f1;
  const char* const f2;
  const char* const f3;
  const char* const f4;
  const float32_t x_factor;
  const uint8_t x_offset;
} SR_Desc;

enum 
{
  CH_RIGHT=0,
  CH_LEFT=1
};

enum State
{
  init=0,
  average=1,
  runtime=2
};

class Filter_FFT : public AudioStream
{
  public:
  
    Filter_FFT(int samplerate, int oversample, bool debug);
  
    virtual void update(void);
    
  private:
    audio_block_t *inputQueueArray[2];
    int       STATE;    
    bool      dataReady;
    int       SAMPLE_RATE;
    int       countAVG;
    int       oversampling;
    int16_t       array_avg[AUDIO_BLOCK_SAMPLES];
    int       BUFFER[2048];
    

    bool printvalues;

};

#endif
