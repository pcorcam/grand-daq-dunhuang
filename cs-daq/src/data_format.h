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
#include <filesystem>

#define rawEventHeader 2
#define newDataSzAdded 6

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
  private:
    uint32_t m_size;
    uint16_t *m_data;
  };
  
  class DaqEvent {
    
  };

  // Validate the filename so it conforms do GRAND format
  int validateFilename(const std::string& filename);

}

