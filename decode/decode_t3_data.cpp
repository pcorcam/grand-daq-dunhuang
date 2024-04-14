// ************ //
// Created by duanbh
// 2022.11.29
// duanbh@pmo.ac.cn
// ************ //

#include <iostream>
#include <eformat.h>
#include <cassert>
#include <map>
#include <string.h>

#define NANO_CHANGE 100000 //use this macro to change nanosec
#define FILE_HEAD_SZ 256
#define newDataAdded 12 // one uint32_t is 4 bytes

using namespace grand;

struct SaveHitID
{
    int sameEventCount = 0;
    uint32_t* buf;
};

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

void printData(FILE *fp, char *buffer, size_t size, uint16_t duID, size_t hitIdInOrder) {
    char Event_ID_tag[4];
    char TriggerNumbers[4];
    char HitId[4];
    fprintf(fp, "DU_Data:\n");
    fprintf(fp, " DAQ_PACK_SIZE = %lld\n", size);
    fprintf(fp, " DU_ID = %d\n", duID);
    
    uint16_t *ptr = (uint16_t*)(buffer+sizeof(DAQHeader));
    memcpy(Event_ID_tag, (char*)(ptr++),4);
    fprintf(fp, " Event_ID_Tag = %d\n", *(uint32_t*)(Event_ID_tag));
    *(ptr++);

    memcpy(TriggerNumbers, (char*)ptr++, 4);
    fprintf(fp, " Number of trigger dus is %d\n", atoi(TriggerNumbers));
    *(ptr++);

    fprintf(fp, " HitId in DU:%d is %d\n", duID, *(ptr++));
    *(ptr++);
    fprintf(fp, "- ELEC_RAW_DATA:\n"); 
    fprintf(fp, "  LENGTH = %d\n", *(ptr++));
    fprintf(fp, "  Evnet ID = %d\n", *(ptr++));
    fprintf(fp, "  HARDWARE_ID = %d\n", *(ptr++));
    fprintf(fp, "  Header length = %d\n", *(ptr++));
    fprintf(fp, "  Seconds (lsb) = %d\n", *(ptr++));
    fprintf(fp, "  Seconds (msb) = %d\n", *(ptr++));
    fprintf(fp, "  Nanoseconds (lsb) = %d\n", ((uint32_t)(*(ptr++))));
    fprintf(fp, "  Nanoseconds (msb) = %d\n", *(ptr++));
    fprintf(fp, "  Trigger position = %d\n", *(ptr++));

    for(int i=0; i<8; i++) {
        if( i == 0 ) 
            fprintf(fp, "  spare: ");
        fprintf(fp, "%d = %d ", i, *(ptr++));
        if( i==3) 
            fprintf(fp, "\n");
            fprintf(fp, "         ");
        if (i==7)
        {
            fprintf(fp, "\n");
        }
    }

    fprintf(fp, "  Atmospheric: \n");
    for(int i=0; i<3; i++) {
        if(i==0) fprintf(fp, "    temperature is %d,", *(ptr++));
        if(i==1) fprintf(fp," pressure = %d,", *(ptr++));
        if(i==2) fprintf(fp," humidity = %d\n", *(ptr++));
    }

    fprintf(fp, "  Accelerometer: X , Y , Z: ");
    for(int i=0; i<3; i++) {
        fprintf(fp, "%d ", *(ptr++));
        if(i==2)
            fprintf(fp,"\n");
    }

    fprintf(fp, "  Battery voltage = %d\n", *(ptr++));
    fprintf(fp, "  Format Version = %d\n", *(ptr++));
    fprintf(fp, "  ADC sample frequency = %d\n", *(ptr++));
    fprintf(fp, "  ADC resolution = %d\n", *(ptr++));
    fprintf(fp, "  Input selection = %d\n", *(ptr++));
    fprintf(fp, "  DAQ Channel Enable = %d\n", *(ptr++));
    fprintf(fp, "  Total number of samples = %d\n", 16*(*(ptr++)));
    
    fprintf(fp, "  Samples in channel: \n");
    for(int i=0; i<4; i++) {
        fprintf(fp, "   ");
        fprintf(fp, " Channel_%d = %d,", i, *(ptr++));
        if(i==3) fprintf(fp, "\n");
    }

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
    fprintf(fp, "  GPS Temperature (lsb) = %g\n", *(float *)(ptr++));
    fprintf(fp, "  GPS Temperature (msb) = %d\n", *(ptr++));
    
    fprintf(fp, "  control parameters: \n");
    for(int i=0; i<8; i++) {
        if(i==0 || i==4) fprintf(fp, "    ");
        fprintf(fp, "%d ", *(ptr++));
        if(i==3 || i==7) fprintf(fp,"\n");
    }

    fprintf(fp, "  window parameters: \n");
    for(int i=0; i<8; i++) {
        if(i==0 || i==4) fprintf(fp, "    ");
        fprintf(fp, "%d ", *(ptr++));
        if(i==3 || i==7) fprintf(fp,"\n");
    }

    fprintf(fp, "  channel property parameters: \n");
    for(int i=0; i<24; i++) {
        if(i==0 || i==6 || i==12 || i==18) fprintf(fp, "    ");
        fprintf(fp, "%d ", *(ptr++));
        if(i==5 || i==11 || i==17 || i==23) fprintf(fp,"\n");
    }

    fprintf(fp, "  trigger parameters: \n");
    for(int i=0; i<24; i++) {
        if(i==0 || i==6 || i==12 || i==18) fprintf(fp, "    ");
        fprintf(fp, "%d ", *(ptr++));
        if(i==5 || i==11 || i==17 || i==23) fprintf(fp,"\n");
    }
    
    for (int m = 1; m < 5; m++)
    {
        fprintf(fp, "  filter parameters filter %d:\n", m);
        for(int i=1; i<5; i++) {
            fprintf(fp, "    Channel %d: ", i);
            for(int j=0; j<8; j++) {
                fprintf(fp, "%d ", *(ptr++));
                if(j==7) fprintf(fp, "\n");
            }
        }
    }
    
    // ptr = (uint16_t*)(buffer+sizeof(DAQHeader)+30*sizeof(uint16_t)+4); // Samples in channel 1
    // Focus! I add Event_ID_tag and trigger dus number before raw data, so the position of "newDataAdded" byte should be added.
    ptr = (uint16_t*)(buffer+sizeof(DAQHeader)+30*sizeof(uint16_t)+newDataAdded);
    uint16_t lenCh1 = *(ptr++);
    uint16_t lenCh2 = *(ptr++);
    uint16_t lenCh3 = *(ptr++);
    uint16_t lenCh4 = *(ptr++);

    fprintf(fp, "  SAMPLES_CHANNEL_1 = %d\n", lenCh1);
    fprintf(fp, "  SAMPLES_CHANNEL_2 = %d\n", lenCh2);
    fprintf(fp, "  SAMPLES_CHANNEL_3 = %d\n", lenCh3);
    fprintf(fp, "  SAMPLES_CHANNEL_4 = %d\n", lenCh4);

    // Focus! I add Event_ID_tag and trigger dus number before raw data, so the position of "newDataAdded" byte should be added.
    // and i need to add eventID to raw data, because in that case i can know the law of    // package-loss during the message-sending process.
    ptr = (uint16_t*)(buffer+sizeof(DAQHeader)+256*sizeof(uint16_t)+newDataAdded);
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
    char tmpTag[100];
    char tagStack[1024] = {};
    size_t TraceOrder = 0;
    uint32_t evtTagOld = 0;
    uint32_t evtTag = 0;
    uint32_t sz = 0;
    uint32_t newSize = 0;

    std::map<size_t, std::map<uint16_t, SaveHitID>> TraceOrderstack;
    std::map<size_t, std::map<uint16_t, SaveHitID>>::iterator it; 
    std::map<uint16_t, SaveHitID>::iterator it2;

    fseek(fin, 256, SEEK_SET);
    while(!feof(fin)) {
        size_t ret = fread(buffer, 1, sizeof(DAQHeader), fin);
        if(ret != sizeof(DAQHeader)) {
            break;
        }   

        DAQHeader *h = (DAQHeader*)buffer;
        assert(h->type == DAQPCK_TYPE_DUEVENT);
        sz = h->size;
        uint16_t duID = h->source;
        ret = fread(buffer+sizeof(DAQHeader), 1, sz-sizeof(DAQHeader), fin);
        memset(tmpTag, 0, 100);
        memcpy(tmpTag, buffer+sizeof(DAQHeader), sizeof(uint32_t));
        evtTag = *(uint32_t*)(tmpTag);
        if(!TraceOrderstack[evtTag].count(duID))
            TraceOrderstack[evtTag][duID].buf = new uint32_t[100];
        TraceOrderstack[evtTag][duID].buf[TraceOrderstack[evtTag][duID].sameEventCount] = TraceOrder;
        TraceOrderstack[evtTag][duID].sameEventCount++;  
        // printf("TraceOrder is %d\n", TraceOrder);
        TraceOrder++;
        if(ret != sz-sizeof(DAQHeader)) {
            break;
        }
    }
    
    for(it = TraceOrderstack.begin(); it != TraceOrderstack.end(); it++) {
        fprintf(fout, "****** Event %lld ******\n", it->first);
        for(it2 = it->second.begin(); it2 != it->second.end(); it2++) {
            for(int i=0; i<it2->second.sameEventCount; i++) {
                fseek(fin, FILE_HEAD_SZ + it2->second.buf[i]*sz, SEEK_SET); // TraceOrder*sz
                size_t ret = fread(buffer, 1, sizeof(DAQHeader), fin);
                if(ret != sizeof(DAQHeader)) {
                    break;
                }
                ret = fread(buffer + sizeof(DAQHeader), 1, sz - sizeof(DAQHeader), fin);
                if(ret != sz - sizeof(DAQHeader)) {
                    break;
                }
                // printf("it->first is %d, it2->first is %d\n", it->first, it2->first);
                printData(fout, buffer, sz, it2->first, it2->second.buf[i]);
            }
            delete TraceOrderstack[it->first][it2->first].buf; // it2->first: duID, it2->second: TraceOrder
        }
    }
    
    delete buffer;
    delete tmp;
    tmp = nullptr;
    buffer = nullptr;

    return(1);
}
