/*********************************************
 *Created by duanbh,
 *2022/4/22,
 *ScopeA stands for now foreign version software.
**********************************************/
#include "scope_a.h"
#include <cassert>

using namespace grand;
using namespace std;

ScopeA::ScopeA()
{
    cout << "let us start dudaq" << endl;
}

ScopeA::~ScopeA(){
    cout << "Son1 Obj ScopeA is being deleted!" << endl;
}

void ScopeA::scope_raw_write(uint32_t reg_addr, uint32_t value)
{
  *((unsigned int *)((char *)axi_ptr+page_offset+reg_addr)) = value;
}

int32_t ScopeA::scope_raw_read(uint32_t reg_addr, uint32_t *value) //new, reading from AXI
{
  *value = *((unsigned int *)((char *)axi_ptr+page_offset+reg_addr));
  return(1);
}

void ScopeA::scope_flush()
{
  assert(("axi_ptr should not be NULL",axi_ptr != NULL));
  scope_raw_write(Reg_GenControl,0x08000000); // clear DAQ Fifo's
  scope_raw_write(Reg_GenControl,0x00000000);
}

void ScopeA::scope_set_parameters(uint32_t reg_addr, uint32_t value, uint32_t to_shadow)
{
  uint32_t Is_Data;

  if(to_shadow == 1) shadowlist[reg_addr>>2] = value;
  if(axi_ptr == NULL) return;
  scope_raw_write(reg_addr,value);
//  scope_raw_read(reg_addr,&value);
  usleep(1000);
}

void ScopeA::elecInit() {
    unsigned int addr, page_addr;
    unsigned int page_size=sysconf(_SC_PAGESIZE);

    printf("open scope\n");
    if(dev != 0) close(dev);
    axi_ptr = NULL;
    printf("Trying to open !%s!\n",DEVFILE);
    assert((dev = open(DEVFILE, O_RDWR)) != -1);
    dev = open(DEVFILE, O_RDWR);
    assert(("dev open failed", dev != -1));
    addr = (unsigned int) TDAQ_BASE;
    page_addr = (addr & ~(page_size-1));
    page_offset = addr - page_addr;
    axi_ptr=mmap(NULL,page_size,PROT_READ|PROT_WRITE, MAP_SHARED,dev,page_addr);
    if ((long)axi_ptr == -1) {
        perror("opening scope\n");
        exit(-1);
    }
    printf("Done opening dev = %d\n",(int)dev);
    sleep(1);
    assert(("axi_ptr should not be NULL",axi_ptr != NULL));
    scope_raw_write(Reg_Dig_Control,0x00004000);
    scope_raw_write(Reg_Dig_Control,0x00000000);
    memset(shadowlist,0,sizeof(shadowlist));
    std::cout << "Scope stop run" << std::endl; 
    assert(("axi_ptr should not be NULL",axi_ptr != NULL));
    scope_set_parameters(Reg_Dig_Control,shadowlist[Reg_Dig_Control>>2] & (~CTRL_PPS_EN & ~CTRL_SEND_EN ),1);
    scope_flush();
    // 创建地址映射
    // 打开设备
}

void ScopeA::elecConfig(void *parameters) {
    uint32_t *shadowlist = (uint32_t*)parameters;
    for(int i=0;i<Reg_End;i+=4){
        scope_set_parameters(i,shadowlist[i>>2],0);
    }
    evtlen = HEADER_EVT;

    uint16_t *sl = (uint16_t *)shadowlist;
    for(int ich=0;ich<4;ich++) {
    if(shadowlist[Reg_TestPulse_ChRead>>2]&(1<<ich)){
      evtlen +=(sl[(Reg_Time1_Pre>>1)+2*ich]+sl[(Reg_Time1_Post>>1)+2*ich]+sl[Reg_Time_Common>>1]);
      }
    }
    if(evtbuf != NULL) {
      free(evtbuf);
    }

    printf("Creating a buffer of size %d\n",BUFSIZE*evtlen);
    evtbuf = (uint16_t *)malloc(BUFSIZE*evtlen*sizeof(uint16_t));
    // ptr_evt = 0;
    // 使用shadowlist 将配置信息全部写入电子学 
}

void ScopeA::elecStartRun() {
    scope_flush();
    scope_set_parameters(Reg_Dig_Control,shadowlist[Reg_Dig_Control>>2] |(CTRL_PPS_EN | CTRL_SEND_EN ),1);
    // 发送开始取数命令
}

