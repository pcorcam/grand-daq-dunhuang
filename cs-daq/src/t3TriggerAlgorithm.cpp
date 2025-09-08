/*******************/
// Created by duanbh,
// 20220910.
// Completed on 20221010.
/*******************/
#include "t3_trigger.h"
#include <utils.h>
#include <cs_sys_config.h>
#include <iomanip>
#include <chrono>
#include <cassert>
#include <algorithm>
#include <eformat.h>
#include <message_impl.h>

using namespace grand;

T3Trigger::T3Trigger( TriggerDone triggerDoneCallback, ZMQClient *client) {

    m_bufferSize = CSSysConfig::instance()->appConfig().t2BufferPageSize;
    int numPages = CSSysConfig::instance()->appConfig().t2BufferNumberOfPages;
    
    paras.m_timeCut = CSSysConfig::instance()->appConfig().t3TriggerTimeCut;
    paras.m_timeOut = CSSysConfig::instance()->appConfig().t3TriggerTimeOut;
    paras.m_timeWindow = CSSysConfig::instance()->appConfig().t3TriggerTimeWindow;
    paras.m_triggerThreshold = CSSysConfig::instance()->appConfig().t3TriggerDuNumber;

    m_buffers = new BufferPool(m_bufferSize, numPages);
    m_threadPool = new ThreadPool(1);
    m_triggerDone = triggerDoneCallback;
    m_client = client;
    m_t3TimeBuf = new char[20000000]();
    m_t3TriggerBuf = new char[20000000]();

    m_defaultConfig = ::getenv("GRAND_DAQ_CONFIG");
    m_defaultConfigSys = m_defaultConfig + "/sysconfig.yaml";
}

T3Trigger::~T3Trigger() {
    delete m_buffers;
    delete m_threadPool;
    delete m_t3TimeBuf;
    delete m_t3TriggerBuf;

    m_t3TriggerBuf = nullptr;
    m_t3TimeBuf = nullptr;
    m_threadPool = nullptr;
    m_buffers = nullptr;
}

void T3Trigger::stop() {
    doLastT3Trigger();
}

void T3Trigger::processData(std::string du, char *data, size_t sz) {
    // TODO: this is dummy
    // *********************** DO random trigger ******************** //
    // randomTrigger(data,sz);

    m_daqMode = CSSysConfig::instance()->appConfig().daqMode;
    if(m_daqMode == 2) {
        doT3Trigger(du, data, sz);
    }
    if(m_daqMode == 3) {
        doT3Trigger(du, data, sz);
    }
}

// Random numbers of testmsg to get stable tests.
int T3Trigger::genRand(int min, int max) {
    return ( rand() % (max - min + 1) ) + min ;
}

void T3Trigger::doTrigger(TriggerDone cb) {
    if(cb) {
        cb();
    }
}

void T3Trigger::eachSecondCount(size_t duID, size_t sz) {
    int frequence = sz/EACH_DATA_SZ;
    // printf("DU is %d, L1 trigger frequence is %d/s\n", duID, frequence);
}

