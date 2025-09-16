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
#include <tuple>
#include "template_FLT.h"
#include "tflite_inference.h"

#define newDataSzAdded 2
#define TEMPLATES_XY_FILE "/home/root/grand-daq/arm/template_lib/templates_ZHAireS_DC2.1rc4_RAW_100_5.txt"

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

    bool scope_t2_template (float thresold);
    std::tuple<uint16_t,uint16_t> scope_t2(TemplateFLT *template_flt_x, TemplateFLT *template_flt_y, S_TFLite *cnn_flt);

  private:
    uint32_t m_size;
    uint16_t *m_data;
  };
  
  class DaqEvent {
    
  };
}

