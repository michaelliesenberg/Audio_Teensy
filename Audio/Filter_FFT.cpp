#include <Arduino.h>
#include "Filter_FFT.h"
#include <stdio.h>


int helptime=0;

Filter_FFT::Filter_FFT(int samplerate , int oversample, bool debug): AudioStream(2,inputQueueArray)
{
  dataReady=false;
  printvalues = debug;
  count=0;
  countAVG=0;
  SAMPLE_RATE=samplerate;
  oversampling = oversample/samplerate;
  

}
    
void Filter_FFT::update(void)
{
  audio_block_t *block;
  block = receiveWritable();//receiveReadOnly(); receiveWritable
  if (!block) return;


  float help=0;
  
  for(int i=0; i<AUDIO_BLOCK_SAMPLES;i=i+oversampling)
  {
    
    //help=block->data[i]/oversampling;
    /*for(int j=1; j<oversampling;j++)
    {
      help = help + block->data[i+j]/oversampling;
    }*/
    //array_avg[countAVG] = (int16_t)help;

    array_avg[countAVG] = (int16_t)block->data[i];
    
    countAVG++;
  }
  
  if(countAVG >= AUDIO_BLOCK_SAMPLES)
  {
    
    memcpy(&block->data[0],&array_avg[0],AUDIO_BLOCK_SAMPLES * sizeof(int16_t));
    
    countAVG=0;

    transmit(block);
    release(block);
    return;
  }
  else
  {
    release(block);
    return; 
  }
}