void T3Trigger::doT3Trigger(std::string du, char* data, size_t sz) {
    int szData = sz-12;
    int szTimeStamp = szData/EACH_DATA_SZ;
    int eachSz = 0;
    int delCounter = 0;
    int erase_n1;
    char tmp[EACH_DATA_SZ] = {0};
    char timeTmp[EACH_DATA_SZ] = {0};
    uint64_t t = 0;
    uint64_t tmCpu = 0;
    uint64_t tmId = 0;
    size_t duId = 0;
    uint64_t delTimeID[100] = {0};

    std::map<size_t, std::map<size_t, evtInfo>>::iterator it;
    std::map<size_t, evtInfo>::iterator it_1;
    std::map<size_t, uint64_t>::iterator it_2;

    // FILE *fp=fopen("/home/grand/workarea/ana/l3TriggerAnaData/trigger_CSDAQ_timestamp.txt", "a+");
    memcpy(m_t3TimeBuf, data+12, szData);
    // for(int i = 0; i< szTimeStamp; i++) {
    //     memset(timeTmp, 0, EACH_DATA_SZ);
    //     memcpy(timeTmp, m_t3TimeBuf+i*EACH_DATA_SZ, EACH_DATA_SZ);
    //     fprintf(fp, "DU is %d, trigger_cs_timestamp is %lld\n", atoi(du.c_str()), atoll(timeTmp));
    // }
    // fclose(fp);
    // printf("DU is %d, trigger_cs_timestamp is %lld\n", atoi(du.c_str()), atoll(timeTmp));

    // get tmID and DuId
    memcpy(tmp, m_t3TimeBuf, EACH_DATA_SZ);
    tmId = atoll(tmp)/paras.m_timeCut;
    duId = size_t(atoi(du.c_str()));
    m_eventInfo[tmId][duId].buf[100000] = {0};
    memcpy(m_eventInfo[tmId][duId].buf, m_t3TimeBuf, szData);
    m_eventInfo[tmId][duId].InfoSz = szData;
    
    // get L1 frequence from each DU
    eachSecondCount(duId, szData);

    // Fill time for each tmID; 
    if(m_eventInfo[tmId].size() > 0) {
        m_tmJudgement[tmId] = XXClock::nowNanoSeconds(); 
    }

    // use each CPU time of each tmID to judge if we need to get ready for trigger 
    for(it_2 = m_tmJudgement.begin(); it_2 != m_tmJudgement.end(); it_2++) {
        tmCpu = XXClock::nowNanoSeconds();

        if(it_2->first == 0) {
            std::cout << "There is no tmId in m_tmJudgement" << std::endl;
            continue;
        }
        
        if(tmCpu - it_2->second > paras.m_timeOut) { // Second level.
            int j=0;
            it = m_eventInfo.find(it_2->first); // find tmID in m_eventInfo.
            eachSz = m_eventInfo[it_2->first].size();
            if(eachSz == 0) continue; // struct number.
            // copy every tmID's info in m_t3TriggerBuf.
            for(it_1 = it->second.begin(); it_1 != it->second.end(); it_1++) {
                int tmpDUid = it_1->first;
                if(tmpDUid>0 && it_2->first>0) {
                    // printf("copy m_eventInfo[%ld][%d].InfoSz is %d\n", it_2->first, it_1->first, m_eventInfo[it_2->first][it_1->first].InfoSz);
                    memcpy(m_t3TriggerBuf+m_szt3TriggerBuf, m_eventInfo[it_2->first][tmpDUid].buf, m_eventInfo[it_2->first][tmpDUid].InfoSz);
                    m_szt3TriggerBuf += m_eventInfo[it_2->first][it_1->first].InfoSz;
                }
                else
                    continue;
            }
            
            // ************************************* //
            //           Trigger algorithm
            //                Do Triger              
            triggerAlgorithm(m_t3TriggerBuf, m_szt3TriggerBuf, it_2->first);
            // ************************************* //

            m_szt3TriggerBuf = 0;
            erase_n1 = m_eventInfo.erase(it_2->first);
            memset(m_t3TriggerBuf, 0, 20000000);
            delTimeID[delCounter] = it_2->first;
            delCounter++;
        }
        else {
            break;
        }
    }

    for(int i = 0; i < delCounter; i++) {
        m_tmJudgement.erase(delTimeID[i] );
    }

    memset(m_t3TimeBuf, 0, 20000000);
}

