#pragma once

#include <string>
#include <mutex>
#include <event_store.h>
#include <data_format.h>

#define timeCount 1000000000

namespace grand {

class RawESFileHeaderWriter : public FileHeaderWriter
{
public :
    RawESFileHeaderWriter();
    virtual ~RawESFileHeaderWriter();
    virtual void fileOpen(std::string filename);
    virtual void fileClose();

private:
    uint32_t m_rawDataVersion;
};

class rawEventStore {
public:
    rawEventStore(std::string module, std::string tag, std::string dir = "", size_t maxFileSize=100*1024*1024, FileHeaderWriter* fh = nullptr, bool enableWriting = true);
    ~rawEventStore();
    void processData(std::string du, char *data, size_t sz);

    void openStream();
    void closeStream();
    void write(char *ptr, size_t size);

private:
    std::string genFilename();
    std::string genSubdir();

    void newFile();
    void openFile();
    void closeFile();

    std::string m_module;
    std::string m_tag;
    std::string m_dir;
    FILE *m_file;
    size_t m_maxFileSize;
    bool m_enableWriting;
    int m_curId;
    
    size_t m_totalWritten;
    size_t m_currentWritten;
    int m_daqMode;
    char* m_dir1;

    uint64_t m_t0, m_t1, m_t2;
    int m_frequenceCountBefore;
    int m_frequenceCountAfter;
    int m_duId;

    std::string m_currentPath;

    FileHeaderWriter *m_fh;

    bool m_withSubdir;
    std::string m_curSubdir;
    std::mutex m_mutex;

    uint16_t* m_evtbuf;
    uint64_t m_time0, m_time1;
    int m_count;
};

}
