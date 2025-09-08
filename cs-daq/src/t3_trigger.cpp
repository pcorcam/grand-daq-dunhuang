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
#include <unordered_set>

using namespace grand;

T3Trigger::T3Trigger( TriggerDone triggerDoneCallback, ZMQClient *client) {
    m_bufferSize = CSSysConfig::instance()->appConfig().t2BufferPageSize;
    int numPages = CSSysConfig::instance()->appConfig().t2BufferNumberOfPages;
    
    paras.m_timeCut = CSSysConfig::instance()->appConfig().t3TriggerTimeCut;
    paras.m_timeOut = CSSysConfig::instance()->appConfig().t3TriggerTimeOut;
    paras.m_timeWindow = CSSysConfig::instance()->appConfig().t3TriggerTimeWindow;
    paras.m_triggerThreshold = CSSysConfig::instance()->appConfig().t3TriggerDuNumber;
    paras.m_t3TriggerType = CSSysConfig::instance()->appConfig().t3TriggerType;
    paras.m_dudaqFileInjection = CSSysConfig::instance()->appConfig().dudaqFileInjection;
    paras.m_antennaDistancesFile = CSSysConfig::instance()->appConfig().antennaDistancesFile;

    m_buffers = new BufferPool(m_bufferSize, numPages);
    m_threadPool = new ThreadPool(1);
    m_triggerDone = triggerDoneCallback;
    m_client = client;
    m_t3TimeBuf = new char[T3TIMEBUF_EACHDU_ALLSZ]();
    m_t3TriggerBuf = new char[T3TIMEBUF_DOTIGGER_SZ]();

    m_defaultConfig = ::getenv("GRAND_DAQ_CONFIG");
    m_defaultConfigSys = m_defaultConfig + "/sysconfig.yaml";

    // Read the antenna distances if using casual trigger
    if (paras.m_t3TriggerType==2)
        read_antenna_distances(paras.m_antennaDistancesFile);
}

T3Trigger::~T3Trigger() {
    delete m_buffers;
    delete m_threadPool;
    delete[] m_t3TimeBuf;
    delete[] m_t3TriggerBuf;

    m_t3TriggerBuf = nullptr;
    m_t3TimeBuf = nullptr;
    m_threadPool = nullptr;
    m_buffers = nullptr;
}

void T3Trigger::stop(bool init) {  
    m_activeProcessors.fetch_add(1);

    if (paras.m_t3TriggerType==0)
        doLastT3Trigger();
    else if (paras.m_t3TriggerType>=1)
        triggerAlgorithm2(true);
    else
    {
        cout << "No t3TriggerType specified in the sysconfig.yaml" << endl;
        exit(0);
    }

    m_activeProcessors.fetch_sub(1);
    m_cv.notify_one();
    if (!init)
    {
        m_stopped.store(true);
        // std::cout << "m_stopped" << std::endl;
    }
    if (!init)
    {
        // std::cout << "waitforcompl start" << std::endl;
        waitForCompletion();
        // std::cout << "waitforcompl end" << std::endl;
    }
}

void T3Trigger::waitForCompletion() {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.wait(lock, [this] {
        return m_activeProcessors.load() == 0;
    });
}

