#include <raw_event_store.h>
#include <utils.h>
#include <cassert>
#include <iomanip>
#include <eformat.h>
#include <message_impl.h>
#include <cs_sys_config.h>

using namespace grand;

#define FILE_HEAD_SIZE 256

class XXClock {
    public:
        inline static uint64_t nowNanoSeconds() {
            std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> tp = std::chrono::time_point_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now());
            return std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();
        }
};

RawESFileHeaderWriter::RawESFileHeaderWriter()
{
    m_rawDataVersion = 0x00010000;
    m_ptr = new char[FILE_HEAD_SIZE];
}

RawESFileHeaderWriter::~RawESFileHeaderWriter() {
    delete m_ptr;
    m_ptr = nullptr;
}

void RawESFileHeaderWriter::fileOpen(std::string filename)
{
    uint32_t runNumber = 0;
    m_size = FILE_HEAD_SIZE;
    memset(m_ptr, 0, FILE_HEAD_SIZE);

    *(uint32_t*)(m_ptr+0) = FILE_HEAD_SIZE; // file head size
    *(uint32_t*)(m_ptr+4) = m_rawDataVersion; // data version

    *(uint32_t*)(m_ptr+8) = runNumber;  // run_number
    *(uint32_t*)(m_ptr+12) = time(nullptr);  // open time
    *(uint32_t*)(m_ptr+16) = 0;  // close time
    *(uint32_t*)(m_ptr+20) = filename.length();  // filename size
    // std::cout << "fileOpen func filename: " << filename << std::endl;
    filename.copy(m_ptr+24, 128); // filename
}

void RawESFileHeaderWriter::fileClose()
{
    *(uint32_t*)(m_ptr+16) = time(nullptr);  // close time
}

