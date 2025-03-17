#include <iostream>
#include <eformat.h>
#include <cassert>
#include <string.h>

#define NANO_CHANGE 100000 //use this macro to change nanosec
#define newDataAdded 4 // one uint32_t is 4 bytes

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

void printData(FILE *fp, char *buffer, size_t size, uint16_t duID, size_t hitIdInOrder) {
    char HitId[4];

    fprintf(fp, "DU_Data:\n");
    fprintf(fp, " DAQ_PACK_SIZE = %lld\n", size);
    fprintf(fp, " DU_ID = %d\n", duID);
    
    // fprintf(fp, " HItID: %d\n", eventID);

    uint16_t *ptr = (uint16_t*)(buffer+sizeof(DAQHeader));
    uint32_t *ptr2 = (uint32_t*)(buffer+sizeof(DAQHeader));
    fprintf(fp, " HitId in DU:%d is %lld\n", duID, *(ptr2++));
    *(ptr2--);
    *(ptr++);
    *(ptr++);
    fprintf(fp, " ELEC_DATA:\n");
    fprintf(fp, "  LENGTH = %d\n", *(ptr++));
    fprintf(fp, "  EventID = %d\n", *(ptr++));
    fprintf(fp, "  HARDWARE_ID = %d\n", *(ptr++));
    fprintf(fp, "  Header length = %d\n", *(ptr++));
    fprintf(fp, "  Seconds = %u\n", *(ptr2+3));
    fprintf(fp, "  Seconds (lsb) = %d\n", *(ptr++));
    fprintf(fp, "  Seconds (msb) = %d\n", *(ptr++));
    fprintf(fp, "  NanoSeconds = %u\n", *(ptr2+4));
    fprintf(fp, "  Nanoseconds (lsb) = %d\n", ((uint32_t)(*(ptr++))));
    fprintf(fp, "  Nanoseconds (msb) = %d\n", *(ptr++));
    fprintf(fp, "  Trigger position = %d\n", *(ptr++));
    fprintf(fp, "  spare 0 = %d\n", *(ptr++));
    fprintf(fp, "  spare 1 = %lld\n", *(ptr++));
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
    fprintf(fp, "  GPS Longitude (lsb) (stamp by last PPS) = %g\n", *(double*)(ptr++));
    fprintf(fp, "  GPS Longitude = %g\n", *(double*)(ptr++));
    fprintf(fp, "  GPS Longitude = %g\n", *(double*)(ptr++));
    fprintf(fp, "  GPS Longitude (msb) = %g\n", *(double*)(ptr++));
    fprintf(fp, "  GPS Latitude (lsb) = %g\n", *(float*)(ptr++));
    fprintf(fp, "  GPS Latitude = %g\n", *(float*)(ptr++));
    fprintf(fp, "  GPS Latitude = %g\n", *(float*)(ptr++));
    fprintf(fp, "  GPS Latitude (msb) = %d\n", *(ptr++));
    fprintf(fp, "  GPS Altitude (lsb) = %d\n", *(ptr++));
    fprintf(fp, "  GPS Altitude = %d\n", *(ptr++));
    fprintf(fp, "  GPS Altitude = %d\n", *(ptr++));
    fprintf(fp, "  GPS Altitude (msb) = %d\n", *(ptr++));
    fprintf(fp, "  GPS Temperature (lsb) = %g\n", *(float *)(ptr++));
    fprintf(fp, "  GPS Temperature (msb) = %d\n", *(ptr++));
    fprintf(fp, "  control parameters = %d\n", *(ptr++));
    fprintf(fp, "  control parameters = %d\n", *(ptr++));
    fprintf(fp, "  control parameters = %d\n", *(ptr++));
    fprintf(fp, "  control parameters = %d\n", *(ptr++));
    fprintf(fp, "  control parameters = %d\n", *(ptr++));
    fprintf(fp, "  control parameters = %d\n", *(ptr++));
    fprintf(fp, "  control parameters = %d\n", *(ptr++));
    fprintf(fp, "  control parameters = %d\n", *(ptr++));
    fprintf(fp, "  window paras(prevous readout) = %d\n", *(ptr++));
    fprintf(fp, "  window paras(post readout) = %d\n", *(ptr++));
    fprintf(fp, "  window paras(prevous readout) = %d\n", *(ptr++));
    fprintf(fp, "  window paras(post readout) = %d\n", *(ptr++));
    fprintf(fp, "  window paras(prevous readout) = %d\n", *(ptr++));
    fprintf(fp, "  window paras(post readout) = %d\n", *(ptr++));
    fprintf(fp, "  window paras(prevous readout) = %d\n", *(ptr++));
    fprintf(fp, "  window paras(post readout) = %d\n", *(ptr++));
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
    fprintf(fp, "  trigger paras(CH1: Sig Thresh T1) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH1: Noise Thresh T1) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH1: Tper, Tpre) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH1: NCmax, TCmax) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH1: Qmax, NCmin) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH1: spare, Qmin) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH2: Sig Thresh T1) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH2: Noise Thresh T1) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH2: Tper, Tpre) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH2: NCmax, TCmax) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH2: Qmax, NCmin) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH2: spare, Qmin) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH3: Sig Thresh T1) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH3: Noise Thresh T1) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH3: Tper, Tpre) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH3: NCmax, TCmax) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH3: Qmax, NCmin) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH3: spare, Qmin) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH4: Sig Thresh T1) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH4: Noise Thresh T1) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH4: Tper, Tpre) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH4: NCmax, TCmax) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH4: Qmax, NCmin) = %d\n", *(ptr++));
    fprintf(fp, "  trigger paras(CH4: spare, Qmin) = %d\n", *(ptr++));
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
    ptr = (uint16_t*)(buffer+sizeof(DAQHeader)+30*sizeof(uint16_t)+newDataAdded);
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
    fseek(fin, 256, SEEK_SET);
    
    size_t hitIdInOrder = 0;

    while(!feof(fin)) {
        size_t ret = fread(buffer, 1, sizeof(DAQHeader), fin);
        if(ret != sizeof(DAQHeader)) {
            break;
        }
        hitIdInOrder++;
        DAQHeader *h = (DAQHeader*)buffer;
        assert(h->type == DAQPCK_TYPE_DUEVENT);
        uint32_t sz = h->size;
        uint16_t duID = h->source;
        ret = fread(buffer+sizeof(DAQHeader), 1, sz-sizeof(DAQHeader), fin);
        if(ret != sz-sizeof(DAQHeader)) {
            break;
        }
        
        // uint32_t newSize = sz + sizeof(uint32_t);
        printData(fout, buffer, sz, duID, hitIdInOrder);
    }
    
    delete buffer;
    buffer = nullptr;
    return(1);
}