void T3Trigger::processData(std::string du, char *data, size_t sz) {
    // *********************** DO random trigger ******************** //
    // randomTrigger(data,sz);

    if (m_stopped.load()) {
        return;  // Don't process new data if stopping
    }

    m_daqMode = CSSysConfig::instance()->appConfig().daqMode;
    if(m_daqMode == 2 || m_daqMode == 3) {
        m_activeProcessors.fetch_add(1);
        try {
            if (paras.m_t3TriggerType==0)
                doT3Trigger(du, data, sz);
            else if (paras.m_t3TriggerType>=1)
                doT3Trigger2(du, data, sz);
            else
            {
                cout << "No t3TriggerType specified in the sysconfig.yaml" << endl;
                exit(0);
            }
        } catch (...) {
            m_activeProcessors.fetch_sub(1);
            m_cv.notify_one();
            throw;
        }
        m_activeProcessors.fetch_sub(1);
        m_cv.notify_one();
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
    int frequence = sz/sizeof(uint64_t);
    // printf("DU is %d, L1 trigger frequence is %d/s\n", duID, frequence);
}

/**
 * Process T3 trigger data from a detector unit (DU)
 * @param du Detector Unit identifier string
 * @param data Raw data buffer containing trigger information
 * @param sz Total size of the data buffer
 */
void T3Trigger::doT3Trigger(std::string du, char* data, size_t sz) {
    // Return if data size is too small to contain valid information (12 bytes header)
    // std::cout << "doT3Trigger func " << std::endl;
    
    if (sz <= DAQ_HEADER_SZ) return;
    // std::cout << "DU Id: " << du << ", sz timeStamps buffer " << sz << std::endl;
    // std::cout << "doT3Trigger data " << du << " " << data << " " << sz << std::endl;

    auto t1 = std::chrono::high_resolution_clock::now();
    // Calculate actual data size by removing header size
    const int szData = sz - DAQ_HEADER_SZ;

    // Calculate number of timestamps in the data
    const int szTimeStamp = szData / sizeof(uint64_t);
    // Convert DU string identifier to numeric ID
    const size_t duId = static_cast<size_t>(std::stoi(du));
    const uint64_t tmId = *(uint64_t*)(data + DAQ_HEADER_SZ) / paras.m_timeCut;
    // Copy all data after header to temporary buffer m_t3TimeBuf
    // ?? Do we use m_t3TimeBuf anywhere except as a data source for info.buf? If not, we should avoid double memcpy
    memcpy(m_t3TimeBuf, data + DAQ_HEADER_SZ, szData);

    // Store event data in the event info map, indexed by timestamp ID and DU ID
    evtInfo& info = m_eventInfo[tmId][duId];
    info.InfoSz = szData;
    // ?? This could be copied directly from the source, skipping the m_t3TimeBuf
    memcpy(info.buf, m_t3TimeBuf, szData);

    // // Update trigger rate statistics for this DU
    // eachSecondCount(duId, szData);

    // Record CPU timestamp for this event's tmId if we have any events for this timestamp
    // ?? Isn't it always non-empty, since we just received data with this timestamp?
    if (!m_eventInfo[tmId].empty()) {
        m_tmJudgement[tmId] = XXClock::nowNanoSeconds();
    }

    // Check for timed-out events that need processing
    std::vector<uint64_t> expiredIds;
    // ?? we could use the same time as for m_tmJudgement[tmId] or rather calculate now earlier, and use for m_tmJudgement[tmId] to avoid double call to nowNanoSeconds()
    const uint64_t now = XXClock::nowNanoSeconds();

    // Iterate through all timestamp judgements to find and process expired events
    // ?? Maybe for this procedure m_tmJudgement should be some reversly sorted list, and we stop iterating when we reach the first non-expired event? Otherwise always going through 1 s worht of events?
    // ?? Maybe initially we store the oldest time, and if oldest not older 1s than now, do not iterate below? But probably rare not to have such an old event in the list
    for (auto& [tmIdIter, cpuTime] : m_tmJudgement) {
        // Skip invalid timestamp IDs
        // ?? Is this the best way to skip empty (?) m_tmJudgement?
        if (tmIdIter == 0) {
            std::cout << "There is no tmId in m_tmJudgement" << std::endl;
            continue;
        }

        // Break if this and subsequent events haven't timed out yet
        // ?? This is equal to 1 s. So we are storing 1 s of triggers from all DUs in memory. Is it optimal? Both from the memory use point of view an following trigger list operations (sort? find?)
        // ?? Also, does it mean that part of antennas of the event may be processed now, and those that happen to come later, are discarded (or processed into another event)? But that's very unlikely. It waits for 1 s before processing, so all DUss of an event should be in
        if (now - cpuTime < paras.m_timeOut) break;

        // Skip if no event data exists for this timestamp
        auto it = m_eventInfo.find(tmIdIter);
        if (it == m_eventInfo.end() || it->second.empty()) continue;

        // Copy trigger data from all DUs for this timestamp into trigger buffer
        // ?? I think this loops through all the DUs for T1 events that happened within the same DU second (due to rounding to 1 s). So probably would skip the rare case of the event happening in different seconds on different DUs
        // If signal passes 30 km in 100 us, which is perhaps possible span for almost horizontal events, this may give ~1/10000 chance of loosing a DU (?). Small, for more vertical even smaller.
        int nWatchdog=0;
        for (const auto& [curDuId, evt] : it->second) {
            // ?? When can this happen?
            if (curDuId == 0 || tmIdIter == 0) continue;

            // Log trigger information
            // std::cout << "evt.InfoSz: " << evt.InfoSz << std::endl;
            // printf("tmId: %llu, DU: %zu, trigger rate: %dHz\n", tmIdIter, curDuId, evt.InfoSz / sizeof(uint64_t));
            
            // get trigger rate and let watch dog work
            int triggerRa=evt.InfoSz / sizeof(uint64_t);
            if(triggerRa>300) nWatchdog++;

            // Append this DU's data to the trigger buffer
            // ?? Is this copy needed? Can't we pass directly evt to triggerAlgorithm? Maybe not that simple, but worth looking into
            memcpy(m_t3TriggerBuf + m_szt3TriggerBuf, evt.buf, evt.InfoSz);
            m_szt3TriggerBuf += evt.InfoSz;
        }

        // Process collected trigger data using the trigger algorithm
        // ****************************** WATCH DOG ********************************** // 
        if(nWatchdog<=20)
            triggerAlgorithm(m_t3TriggerBuf, m_szt3TriggerBuf, tmIdIter);
        else
            printf("w-dog 1s\n");

        // Clean up processed data 
        m_szt3TriggerBuf = 0;
        // ?? Is it needed?
        memset(m_t3TriggerBuf, 0, T3TIMEBUF_DOTIGGER_SZ);
        m_eventInfo.erase(tmIdIter);
        expiredIds.push_back(tmIdIter);
    }

    // Remove processed timestamps from the judgement map
    for (uint64_t id : expiredIds) {
        m_tmJudgement.erase(id);
    }

    // Clear temporary buffer for next use
    memset(m_t3TimeBuf, 0, T3TIMEBUF_EACHDU_ALLSZ);

    auto t3 = std::chrono::high_resolution_clock::now();
    auto milliseconds2 = std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t1);
    // std::cout << "doT3Trigger Func" << std::endl;
}

