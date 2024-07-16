#pragma once
#include <iostream>
#include <string.h>
#include <scope.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/mman.h>
#include "tflite_inference.h"
// #include "/home/grand/externals/tensorflow-master/tensorflow/lite/c/c_api.h"

#define newDataSzAdded 2
#define TFLT_SAMPLE_IN_TRACE 1024
#define EVT_TRACELENGTH 28
#define EVT_START_ADC 254
#define EVT_PROBABILITY 8

namespace grand{

  class ElecEvent {
  public:
    ElecEvent(uint16_t *data, int sz);
    ~ElecEvent();
    
    //uint32_t getEvID();
    //uint32_t getSize();
    //uint32_t getLongitude();
  typedef struct 
  {
    uint32_t sec;
    uint32_t nanosec;
    uint64_t totalSec;
  }s_time;
    
    s_time getTimeNotFullDataSz();
    s_time getTimeFullDataSz();
    S_TFLite *G_ptflt1 = NULL;
    int scope_t2 (float thresold);

  private:
    uint32_t m_size;
    uint16_t *m_data;
  };
  

  // class DaqEvent {
    
  // };
}

