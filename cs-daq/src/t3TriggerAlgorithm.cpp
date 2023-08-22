#include<t3_trigger.h>
#include<iostream>
#include<algorithm>
#include <message_impl.h>

using namespace grand;

void T3Trigger::triggerAlgorithm(char *data, size_t sz, uint64_t tmID) {
    int num = sz/20;
    uint64_t t1=0, t2=0;
    m_t3Container = new uint64_t[num]();
    m_t3TimeContainer = new char[sz]();
    char tmp[20]={0};
    char tmp2[20]={0};
    char tmp3[20]={0};
    char p1[20]={0};
    char p2[20]={0};
    int diff=0;
    int nStruct=0;

    t3TmDuid win[num/3]={0};
    std::map<size_t, evtInfo>::iterator it_t3;

    for(size_t i=0; i<num; i++) {
        memcpy(tmp, data+i*20, 20);
        *(m_t3Container+i) = atoll(tmp);
    }
    std::sort(m_t3Container,m_t3Container+num-1);
    for(size_t i=0; i<num; i++) {
        snprintf(m_t3TimeContainer+i*20, 20, "%lld", m_t3Container[i]);
    }
    for(size_t i=0; i<num; i++) {
        memcpy(p1, m_t3TimeContainer+i*20, 20);
        t1 = atoll(p1);
        for(size_t j=i; j<num; j++) {
            memcpy(p2, m_t3TimeContainer+j*20, 20);
            t2 = atoll(p2);
            if(t2-t1>timeWindow) {
                diff = j-i;
                // memcpy(m_tmWinData, m_t3TimeContainer+i*20, diff*20);
                memcpy(win[nStruct].buf, m_t3TimeContainer+i*20, diff*20);
                win[nStruct].sz = diff*20;
                // m_tmWinData = new char[diff*20]();
                i=j-1;
                nStruct++;
                break;
            }
        }
    }
    for(size_t i=0; i<nStruct+1; i++) {
        for(size_t j=0; j<win[i].sz/20; j++) {
            memcpy(tmp3, win[i].buf+j*20, 20);
            std::string str1(tmp3);
            for(it_t3 = m_eventInfo[tmID].begin(); it_t3 != m_eventInfo[tmID].end(); it_t3++) {
                std::string str2(it_t3->second.buf);
                int ret = str2.find(str1, 0);
                if(ret > 0) {
                    m_trigerDU[it_t3->first] = 1; // one du exits.
                    break;
                }
            }  
        }

        int n=m_trigerDU.size();
        if(n>=triggerThreshold) {
            char buf[20480] = {0}; // Initialize
            CommandMessage msg(buf, 20480, true);
            msg.setCmd("DOTRIGGER", win[i].buf, win[i].sz);
            size_t szofMsg = msg.size();
            m_client->writeAll(buf, szofMsg);
        }
    }

    delete m_tmWinData;
    delete m_t3Container;
    delete m_t3TimeContainer;
    m_t3TimeContainer = nullptr;
    m_t3Container = nullptr;
    m_tmWinData = nullptr;
}