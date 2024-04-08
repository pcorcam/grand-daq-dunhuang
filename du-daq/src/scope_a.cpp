/*********************************************
 *Created by duanbh,
 *2022/4/22,
 *ScopeA stands for now foreign version software.
**********************************************/
#include "scope_a.h"
#include <cassert>
#include <stdint.h>
#include <utils.h>
#include <chrono>

using namespace grand;
using namespace std;

ScopeA::ScopeA()
{
}

ScopeA::~ScopeA(){
    delete evtbuf;
    evtbuf = nullptr;
}

void ScopeA::scopeRawRead(uint32_t regAddr, uint32_t *value) // new, reading from AXI
{
    *value = *((unsigned int *)((char *)m_axiPtr+m_pageOffset+regAddr));
}

void ScopeA::scopeRawWrite(uint32_t regAddr, uint32_t value)
{
  *((unsigned int *)((char *)m_axiPtr+m_pageOffset+regAddr)) = value;
}

void ScopeA::scopeFlush()
{
  assert(("m_axiPtr should not be NULL", m_axiPtr != nullptr));
  scopeRawWrite(Reg_GenControl, 0x08000000); // clear DAQ Fifo's
  scopeRawWrite(Reg_GenControl, 0x00000000);
}

void ScopeA::scopeSetParameter(uint32_t regAddr, uint32_t value, bool toShadow)
{
    assert(m_axiPtr != nullptr);
    if(toShadow) m_shadowList[regAddr>>2] = value;
    scopeRawWrite(regAddr,value);
    usleep(1000);
}

void ScopeA::elecInit() {
    unsigned int addr, page_addr;
    unsigned int page_size=sysconf(_SC_PAGESIZE);
    
    printf("open scope\n");
    if(m_dev != 0) close(m_dev);
    m_axiPtr = NULL;
    printf("Trying to open !%s!\n",DEVFILE);
    assert((m_dev = open(DEVFILE, O_RDWR)) != -1);
    m_dev = open(DEVFILE, O_RDWR);
    assert(("m_dev open failed", m_dev != -1));
    addr = (unsigned int) TDAQ_BASE;
    page_addr = (addr & ~(page_size-1));
    m_pageOffset = addr - page_addr;
    m_axiPtr=mmap(NULL,page_size,PROT_READ|PROT_WRITE, MAP_SHARED,m_dev,page_addr);
    if ((long)m_axiPtr == -1) {
        perror("opening scope\n");
        exit(-1);
    }
    printf("Done opening m_dev = %d\n",(int)m_dev);
    sleep(1);
    assert(("m_axiPtr should not be NULL",m_axiPtr != NULL));
    //scopeRawWrite(Reg_Dig_Control,0x00004000);
    scopeRawWrite(Reg_Dig_Control,0x00000000);
    memset(m_shadowList,0,sizeof(m_shadowList));
    assert(("m_axiPtr should not be NULL",m_axiPtr != NULL));
    scopeFlush();
}

void ScopeA::elecConfig(void *parameters) {
    memcpy(m_shadowList, parameters, Reg_End);
    uint32_t *m_shadowList = (uint32_t*)parameters;

    for(int i=0;i<Reg_End;i+=4){
        scopeSetParameter(i, m_shadowList[i>>2]);
    }
}

void ScopeA::elecStartRun() {
    scopeFlush();
    scopeSetParameter(Reg_Dig_Control,m_shadowList[Reg_Dig_Control>>2] |(CTRL_PPS_EN | CTRL_SEND_EN ), true);
}