void T3Trigger::doLastT3Trigger() {
    int eachSz = 0;
    int delCounter = 0;
    int erase_n1;
    uint64_t delTimeID[100] = {0};

    std::map<size_t, std::map<size_t, evtInfo>>::iterator it;
    std::map<size_t, evtInfo>::iterator it_1;
    std::map<size_t, uint64_t>::iterator it_2;
    
    for(it_2 = m_tmJudgement.begin(); it_2 != m_tmJudgement.end(); it_2++) {
        
        if(it_2->first == 0) {
            std::cout << "There is no tmId in m_tmJudgement" << std::endl;
            continue;
        }

        // if(m_stop == true) { // Second level.
        int j=0;
        it = m_eventInfo.find(it_2->first); // find tmID in m_eventInfo.
        eachSz = m_eventInfo[it_2->first].size();
        if(eachSz == 0) continue; // struct number.
        // copy every tmID's info in m_t3TriggerBuf.
        for(it_1 = it->second.begin(); it_1 != it->second.end(); it_1++) {
            int tmpDUid = it_1->first;
            if(tmpDUid>0 && it_2->first>0) {
                // printf("copy m_eventInfo[%ld][%d].InfoSz is %d\n", it_2->first, it_1->first, m_eventInfo[it_2->first][it_1->first].InfoSz);
                memcpy(m_t3TriggerBuf+m_szt3TriggerBuf, m_eventInfo[it_2->first][tmpDUid].buf, m_eventInfo[it_2->first][tmpDUid].InfoSz);
                m_szt3TriggerBuf += m_eventInfo[it_2->first][it_1->first].InfoSz;
            }
            else
                continue;
        }
        
        // ************************************* //
        //           Trigger algorithm
        //                Do Triger              
        triggerAlgorithm(m_t3TriggerBuf, m_szt3TriggerBuf, it_2->first);
        // ************************************* //
        m_szt3TriggerBuf = 0;
        erase_n1 = m_eventInfo.erase(it_2->first);
        memset(m_t3TriggerBuf, 0, 20000000);
        delTimeID[delCounter] = it_2->first;
        delCounter++;
    }

    for(int i = 0; i < delCounter; i++) {
        m_tmJudgement.erase(delTimeID[i] );
    }

    memset(m_t3TimeBuf, 0, 20000000);
}

void T3Trigger::randomTrigger(char* data, size_t sz) {
    //************************Random Trigger can be packed.**********************************//
    int num[5] = {0};
    int count = 0;
    int tmp;
    int numTimeStamp = (sz -12)/EACH_DATA_SZ;

    srand(time(0));
    for(int i=0; i < 2; i++) {
        int r = genRand(0, numTimeStamp);
        count++;
        num[i] = r;
        for (int j = 0; j < i; j++)
		{
			if (num[j] == num[i])
			{
				num[j] = genRand(1, numTimeStamp);
			}
		}
    }

    if(num[0] > num[1]) {
        tmp = num[1];
        num[1] = num[0];
        num[0] = tmp;
        std::cout << "0 is " << num[0] << ", 1 is " << num[1] << std::endl;
    } // done here!

    int szofTimeSlice = count*EACH_DATA_SZ;
    char timeTmp[EACH_DATA_SZ];
    char* timeSlice = new char[count*EACH_DATA_SZ];
    for (int i = 0; i < count; i++)
    {
        memcpy(timeSlice + i*EACH_DATA_SZ, data + 8 + 4 + num[i]*EACH_DATA_SZ, EACH_DATA_SZ);
        memcpy(timeTmp, timeSlice + i*EACH_DATA_SZ, EACH_DATA_SZ);
    }

    char buf[102400] = {0};
    CommandMessage msg(buf, 102400, true);
    msg.setCmd("DOTRIGGER", timeSlice, szofTimeSlice);
    size_t szofMsg = msg.size();
    m_client->writeAll(buf, sz);

    delete timeSlice;
    timeSlice = nullptr;
}

