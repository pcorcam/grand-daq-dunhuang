#pragma once

#include <string>
#include <mutex>
#include "map"

namespace grand {

class FileHeaderWriter
{
public:
    FileHeaderWriter(): m_ptr(nullptr), m_size(0) {}
    ~FileHeaderWriter() {}
    virtual void fileOpen(std::string filename) {}
    virtual void fileClose() {};

    char *ptr() { return m_ptr; }
    int size() { return m_size; }

protected:
    char *m_ptr;
    int m_size;
};

class ESFileHeaderWriter : public FileHeaderWriter
{
public :
    ESFileHeaderWriter();
    virtual ~ESFileHeaderWriter();
    virtual void fileOpen(std::string filename);
    virtual void fileClose();

private:
    uint32_t m_dataVersion;
};

class EventStore {
public:
    EventStore(std::string module, std::string tag, std::string dir = "", size_t maxFileSize=100*1024*1024, FileHeaderWriter* fh = nullptr, bool enableWriting = true);
    ~EventStore();
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
    size_t m_maxEventNumberSaved;
    bool m_enableWriting;
    int m_curId;
    
    size_t m_totalWritten;
    size_t m_currentWritten;
    int m_daqMode;
    char* m_dir1;

    std::string m_currentPath;

    FileHeaderWriter *m_fh;

    bool m_withSubdir;
    std::string m_curSubdir;
    std::mutex m_mutex;

    std::map<size_t, size_t> m_eventSave;
    size_t m_dataSz = 0;
};

}
