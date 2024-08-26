#!/bin/bash
## To do communication tests, model: one to multiple points

#ssh root@192.168.61.10 "iperf3 -c 192.168.61.202 &" avg: 85.9 Mbps
#ssh root@192.168.61.13 "iperf3 -c 192.168.61.202 $" avg: 93.5 Mbps
#ssh root@192.168.61.17 "iperf3 -c 192.168.61.202 $" avg: 92.7 Mbps
#ssh root@192.168.61.19 "iperf3 -c 192.168.61.202 $" avg: 93.3 Mbps
#ssh root@192.168.61.20 "iperf3 -c 192.168.61.202 $" avg: 93.0 Mbps
#ssh root@192.168.61.29 "iperf3 -c 192.168.61.202 $" avg: 64.4 Mbps
#ssh root@192.168.61.31 "iperf3 -c 192.168.61.202 $" avg: 62.3 Mbps
#ssh root@192.168.61.32 "iperf3 -c 192.168.61.202 $" avg: 87.4 Mbps
#ssh root@192.168.61.41 "iperf3 -c 192.168.61.202 $" avg: 92.1 Mbps
#ssh root@192.168.61.71 "iperf3 -c 192.168.61.202 $" avg: 84.0 Mbps
#ssh root@192.168.61.75 "iperf3 -c 192.168.61.202 $" avg: 77.2 Mbps
#ssh root@192.168.61.85 "iperf3 -c 192.168.61.202 $" avg: 84.8 Mbps
#ssh root@192.168.61.xx "iperf3 -c 192.168.61.202 $"

#ssh root@192.168.61.10 "iperf3 -c 192.168.61.202 -t 20 -p 2001 &" 
#ssh root@192.168.61.13 "iperf3 -c 192.168.61.202 -t 20 -p 2002 &" 
#ssh root@192.168.61.17 "iperf3 -c 192.168.61.202 -t 20 -p 2003 &" 
#ssh root@192.168.61.19 "iperf3 -c 192.168.61.202 -t 20 -p 2004 &" 
#ssh root@192.168.61.20 "iperf3 -c 192.168.61.202 -t 20 -p 2005 &" 
#ssh root@192.168.61.29 "iperf3 -c 192.168.61.202 -t 20 -p 2006 &" 
#ssh root@192.168.61.31 "iperf3 -c 192.168.61.202 -t 20 -p 2007 &" 
#ssh root@192.168.61.32 "iperf3 -c 192.168.61.202 -t 20 -p 2008 &" 
#ssh root@192.168.61.41 "iperf3 -c 192.168.61.202 -t 20 -p 2009 &" 
#ssh root@192.168.61.71 "iperf3 -c 192.168.61.202 -t 20 -p 2010 &" 
#ssh root@192.168.61.75 "iperf3 -c 192.168.61.202 -t 20 -p 2011 &" 
#ssh root@192.168.61.85 "iperf3 -c 192.168.61.202 -t 20 -p 2012 &"
## UDP
#ssh root@192.168.61.10 "iperf3 -u -c 192.168.61.202 -b 9M -t 20 -p 2010 &"
#ssh root@192.168.61.13 "iperf3 -u -c 192.168.61.202 -b 9M -t 20 -p 2013 &"
## ssh root@192.168.61.17 "iperf3 -u -c 192.168.61.202 -b 9M -t 20 -p 2009 &"
#ssh root@192.168.61.19 "iperf3 -u -c 192.168.61.202 -b 9M -t 20 -p 2003 &"
#ssh root@192.168.61.31 "iperf3 -u -c 192.168.61.202 -b 9M -t 20 -p 2004 &"
#ssh root@192.168.61.32 "iperf3 -u -c 192.168.61.202 -b 9M -t 20 -p 2005 &"
#ssh root@192.168.61.71 "iperf3 -u -c 192.168.61.202 -b 9M -t 20 -p 2006 &"
#ssh root@192.168.61.72 "iperf3 -u -c 192.168.61.202 -b 9M -t 20 -p 2007 &"
#ssh root@192.168.61.75 "iperf3 -u -c 192.168.61.202 -b 9M -t 20 -p 2008 &"
## TCP/IP
# ssh root@192.168.61.10 "iperf3 -c 192.168.61.202 -t 60 -p 2010 &"
# ssh root@192.168.61.13 "iperf3 -c 192.168.61.202 -t 60 -p 2013 &"
# ssh root@192.168.61.17 "iperf3 -c 192.168.61.202 -t 60 -p 2017 &"
# ssh root@192.168.61.19 "iperf3 -c 192.168.61.202 -t 60 -p 2019 &"
# ssh root@192.168.61.20 "iperf3 -c 192.168.61.202 -t 60 -p 2020 &"
ssh root@192.168.61.29 "iperf3 -c 192.168.61.202 -t 60 -p 2029 &"
ssh root@192.168.61.31 "iperf3 -c 192.168.61.202 -t 60 -p 2031 &"
ssh root@192.168.61.32 "iperf3 -c 192.168.61.202 -t 60 -p 2032 &"
# ssh root@192.168.61.41 "iperf3 -c 192.168.61.202 -t 60 -p 2041 &"
# ssh root@192.168.61.71 "iperf3 -c 192.168.61.202 -t 60 -p 2071 &"
# ssh root@192.168.61.72 "iperf3 -c 192.168.61.202 -t 60 -p 2072 &"
# ssh root@192.168.61.75 "iperf3 -c 192.168.61.202 -t 60 -p 2075 &"
# ssh root@192.168.61.85 "iperf3 -c 192.168.61.202 -t 60 -p 2085 &"
