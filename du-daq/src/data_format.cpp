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
}

ElecEvent::s_time ElecEvent::getTimeNotFullDataSz()
{

    struct tm tt;
    double fracsec;
    uint32_t sec, nanosec;
    int offset = 0;
    int CTP_value;
    tt.tm_sec = (m_data[offset+EVT_STATSEC-2]&0xff)-m_data[offset+EVT_LEAP-2];    // Convert GPS in a number of seconds
    tt.tm_min = (m_data[offset+EVT_MINHOUR-2]>>8)&0xff;
    tt.tm_hour = (m_data[offset+EVT_MINHOUR-2])&0xff;
    tt.tm_mday = (m_data[offset+EVT_DAYMONTH-2]>>8)&0xff;
    tt.tm_mon = (m_data[offset+EVT_DAYMONTH-2]&0xff)-1;
    tt.tm_year = m_data[offset+EVT_YEAR-2] - 1900;
    sec = (unsigned int)timegm(&tt);
    if(*(m_data+36)!=0) {
        CTP_value = *(uint32_t*)(m_data+36);
        fracsec = (double)(*(uint32_t*)(m_data+34))/(double)(CTP_value &= ~(1<<31));
        nanosec = uint32_t(fracsec*1000000000ULL);
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
    int CTP_value;
    
    tt.tm_sec = (m_data[offset+EVT_STATSEC+newDataSzAdded]&0xff)-m_data[offset+EVT_LEAP+newDataSzAdded];    // Convert GPS in a number of seconds
    tt.tm_min = (m_data[offset+EVT_MINHOUR+newDataSzAdded]>>8)&0xff;
    tt.tm_hour = (m_data[offset+EVT_MINHOUR+newDataSzAdded])&0xff;
    tt.tm_mday = (m_data[offset+EVT_DAYMONTH+newDataSzAdded]>>8)&0xff;
    tt.tm_mon = (m_data[offset+EVT_DAYMONTH+newDataSzAdded]&0xff)-1;
    tt.tm_year = m_data[offset+EVT_YEAR+newDataSzAdded] - 1900;
    sec = (unsigned int)timegm(&tt);
	
    if(*(m_data+EVT_CTP)!=0) {
        // fracsec = (double)(*(uint32_t*)(m_data+EVT_CTD+newDataSzAdded))/(double)(*(uint32_t*)(m_data+EVT_CTP+newDataSzAdded));
        CTP_value = *(uint32_t*)(m_data+EVT_CTP+newDataSzAdded);
        fracsec = (double)(*(uint32_t*)(m_data+EVT_CTD+newDataSzAdded))/(double)(CTP_value &= ~(1<<31));
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

int ElecEvent::scope_t2(float thresold) {
    
    static float proba = -1.0;
    float * p_float;
    int nb_sample = *(int*)*(m_data + EVT_TRACELENGTH);
    uint32_t *channenLen;
    
    if (nb_sample != TFLT_SAMPLE_IN_TRACE) 
    {
        printf ("\nTrigger ConvNet is defined for 1024 samples ONLY, no trigger T2");
        proba = 0.0;
        return (1);
    }

    if (G_ptflt1 == NULL)
    {
        /* use multithreading of Tensorflow Lite => 2 CPUs*/
        G_ptflt1 = TFLT_create (2);
    }

    channenLen = (uint32_t*)(m_data + EVT_START_ADC);
    TFLT_preprocessing (G_ptflt1, channenLen);
    TFLT_inference (G_ptflt1, &proba);
    p_float = (float*)(m_data+EVT_PROBABILITY);
    *p_float=proba;
    printf("\nproba: %f", proba);

    return(1);

    //   if (proba > thresold) return (1);
    //   return (0);
}