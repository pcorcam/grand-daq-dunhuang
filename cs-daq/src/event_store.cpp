#include <event_store.h>
#include <utils.h>
#include <cassert>
#include <iomanip>

using namespace grand;

#define FILE_HEAD_SIZE 256

ESFileHeaderWriter::ESFileHeaderWriter()
{
    m_dataVersion = 0x00010000;
    m_ptr = new char[FILE_HEAD_SIZE];
}

ESFileHeaderWriter::~ESFileHeaderWriter() {
    delete m_ptr;
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
    m_file = nullptr;
    m_module = module;
    m_tag = tag;
    m_enableWriting = enableWriting;
    if(!m_enableWriting) {
        LOG(WARNING) << "data writing is disable by user";
    }

    if(dir == "") {
        char *dir1 = ::getenv("GRAND_DATA_DIR");
        if(dir1) {
            m_dir = dir;
        }
        else {
            m_dir = "./";
            LOG(WARNING) << "GRAND_DATA_DIR is not set, use working directory";
        }
    }
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
    // TODO: this is dummy
    CLOG(INFO, "data") << "input event from DU = " << du
            << ", datasize = " << sz;
    write(data, sz);
}

#define WRITE_ONE_SIZE 128000000
void EventStore::write(char *ptr, size_t size)
{
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

void EventStore::openStream()
{
    assert(m_file == nullptr);
    m_curId = 0;
    m_totalWritten = 0;
    m_currentWritten = 0;
    openFile();
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
    ss << m_module << "." << m_tag << "." << szBuf << "." << std::setw(3) << std::setfill('0') << m_curId << ".dat";
    return ss.str();
}

std::string EventStore::genSubdir()
{
    char szBuf[256] = {0};
    time_t ct = time(NULL);
    strftime(szBuf, 256, "%Y/%m%d/%H", localtime(&ct));
    return szBuf;
}
