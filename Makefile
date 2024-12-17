INCLUDEs = -I/home/grand/workarea/l3OfflineTrigger/ -I/usr/include/c++/11/pstl -I/home/grand/pc_installed/include 
LIBRARYs = -L/home/grand/pc_installed/lib -lyaml-cpp -Wl,-rpath,/home/grand/pc_installed/lib
CXXFLAGS = -Wall -g $(INCLUDEs) 
LDFLAGS = $(LIBRARYs)

l3OfflineTrigger:data_format.o l3OfflineTrigger.o
	g++ data_format.o l3OfflineTrigger.o -o l3OfflineTrigger $(LDFLAGS)

data_format.o:data_format.cpp
	g++ $(CXXFLAGS) -c data_format.cpp -o data_format.o

l3OfflineTrigger.o:l3OfflineTrigger.cpp
	g++ $(CXXFLAGS) -c l3OfflineTrigger.cpp -o l3OfflineTrigger.o

.PHONY:
clean:
	rm data_format.o l3OfflineTrigger.o l3OfflineTrigger