/**
 * Process T3 trigger data from a detector unit (DU)
 * @param du Detector Unit identifier string
 * @param data Raw data buffer containing trigger information
 * @param sz Total size of the data buffer
 */
void T3Trigger::doT3Trigger2(std::string du, char* data, size_t sz) {
    // Return if data size is too small to contain valid information (12 bytes header)
    if (sz <= 12) return;

    auto t1 = std::chrono::high_resolution_clock::now();

    // For trigger rate calculation
    uint64_t smallest_time = 18446744073709551615ULL;
    uint64_t largest_time = 0;

    size_t total_records = sz / EACH_DATA_SZ;
    for (size_t i = 0; i < total_records; ++i) {
        const char* record_start = (data+12) + i * EACH_DATA_SZ;
        // cout << "emplacing " << du_events.size() << " " << record_start << " " << std::strtoull(record_start, nullptr, 10) << endl;
        // Process the timestamp as needed
        auto tm = std::strtoull(record_start, nullptr, 10);
        du_events.emplace_back(std::pair<uint64_t, unsigned short>(tm, std::strtoul(du.c_str(), nullptr, 10)));
        if (smallest_time > tm) smallest_time = tm;
        if (largest_time < tm) largest_time = tm;
    }

    auto timedif = largest_time-smallest_time;
    if (timedif>0)
        cout << "DU: " << du << ", trigger rate: " << total_records/(timedif/1e9) << " Hz" << endl;

    if (du_events.size()>=paras.m_triggerThreshold)
        triggerAlgorithm2(paras.m_dudaqFileInjection);

    auto t3 = std::chrono::high_resolution_clock::now();
    auto milliseconds2 = std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t1);
    // std::cout << "T3Trigger time: " << milliseconds2.count() << std::endl;
}

