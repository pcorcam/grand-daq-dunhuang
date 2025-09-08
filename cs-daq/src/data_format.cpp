#include "data_format.h"
#include <utils.h>
#include <chrono>

// #define CTP_value 499998552

using namespace grand;

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
    std::cout << "getTimeNotFullDataSz" << std::endl;
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
        // *(m_data-m_size+2) = nanosec;
    }
    else {
        fracsec = 0;
    }
    s_time getTimeNotFullData_Sz;
    getTimeNotFullData_Sz.totalSec = (uint64_t)sec*1000000000 + (uint64_t)nanosec;
    getTimeNotFullData_Sz.sec = (uint64_t)sec;
    getTimeNotFullData_Sz.nanosec = (uint64_t)nanosec;

    std::cout << "getTimeNotFullDataSz end" << std::endl;
    return getTimeNotFullData_Sz;
}

ElecEvent::s_time ElecEvent::getTimeFullDataSz()
{   
    // std::cout << "getTimeFullDataSz" << std::endl;
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

    if(*(m_data+EVT_CTP)!=0) {
        // fracsec = (double)(*(uint32_t*)(m_data+EVT_CTD+newDataSzAdded))/(double)(*(uint32_t*)(m_data+EVT_CTP+newDataSzAdded));
	    CTP_value = *(uint32_t*)(m_data+EVT_CTP+newDataSzAdded);
        CTP_value &= ~(1 << 31);
        fracsec = (double)(*(uint32_t*)(m_data+EVT_CTD+newDataSzAdded))/(double)(CTP_value);
        nanosec = uint32_t(fracsec*1000000000ULL);
	// *(m_data-m_size+EVT_NANOSEC+newDataSzAdded) = nanosec;
    }
    else {
        fracsec = 0;
    }
    s_time getTimeFullData_Sz;
    getTimeFullData_Sz.totalSec = (uint64_t)sec*1000000000 + (uint64_t)nanosec;
    getTimeFullData_Sz.sec = (uint64_t)sec;
    getTimeFullData_Sz.nanosec = (uint64_t)nanosec;

    // std::cout << "getTimeFullDataSz end" << std::endl;
    return getTimeFullData_Sz;
}

namespace grand
{
    // Validate the filename so it conforms do GRAND format
    int validateFilename(const std::string& filename)
    {
        std::string rest;
        // Operate on just the file name without extension
        std::istringstream stream(std::filesystem::path(filename).stem());
        std::string chunk;
        std::vector<std::string> parts;

        // Splitting the filename by "_"
        while (std::getline(stream, chunk, '_'))
        {
            parts.push_back(chunk);
        }

        // If less than 6 parts, it is an old file or has a malformed name
        if (parts.size()<6)
        {
            std::cout << "The file name " << filename << " is not in the expected format, there are less than 6 parts between underscores" << std::endl;
            return -1;
        }
        // If more than 6 parts, it is an old file or has a malformed name
        if (parts.size()>6)
        {
            std::cout << "The file name " << filename << " is not in the expected format, there are more than 6 parts between underscores" << std::endl;
            return -2;
        }

        // If the number of parts is correct, check them

        // If the first part is not a date
        if(parts[1].size()!=8 || parts[1].find_first_not_of("0123456789") != std::string::npos)
        {
            std::cout << "Incorrect date format " << parts[1] <<", please use YYYYMMDD format" << std::endl;
            return -3;
        }

        // If the second part is not a time
        if(parts[2].size()!=6 || parts[2].find_first_not_of("0123456789") != std::string::npos)
        {
            std::cout << "Incorrect time format " << parts[2] <<", please use hhmmss format" << std::endl;
            return -4;
        }

        // If the third part does not contain a run number
        if (parts[3].size()>3)
        {
            if (parts[3].substr(0,3)=="RUN" && parts[3].substr(4).find_first_not_of("0123456789") != std::string::npos)
            {
                std::cout << "Incorrect RUN format " << parts[3] <<", please use RUN<number>" << std::endl;
                return -4;
            }
        }
        else
        {
            std::cout << "Incorrect RUN format " << parts[3] <<", please use RUN<number>" << std::endl;
            return -4;
        }

        // If the fourth part does not contain a valid acquisition type
        if(parts[4]!="CD" && parts[4]!="MD" && parts[4]!="UD")
        {
            std::cout << "Incorrect mode format " << parts[4] <<", please use CD, MD or UD" << std::endl;
            return -5;
        }

        return 0;
    }
}