int ScopeA::elecReadData(char *data, size_t maxSize, uint32_t* hitId){
    std::unique_lock<mutex> lock(m_mtx);

    assert(("m_axiPtr should not be NULL", m_axiPtr != NULL));
    uint32_t isData, tbuf, *pbuf, ctp;
    bool hasData = false;
    int ret = 0;
    int offset = 0;
    int32_t prevgps = 0;
    int32_t evgps=0; 
    int length;
    struct tm tt;
    double fracsec;
    uint32_t sec, nanosec;

    scopeRawRead(Reg_GenStatus,&isData);
    if((isData&(GENSTAT_EVTFIFO)) == 0) {
        scopeRawWrite(Reg_GenControl,GENCTRL_EVTREAD);
        // buffer中有数据
        uint32_t dataSizeRaw;
        scopeRawRead(Reg_Data, &dataSizeRaw);
        if((dataSizeRaw>>16) == 0xADC0) {
            uint32_t dataSizeBytes = (dataSizeRaw&0xffff)*sizeof(uint16_t);
            int dataSizeDWord = dataSizeBytes/(sizeof(uint32_t));
            assert(("data buffer size >= real data size", maxSize >= dataSizeBytes+sizeof(uint32_t) ));    
            uint32_t *ptr = (uint32_t*)data;
            *ptr++ = *hitId;
            (*hitId)++;
            (*ptr++) = dataSizeRaw;

            evtbuf = new uint16_t[2*dataSizeDWord];
            evtbuf[2*dataSizeDWord] = {0};

            int n=0;
            for(int i=0; i<dataSizeDWord-1; i++) {
                scopeRawRead(Reg_Data,ptr);
                *(evtbuf+2*n) = *(ptr);
                *(evtbuf+2*n+1) = (*(ptr))>>16;
                n++;
                ptr++;
            }
            *(ptr-dataSizeDWord+1) = HEADER_EVT << 16;

            ElecEvent ev(evtbuf, dataSizeDWord*2);
            ev.getTimeNotFullDataSz(); // use getTimeNotFullDataSz because we do not include index 0 data.
            *(ptr-dataSizeDWord+2) = ev.getTimeNotFullDataSz().sec;
            *(ptr-dataSizeDWord+3) = ev.getTimeNotFullDataSz().nanosec;

            ret = dataSizeBytes+sizeof(uint32_t);
        }
        else {
            // dat in buffer format error
            // read data to empty
            CLOG(WARNING, "data") << "data format error 1";
            int readToEmptyLen = 10000;
            uint32_t tmpData;

            do {
                //rate.add();
                scopeRawRead(Reg_Data, &tmpData);
                scopeRawRead(Reg_GenStatus, &isData);
                readToEmptyLen --;

                if((tmpData>>16) == 0xADC0) {
                    uint32_t dataSizeBytes = (tmpData&0xffff)*sizeof(uint16_t);
                    int dataSizeDWord = dataSizeBytes/(sizeof(uint32_t));
                    printf("error level 2 maxsize: %d\n", maxSize);
                    assert(("data buffer size >= real data size", maxSize >= dataSizeBytes+sizeof(uint32_t) ));    
                    uint32_t *ptr = (uint32_t*)data;
                    *ptr++ = *hitId;
                    (*hitId)++;
                    printf("hit: %lld\n", *hitId);
                    (*ptr++) = tmpData;
                    evtbuf = new uint16_t[2*dataSizeDWord];
                    evtbuf[2*dataSizeDWord] = {0};
                    
                    int n=0;
                    for(int i=0; i<(dataSizeDWord-1); i++) {
                        scopeRawRead(Reg_Data,ptr);
                        *(evtbuf+2*n) = *(ptr);
                        *(evtbuf+2*n+1) = (*(ptr))>>16;
                        n++;
                        ptr++;
                    }
                    *(ptr-dataSizeDWord+1) = HEADER_EVT << 16;

                    ElecEvent ev(evtbuf, dataSizeDWord*2);
                    ev.getTimeNotFullDataSz(); // use getTimeNotFullDataSz because we do not include index 0 data.
                    *(ptr-dataSizeDWord+2) = ev.getTimeNotFullDataSz().sec;
                    *(ptr-dataSizeDWord+3) = ev.getTimeNotFullDataSz().nanosec;
                    // uint64_t tmp; // test block
                    // tmp = ev.getTimeNotFullDataSz().totalSec;
                    ret = dataSizeBytes+sizeof(uint32_t);
                    break;
                }
            } while((isData&(GENSTAT_EVTFIFO)) == 0 && readToEmptyLen > 0);
            CLOG(WARNING, "data") << "try read fifo to empty done";
            //scopeFlush();
        }
        //t1.stop();
        //CLOG(DEBUG, "data") << "CPU time of 1 event readout = " << t1.timeMs() << " ms";
    }

    return ret;
}

void ScopeA::elecStopRun() {
    printf("in elecStopRun: %08x\n", m_shadowList[Reg_Dig_Control>>2]);
    scopeSetParameter(Reg_Dig_Control, m_shadowList[Reg_Dig_Control>>2] & (~CTRL_PPS_EN & ~CTRL_SEND_EN ), true);
    scopeFlush();
}

void ScopeA::elecTerminate() {
    close(m_dev);
    m_dev = 0;
    m_axiPtr = NULL;
}

