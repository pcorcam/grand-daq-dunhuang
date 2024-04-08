#include <event_store.h>
#include <utils.h>
#include <cassert>
#include <iomanip>
#include <eformat.h>
#include <message_impl.h>
#include <cs_sys_config.h>

using namespace grand;

#define FILE_HEAD_SIZE 256

ESFileHeaderWriter::ESFileHeaderWriter()
{
    m_dataVersion = 0x00010000;
    m_ptr = new char[FILE_HEAD_SIZE];
}

ESFileHeaderWriter::~ESFileHeaderWriter() {
    delete m_ptr;
    m_ptr = nullptr;
}

void ESFileHeaderWriter::fileOpen(std::string filename)
{
    uint32_t runNumber = 0;
    m_size = FILE_HEAD_SIZE;
    memset(m_ptr, 0, FILE_HEAD_SIZE);

    *(uint32_t*)(m_ptr+0) = FILE_HEAD_SIZE; // file head size
    *(uint32_t*)(m_ptr+4) = m_dataVersion; // data version

    *(uint32_t*)(m_ptr+8) = runNumber;  // run_number
    *(uint32_t*)(m_ptr+12) = time(nullptr);  // open time
    *(uint32_t*)(m_ptr+16) = 0;  // close time
    *(uint32_t*)(m_ptr+20) = filename.length();  // filename size
    filename.copy(m_ptr+24, 128); // filename
}

void ESFileHeaderWriter::fileClose()
{
    *(uint32_t*)(m_ptr+16) = time(nullptr);  // close time
}

EventStore::EventStore(std::string module, std::string tag, std::string dir, size_t maxFileSize, FileHeaderWriter* fh, bool enableWriting) {
    std::cout << "Here is EventStore!" << std::endl;
    m_file = nullptr;
    m_module = module;
    m_tag = tag;
    m_enableWriting = enableWriting;

    if(!m_enableWriting) {
        LOG(WARNING) << "Data writing is disabled by user";
    }
    
    if(dir == "") {
        m_daqMode = CSSysConfig::instance()->appConfig().daqMode;
        std::cout << "EventStore DAQ MODE IS " << m_daqMode;
        
        m_dir1 = ::getenv("GRAND_T3_DATA_DIR");
        
        if(m_dir1) {
            m_dir = m_dir1;
        }
        else {
            m_dir = "./";
            LOG(WARNING) << "GRAND_DATA_DIR is not set, use working directory";
        }
    }
    
    LOG(INFO) << "data store directory = " << m_dir;
    m_maxFileSize = maxFileSize;
    if(m_maxFileSize == 0) {
        char *s = ::getenv("GRAND_MAX_FILE_SIZE");
        if(s) {
            m_maxFileSize = strtol(s, nullptr, 0);
        }
    }

    m_maxEventNumberSaved =  CSSysConfig::instance()->appConfig().eventNumberSaved;

    m_withSubdir = false;
    char *s = ::getenv("GRAND_STORE_WITH_SUBDIR");
    if(s) {
        m_withSubdir = strtol(s, nullptr, 0);
    }

    m_curId = 0;
    m_totalWritten = 0;
    m_currentWritten = 0;
    m_fh = fh;
    m_currentPath = "";

    std::cout << "creating EventStore for " << module << ", " << tag << std::endl;
}

EventStore::~EventStore() {
    if(m_file) {
        closeStream();
    }
}

void EventStore::processData(std::string du, char *data, size_t sz) {
    // std::cout << "DuID is " << du << ",size is " << sz << std::endl;
    static XRate rate("SAVE");
    uint16_t triggerpattern = *(uint16_t*)(data + (40)*sizeof(uint16_t));

    CLOG(INFO, "data") << "input event from DU = " << du
            << ", datasize = " << sz << ", trigger pattern is " << triggerpattern;
    uint32_t duID = atol(du.c_str());
    DAQEvent msg(data, sz);
    
    struct DAQHeader header;
    header.size = sizeof(DAQHeader) + msg.dataSize();
    header.type = DAQPCK_TYPE_DUEVENT;
    header.source = duID;

    m_dataSz = msg.dataSize();
    write((char*)&header, sizeof(DAQHeader));
    write(msg.data(), msg.dataSize());
    
    rate.add();
}