void T3Trigger::triggerAlgorithm2(bool process_young)
{
    // Sort the vector of timestamps
    std::sort(du_events.begin(), du_events.end(),
        [](const std::pair<uint64_t, unsigned short>& a, const std::pair<uint64_t, unsigned short>& b) {
              return a.first < b.first;
          });

    // uint64_t trigger_timestamp;
    // Could be a set to avoid sort and unique, but probably with vector and sort+unique after all inserts is faster
    vector<size_t> event_duIds;

    // Containers to store trigger information:
    std::vector<char*> allTriggers;     // Buffer for all triggered event data
    std::vector<uint32_t> duCounts;     // Number of DUs involved in each trigger
    std::vector<uint32_t> allTriggersSz; // Size of each trigger's data

    // auto now = XXClock::nowNanoSeconds();

    // The youngest/newest event that we have
    uint64_t youngest_event;
    uint64_t oldest_event;
    if (du_events.size()>0)
    {
        youngest_event = du_events.back().first;
        oldest_event = du_events.front().first;
    }

    // Iterate through timestamps, starting from the oldest
    for (auto it = du_events.begin(); it != du_events.end(); )
    {
        // If the event is too old compared to the youngest event, remove it from the vector
        // TODO: The oldest kept event time should be set in the config file
        // if (now - it->first > (uint64_t)paras.m_timeOut*10) {
        if (youngest_event - it->first > (uint64_t)paras.m_timeOut*10) {
            it = du_events.erase(it); // erase returns the next valid iterator
            // cout << "event too old " << now - it->first << " " << now << " " << it->first << " " << (uint64_t)paras.m_timeOut*10 << " " << paras.m_timeOut << endl;
            continue;
        }

        // If the event is too young, break the loop (1s on the DU and 1s waiting here)
        if (!process_young && oldest_event - it->first < paras.m_timeOut*2)
        {
            // cout << "event too young " << oldest_event - it->first << " " << oldest_event << " " << it->first << " " << paras.m_timeOut*1 << " " << paras.m_timeOut << " " << process_young << endl;
            break;
        }

        // cout << "looking for coinc" << endl;
        // Begin a new group starting at the current timestamp.
        const auto windowStart = it->first;

        // Start of vector erasing
        auto vec_er_start = it;

        // The map of distances from this antenna
        std::unordered_map<uint16_t, double>* inner_distance_map;
        // Init only in the casual trigger mode
        if (paras.m_t3TriggerType==2)
            inner_distance_map = &antenna_distance_map[it->second];

        // Init the trigger timestamp
        // if (it==du_events.begin())
        //     trigger_timestamp = windowStart;

        auto du_id = it->second;

        std::vector<std::string> windowStrings;
        // Loop through traces inside the coincidence time window
        while (it != du_events.end() && (it->first - windowStart) <= paras.m_timeWindow)
        {
            // Antenna casualty check
            if (paras.m_t3TriggerType==2)
            {
                // Count in for the trigger minimal requirements only the casual antennas or... antennas not in the map
                if (auto distance_it = inner_distance_map->find(it->second); distance_it != inner_distance_map->end() && (it->first - windowStart) <= distance_it->second*1.1)
                {
                    // if (du_id!=it->second) cout << "Got casual antenna " << du_id << " " << it->second << " " << (it->first - windowStart) << " " << distance_it->second*1.2 << endl;
                    event_duIds.push_back(it->second);
                }
            }
            // No casualty requirement - count in all the antennas in the big time window
            else
                // Add the DU to the list of DUs in the current event
                event_duIds.push_back(it->second);

            // Store all antennas within the large time window for traces request (can be modified)
            windowStrings.push_back(std::to_string(it->first));
            ++it;
        }

        // Count the unique DUs
        sort(event_duIds.begin(), event_duIds.end());
        const auto unique_DUs = unique(event_duIds.begin(), event_duIds.end())-event_duIds.begin();
        if (unique_DUs >= paras.m_triggerThreshold)
        {

            // Package all timestamps in the window into a trigger buffer
            int triggerLen = windowStrings.size() * EACH_DATA_SZ;
            char* triggerBuf = new char[triggerLen]();
            for (size_t k = 0; k < windowStrings.size(); ++k) {
                memcpy(triggerBuf + k * EACH_DATA_SZ, windowStrings[k].c_str(), EACH_DATA_SZ);
            }
            // Store the trigger information
            // ?? This copies, better to std::move()
            allTriggers.emplace_back(triggerBuf);     // Store the trigger data
            duCounts.push_back(unique_DUs);  // Store number of DUs involved
            allTriggersSz.push_back(windowStrings.size()*EACH_DATA_SZ);

            // Erase the entries from which the coincidence event was formed from the du_events vector
            it = du_events.erase(vec_er_start, it);
        }
        event_duIds.clear();
    }

    // Process all triggers that met the conditions
    if (!allTriggers.empty())
    {
        for (size_t i = 0; i < allTriggers.size(); ++i) {
            // Generate and wrap event tag
            m_tag = (m_tag + 1 > UINT32_MAX) ? 0 : m_tag + 1;
            // Create and send trigger command message
            // char buf[2048000] = {0};
            // ToDo: Would be better to allocate it once, probably in CommandMessage class, then reuse
            char buf[204800];
            // ?? Are we always sending back 2 MBs to DUs?
            CommandMessage msg(buf, sizeof(buf), true);
            // set in data and triggered DUs' number
            msg.setCmd("DOTRIGGER", allTriggers[i], allTriggersSz[i]);

            // Add event tag to message
            uint32_t tagCopy = m_tag;
            msg.copyFrom(reinterpret_cast<char*>(&tagCopy), sizeof(uint32_t));

            uint32_t duCount = duCounts[i];
            msg.copyFrom(reinterpret_cast<char*>(&duCount), sizeof(uint32_t));
            m_client->writeAll(buf, msg.size());
            delete[] allTriggers[i];  // free pointers
        }
        // {
        //     // That's the number of written events. Should be renamed.
        //     // evtCount++;
        //     auto du_count = event_duIds.size();
        //     // Loop through DUs in the event and write them to the file
        //     // WARNING: this writes double DUs if they exist in the event (as in original implementation). If we want to just write the first one of each DU, it would have to be implemented differently.
        //     // for (auto it2 = it-du_count; it2 != it; ++it2)
        //     //     writeIntoL3Datafile2(*it2, events_count, du_count);
        //     // events_count++;
        // }
        //



        // Increase the iterator
        // ++it;
    }
}