void T3Trigger::triggerAlgorithm(char *data, int sz, uint64_t tmID) {
    int num = sz/EACH_DATA_SZ;
    const int dataSz = sz;
    uint64_t t1=0, t2=0;
    uint64_t t_be=0, t_af=0;
    m_t3Container = new uint64_t[num]();
    m_t3TimeContainer = new char[sz]();
    char tmp[EACH_DATA_SZ]={0};
    char tmp2[EACH_DATA_SZ]={0};
    char p[EACH_DATA_SZ]={0};
    char p2[EACH_DATA_SZ]={0};
    int diff=0;
    int nStruct=0;
    int N_first_position_in_last_part = 0; // last paras.m_triggerThreshold
    uint64_t N_first_time = 0;
    int N_counts = 0;

    t3TmDuid win[num];
    std::map<size_t, evtInfo>::iterator it_t3;
    
    // Set in timeStamp, and sort
    for(size_t i=0; i<num; i++) {
        memcpy(tmp, data+i*EACH_DATA_SZ, EACH_DATA_SZ);
        *(m_t3Container+i) = atoll(tmp);
    }

    t_be = XXClock::nowNanoSeconds();
    std::sort(m_t3Container,m_t3Container+num);
    t_af = XXClock::nowNanoSeconds();

    // printf("after sort, the time diff is %lld\n",t_af - t_be);
    // FILE *fp=fopen("/home/grand/workarea/ana/l3TriggerAnaData/trigger_Algorithm_CSDAQ_timestamp.txt", "a+");
    // for(int i=0; i<num; i++) {
    //     fprintf(fp, "timestamp %d is %lld\n", i, m_t3Container[i]);
    // }
    // fclose(fp);
    
    for(size_t i=0; i<num; i++) {
        snprintf(m_t3TimeContainer+i*EACH_DATA_SZ, EACH_DATA_SZ, "%lld", m_t3Container[i]);
    }
    
    for(int i=0; i<num; i++) {
        memset(p,0,EACH_DATA_SZ);
        memcpy(p, m_t3TimeContainer+i*EACH_DATA_SZ, EACH_DATA_SZ);
        t1 = atoll(p);

        if(num - i == paras.m_triggerThreshold) {
            if(N_counts == 0) {
                N_first_time = t1;
                N_first_position_in_last_part = i;
            }   
            
            for(int j=i; j<num; j++) {
                memset(p2, 0, EACH_DATA_SZ);
                memcpy(p2, m_t3TimeContainer+j*EACH_DATA_SZ, EACH_DATA_SZ);
                t2 = atoll(p2);
                if( 0 < t2-N_first_time && t2-N_first_time <  paras.m_timeWindow) {
                    N_counts++;
                    continue;
                }
            }
            if( N_counts==paras.m_triggerThreshold ){
                char duTriggerNumbers[4] = {0};
                win[nStruct].buf = new char[(N_counts+1)*EACH_DATA_SZ]();
                assert((N_first_position_in_last_part*EACH_DATA_SZ+(N_counts+1)*EACH_DATA_SZ < dataSz, "sz is out of range"));                
                memcpy(win[nStruct].buf, m_t3TimeContainer+N_first_position_in_last_part*EACH_DATA_SZ,(N_counts+1)*EACH_DATA_SZ);        
                win[nStruct].sz = (N_counts+1)*EACH_DATA_SZ;
                std::cout << "Trigger buffer sz is " << win[nStruct].sz << std::endl;
                if(m_tag + 1 > 4294967295) 
                    m_tag = 0;
                m_tag++; 
                char tmpTag[20];
                memset(tmpTag, 0, 20);
                sprintf(tmpTag, "%d", m_tag);
                char buf[2048000] = {0}; // Initialize
                CommandMessage msg(buf, 2048000, true);
                msg.setCmd("DOTRIGGER", win[nStruct].buf, win[nStruct].sz);
                printf("m_tag is %d\n", atol(tmpTag));
                sprintf(duTriggerNumbers, "%d", n);
                if(msg.size()+sizeof(uint32_t)>20480) {
                    memset(buf, 0, 20480);
                    msg.copyFrom((char*)(&m_tag), sizeof(uint32_t)); 
                }
                else {
                    msg.copyFrom((char*)(&m_tag), sizeof(uint32_t));
                }
                msg.copyFrom(duTriggerNumbers, sizeof(uint32_t));
                size_t szofMsg = msg.size();
                m_client->writeAll(buf, szofMsg);
                delete win[nStruct].buf;
                win[nStruct].buf = nullptr;
                nStruct++;
            }
            break;
        }

        // *************Modified by duanbh 2023.11.24**************
        for(int j=i; j<num; j++) {
            memset(p2, 0, EACH_DATA_SZ);
            memcpy(p2, m_t3TimeContainer+j*EACH_DATA_SZ, EACH_DATA_SZ);
            t2 = atoll(p2);

            // printf("t1: %lld, t2: %lld\n", t1, t2);
        
            if(t2-t1>paras.m_timeWindow) {
                diff = j-i;
                win[nStruct].buf = new char[(diff)*EACH_DATA_SZ]();
                assert((i*EACH_DATA_SZ+diff*EACH_DATA_SZ < dataSz, "sz is out of range"));                
                memcpy(win[nStruct].buf, m_t3TimeContainer+i*EACH_DATA_SZ, diff*EACH_DATA_SZ);
                win[nStruct].sz = diff*EACH_DATA_SZ;
                for(size_t j2=0; j2<win[nStruct].sz/EACH_DATA_SZ; j2++) { 
                    std::string str1;
                    str1.assign(win[nStruct].buf+j*EACH_DATA_SZ, EACH_DATA_SZ);
                    if(str1.empty()) continue;
                    // check whether each DU has T2buf.
                    for(it_t3 = m_eventInfo[tmID].begin(); it_t3 != m_eventInfo[tmID].end(); it_t3++) {
                        std::string str2;
                        str2.assign(it_t3->second.buf, it_t3->second.InfoSz);
                        int ret = str2.find(str1, 0);
                        if(ret >= 0) {
                            m_trigerDU[it_t3->first] = 1; // one du exits.
                            continue;
                        }
                    }  
                }

                int n=m_trigerDU.size();
                char duTriggerNumbers[4] = {0};
                char testtmstamp[20] = {0};
                if(n>=paras.m_triggerThreshold) {
                    std::cout << "Trigger buffer sz is " << win[nStruct].sz << std::endl;
                    if(m_tag + 1 > 4294967295) 
                        m_tag = 0;
                    m_tag++; 
                    char tmpTag[20];
                    memset(tmpTag, 0, 20);
                    sprintf(tmpTag, "%d", m_tag);
                    char buf[2048000] = {0}; // Initialize
                    CommandMessage msg(buf, 2048000, true);
                    msg.setCmd("DOTRIGGER", win[nStruct].buf, win[nStruct].sz);
                    printf("m_tag is %d\n", atol(tmpTag));
                    for(itt = m_trigerDU.begin(); itt != m_trigerDU.end(); itt++){
                        memset(testtmstamp, 0, 20);
                        memcpy(testtmstamp, win[i].buf + k*EACH_DATA_SZ, EACH_DATA_SZ);
                        // fprintf(fp, "triggered DU id is %d, No.%d timestamp is %lld\n", itt->first, k, atoll(testtmstamp));
                        printf( "triggered DU id is %d, No.%d timestamp is %lld\n", itt->first, k, atoll(testtmstamp));
                        k++;
                    }
                    sprintf(duTriggerNumbers, "%d", n);
                    if(msg.size()+sizeof(uint32_t)>20480) {
                        memset(buf, 0, 20480);
                        msg.copyFrom((char*)(&m_tag), sizeof(uint32_t)); 
                    }
                    else {
                        msg.copyFrom((char*)(&m_tag), sizeof(uint32_t));
                    }
                    msg.copyFrom(duTriggerNumbers, sizeof(uint32_t));
                    size_t szofMsg = msg.size();
                    m_client->writeAll(buf, szofMsg);
                    i = j-1; 
                }

                std::map<size_t, size_t>::iterator itter;
                for(itter = m_trigerDU.begin(); itter != m_trigerDU.end(); ) {
                    m_trigerDU.erase(itter++);
                }

                delete win[nStruct].buf;
                win[nStruct].buf = nullptr;
                nStruct++;
                break;
            }
        }
    }
    
    // // now will send info to dudaq
    // for(size_t i=0; i<nStruct; i++) { // read every struct buf
    //     for(size_t j=0; j<win[i].sz/EACH_DATA_SZ; j++) {
    //         std::string str1;
    //         str1.assign(win[i].buf+j*EACH_DATA_SZ, EACH_DATA_SZ);
    //         if(str1.empty()) continue;

    //         // check whether each DU has T2buf.
    //         for(it_t3 = m_eventInfo[tmID].begin(); it_t3 != m_eventInfo[tmID].end(); it_t3++) {
    //             std::string str2;
    //             str2.assign(it_t3->second.buf, it_t3->second.InfoSz);
    //             // int ssz = str2.size();
    //             int ret = str2.find(str1, 0);
    //             if(ret >= 0) {
    //                 m_trigerDU[it_t3->first] = 1; // one du exits.
    //                 continue;
    //             }
    //         }  
    //     }

    //     int n=m_trigerDU.size();
    //     char duTriggerNumbers[4] = {0};
    //     char testtmstamp[20] = {0};
    //     // int k=0;
    //     if(n>=paras.m_triggerThreshold) {
    //         // std::cout << "trigger DU number is " << n << std::endl;
    //         // std::map<size_t, size_t>::iterator itt;
    //         // for(itt = m_trigerDU.begin(); itt != m_trigerDU.end(); itt++) {
    //         //     std::cout << "trigger du is " << itt->first << std::endl;
    //         // }

    //         // char buf[2048000] = {0}; // Initialize
    //         // CommandMessage msg(buf, 2048000, true);
    //         // msg.setCmd("DOTRIGGER", win[i].buf, win[i].sz);
    //         // if(win[i].sz > 60) {
    //         std::cout << "Trigger buffer sz is " << win[i].sz << std::endl;
    //         // //         for(itt = m_trigerDU.begin(); itt != m_trigerDU.end(); itt++) {
    //         // //         std::cout << "trigger du is " << itt->first << std::endl;
    //         // //     }
    //         // }
            
    //         // **************add tag and trigger dus count************** //
    //         if(m_tag + 1 > 4294967295) 
    //             m_tag = 0;
    //         m_tag++; 
    //         char tmpTag[20];
    //         memset(tmpTag, 0, 20);
    //         sprintf(tmpTag, "%d", m_tag);

    //         char buf[2048000] = {0}; // Initialize
    //         CommandMessage msg(buf, 2048000, true);
    //         msg.setCmd("DOTRIGGER", win[i].buf, win[i].sz);
    //         // if(win[i].sz > 60) {
    //         //     std::cout << "Trigger buffer sz is " << win[i].sz << std::endl;
    //         // //         for(itt = m_trigerDU.begin(); itt != m_trigerDU.end(); itt++) {
    //         // //         std::cout << "trigger du is " << itt->first << std::endl;
    //         // //     }

    //         //     printf("m_tag is %d\n", atol(tmpTag));
    //         // }

    //         printf("m_tag is %d\n", atol(tmpTag));
    //         // FILE *fp=fopen("/home/grand/workarea/ana/l3TriggerAnaData/Triggered_CSDAQ_timestamp.txt", "a+");
    //         // fprintf(fp, "m_tag is %d\n", atol(tmpTag));
    //         // // fprintf(fp, "trigger DU number is %d\n", n);
    //         // for(itt = m_trigerDU.begin(); itt != m_trigerDU.end(); itt++){
    //         //     memset(testtmstamp, 0, 20);
    //         //     memcpy(testtmstamp, win[i].buf + k*EACH_DATA_SZ, EACH_DATA_SZ);
    //         //     fprintf(fp, "triggered DU id is %d, No.%d timestamp is %lld\n", itt->first, k, atoll(testtmstamp));
    //         //     k++;
    //         // }
    //         // fclose(fp);
    //         sprintf(duTriggerNumbers, "%d", n);
    //         if(msg.size()+sizeof(uint32_t)>20480) {
    //             memset(buf, 0, 20480);
    //             msg.copyFrom((char*)(&m_tag), sizeof(uint32_t)); 
    //         }
    //         else {
    //             msg.copyFrom((char*)(&m_tag), sizeof(uint32_t));
    //         }
    //         msg.copyFrom(duTriggerNumbers, sizeof(uint32_t));
    //         size_t szofMsg = msg.size();
    //         // std::cout << "t3 msg sz is " << msg.size() << std::endl;
            
    //         m_client->writeAll(buf, szofMsg); 
        
    //     }
    //     // ******************** True trigger ********************* //
    //     std::map<size_t, size_t>::iterator itter;
    //     for(itter = m_trigerDU.begin(); itter != m_trigerDU.end(); ) {
    //         m_trigerDU.erase(itter++);
    //     }
    //     delete win[i].buf;
    //     win[i].buf = nullptr;
    // }

    delete m_t3Container;
    delete m_t3TimeContainer;
    m_t3TimeContainer = nullptr;
    m_t3Container = nullptr;
}