int ScopeA::elecReadData(char *data, size_t maxSize){
    static uint16_t evtnr=0;
    int station_id;
    uint32_t Is_Data,tbuf,*ebuf;
    struct tm tt;
    int length;
    double fracsec;
    uint32_t *sec,*nanosec;
    int offset = 0;

    assert(("axi_ptr should not be NULL", axi_ptr != NULL));
    assert(("evtbuf should not be NULL", evtbuf != NULL));

    // 检查电子学buffer是否有数据
    bool hasData = false;
    scope_raw_write(Reg_GenControl,GENCTRL_EVTREAD);
    scope_raw_read(Reg_GenStatus,&Is_Data);
    if((Is_Data&(GENSTAT_EVTFIFO)) == 0){
        // buffer中有数据
        uint32_t dataSizeRaw, dataSizeBytes;
        uint32_t *ptr = (uint32_t*)data;
        if((dataSizeRaw>>16) == 0xADC0) {
            dataSizeBytes = (dataSizeRaw&0xffff)*sizeof(uint16_t);
            maxSize = dataSizeBytes/2;
            length = dataSizeBytes/2;

            if(length>0) {
                printf("Offset = %d (%d %d)\n",offset,evtlen,length);
                ptr = (uint32_t *)&evtbuf[0];
                (*ptr++) = dataSizeRaw;
                for(int i=0; i<dataSizeBytes/4; i++) {
                    scope_raw_read(Reg_Data, ptr++);
                }
                evtbuf[offset+EVT_HDRLEN] = HEADER_EVT;
                //TO DO:
                //sec = (uint32_t *)&evtbuf[offset+EVT_SECOND];
                //tt.tm_sec = (evtbuf[offset+EVT_STATSEC]&0xff)-evtbuf[offset+EVT_LEAP];    // Convert GPS in a number of seconds
                //tt.tm_min = (evtbuf[offset+EVT_MINHOUR]>>8)&0xff;
                //tt.tm_hour = (evtbuf[offset+EVT_MINHOUR])&0xff;
                //tt.tm_mday = (evtbuf[offset+EVT_DAYMONTH]>>8)&0xff;
                //tt.tm_mon = (evtbuf[offset+EVT_DAYMONTH]&0xff)-1;
                //tt.tm_year = evtbuf[offset+EVT_YEAR] - 1900;
                ////printf("Event timestamp %02d/%02d/%04d %2d:%2d:%2d\n",tt.tm_mday,tt.tm_mon+1,tt.tm_year+1900,tt.tm_hour,tt.tm_min,tt.tm_sec);
                //*sec = (unsigned int)timegm(&tt);
                //fracsec = (double)(*(uint32_t *)&evtbuf[offset+EVT_CTD])/(double)(*(uint32_t *)&evtbuf[offset+EVT_CTP]);
                //nanosec = (uint32_t *)&evtbuf[offset+EVT_NANOSEC];
                //*nanosec = 1.E9*fracsec;
                evtbuf[offset+EVT_TRIGGERPOS] = shadowlist[Reg_Time1_Pre>>1]+shadowlist[Reg_Time_Common>>1];
                evtbuf[offset+EVT_ID] = evtnr++;
                evtbuf[offset+EVT_HARDWARE] = station_id;

                data = (char *)evtbuf; 
                std::cout << 1111111 << std::endl;

                FILE* fp;
                fp = fopen("test_5.20_event_binfile.txt","a");
                //fprintf(fp,"Reading event %08x %d %u.%09d %g\n",tbuf,evtbuf[EVT_STATSEC]&0xff,*sec,*nanosec,fracsec);
                fwrite(evtbuf, (uint8_t*)ebuf-(uint8_t*)evtbuf, 1, fp);
                //    printf("d%\n",(uint8_t*)ebuf-(uint8_t*)evtbuf);
                //    printf("Reading event %08x %d %u.%09d %g\n",tbuf,evtbuf[EVT_STATSEC]&0xff,*sec,*nanosec,fracsec);
                fclose(fp);
                //timestampbuf[next_write].ts_seconds = *sec;
                //timestampbuf[next_write].ts_nanoseconds = *nanosec;
                //timestampbuf[next_write].event_nr = evtbuf[offset+EVT_ID];
                //timestampbuf[next_write].trigmask = evtbuf[offset+EVT_TRIG_PAT];
                //printf("Next offset = %d\n",ptr_evt*evtlen);
                return SCOPE_EVENT;                  // success!
            }
            else return -1;
        }
        else length = -1;
    }    
    else {
        // buffer中没有数据
        return(0);
    }
}

void ScopeA::elecStopRun() {
    scope_set_parameters(Reg_Dig_Control,shadowlist[Reg_Dig_Control>>2] & (~CTRL_PPS_EN & ~CTRL_SEND_EN ),1);
    scope_flush();
}

void ScopeA::elecTerminate() {
    close(dev);
    dev = 0;
    axi_ptr = NULL;
}

