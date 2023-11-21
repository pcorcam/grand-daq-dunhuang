/****************/
// Created by duanbh,
// 20220910.
/****************/
#include <data_manager.h>
#include <cassert>
#include <message_impl.h>
#include <chrono>
#include <cmath>
#include <mutex>

using namespace grand;

extern int duID;
// extern int duDAQMODE;

DataManager::DataManager() : m_eventBuffer(nullptr), m_duTimeStampSave(nullptr) {
    m_server = new ZMQServer;
    m_frequenceCountBefore = 0;
    m_frequenceCountAfter = 0;
}
DataManager::~DataManager() {
    terminate();
}

void DataManager::setEventOutput(EventOutput fun) {
    m_eventOutputFun = fun;
}

void DataManager::setT2EventOutput(EventOutput fun) { // decide if we output TR info.
    m_t2EventOutputFun = fun;
}

void DataManager::setRawEventOutput(EventOutput fun) { // decide if we output TR info.
    m_rawEventOutputFun = fun;
}

void DataManager::initialize()
{
    // TODO: create event buffer
    if(m_eventBuffer == nullptr) {
        m_eventBuffer = new char[1024000]();
        m_ringBuffer = new char[M_RINGBUFSZ]();
        m_tag = new char[20]();
        m_duNumbers = new char[20];
    }
    
    if(m_duTimeStampSave == nullptr) {
        m_duTimeStampSave = new char[M_DUTIMESTAMPSZ]();
        m_timeBuffer = new char[M_TIMEBUFFERSZ](); // This array should keep same as m_duTimeStampSave.
    }

    m_rawEvent = new char[1024000]();
    m_rawEventSz = 1024000;
}

void DataManager::terminate()
{
    // TODO: release all
    if(m_eventBuffer) {
        delete m_eventBuffer;
        delete m_server;
        delete m_msgDispatcher;
        delete m_ringBuffer;
        delete m_tag;
        delete m_duNumbers;
        m_duNumbers = nullptr;
        m_tag = nullptr;
        m_ringBuffer = nullptr;
        m_msgDispatcher = nullptr;
        m_server = nullptr;
        m_eventBuffer = nullptr;
    }
    if(m_duTimeStampSave) {
        delete m_duTimeStampSave;
        delete m_timeBuffer;
        delete m_evtbuf;
        m_evtbuf = nullptr;
        m_timeBuffer= nullptr;
        m_duTimeStampSave = nullptr;
    }

    delete m_rawEvent;
    m_rawEvent = nullptr;
}

void DataManager::addRawEvent(char *data, size_t sz)
{   
    uint64_t nanoTime = 0;
    	
    m_evtbuf = new uint16_t[sz/sizeof(uint16_t)];
    memset(m_evtbuf, 0, sz);
    memcpy(m_evtbuf, data, sz);
    ElecEvent ev(m_evtbuf, sz/sizeof(uint16_t));
    nanoTime = ev.getTimeFullDataSz().totalSec; // use getTimeFullDataSz because we include index 0 data length here.
    printf("nanotime is %lld\n", nanoTime);
	
    RawEvent msg(m_eventBuffer, 1024000, true);
    msg.copyFrom(data, sz);
    if(m_rawEventOutputFun) {
        // std::cout << "msg.size() in addRawEvent is " << msg.size() << std::endl;
        m_rawEventOutputFun(msg.base(), msg.size());
    }
    
    if(m_frequenceCountBefore == 0) {
    m_t0 = XClock::nowNanoSeconds();
    }
    m_frequenceCountBefore++;
    m_t1 = XClock::nowNanoSeconds();
    if(m_t1 - m_t0 > timeCount) {
        printf("L1 trigger frequence is %d/s\n", m_frequenceCountBefore - m_frequenceCountAfter);
        m_frequenceCountAfter = m_frequenceCountBefore;
        m_t0 = m_t1;
    }
    

}