rawEventStore::rawEventStore(std::string module, std::string tag, std::string dir, size_t maxFileSize, FileHeaderWriter* fh, int daqMode, bool enableWriting) {
    m_file = nullptr;
    m_MDFile = nullptr;
    m_module = module;
    m_tag = tag;
    m_enableWriting = enableWriting;
    m_daqMode = daqMode;
    
    if(!m_enableWriting) {
        LOG(WARNING) << "Data writing is disabled by user";
    }
    if(dir == "") {
        
        m_dir1 = ::getenv("GRAND_RAW_DATA_DIR");
        // std::cout << "RAW_DATA_Meantime'S SAVE PATH IS " << m_dir1 << std::endl;

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

    m_withSubdir = false;
    char *s = ::getenv("GRAND_STORE_WITH_SUBDIR");
    if(s) {
        m_withSubdir = strtol(s, nullptr, 0);
    }

    m_curId = 0;
    m_curMDId = 0;
    m_totalWritten = 0;
    m_currentWritten = 0;
    m_totalMDWritten = 0;
    m_currentMDWritten = 0;
    m_fh = fh;
    m_currentPath = "";
    m_currentMDPath = "";
    m_frequenceCountBefore = 0;
    m_frequenceCountAfter = 0;
	
    m_time0 = 0;
    m_time1 = 0;
    m_count = 0;

    // std::cout << "creating RawEventStore for " << module << ", " << tag << std::endl;
}

rawEventStore::~rawEventStore() {
    if(m_file) {
        closeStream();
    }
    if(m_MDFile) {
        closeMDStream();
    }
}

void rawEventStore::processData(std::string du, char *data, size_t sz) {
    // std::cout << "DuID is " << du << ",daq data is " << data << ",size is " << sz << std::endl;
	
    int hitId = 0;
    uint64_t nanoTime = 0;	
    uint32_t *hit;
    uint16_t* m_evtbuf = new uint16_t[sz/sizeof(uint16_t)];
    memset(m_evtbuf, 0, sz);
    memcpy(m_evtbuf, data, sz);
    ElecEvent ev(m_evtbuf, sz/sizeof(uint16_t));
    // printf("length is %d\n", m_evtbuf[4]);
    nanoTime = ev.getTimeFullDataSz().totalSec; // use getTimeFullDataSz because we include index 0 data length here.
    hit = (uint32_t *)m_evtbuf;
    hitId = hit[2];
    m_time1 = XXClock::nowNanoSeconds();
    

    if(m_count == 0) {
	    // printf("DUId is %s, hitId is %d, nanotime is %lld\n", du.c_str(), hitId+1, nanoTime);
    }
    else if(m_count>0) {
	    // printf("DUId is %s, hitId is %d, timediff is %lld, nanotime is %lld\n", du.c_str(), hitId+1, m_time1 - m_time0, nanoTime);
    }
    m_time0 = m_time1;
    m_count++;
    delete[] m_evtbuf;
    m_evtbuf = nullptr;

    if(m_frequenceCountBefore == 0) {
        m_t0 = XXClock::nowNanoSeconds();
    }
    m_frequenceCountBefore++;
    m_t1 = XXClock::nowNanoSeconds();
    if(m_t1 - m_t0 > timeCount) {
        printf("L1 trigger frequence is %d/s\n", m_frequenceCountBefore - m_frequenceCountAfter);
        m_frequenceCountAfter = m_frequenceCountBefore;
        m_t0 = m_t1;
    }

    static XRate rate("SAVE");
    // TODO: this is dummy
    uint16_t triggerpattern = *(uint16_t*)(data + (40)*sizeof(uint16_t));
    // printf("triggerpattern: %ld\n", triggerpattern);
    CLOG(INFO, "data") << "input event from DU = " << du
            << ", datasize = " << sz << ", trigger pattern is " << triggerpattern;
    
    // TODO:
    // 1. Use nanosec to pick up MD data
    // 2. waiting for trigger pattern fixed
    // if(triggerpattern == 32  &&  m_daqMode == 4) { // 10s mode data check
    if(0 < nanoTime < 200 && m_daqMode == 4) {
        uint32_t duID = atol(du.c_str());
        RawEvent MDmsg(data, sz);
        struct RawEventHeader header;
        header.size = sizeof(RawEventHeader) + MDmsg.dataSize();
        // std::cout << "MDmsg.dataSize(): " << MDmsg.dataSize() << std::endl;
        header.type = DAQPCK_TYPE_DUEVENT;
        header.source = duID;
        writeMD((char*)&header, sizeof(RawEventHeader));
        writeMD(MDmsg.data(), MDmsg.dataSize());
        rate.add();
    }
    else {
        // std::cout << "should not appear " << std::endl;
        uint32_t duID = atol(du.c_str());
        RawEvent msg(data, sz);
        struct RawEventHeader header;
        header.size = sizeof(RawEventHeader) + msg.dataSize();
        header.type = DAQPCK_TYPE_DUEVENT;
        header.source = duID;
        write((char*)&header, sizeof(RawEventHeader));
        write(msg.data(), msg.dataSize());
        rate.add();
    }
    
}

#define WRITE_ONE_SIZE 128000000
void rawEventStore::write(char *ptr, size_t size)
{   
    // std::cout << "size in write func: " << size << std::endl;
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_file == nullptr) {
        LOG(WARNING) << "DAQ Data is still arriving after stop..";
        return;
    }

    if(!m_enableWriting) {
        return;
    }
    assert(m_file != nullptr);
    if((m_maxFileSize != (size_t)0) && (m_currentWritten > m_maxFileSize)) {
        newFile();
    }

    size_t pos = 0;
    while(pos + WRITE_ONE_SIZE  < size) {
        fwrite(ptr+pos, WRITE_ONE_SIZE, 1, m_file);
        pos += WRITE_ONE_SIZE;
    }

    fwrite(ptr+pos, size-pos, 1, m_file);
    m_currentWritten += size;
    m_totalWritten += size;
}

void rawEventStore::writeMD(char *ptr, size_t size) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_MDFile == nullptr) {
        LOG(WARNING) << "MD mode DAQ Data is still arriving after stop..";
        return;
    }

    if(!m_enableWriting) {
        return;
    }
    
    assert(m_MDFile != nullptr);

    if((m_maxFileSize != (size_t)0) && (m_currentMDWritten > m_maxFileSize)) {
        newMDFile();
    }

    size_t pos = 0;
    while(pos + WRITE_ONE_SIZE  < size) {
        fwrite(ptr+pos, WRITE_ONE_SIZE, 1, m_MDFile);
        pos += WRITE_ONE_SIZE;
    }

    fwrite(ptr+pos, size-pos, 1, m_MDFile);
    m_currentMDWritten += size;
    m_totalMDWritten += size;   
}

void rawEventStore::openStream()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_file == nullptr) {
        m_curId = 0;
        m_totalWritten = 0;
        m_currentWritten = 0;
        openFile();
    }
}

