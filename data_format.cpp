#include "data_format.h"
#include <utils.h>
#include <chrono>
#include <iomanip> // for std::put_time
#include <sstream> // for std::ostringstream

ElecEvent::ElecEvent(uint16_t *data, int sz)
{
    m_data = data;
    m_size = sz;
}

ElecEvent::~ElecEvent(){
    // delete m_data;
    // m_data = nullptr;
}

ElecEvent::s_time ElecEvent::getTimeNotFullDataSz()
{
    struct tm tt;
    double fracsec;
    uint32_t sec, nanosec;
    int offset = 0;
    uint32_t CTP_value;

    tt.tm_sec = (m_data[offset+EVT_STATSEC-2]&0xff)-m_data[offset+EVT_LEAP-2];    // Convert GPS in a number of seconds
    tt.tm_min = (m_data[offset+EVT_MINHOUR-2]>>8)&0xff;
    tt.tm_hour = (m_data[offset+EVT_MINHOUR-2])&0xff;
    tt.tm_mday = (m_data[offset+EVT_DAYMONTH-2]>>8)&0xff;
    tt.tm_mon = (m_data[offset+EVT_DAYMONTH-2]&0xff)-1;
    tt.tm_year = m_data[offset+EVT_YEAR-2] - 1900;
    sec = (unsigned int)timegm(&tt);

    if(*(m_data+36)!=0) {
        // fracsec = (double)(*(uint32_t*)(m_data+34))/(double)(*(uint32_t*)(m_data+36));
        CTP_value = *(uint32_t*)(m_data+36);
        CTP_value &= ~(1 << 31);
        fracsec = (double)(*(uint32_t*)(m_data+34))/(double)(CTP_value);
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
    uint32_t CTP_value;
    
    tt.tm_sec = (m_data[offset+EVT_STATSEC+newDataSzAdded]&0xff)-m_data[offset+EVT_LEAP+newDataSzAdded];    // Convert GPS in a number of seconds
    tt.tm_min = (m_data[offset+EVT_MINHOUR+newDataSzAdded]>>8)&0xff;
    tt.tm_hour = (m_data[offset+EVT_MINHOUR+newDataSzAdded])&0xff;
    tt.tm_mday = (m_data[offset+EVT_DAYMONTH+newDataSzAdded]>>8)&0xff;
    tt.tm_mon = (m_data[offset+EVT_DAYMONTH+newDataSzAdded]&0xff)-1;
    tt.tm_year = m_data[offset+EVT_YEAR+newDataSzAdded] - 1900;
    sec = (unsigned int)timegm(&tt);

    // 使用 std::chrono 处理时间戳
    using namespace std::chrono;
    system_clock::time_point tp = system_clock::from_time_t(sec);
    std::time_t time_t_sec = system_clock::to_time_t(tp);

    // 格式化时间
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time_t_sec), "%Y%m%d-%H%M%S");
	
    if(*(m_data+EVT_CTP)!=0) {
        // fracsec = (double)(*(uint32_t*)(m_data+EVT_CTD+newDataSzAdded))/(double)(*(uint32_t*)(m_data+EVT_CTP+newDataSzAdded));
        CTP_value = *(uint32_t*)(m_data+EVT_CTP+newDataSzAdded);
        CTP_value &= ~(1 << 31);
        fracsec = (double)(*(uint32_t*)(m_data+EVT_CTD+newDataSzAdded))/(double)(CTP_value);
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
    getTimeFullData_Sz.gpsReadableTm = oss.str();

    return getTimeFullData_Sz;
}