void T3Trigger::doLastT3Trigger() {
    uint64_t delTimeID[100] = {0};
    int delCounter = 0;

    for (auto it_2 = m_tmJudgement.begin(); it_2 != m_tmJudgement.end(); ++it_2) {
        uint64_t tmID = it_2->first;
        if (tmID == 0) {
            std::cout << "There is no tmId in m_tmJudgement" << std::endl;
            continue;
        }

        auto it = m_eventInfo.find(tmID);
        if (it == m_eventInfo.end() || it->second.empty()) continue;

        for (const auto& [duid, evt] : it->second) {
            if (duid > 0 && tmID > 0) {
                memcpy(m_t3TriggerBuf + m_szt3TriggerBuf, evt.buf, evt.InfoSz);
                m_szt3TriggerBuf += evt.InfoSz;
            }
        }

        triggerAlgorithm(m_t3TriggerBuf, m_szt3TriggerBuf, tmID);
        m_szt3TriggerBuf = 0;
        memset(m_t3TriggerBuf, 0, T3TIMEBUF_DOTIGGER_SZ);

        m_eventInfo.erase(tmID);
        delTimeID[delCounter++] = tmID;
    }

    for (int i = 0; i < delCounter; ++i) {
        m_tmJudgement.erase(delTimeID[i]);
    }

    memset(m_t3TimeBuf, 0, T3TIMEBUF_EACHDU_ALLSZ);
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
    // std::cout << "triggerAlgorithm func" << std::endl;
    // Calculate number of timestamps in the data buffer
    int num = sz / sizeof(uint64_t);
    if (num <= 0) return;

    // Convert all timestamps from string format to uint64_t numbers
    std::vector<uint64_t> timestamps(num);
    for (int i = 0; i < num; ++i) {
        // ?? I don't think copying to a buffer is needed for this conversion
        // ?? Anyway, are the timestamps really string? If so, very inefficient! I agree
        
        timestamps[i] = *(uint64_t*)(data + i * sizeof(uint64_t));
        // printf("20250830 test timestamps[%d]: %lld\n", i, timestamps[i]);
    }

    // Sort timestamps in ascending order for time window analysis
    // ?? Check if timestamps, which is only initialised, sorted then read, is most optimal in vector. Maybe an ordered set? But probably vector best, to avoid sorting after each insert
    std::sort(timestamps.begin(), timestamps.end());

    // Containers to store trigger information:
    std::vector<char*> allTriggers;     // Buffer for all triggered event data
    std::vector<uint32_t> duCounts;     // Number of DUs involved in each trigger
    std::vector<uint32_t> allTriggersSz; // Size of each trigger's data
    
    // Process each timestamp as a potential trigger start time
    for (int i = 0; i < num; ++i) {
        uint64_t t1 = timestamps[i];
        int diff = 0;

        // collect the timestamps within the timewindow and the correspanding DU
        std::vector<std::pair<uint64_t, size_t>> windowTanks; // <timestamp, duid>
        int lastIdx = i;
        for (int j = i; j < num && timestamps[j] - t1 <= paras.m_timeWindow; ++j) {
            // 遍历每个 DU 的事件数据
            for (const auto& [duid, evt] : m_eventInfo[tmID]) {
                size_t count = evt.InfoSz / sizeof(uint64_t);
                const uint64_t* tsBuf = reinterpret_cast<const uint64_t*>(evt.buf);

                for (size_t k = 0; k < count; ++k) {
                    if (tsBuf[k] == timestamps[j]) {
                        windowTanks.emplace_back(timestamps[j], duid);
                        break; // one DU means records timestamp once
                    }
                }
            }

            lastIdx = j;
        }

        // 统计唯一的 DU 数量
        std::unordered_set<size_t> uniqueDUs;
        for (const auto& [t, duid] : windowTanks) {
            uniqueDUs.insert(duid);
            if (uniqueDUs.size() >= static_cast<size_t>(paras.m_triggerThreshold)) {
                break; // 阈值已满足，可以提前结束
            }
        }

        // package the event's data and give it back to DU-DAQ
        // If enough unique DUs detected events (meets trigger threshold)
        // printf("prepare to package events\n");
        if ((int)uniqueDUs.size() >= paras.m_triggerThreshold) {
            
            std::unordered_set<size_t> finalDUs;
            for (const auto& [ts, duid] : windowTanks) {
                finalDUs.insert(duid);
            }

            // --- print the event's info ---
            if (!windowTanks.empty()) {
                uint64_t first_ts = windowTanks[0].first;  // the first timestamp in the timewindow
                uint64_t sec = first_ts / 1000000000ULL;   // second
                uint64_t ns  = first_ts % 1000000000ULL;   // nano-second

                std::ostringstream oss;
                oss << sec << "." << ns << ": [";

                for (size_t k = 0; k < windowTanks.size(); ++k) {
                    uint64_t ts = windowTanks[k].first;
                    uint64_t ns_part = ts % 1000000000ULL;  // 纳秒部分
                    size_t duid = windowTanks[k].second;

                    oss << "(" << duid << ", " << ns_part << ")";
                    if (k + 1 < windowTanks.size()) oss << ", ";
                }
                oss << "]";
                std::cout << oss.str() << std::endl;
            }
            // ----------------------

            // printf("triggered DUs' number: %d\n", finalDUs.size());

            // Package all timestamps in the window into a trigger buffer
            int triggerLen = windowTanks.size() * sizeof(uint64_t);
            char* triggerBuf = new char[triggerLen]();

            // copy timestamp to triggerBuf
            for (size_t k = 0; k < windowTanks.size(); ++k) {
                uint64_t ts = windowTanks[k].first; // pick timestamp from pair
                std::memcpy(triggerBuf + k * sizeof(uint64_t), &ts, sizeof(uint64_t));
            }

            // Store the trigger information
            allTriggers.push_back(triggerBuf);     // Store the trigger data
            duCounts.push_back(finalDUs.size());  // Store number of DUs involved
            allTriggersSz.push_back(triggerLen);
            i = lastIdx; // Skip timestamps we've already processed in this window
        }

        if (i >= num) break; // Prevent buffer overflow
    }

    // Process all triggers that met the conditions
    if (!allTriggers.empty()) {
        int evtCounter = 0;
        for (size_t i = 0; i < allTriggers.size(); ++i) {

            // Generate and wrap event tag
            m_tag = (m_tag + 1 > UINT32_MAX) ? 0 : m_tag + 1;
            std::cout << "event: " << m_tag << std::endl;
            evtCounter++;
            
            // Create and send trigger command message
            int sz_sndBuf = allTriggersSz[i] + 60;
            char* buf = new char[sz_sndBuf];
            memset(buf, 0, sz_sndBuf);

            // Change the snd bufSz into a flexible value.
            CommandMessage msg(buf, sz_sndBuf, true);
            
            // set in data and triggered DUs' number
            msg.setCmd("DOTRIGGER", allTriggers[i], allTriggersSz[i]);  
            // printf("allTriggersSz[%d]: %d\n", i, allTriggersSz[i]);
            // Add event tag to message
            uint32_t tagCopy = m_tag;
            msg.copyFrom(reinterpret_cast<char*>(&tagCopy), sizeof(uint32_t));
            uint32_t duCount = duCounts[i];
            msg.copyFrom(reinterpret_cast<char*>(&duCount), sizeof(uint32_t));
            // m_client->writeAll(buf, msg.size());
            delete[] allTriggers[i];  // free pointers
            delete[] buf;
        }
        printf("evt rate: %d\n", evtCounter);
    }
    // std::cout << "triggerAlgorithm func end\n" << std::endl;
}