void rawEventStore::openMDStream() 
{   
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_MDFile == nullptr) {
        m_curMDId = 0;
        m_totalMDWritten = 0;
        m_currentMDWritten = 0;
        openMDFile();
    }
}

void rawEventStore::newFile()
{
    closeFile();
    openFile();
    assert(m_file);
    m_currentWritten = 0;
}

void rawEventStore::newMDFile()
{
    closeMDFile();
    openMDFile();
    assert(m_MDFile);
    m_currentMDWritten = 0;
}

void rawEventStore::closeStream()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_file) {
        closeFile();
        m_curId = 0;
        m_totalWritten = 0;
        m_currentWritten = 0;
    }
}

void rawEventStore::closeMDStream()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if(m_MDFile) {
        closeMDFile();
        m_curMDId = 0;
        m_totalMDWritten = 0;
        m_currentMDWritten = 0;
    }
}

void rawEventStore::openFile()
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
            fwrite(m_fh->ptr(), m_fh->size(), 1, m_file);
            m_currentWritten += m_fh->size();
            m_totalWritten += m_fh->size();
        }
    }
}

void rawEventStore::openMDFile() 
{
    if(!m_enableWriting) {
        return;
    }

    // open new file
    std::string mdFilename = genMDFilename();
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

    m_currentMDPath = std::string(m_dir) + "/" + m_curSubdir + "/" + mdFilename;
    LOG(INFO) << "Opening file for MD(10s) storage, file = " << m_currentMDPath;
    m_MDFile = fopen(m_currentMDPath.c_str(), "wb");
    assert(m_MDFile);
    
    if(m_fh) {
        m_fh->fileOpen(mdFilename);
        if(m_fh->ptr()) {
            // std::cout << 1111111 << std::endl;
            fwrite(m_fh->ptr(), m_fh->size(), 1, m_MDFile);
            m_currentMDWritten += m_fh->size();
            m_totalMDWritten += m_fh->size();
        }
    }
}

void rawEventStore::closeFile()
{
    if(!m_enableWriting) {
        return;
    }
    assert(m_file != nullptr);
    if(m_fh) {
        m_fh->fileClose();
        if(m_fh->ptr()) {
            fseek(m_file, 0, SEEK_SET);
            fwrite(m_fh->ptr(), m_fh->size(), 1, m_file);
            m_currentWritten += m_fh->size();
            m_totalWritten += m_fh->size();
        }
    }
    fclose(m_file);
    m_file = nullptr;
}

void rawEventStore::closeMDFile() 
{
    if(!m_enableWriting) {
        return;
    }
    assert(m_MDFile != nullptr);
    if(m_fh) {
        m_fh->fileClose();
        if(m_fh->ptr()) {
            fseek(m_MDFile, 0, SEEK_SET);
            fwrite(m_fh->ptr(), m_fh->size(), 1, m_MDFile);
            m_currentWritten += m_fh->size();
            m_totalWritten += m_fh->size();
        }
    }
    fclose(m_MDFile);
    m_MDFile = nullptr;
}

std::string rawEventStore::genFilename()
{
    char szBuf[256] = {0};
    time_t ct = time(NULL);
    strftime(szBuf, 256, "%Y%m%d_%H%M%S", gmtime(&ct));

    std::stringstream ss;
    m_curId ++ ;
    ss << m_module << "_" << szBuf << "_" << m_tag << "-" << std::setfill('0') << std::setw(4) <<  m_curId << ".dat";

    // If failed to validate the filename, exit
    if (auto ret=validateFilename(ss.str())!=0)
        exit(ret);

    return ss.str();
}

std::string rawEventStore::genMDFilename()
{
    char szBuf[256] = {0};
    time_t ct = time(NULL);
    strftime(szBuf, 256, "%Y%m%d_%H%M%S", gmtime(&ct));

    std::stringstream ss;
    m_curMDId++;
    ss << m_module << "_" << szBuf << "_" << m_tag << "-MD-" << std::setfill('0') << std::setw(4) <<  m_curMDId << ".dat";
    return ss.str();
}

std::string rawEventStore::genSubdir()
{
    char szBuf[256] = {0};
    time_t ct = time(NULL);
    strftime(szBuf, 256, "%Y%m%d_%H%M%S", gmtime(&ct));
    return szBuf;
}
