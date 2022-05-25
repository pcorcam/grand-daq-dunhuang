#include <iostream>
#include <eformat.h>
#include <cassert>

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

void printData(FILE *fp, char *buffer, size_t size, uint32_t duID) {
    fprintf(fp, "DU_Data:\n");
    fprintf(fp, " DAQ_PACK_SIZE = %d\n", size);
    fprintf(fp, " DU_ID = %d\n", duID);
    fprintf(fp, " ELEC_DATA:\n");

    uint16_t *ptr = (uint16_t*)(buffer+sizeof(DAQHeader));
    fprintf(fp, "  LENGTH = %d\n", *(ptr++));
    fprintf(fp, "  EVENT_ID = %d\n", *(ptr++));
    fprintf(fp, "  HARDWARE_ID = %d\n", *(ptr++));

    ptr = (uint16_t*)(buffer+sizeof(DAQHeader)+30*sizeof(uint16_t)); // Samples in channel 1
    uint16_t lenCh1 = *(ptr++);
    uint16_t lenCh2 = *(ptr++);
    uint16_t lenCh3 = *(ptr++);
    uint16_t lenCh4 = *(ptr++);

    fprintf(fp, "  SAMPLES_CHANNEL_1 = %d\n", lenCh1);
    fprintf(fp, "  SAMPLES_CHANNEL_2 = %d\n", lenCh2);
    fprintf(fp, "  SAMPLES_CHANNEL_3 = %d\n", lenCh3);
    fprintf(fp, "  SAMPLES_CHANNEL_4 = %d\n", lenCh4);

    ptr = (uint16_t*)(buffer+sizeof(DAQHeader)+256*sizeof(uint16_t)); // ADC Data

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

    char buffer[1024*1024];

    fseek(fin, 256, SEEK_SET);
    while(!feof(fin)) {
        size_t ret = fread(buffer, 1, sizeof(DAQHeader), fin);
        if(ret != sizeof(DAQHeader)) {
            break;
        }

        DAQHeader *h = (DAQHeader*)buffer;
        assert(h->type == DAQPCK_TYPE_DUEVENT);
        uint32_t sz = h->size;
        uint32_t duID = h->source;

        ret = fread(buffer+sizeof(DAQHeader), 1, sz-sizeof(DAQHeader), fin);
        if(ret != sz-sizeof(DAQHeader)) {
            break;
        }
        
        printData(fout, buffer, sz, duID);
    }

    return(1);
}
