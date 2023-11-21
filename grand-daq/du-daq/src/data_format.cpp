#include "data_format.h"
#include <utils.h>
#include <chrono>

using namespace grand;

ElecEvent::ElecEvent(uint16_t *data, int sz)
{
  m_data = data;
  m_size = sz;
}

ElecEvent::~ElecEvent(){
    delete m_data;
    m_data = nullptr;
}

ElecEvent::s_time ElecEvent::getTimeNotFullDataSz()
{
    struct tm tt;
    double fracsec;
    uint32_t sec, nanosec;
    int offset = 0;

    tt.tm_sec = (m_data[offset+EVT_STATSEC-2]&0xff)-m_data[offset+EVT_LEAP-2];    // Convert GPS in a number of seconds
    tt.tm_min = (m_data[offset+EVT_MINHOUR-2]>>8)&0xff;
    tt.tm_hour = (m_data[offset+EVT_MINHOUR-2])&0xff;
    tt.tm_mday = (m_data[offset+EVT_DAYMONTH-2]>>8)&0xff;
    tt.tm_mon = (m_data[offset+EVT_DAYMONTH-2]&0xff)-1;
    tt.tm_year = m_data[offset+EVT_YEAR-2] - 1900;
    sec = (unsigned int)timegm(&tt);

    if(*(m_data+36)!=0) {
        fracsec = (double)(*(uint32_t*)(m_data+34))/(double)(*(uint32_t*)(m_data+36));
        nanosec = uint32_t(fracsec*1000000000ULL);
        *(m_data-m_size+2) = nanosec;
    }
    else {
        fracsec = 0;
    }
    s_time getTimeNotFullData_Sz;
    getTimeNotFullData_Sz.totalSec = (uint64_t)sec*1000000000 + (uint64_t)nanosec;
    getTimeNotFullData_Sz.sec = (uint64_t)sec;
    getTimeNotFullData_Sz.nanosec = (uint64_t)nanosec;

    return getTimeNotFullData_Sz;
}

ElecEvent::s_time ElecEvent::getTimeFullDataSz()
{
    struct tm tt;
    double fracsec;
    uint32_t sec, nanosec;
    int offset = 0;
    
    tt.tm_sec = (m_data[offset+EVT_STATSEC+newDataSzAdded]&0xff)-m_data[offset+EVT_LEAP+newDataSzAdded];    // Convert GPS in a number of seconds
    tt.tm_min = (m_data[offset+EVT_MINHOUR+newDataSzAdded]>>8)&0xff;
    tt.tm_hour = (m_data[offset+EVT_MINHOUR+newDataSzAdded])&0xff;
    tt.tm_mday = (m_data[offset+EVT_DAYMONTH+newDataSzAdded]>>8)&0xff;
    tt.tm_mon = (m_data[offset+EVT_DAYMONTH+newDataSzAdded]&0xff)-1;
    tt.tm_year = m_data[offset+EVT_YEAR+newDataSzAdded] - 1900;
    sec = (unsigned int)timegm(&tt);
	
    if(*(m_data+EVT_CTP)!=0) {
        int CTP_value = 500000000;
        fracsec = (double)(*(uint32_t*)(m_data+EVT_CTD+newDataSzAdded))/(double)(*(uint32_t*)(m_data+EVT_CTP+newDataSzAdded));
        // fracsec = (double)(*(uint32_t*)(m_data+EVT_CTD+newDataSzAdded))/(double)(CTP_value);
        nanosec = uint32_t(fracsec*1000000000ULL);
        *(m_data-m_size+EVT_NANOSEC+newDataSzAdded) = nanosec;
    }
    else {
        fracsec = 0;
    }

    s_time getTimeFullData_Sz;
    getTimeFullData_Sz.totalSec = (uint64_t)sec*1000000000 + (uint64_t)nanosec;
    getTimeFullData_Sz.sec = (uint64_t)sec;
    getTimeFullData_Sz.nanosec = (uint64_t)nanosec;

    return getTimeFullData_Sz;
}