// Read distances between antennas from a file
int T3Trigger::read_antenna_distances(string &filename)
{
    cout << "Reading antenna distances file" << endl;
    // antenna_distance_map
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Error opening file " << filename << std::endl;
        exit(0);
    }

    int pairs_count = 0;

    std::string line;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        uint16_t valA, valB;
        // Expecting meters
        double valC;

        if (!(iss >> valA >> valB >> valC)) {
            std::cerr << "Invalid line format: " << line << std::endl;
            exit(0); // Skip invalid lines
        }

        // Distance is actually time for the light to pass between antennas in nanoseconds ;)
        antenna_distance_map[valA][valB] = valC/299702547.*1e9;
        pairs_count++;
    }

    // antenna_distance_map[3001][3001] = 0;
    // antenna_distance_map[3001][3002] = 8000/299702547.*1e12;
    // antenna_distance_map[3001][3003] = 8500/299702547.*1e12;
    // antenna_distance_map[3002][3001] = 8400/299702547.*1e12;
    // antenna_distance_map[3002][3002] = 0/299702547.*1e12;
    // antenna_distance_map[3002][3003] = 8400/299702547.*1e12;
    // antenna_distance_map[3003][3003] = 8400/299702547.*1e12;
    // antenna_distance_map[3003][3001] = 8400/299702547.*1e12;
    // antenna_distance_map[3003][3002] = 8400/299702547.*1e12;

    return pairs_count;
}