void DataManager::addEvent(char *data, size_t sz, int daqMode) 
{   
    if(daqMode == 3) {
        RawEvent rawMsg(m_rawEvent, m_rawEventSz, true);
        rawMsg.copyFrom(data, sz);
        if(m_rawEventOutputFun) {
            // std::cout << "raw data msg.size is " << rawMsg.size() << std::endl;
            m_rawEventOutputFun(rawMsg.base(), rawMsg.size());
        }
    }

    std::lock_guard<std::mutex> mylockguard(m_mutex_IF);
    m_rawDataSize = sz;
    uint64_t t0, t_acceptT2Msg;
    int timeID_0, time_ID_end;
    char timeTmp[EACH_DATA_SZ] = {0};
    char timeT0[EACH_DATA_SZ] = {0};
    char timeAcceptT2Msg[EACH_DATA_SZ] = {0};
    char duIdTmp[4] = {0};
   
    m_time = 0;


    m_evtbuf = new uint16_t[sz/sizeof(uint16_t)];
    memset(m_evtbuf, 0, sz);
    memcpy(m_evtbuf, data, sz);
    ElecEvent ev(m_evtbuf, sz/sizeof(uint16_t));

    m_time = ev.getTimeFullDataSz().totalSec; // use getTimeFullDataSz because we include index 0 data length here.
    // printf("m_time is %lld, duID is %d\n", m_time, duID);
    printf("m_time is %lld\n", m_time);
    sprintf(duIdTmp, "%d", duID);
    if (szofRingBuffer+sz < M_RINGBUFSZ) {
        memcpy(m_ringBuffer + szofRingBuffer, data, sz); // save raw DAQ data.
    } else {
        memset(m_duTimeStampSave, 0, M_DUTIMESTAMPSZ); // (szof_m_duTimeStampSave/EACH_DATA_SZ)*sz should less than szofRingBuffer
        szof_m_duTimeStampSave = 0; // These two keep pace. 
        memset(m_ringBuffer, 0, M_RINGBUFSZ);
        szofRingBuffer = 0;
        szSended = 0;
    }

    if(szof_m_duTimeStampSave == 0)
        memcpy(m_duTimeStampSave, duIdTmp, sizeof(int)); // ****** need to be improved
    
    // *************test for now, if GPS can not work well, we can use this block to put time in order**************** //
    // if(m_count == 0) {
    //     m_timeInit = m_time;
    // }
    // if(m_time < m_timeInit) {
    //     m_timeInit = m_time;
    // }
    // int ret = snprintf(timeTmp, EACH_DATA_SZ, "%lld", m_time-m_timeInit+1000000000);
    // m_count++;
    // ***************************************** //
    int ret = snprintf(timeTmp, EACH_DATA_SZ, "%lld", m_time);
    memcpy(m_duTimeStampSave + sizeof(int) + szof_m_duTimeStampSave, timeTmp, EACH_DATA_SZ);
    assert(("sizeof(int) + szSended should less than M_DUTIMESTAMPSZ",sizeof(int) + szSended < M_DUTIMESTAMPSZ));
    memcpy(timeT0, m_duTimeStampSave+sizeof(int) + szSended, EACH_DATA_SZ); // get first time in m_duTimeStampSave
    assert(("sizeof(int) + szof_m_duTimeStampSave should less than M_DUTIMESTAMPSZ", sizeof(int) + szof_m_duTimeStampSave < M_DUTIMESTAMPSZ));
    memcpy(timeAcceptT2Msg, m_duTimeStampSave + sizeof(int) + szof_m_duTimeStampSave, EACH_DATA_SZ);
    t_acceptT2Msg = atoll(timeAcceptT2Msg);
    m_timeStart = (atoll(timeT0)/timeCut)*timeCut;
    // printf("m_timeStart is %lld\n", m_timeStart);
    // printf("t_acceptT2Msg is %lld\n", t_acceptT2Msg);
    if ( t_acceptT2Msg-m_timeStart>timeCut )
    {   
        printf("t_acceptT2Msg-m_timeStart is %lld\n", t_acceptT2Msg-m_timeStart);
        if(t_acceptT2Msg-m_timeStart<0) {
            m_timeStart += timeCut;
        }
        else {
            if(szof_m_duTimeStampSave >= szSended) {
                memset(m_timeBuffer, 0 ,M_TIMEBUFFERSZ);
                T2Message t2msg(m_timeBuffer, M_TIMEBUFFERSZ, true);
                assert(szSended < M_DUTIMESTAMPSZ);
                assert(szof_m_duTimeStampSave+sizeof(int)-szSended < M_DUTIMESTAMPSZ);
                t2msg.addTime(m_duTimeStampSave+szSended, szof_m_duTimeStampSave+sizeof(int)-szSended); // keep tmID same, because t_acceptT2Msg-m_timeStart>timeCut 
                szSended = szof_m_duTimeStampSave;
                if(m_t2EventOutputFun){
                    m_t2EventOutputFun(t2msg.base(), t2msg.size());
                    // printf("t2msg header type is %d\n", t2msg.type());
                }
                m_timeStart += timeCut;
            }
            else {
                szSended = 0; // In this case, m_duTimeStampSave has been initialized;
            }
        }
    }

    szof_m_duTimeStampSave += EACH_DATA_SZ;
    szofRingBuffer+=sz;
    
    // delete m_evtbuf;
    // m_evtbuf = nullptr;
}

