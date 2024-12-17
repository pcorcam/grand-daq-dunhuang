#include <iostream>
#include <string.h>
#include <string>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <filesystem>
#include <eformat.h>
#include <cassert>
#include "map"
#include <data_format.h>
#include <algorithm>
#include <unistd.h>
#include "yaml-cpp/yaml.h"
#include "yaml-cpp/node/parse.h"
#include <mutex>
#include <memory> // for smart pointers
// #include <cs_sys_config.h>


#define newT3DataAdded 8
#define CHANNEL_LENGTH 64
#define LIMIT_l3TIMESTAMPS_BUF 1000000

// File Header
#define FILE_HEAD_SIZE 256
#define DATAVERSION 0x00010000
#define FILEDIR "/home/grand/workarea/t3_data"
// #define FILEDIR "/home/grand/workarea/l3OfflineTrigger/testarea/20241210_version1.4_denyMD/testDataL3"

// Control L3 file.
#define L3CONFIGFILE "/home/grand/workarea/l3OfflineTrigger/l3control.yaml"

struct traceInfo{
    std::string filename;
    size_t traceSz = 0;
    int idx = 0;
};

struct t3TmDuid {
    size_t sz;
    uint64_t* tmBuf;
};

FILE* fin, fout;
DAQHeader daqHeader;
uint64_t tmId;
uint64_t* l3Timestamps;
size_t szl3Timestamps;
uint32_t evtTag, evtCount, traceCount;
char *buffer;
bool enableWriting;
int curId, nRun, dataVersion;

std::mutex mapMutex;

// File writer
FILE *l3file;
std::string fileGPSTimeStamp;
std::string filePath;
int szCurrentWritten;
char headerptr[FILE_HEAD_SIZE] = {0};
std::map<uint64_t, std::map<std::size_t, traceInfo>> traceData;
std::map<int, std::map<std::size_t, uint64_t>> evtInfo;

// L3 control file
std::string l3ControlFile;
int l3TriggerDuNumber;
int l3TriggerTimeWindow;
int eventNumberSaved;
int runNumber;
std::string l3FileName;

// functions
void readData(std::string filename, int idx, char* buffer, size_t size, uint16_t duID, size_t hitIdInOrder);
// void readData( char* buffer, size_t size, uint16_t duID, size_t hitIdInOrder);
void offlineL3Trigegr(std::map<size_t, std::map<std::size_t, traceInfo>> traceBuf, uint64_t* timestamps, size_t szTimeStamps);
void writeIntoL3Datafile(std::map<size_t, std::map<std::size_t, traceInfo>> traceBuf, uint64_t timestamp, uint32_t duid ,uint32_t evtTag, uint32_t nTriggerDUs);
void createNewL3File();
void closeL3File();
std::string genFilename();
void headerWriter(char* header, std::string filename);
void writel3Data(char* buffer, int sz, FILE* file, bool enableWriting);

void loadYaml(std::string configFile);