#include <l3OfflineTrigger.h>

namespace fs = std::filesystem;

std::vector<std::string> getAllFiles(const std::string& directoryPath) {
    std::vector<std::string> files;

    try {
        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            if (entry.is_regular_file()) {  // 只获取普通文件
                files.push_back(entry.path().string());
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error accessing directory: " << e.what() << std::endl;
    }

    return files;
}

// TODO: create indexs to save data info.
void readData(std::string filename, int idx, char* buffer, size_t size, uint16_t duID, size_t hitIdInOrder) {
    // std::lock_guard<std::mutex> lock(mapMutex);
    int hitSz = (size-sizeof(DAQHeader))/sizeof(uint16_t);
    // uint16_t* hitBuf = new uint16_t[hitSz];
    uint64_t nanoTmId;

    auto hitBuf = std::make_unique<uint16_t[]>(hitSz);
    memcpy(hitBuf.get(), buffer + sizeof(DAQHeader), size - sizeof(DAQHeader));
    ElecEvent hit(hitBuf.get(), hitSz);
    tmId = hit.getTimeFullDataSz().totalSec;
    nanoTmId = hit.getTimeFullDataSz().nanosec;
    if(szl3Timestamps==0)
        fileGPSTimeStamp = hit.getTimeFullDataSz().gpsReadableTm;
    if(tmId > 1740000000000000000) return;
    if(nanoTmId <200 && nanoTmId > 100) return;
    if (traceData[tmId].size() > 0) return;
    traceData[tmId][duID].traceSz = size;
    traceData[tmId][duID].filename = filename;
    traceData[tmId][duID].idx = idx;
    *(l3Timestamps+szl3Timestamps) = tmId;
    szl3Timestamps++;
}


void offlineL3Trigegr(std::map<size_t, std::map<std::size_t, traceInfo>> traceBuf, uint64_t* timestamps, size_t szTimeStamps) {
    uint64_t p1, p2, timediff, p1LastPart, p2LastPart, p1_triggerRate, p2_triggerRate, tmdiff_triggerRate;
    int nTraces, nToEvent=0, nCounts=0, triggerCounter=0;
    // std::cout << "offlineL3Trigegr func, szTimeStamps: " << szTimeStamps << std::endl;
    // t3TmDuid toEvent[szTimeStamps];
    t3TmDuid* toEvent = new t3TmDuid[szTimeStamps];
    for (int i = 0; i < szTimeStamps; ++i) {
        toEvent[i].tmBuf = nullptr;
        toEvent[i].sz = 0;
    }

    std::sort(timestamps, timestamps+szTimeStamps);
    int n_sz = traceBuf.size();

    for(int i=0; i<szTimeStamps; i++) {
        if(i==0) p1_triggerRate = *(timestamps+i);
        p1 = *(timestamps+i);
        std::map<size_t, size_t> eventDUs;

        if(szTimeStamps-i <= l3TriggerDuNumber) {
            if(nCounts==0){
                p1LastPart = *(timestamps+i);
            }
            
            for(int j=i; j<szTimeStamps; j++){
                p2LastPart = *(timestamps+j);
                if((p2LastPart-p1LastPart>=0)&&(p2LastPart-p1LastPart<l3TriggerTimeWindow)) {
                    i=j+1;
                    nCounts++;
                    continue;
                }
            }
            
            toEvent[nToEvent].tmBuf = new uint64_t[nCounts+1];
            memcpy(toEvent[nToEvent].tmBuf, timestamps+i, (nCounts+1)*sizeof(uint64_t));
            toEvent[nToEvent].sz = nCounts+1;

            // Then judge the DUs' number
            std::map<size_t, std::map<size_t, traceInfo>>::iterator it;
            
            for(int k=0; k<toEvent[nToEvent].sz; k++) {
                if(traceBuf.count(*(toEvent[nToEvent].tmBuf+k)) == 1){
                std::map<size_t, traceInfo>::iterator it2;
                // Find tmid in traceBuf.
                it = traceBuf.find(*(toEvent[nToEvent].tmBuf+k));
                // Caculate the DU numbers.
                for(it2 = it->second.begin(); it2 != it->second.end(); it2++) {
                    // printf("    - DUid: %d\n", it2->first);
                    eventDUs[it2->first] = 1;
                }
                continue;
                }
            }

            int n=eventDUs.size();
            if(n>=l3TriggerDuNumber) {
                evtTag++;
                std::map<size_t, std::map<size_t, traceInfo>>::iterator itt;
                // std::cout << "event number: " << evtTag << std::endl;
                // Write the L3 data into the L3 data files
                // std::cout << "Trigger DU number: " << n << std::endl;
                for(size_t k2=0; k2<toEvent[nToEvent].sz; k2++){
                    std::map<size_t, traceInfo>::iterator itt2;
                    itt = traceBuf.find(*(toEvent[nToEvent].tmBuf+k2));
                    itt2 = itt->second.begin();
                    // std::cout << "DUid: " << itt2->first << std::endl;
                    evtInfo[evtTag][*(toEvent[nToEvent].tmBuf+k2)] = itt2->first;
                    // ******************************* write into t3 data files ******************************* //
                    // writeIntoL3Datafile(traceBuf, *(toEvent[nToEvent].tmBuf+k2), itt2->first, evtTag, n);
                }
            }
            if(toEvent[nToEvent].tmBuf == NULL) printf("ERROR!!\n");
            delete toEvent[nToEvent].tmBuf;
            toEvent[nToEvent].tmBuf = nullptr;
            nToEvent++;
        }

        // TODO: P1 + timeWindow, then use the P2 to compare with P1 + timeWindow
        for(int j=i; j<szTimeStamps; j++){
            p2_triggerRate = *(timestamps+j);
            p2 = *(timestamps+j);
            timediff = p2 - p1;

            // Meet the condition of L3 trigger time window
            if(timediff > l3TriggerTimeWindow){ // good for now
                nTraces = j-i;
                toEvent[nToEvent].sz = nTraces;
                toEvent[nToEvent].tmBuf = new uint64_t[nTraces]();
                memcpy(toEvent[nToEvent].tmBuf, timestamps+i, nTraces*sizeof(uint64_t));
                
                // Then judge the DUs' number
                std::map<size_t, std::map<size_t, traceInfo>>::iterator it;
                for(int k=0; k<toEvent[nToEvent].sz; k++) {
                    if(traceBuf.count(*(toEvent[nToEvent].tmBuf+k)) == 1){
                    std::map<size_t, traceInfo>::iterator it2;
                    it = traceBuf.find(*(toEvent[nToEvent].tmBuf+k));
                    for(it2 = it->second.begin(); it2 != it->second.end(); it2++) {
                        // printf("    - DUid: %d\n", it2->first);
                        eventDUs[it2->first] = 1;
                    }
                    continue;
                    }
                }

                int n=eventDUs.size();
                if(n>=l3TriggerDuNumber) {
                    evtTag++;
                    std::map<size_t, std::map<size_t, traceInfo>>::iterator itt;
                    // std::cout << "event number: " << evtTag << std::endl;
                    // Write the L3 data into the L3 data files
                    // std::cout << "Trigger DU number: " << n << std::endl;
                    for(size_t k2=0; k2<toEvent[nToEvent].sz; k2++){
                        std::map<size_t, traceInfo>::iterator itt2;
                        itt = traceBuf.find(*(toEvent[nToEvent].tmBuf+k2));
                        itt2 = itt->second.begin();
                        evtInfo[evtTag][*(toEvent[nToEvent].tmBuf+k2)] = itt2->first;
                        // ******************************* write into t3 data files ******************************* //
                        // writeIntoL3Datafile(traceBuf, *(toEvent[nToEvent].tmBuf+k2), itt2->first, evtTag, n);
                    }
                    i = j-1;
                }
                // std::cout << "nToEvent: " << nToEvent << std::endl;
                delete toEvent[nToEvent].tmBuf;
                toEvent[nToEvent].tmBuf = nullptr;
                nToEvent++;
                break;
            }

            timediff = p2_triggerRate - p1_triggerRate;
            if(timediff > 1000000000) {
                int triRate = evtTag - triggerCounter;
                nRun = runNumber;
                int tmId = int(p1_triggerRate/1000000000);
                std::string str_Nrun = std::to_string(nRun);
                std::string triggerRateFN = "/home/grand/results/triggerRateLOGs/triggerRateLOGs_" + fileGPSTimeStamp + "_RUN" + str_Nrun + +".txt";
                FILE *fp=fopen(triggerRateFN.c_str(), "a+");
                fprintf(fp, "time: %lld, trigger rate: %dHz\n", tmId, triRate);
                fclose(fp);
                triggerCounter = evtTag;
                p1_triggerRate = p2_triggerRate;
            }
        }
    }
    std::cout << "total events: " << evtTag << std::endl;
    if(evtTag > 360) return;
    std::map<int, std::map<uint64_t, size_t>>::iterator evtIte;
    std::map<uint64_t, size_t>::iterator tmIte;
    for(evtIte = evtInfo.begin(); evtIte != evtInfo.end(); evtIte++) {
        enableWriting = true;
        std::cout << "event number: " << evtIte->first << std::endl;
        evtCount++;
        for(tmIte = evtIte->second.begin(); tmIte != evtIte->second.end(); tmIte++) {
            writeIntoL3Datafile(traceBuf, tmIte->first, tmIte->second, evtIte->first, evtInfo[evtIte->first].size());
        }
    }
    delete toEvent;
    toEvent = nullptr;
}

void writeIntoL3Datafile(std::map<size_t, std::map<std::size_t, traceInfo>> traceBuf, uint64_t timestamp, uint32_t duid, uint32_t evtTag, uint32_t nTriggerDUs) {
    char L3buffer[30000] = {0};
    int szL3L3buffer = 0;
    int szTrace = 0;
    char tmp[4] = {0};
    traceCount++;
    szTrace = traceBuf[timestamp][duid].traceSz;
    std::string datafile = traceBuf[timestamp][duid].filename;

    fin=fopen(datafile.c_str(), "rb");
    fseek(fin, FILE_HEAD_SIZE+traceBuf[timestamp][duid].idx, SEEK_SET);
    size_t ret = fread(L3buffer, 1, sizeof(DAQHeader), fin);
    if(ret != sizeof(DAQHeader)) {
        return;
    }
    *(uint32_t*)(L3buffer) = *(uint32_t*)(L3buffer) + newT3DataAdded;
    szL3L3buffer = szL3L3buffer + sizeof(DAQHeader);
    memcpy(L3buffer+szL3L3buffer, &evtTag, sizeof(uint32_t));
    szL3L3buffer = szL3L3buffer + sizeof(uint32_t);
    memcpy(L3buffer+szL3L3buffer, &nTriggerDUs, sizeof(uint32_t));
    szL3L3buffer = szL3L3buffer + sizeof(uint32_t);
    ret = fread(L3buffer+szL3L3buffer, 1, szTrace-sizeof(DAQHeader), fin);
    szL3L3buffer = szL3L3buffer + szTrace-sizeof(DAQHeader);
    if(evtCount == 1 && traceCount == 1){
        szCurrentWritten = 0;
        createNewL3File();
    }
    if(evtCount > eventNumberSaved){
        closeL3File();
        createNewL3File();
    }
    assert(l3file);
    writel3Data(L3buffer, szL3L3buffer, l3file, enableWriting);
    fclose(fin);
    // std::cout << "writeIntoL3Datafile func end" << std::endl;
}

void writel3Data(char* buffer, int sz, FILE* file, bool enableWriting) {
    if(enableWriting){
        fwrite(buffer, sz, 1, file);
    }
}

std::string genFilename(){
    char tmBuf[256] = {0};    
    time_t ct = time(NULL);
    strftime(tmBuf, 256, "%Y%m%d_%H%M%S", gmtime(&ct));
    std::string comment = l3FileName;
    int nEvtSaved = eventNumberSaved;
    std::stringstream ss;
    nRun = runNumber;
    curId++;
    ss << "GP80_" << tmBuf << "_RUN" << nRun << comment << fileGPSTimeStamp << "-" << nEvtSaved << "-" << std::setfill('0') << std::setw(4) << curId << ".dat";
    return ss.str();
}   

// Write info into the file header
void headerWriter(char* header, std::string filename){
    nRun = runNumber;
    dataVersion = DATAVERSION;
    memset(header, 0, FILE_HEAD_SIZE);
    *(uint32_t*)(header+0) = FILE_HEAD_SIZE;
    *(uint32_t*)(header+4) = dataVersion;
    *(uint32_t*)(header+8) = nRun;
    *(uint32_t*)(header+12) = time(nullptr); 
    *(uint32_t*)(header+16) = 0; // close time
    *(uint32_t*)(header+20) = filename.length(); 
    filename.copy(header+24, 128); // filename
}

void createNewL3File() {
    int szHeader = FILE_HEAD_SIZE;
    std::string l3Filename = genFilename();
    std::string dirPath = FILEDIR;
    filePath = dirPath + "/./" + l3Filename;
    std::cout << "Opening file for storage, file = " << filePath << std::endl;
    l3file = fopen(filePath.c_str(), "wb");
    assert(l3file);
    headerWriter(headerptr, filePath);
    fwrite(headerptr, szHeader, 1, l3file);
    szCurrentWritten += szHeader;
}

void closeL3File() {
    if(!enableWriting) {
        return;
    }
    if(headerptr){
        *(uint32_t*)(headerptr+16) = time(nullptr);  // close time
        fseek(l3file, 0, SEEK_SET);
        fwrite(headerptr, FILE_HEAD_SIZE, 1, l3file);
    }
    fclose(l3file);
    evtCount = 1;
    traceCount = 1;
    memset(headerptr, 0, FILE_HEAD_SIZE);
}

void loadYaml(std::string configFile){
    YAML::Node config = YAML::LoadFile(configFile);
    if(config["l3ControlParas"]) {
        YAML::Node l3ControlParas = config["l3ControlParas"];
        l3TriggerDuNumber = l3ControlParas["l3TriggerDuNumber"].as<int>();
        l3TriggerTimeWindow = l3ControlParas["l3TriggerTimeWindow"].as<int>();
        eventNumberSaved = l3ControlParas["eventNumberSaved"].as<int>();
        runNumber = l3ControlParas["runNumber"].as<int>();
        l3FileName = l3ControlParas["l3FileName"].as<std::string>();
        std::cout << "l3TriggerDuNumber: " << l3TriggerDuNumber << std::endl;
        std::cout << "l3TriggerTimeWindow: " << l3TriggerTimeWindow << std::endl;
        std::cout << "eventNumberSaved: " << eventNumberSaved << std::endl;
        std::cout << "runNumber: " << runNumber << std::endl;
    }
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <directory_path>" << std::endl;
        return 1;
    }

    std::string directoryPath = argv[1];
    auto files = getAllFiles(directoryPath);
    buffer = new char[1024*30];
    l3Timestamps = new uint64_t[LIMIT_l3TIMESTAMPS_BUF];
    enableWriting = false;
    szl3Timestamps = 0;
    evtTag = 0;
    evtCount = 0;
    traceCount = 0;
    curId = 0;

    // Control bloc
    l3ControlFile = L3CONFIGFILE;
    loadYaml(l3ControlFile);

    for (const auto& file : files) {
        std::cout << "Found file: " << file << std::endl;
        fin=fopen(file.c_str(), "rb");
        size_t hitIdInOrder = 0;
        int idx = 0;
        fseek(fin, FILE_HEAD_SIZE, SEEK_SET);
        
        while(!feof(fin)) {
            memset(buffer, 0, 1024*30);
            size_t ret = fread(buffer, 1, sizeof(DAQHeader), fin);
            if(ret != sizeof(DAQHeader)) {
                break;
            }
            hitIdInOrder++;
            DAQHeader *h = (DAQHeader*)buffer;
            assert(h->type == DAQPCK_TYPE_DUEVENT);
            uint32_t sz = h->size;
            uint16_t duID = h->source;
            ret = fread(buffer+sizeof(DAQHeader), 1, sz-sizeof(DAQHeader), fin);
            if(ret != sz-sizeof(DAQHeader)) {
                break;
            }
            
            // READ DATA TO A MAP STRUCTURE.
            readData(file.c_str(), idx, buffer, sz, duID, hitIdInOrder);
            idx+=sz;
        }
        fclose(fin);
    }

    std::cout << "szl3Timestamps before offlineL3Trigegr: " << szl3Timestamps << std::endl;

    offlineL3Trigegr(traceData, l3Timestamps, szl3Timestamps);

    // ******* release all pointers ******* //
    delete buffer;
    delete l3Timestamps;
    buffer = nullptr;
    l3Timestamps = nullptr;
    return 0;
}