void DataManager::accept(char *data, size_t sz) // send a buffer which include eventIDs' information
{   
    // *********** Random Trigger *************//
    // acceptRandomTrigger(data, sz);

    // *********** T3 Trigger ************ //
    // acceptT3Trigger(data, sz);

    int daqMode = 0;
    daqMode = m_server->m_duDAQMode;
    // daqMode = duDAQMODE;
    // std::cout << "DAQ MODE IS " << daqMode;
    if(daqMode == 1) 
        std::cout << "now will run RAW_DATA mode!" << std::endl;
    if(daqMode == 2)
        std::cout << "now will run T3_DATA mode!" << std::endl;
        acceptT3Trigger(data, sz);
    if(daqMode == 3) {
        std::cout << "now will run BOTH_DATA mode!" << std::endl;
        acceptT3Trigger(data, sz);
    }
}

void DataManager::acceptT3Trigger(char* data, size_t sz) {
    std::lock_guard<std::mutex> mylockguard(m_mutex_BA);
    szT3Trigger = sz;
    int q = sz/EACH_DATA_SZ;
    int sign=0;
    int szUsed = 0;
    uint32_t TAG = 0;
    char tmp[EACH_DATA_SZ] = {0};
    char tmpTag[4] = {0};
    char tmpBuf[EACH_DATA_SZ] = {0};
    
    uint64_t t1 = 0;
    uint64_t t2[q];
    
    for(int i=0; i<q; i++) {
        memcpy(tmp, data+i*EACH_DATA_SZ, EACH_DATA_SZ);
        t2[i] = atoll(tmp);
    }
    // m_tag and m_duNumbers is added after all raw data, the position may change if we add another new data when we get trigger timestamp.
    memset(m_tag, 0, 4);
    memcpy(m_tag, data+szT3Trigger-2*sizeof(uint32_t), sizeof(uint32_t));
    memset(m_duNumbers, 0, 20);
    memcpy(m_duNumbers, data+szT3Trigger-sizeof(uint32_t), sizeof(uint32_t));

    printf("m_tag is %d\n", atoi(m_tag));
    // printf("trigger dus number is %d\n", atoi(m_duNumbers));
    for(int i=0; i<szof_m_duTimeStampSave/EACH_DATA_SZ; i++) {
        tmpBuf[EACH_DATA_SZ] = {0};
        memcpy(tmpBuf, m_duTimeStampSave+sizeof(int)+i*EACH_DATA_SZ, EACH_DATA_SZ);
        for(int j=0; j<q; j++) {
            t1 = t2[j];
            assert((j<q, "j should less than q"));
            if(t1 == atoll(tmpBuf)) {
                m_mutex.lock();
                evtID1 = 0;
                evtID1 = i; 
                DAQEvent t3msg(m_eventBuffer, 100000, true);
                assert((evtID1*m_rawDataSize < szofRingBuffer,"evtID1*m_rawDataSize shoule less than szofRingBuffer"));
                t3msg.copyFrom(m_tag, sizeof(uint32_t));
                t3msg.copyFrom(m_duNumbers, sizeof(uint32_t));
                t3msg.copyFrom(m_ringBuffer+evtID1*m_rawDataSize, m_rawDataSize);
                
                // char test1[4]; // ok here
                // memcpy(test1, m_eventBuffer + sizeof(MessageHeader), 4);
                // printf("m_tag is %d\n", atol(test1));
                
                if(m_eventOutputFun) {
                    m_eventOutputFun(t3msg.base(), t3msg.size()); // call write function, which is packed by m_dataManager.
                }
                // printf("t3msg sz is %d\n", t3msg.size());
                memset(m_eventBuffer,0,100000);
                m_mutex.unlock();
                break;
            }
            else 
                continue;
        }
    }
}

