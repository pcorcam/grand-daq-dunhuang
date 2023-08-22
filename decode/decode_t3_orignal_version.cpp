#include <iostream>
#include <eformat.h>
#include <cassert>
#include <map>
#include <string.h>

#define NANO_CHANGE 100000 //use this macro to change nanosec
#define FILE_HEAD_SZ 256

using namespace grand;

void printChannelData(FILE *fp, uint16_t len, uint16_t *data) {
    fprintf(fp, "    ");
    for(uint16_t i=0; i<len; i++) {
        int d = (int16_t)data[i];
        fprintf(fp, "%7d ", (int)d);
        if((i+1)%10 == 0) {
            fprintf(fp, "\n    ");
        }
    }
    if(len%10 != 0 || len == 0) {
        fprintf(fp, "\n");
    }
}

void printData(FILE *fp, char *buffer, size_t size, uint16_t duID, size_t hitID) {
    char Event_ID_tag[4];
    
    fprintf(fp, "DU_Data:\n");
    fprintf(fp, " DAQ_PACK_SIZE = %lld\n", size);
    fprintf(fp, " DU_ID = %d\n", duID);
    fprintf(fp, " ELEC_DATA:\n");
    fprintf(fp, " HitID: %d\n", hitID);

    uint16_t *ptr = (uint16_t*)(buffer+sizeof(DAQHeader));
    // uint32_t *_ptr = (uint32_t*)(buffer+sizeof(DAQHeader));
    memcpy(Event_ID_tag, (char*)(ptr++),4);
    fprintf(fp, "  Event_ID_Tag = %d\n", atoi(Event_ID_tag));
    ptr++; 
    fprintf(fp, "- ELEC_RAW_DATA:\n"); 
    fprintf(fp, "  LENGTH = %d\n", *(ptr++));
    fprintf(fp, "  Evnet ID = %d\n", *(ptr++));
    // _ptr++;
    // fprintf(fp, "  EventID2 = %d\n", *(_ptr));
    // fprintf(fp, "  EventID = %d\n", *(ptr++));
    // ptr++;
    fprintf(fp, "  HARDWARE_ID = %d\n", *(ptr++));
    fprintf(fp, "  Header length = %d\n", *(ptr++));
    fprintf(fp, "  Seconds (lsb) = %d\n", *(ptr++));
    fprintf(fp, "  Seconds (msb) = %d\n", *(ptr++));
    fprintf(fp, "  Nanoseconds (lsb) = %d\n", ((uint32_t)(*(ptr++))));
    fprintf(fp, "  Nanoseconds (msb) = %d\n", *(ptr++));
    fprintf(fp, "  Trigger position = %d\n", *(ptr++));
    fprintf(fp, "  spare 0 = %d\n", *(ptr++));
    fprintf(fp, "  spare 1 = %d\n", *(ptr++));
    fprintf(fp, "  spare 2 = %d\n", *(ptr++));
    fprintf(fp, "  spare 3 = %d\n", *(ptr++));
    fprintf(fp, "  spare 4 = %d\n", *(ptr++));
    fprintf(fp, "  spare 5 = %d\n", *(ptr++));
    fprintf(fp, "  spare 6 = %d\n", *(ptr++));
    fprintf(fp, "  spare 7 = %d\n", *(ptr++));
    fprintf(fp, "  Atmospheric temperature = %d\n", *(ptr++));
    fprintf(fp, "  Atmospheric pressure = %d\n", *(ptr++));
    fprintf(fp, "  Atmospheric humidity = %d\n", *(ptr++));
    fprintf(fp, "  Accelerometer X = %d\n", *(ptr++));
    fprintf(fp, "  Accelerometer Y = %d\n", *(ptr++));
    fprintf(fp, "  Accelerometer Z = %d\n", *(ptr++));
    fprintf(fp, "  Battery voltage = %d\n", *(ptr++));
    fprintf(fp, "  Format Version = %d\n", *(ptr++));
    fprintf(fp, "  ADC sample frequency = %d\n", *(ptr++));
    fprintf(fp, "  ADC resolution = %d\n", *(ptr++));
    fprintf(fp, "  Input selection = %d\n", *(ptr++));
    fprintf(fp, "  DAQ Channel Enable = %d\n", *(ptr++));
    fprintf(fp, "  Total number of samples = %d\n", 16*(*(ptr++)));
    fprintf(fp, "  Samples in channel 1 = %d\n", *(ptr++));
    fprintf(fp, "  Samples in channel 2 = %d\n", *(ptr++));
    fprintf(fp, "  Samples in channel 3 = %d\n", *(ptr++));
    fprintf(fp, "  Samples in channel 4 = %d\n", *(ptr++));
    fprintf(fp, "  Trigger Pattern = %d\n", *(ptr++));
    fprintf(fp, "  Trigger rate (stamp by last PPS) = %d\n", *(ptr++));
    fprintf(fp, "  CTD (lsb) (stamp by last trigger) = %d\n", *(ptr++));
    fprintf(fp, "  CTD (msb) = %d\n", *(ptr++));
    fprintf(fp, "  CTP (lsb) (stamp by last PPS) = %d\n", *(ptr++));
    fprintf(fp, "  CTP (msb) = %d\n", *(ptr++));
    fprintf(fp, "  PPS Offset (lsb) = %d\n", *(ptr++));
    fprintf(fp, "  PPS Offset (msb) = %d\n", *(ptr++));
    fprintf(fp, "  GPS UTC offset = %d\n", *(ptr++));
    fprintf(fp, "  GPS Dec Status, UTC Timing flag = %d\n", *(ptr++));
    fprintf(fp, "  GPS Critical Alarm, Receiver Mode = %d\n", *(ptr++));
    fprintf(fp, "  GPS Minor Alarms = %d\n", *(ptr++));
    fprintf(fp, "  GPS year (stamp by trigger) = %04d\n", *(ptr++));
    fprintf(fp, "  GPS day, month = %02d %02d\n", (*(ptr++)>>8)&0xff, (*ptr)&0xff);
    fprintf(fp, "  GPS hour, minute = %02d %02d\n", *(ptr++)&0xff, ((*ptr)>>8)&0xff);
    fprintf(fp, "  Status, GPS second = %d %02d\n", *(ptr++),(*ptr)&0xff);
    fprintf(fp, "  GPS Longitude (lsb) (stamp by last PPS) = %d\n", *(ptr++));
    fprintf(fp, "  GPS Longitude = %d\n", *(ptr++));
    fprintf(fp, "  GPS Longitude = %d\n", *(ptr++));
    fprintf(fp, "  GPS Longitude (msb) = %d\n", *(ptr++));
    fprintf(fp, "  GPS Latitude (lsb) = %d\n", *(ptr++));
    fprintf(fp, "  GPS Latitude = %d\n", *(ptr++));
    fprintf(fp, "  GPS Latitude = %d\n", *(ptr++));
    fprintf(fp, "  GPS Latitude (msb) = %d\n", *(ptr++));
    fprintf(fp, "  GPS Altitude (lsb) = %d\n", *(ptr++));
    fprintf(fp, "  GPS Altitude = %d\n", *(ptr++));
    fprintf(fp, "  GPS Altitude = %d\n", *(ptr++));
    fprintf(fp, "  GPS Altitude (msb) = %d\n", *(ptr++));
    fprintf(fp, "  GPS Temperature (lsb) = %d\n", *(ptr++));
    fprintf(fp, "  GPS Temperature (msb) = %d\n", *(ptr++));
    fprintf(fp, "  control parameters = %d\n", *(ptr++));
    fprintf(fp, "  control parameters = %d\n", *(ptr++));
    fprintf(fp, "  control parameters = %d\n", *(ptr++));
    fprintf(fp, "  control parameters = %d\n", *(ptr++));
    fprintf(fp, "  control parameters = %d\n", *(ptr++));
    fprintf(fp, "  control parameters = %d\n", *(ptr++));
    fprintf(fp, "  control parameters = %d\n", *(ptr++));
    fprintf(fp, "  control parameters = %d\n", *(ptr++));
    fprintf(fp, "  window parameters = %d\n", *(ptr++));
    fprintf(fp, "  window parameters = %d\n", *(ptr++));
    fprintf(fp, "  window parameters = %d\n", *(ptr++));
    fprintf(fp, "  window parameters = %d\n", *(ptr++));
    fprintf(fp, "  window parameters = %d\n", *(ptr++));
    fprintf(fp, "  window parameters = %d\n", *(ptr++));
    fprintf(fp, "  window parameters = %d\n", *(ptr++));
    fprintf(fp, "  window parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  channel property parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  trigger parameters = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 1 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 2 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 3 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch1) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch2) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch3) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch4) = %d\n", *(ptr++));
    fprintf(fp, "  filter parameters filter 4 (ch4) = %d\n", *(ptr++));
    
    // ptr = (uint16_t*)(buffer+sizeof(DAQHeader)+30*sizeof(uint16_t)+4); // Samples in channel 1
    // Focus! I add a EventID in raw data, so the position of 4 byte should be added.
    ptr = (uint16_t*)(buffer+sizeof(DAQHeader)+32*sizeof(uint16_t));
    uint16_t lenCh1 = *(ptr++);
    uint16_t lenCh2 = *(ptr++);
    uint16_t lenCh3 = *(ptr++);
    uint16_t lenCh4 = *(ptr++);

    fprintf(fp, "  SAMPLES_CHANNEL_1 = %d\n", lenCh1);
    fprintf(fp, "  SAMPLES_CHANNEL_2 = %d\n", lenCh2);
    fprintf(fp, "  SAMPLES_CHANNEL_3 = %d\n", lenCh3);
    fprintf(fp, "  SAMPLES_CHANNEL_4 = %d\n", lenCh4);

    // ptr = (uint16_t*)(buffer+sizeof(DAQHeader)+256*sizeof(uint16_t)+4); // ADC Data
    // Focus! I add a EventID in raw data, so the position of 4 byte should be added.
    // and i need to add eventID to raw data, because in that case i can know the law of    // package-loss during the message-sending process.
    ptr = (uint16_t*)(buffer+sizeof(DAQHeader)+256*sizeof(uint16_t)+2*sizeof(uint16_t));
    fprintf(fp, "  ADC_CHANNEL_1:\n");
    printChannelData(fp, lenCh1, ptr);
    ptr += lenCh1;
    fprintf(fp, "  ADC_CHANNEL_2:\n");
    printChannelData(fp, lenCh2, ptr);
    ptr += lenCh2;
    fprintf(fp, "  ADC_CHANNEL_3:\n");
    printChannelData(fp, lenCh3, ptr);
    ptr += lenCh3;
    fprintf(fp, "  ADC_CHANNEL_4:\n");
    printChannelData(fp, lenCh4, ptr);
    ptr += lenCh4;
}

