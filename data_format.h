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

#define newDataSzAdded 2

class ElecEvent {
  public:
    ElecEvent(uint16_t *data, int sz);
    ~ElecEvent();
    
    typedef struct {
      uint32_t sec;
      uint32_t nanosec;
      uint64_t totalSec;
      std::string gpsReadableTm;
    }s_time;
    
    s_time getTimeNotFullDataSz();
    s_time getTimeFullDataSz();
    
  private:
    uint32_t m_size;
    uint16_t *m_data;
};