// void DataManager::acceptRandomTrigger(char* data, size_t sz) {
//     int q = sz/20;
//     char tmp[20] = {0};
//     szofRandomTrigger = sz;
//     m_triggerTime = new char[sz];
//     // std::cout << "q is " << q << std::endl;
//     if(q!=0){
//         // for (int i = 0; i<q; i++) {
//         // memcpy(tmp, data + i*20, 20);
//         // printf("tmp is %lld\n", atoll(tmp));
//         // tmp[20] = {0};
//         // }
//         memcpy(m_triggerTime, data, sz);
//     }

//         uint64_t t1, t2;
//         if(szofRandomTrigger > 0) {
//            std::cout << "szofRandomTrigger is " << szofRandomTrigger << std::endl;
//            char tmp[20] = {0};
//            for(int i = 0; i < szofRandomTrigger/20; i++){
//                memcpy(tmp, m_triggerTime + i*20, 20);
//                // printf("tmp is %lld\n", atoll(tmp));
//                if(i == 0) {
//                    t1 = atoll(tmp);
//                    printf("t1 is %lld\n", t1);
//                }
//                else if(i == 1) {
//                    t2 = atoll(tmp);
//                    printf("t2 is %lld\n", t2);
//                }
//            }
//            szofRandomTrigger = 0;
//         }
//         /*******************************************/
//         /* ************************************************************* can be packed*/
//         t3msg = new DAQEvent(m_eventBuffer, 10240000, true); // chage it to stable array
//         char tmpBuf[20] = {0};
//         if((t1>0) && (t2>t1)) {
//             int eventID1=0; 
//             int eventID2=0;
//             for(int i=0; i < (szofTimeBufferBak - 12)/20; i++) {
//                memcpy(tmpBuf, m_timeBufferBak+12+i*20, 20);
//             // printf("tmpBuf is %lld\n", atoll(tmpBuf));
//                if(atoll(tmpBuf) == t1) {
//                    eventID1 = i;
//                    std::cout << "eventID1 is " << eventID1 << std::endl;
//                 //    printf("tmpBuf is %lld\n", atoll(tmpBuf));
//                }
//                if(atoll(tmpBuf) == t2) {
//                    eventID2 = i;
//                    std::cout << "eventID2 is " << eventID2 << std::endl;
//                 //    printf("tmpBuf is %lld\n", atoll(tmpBuf));
//                }
//             }
//             // up is ok.
//             // assert((eventID2 >= eventID1,"eventID2 should larger or equal eventID1");
//             if(eventID2-eventID1>=0) {
//                 std::cout << "sz of t3msg is " << (eventID2-eventID1)*m_rawDataSize << std::endl;
//                 t3msg->copyFrom(m_ringBuffer + eventID1*m_rawDataSize, (eventID2-eventID1)*m_rawDataSize);
//                 memset(m_ringBuffer,0,40000000);
//                 szofRingBuffer = 0;
//                 if(m_eventOutputFun) {
//                     m_eventOutputFun(t3msg->base(), t3msg->size()); // call write function, which is packed by m_dataManager.
//                 }
//                 memset(m_eventBuffer, 0, 10240000);
//             }
//             // else {
//             //     FILE *fp = fopen("abnormal.txt","a+");
//             //     fprintf(fp, "eventID2 < eventID1.\n");
//             //     fclose(fp);
//             // }
            
//         }

//         // **************************************************************
//         // memcpy(m_timeBufferBak, (char*)t2msg->base(), t2msg->size());
//         // szofTimeBufferBak = t2msg->size();
//         memset(m_timeBuffer, 0 ,200000);
//     // }
//     // // **************************************************************
// }