int main(int argc, char* argv[])
{   
    FILE* fin=fopen(argv[1],"rb");
    FILE* fout=fopen(argv[2],"w");

    char *buffer = new char[2048*2048];
    char *tmp = new char[2048*2048];
    char tmpTag[4];
    size_t hitID = 0;
    uint32_t evtTag = 0;
    uint32_t sz = 0;
    uint32_t newSize = 0;
    
    std::map<size_t, std::map<size_t, size_t>> m_sort;
    std::map<size_t, std::map<size_t, size_t>>::iterator it; 
    std::map<size_t, size_t>::iterator it2;

    fseek(fin, 256, SEEK_SET);
    while(!feof(fin)) {
        size_t ret = fread(buffer, 1, sizeof(DAQHeader), fin);
        if(ret != sizeof(DAQHeader)) {
            break;
        }    
        DAQHeader *h = (DAQHeader*)buffer;
	    // std::cout << "type is " << h->type << std::endl;
        assert(h->type == DAQPCK_TYPE_DUEVENT);
        sz = h->size;
        uint16_t duID = h->source;
        newSize = sz;
        // std::cout << "sz is " << sz << std::endl;
        // std::cout << "duID is " << duID << std::endl;
        // std::cout << "new event sz is " << newSize << std::endl;
        // ret = fread(buffer+sizeof(DAQHeader), 1, newSize-sizeof(DAQHeader), fin);
        ret = fread(buffer+sizeof(DAQHeader), 1, sz-sizeof(DAQHeader), fin);
        memset(tmpTag, 0, 4);
        memcpy(tmpTag, buffer+sizeof(DAQHeader), sizeof(uint32_t));
        evtTag = atoi(tmpTag);
        // std::cout << "evtID is " << evtTag << std::endl;        
        m_sort[evtTag][duID] = hitID;
        // printf("hitID is %d\n",hitID);
        hitID++;
        if(ret != sz-sizeof(DAQHeader)) {
            break;
        }
    }
    
    for(it = m_sort.begin(); it != m_sort.end(); it++) {
        // std::cout << 222 << std::endl;
        fprintf(fout, "****** Event %lld\n ******", it->first);
        // std::cout << 333 << std::endl;
        for(it2 = it->second.begin(); it2 != it->second.end(); it2++) {
            fseek(fin, FILE_HEAD_SZ + it2->second*sz, SEEK_SET);
            size_t ret = fread(buffer, 1, sizeof(DAQHeader), fin);
            if(ret != sizeof(DAQHeader)) {
                break;
            }
            ret = fread(buffer + sizeof(DAQHeader), 1, sz - sizeof(DAQHeader), fin);
            if(ret != sz - sizeof(DAQHeader)) {
                break;
            }
            // printf("map size is %d\n", m_sort[it->first].size());
            // printf("eventID is %d, hitID is %d\n", it->first, it2->second);
            printData(fout, buffer, sz, it2->first, it2->second); // it2->first: duID, it2->second: hitID
        }
    }
    
    delete buffer;
    delete tmp;
    tmp = nullptr;
    buffer = nullptr;

    return(1);
}
