
#ifndef Filter_FFT_h_
#define Filter_FFT_h_

#include "AudioStream.h"

#include "Dsp.h"
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
    int       count;
    int       countAVG;
    int       oversampling;
    int16_t       array_avg[AUDIO_BLOCK_SAMPLES];
    int       array_help[FFT_SIZE / 2];
    int       BUFFER[2048];
    
    float     hanning[FFT_SIZE]; 
    
    arm_cfft_instance_f32 *fft;
    arm_cfft_instance_f32 *fft_inv;
    void initFTT(void);
    float    FFT_BUFFER_MAG[FFT_SIZE*2]       = {0.0};       // SQRT buffer
    float    FFT_BUFFER_prev[FFT_SIZE/2]      = {0.0};       // prior input buffer
    float    FFT_BUFFER[FFT_SIZE*2]           = {0.0};       // FFT of input
    float    FFT_BUFFER_out_prev[FFT_SIZE*2]  = {0.0};
    float    NoiseBuffer[FFT_SIZE*2];
    
    float32_t SNR_prio[FFT_SIZE / 2];
    float32_t SNR_post[FFT_SIZE / 2];
    float32_t ph1y[FFT_SIZE / 2];
    float32_t pslp[FFT_SIZE / 2];
    float32_t xt[FFT_SIZE / 2];
    float32_t Hk_old[FFT_SIZE / 2];
    float32_t Gain[FFT_SIZE / 2]; // preliminary gain factors (before time smoothing) and after that contains the frequency smoothed gain factors
    
    const float32_t asnr = 15;//20;  // active SNR in dB
    const float32_t Initial_Noise_Probrability = 0.5; // initial noise probability [0.5]
    const float32_t Prior_Noise_Probrability = 0.5;   // prior noise probability [0.5]
    
    float32_t tinc;               // frame time 5.3333ms (TODO: calculate tinc from sample rate and decimation factor)
    float32_t tax;                // noise output smoothing time constant = -tinc/ln(0.8)
    float32_t tap;                // signal prob smoothing time constant = -tinc/ln(0.9) tinc = frame time (5.33ms)
    float32_t psthr = 0.99;       // threshold for smoothed signal probability [0.99]
    float32_t pnsaf = 0.01;       // noise probability safety value [0.01]
    
    float32_t ax;                 // ax=exp(-tinc/tax); % noise output smoothing factor
    float32_t ap;                 // ap=exp(-tinc/tap); % noise output smoothing factor
    float32_t xih1;               // xih1=10^(asnr/10); % signal-present SNR
    float32_t xtr;
    float32_t xih1r;
    float32_t pfac;
    float32_t snr_prio_min;
    float32_t NR_alpha;// default 0.99 --> range 0.98 - 0.9999; 0.95 acts much too hard: reverb effects
    int counter_samples =0;

    Dsp::Filter* lowpass_average;
    
    Dsp::Filter* lowpass;
    Dsp::Filter* highpass;

    Dsp::Filter* lowpass_second;
    Dsp::Filter* highpass_second;
    
    float * audiodata;

    bool printvalues;

};

#endif