#define WRITE_ONE_SIZE 128000000
void EventStore::write(char *ptr, size_t size)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_file == nullptr) {
        LOG(WARNING) << "DAQ Data is still arriving after stop..";
        return;
    }
    if(!m_enableWriting) {
        return;
    }
    assert(m_file != nullptr);
    // if((m_maxFileSize != (size_t)0) && (m_currentWritten > m_maxFileSize)) {
    //     newFile();
    // }

    size_t pos = 0;
    while(pos + WRITE_ONE_SIZE  < size) {
        fwrite(ptr+pos, WRITE_ONE_SIZE, 1, m_file);
        pos += WRITE_ONE_SIZE;
    }
    fwrite(ptr+pos, size-pos, 1, m_file);

    // use evt ID to save data.
    if(size == m_dataSz) {
        char tmp[10] = {0};
        memcpy(tmp, ptr, 4);
        m_eventSave[*(uint32_t*)(tmp)] = 1;
        if((m_maxEventNumberSaved != (size_t)0) && m_eventSave.size() > m_maxEventNumberSaved) {
            newFile();
            std::map<size_t, size_t>::iterator it;
            for(it = m_eventSave.begin(); it != m_eventSave.end(); ) {
                m_eventSave.erase(it++);
            }
        }
    }

    m_currentWritten += size;
    m_totalWritten += size;
}

void EventStore::openStream()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_file == nullptr) {
        m_curId = 0;
        m_totalWritten = 0;
        m_currentWritten = 0;
        openFile();
    }
}

void EventStore::newFile()
{
    closeFile();
    openFile();
    assert(m_file);
    m_currentWritten = 0;
}

void EventStore::closeStream()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_file) {
        closeFile();
        m_curId = 0;
        m_totalWritten = 0;
        m_currentWritten = 0;
    }
}

void EventStore::openFile()
{
    if(!m_enableWriting) {
        return;
    }
    // open new file
    std::string filename = genFilename();
    if(m_withSubdir) {
      std::string subdir = genSubdir();
      if(subdir != m_curSubdir) {
          std::string subdirPath = std::string(m_dir) + "/" + subdir;
          std::string cmd = "mkdir -p ";
          cmd += subdirPath;
          system(cmd.c_str());
          m_curSubdir = subdir;
      }
    }
    else {
      m_curSubdir = ".";
    }

    m_currentPath = std::string(m_dir) + "/" + m_curSubdir + "/" + filename;
    LOG(INFO) << "Opening file for storage, file = " << m_currentPath;
    m_file = fopen(m_currentPath.c_str(), "wb");
    assert(m_file);
    if(m_fh) {
        m_fh->fileOpen(filename);
        if(m_fh->ptr()) {
            //m_file.write(m_fh->ptr(), m_fh->size());
            fwrite(m_fh->ptr(), m_fh->size(), 1, m_file);
            m_currentWritten += m_fh->size();
            m_totalWritten += m_fh->size();
        }
    }
}

void EventStore::closeFile()
{
    if(!m_enableWriting) {
        return;
    }
    assert(m_file != nullptr);
    if(m_fh) {
        m_fh->fileClose();
        if(m_fh->ptr()) {
            //m_file.seekp(0);
            fseek(m_file, 0, SEEK_SET);
            //m_file.write(m_fh->ptr(), m_fh->size());
            fwrite(m_fh->ptr(), m_fh->size(), 1, m_file);
            m_currentWritten += m_fh->size();
            m_totalWritten += m_fh->size();
        }
    }
    fclose(m_file);
    m_file = nullptr;
}

std::string EventStore::genFilename()
{
    char szBuf[256] = {0};
    time_t ct = time(NULL);
    strftime(szBuf, 256, "%Y%m%d%H%M%S", localtime(&ct));

    std::stringstream ss;
    m_curId ++ ;
     ss << m_module << "." << m_tag << "." << szBuf << "." << std::setw(3) << std::setfill('0') << m_maxEventNumberSaved << "."<<  m_curId << ".dat";
    return ss.str();
}

std::string EventStore::genSubdir()
{
    char szBuf[256] = {0};
    time_t ct = time(NULL);
    strftime(szBuf, 256, "%Y/%m%d/%H", localtime(&ct));
    return szBuf;
}
