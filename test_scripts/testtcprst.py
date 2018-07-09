#!/usr/bin/python
from scapy.all import *
import time

my_sbase_port=1602
my_d_port=12345
my_s_ip="192.168.23.2"
my_d_ip="192.168.23.168"
cycles=100
secs_to_sleep=1

for i in range(1,cycles):
 print("Sending fake connection #")
 my_s_port = my_sbase_port + i 
 print("\t using port " + str(my_s_port))
 ip=IP(src=my_s_ip, dst=my_d_ip)
 SYN=TCP(sport=my_s_port, dport=my_d_port, flags="S", seq=100)
 SYNACK=sr1(ip/SYN)

 my_ack = SYNACK.seq + 1
 ACK=TCP(sport=my_s_port, dport=my_d_port, flags="A", seq=101, ack=my_ack)
 send(ip/ACK)

#payload="stuff"
#PUSH=TCP(sport=mysport, dport=12345, flags="PA", seq=11, ack=my_ack)
#send(ip/PUSH/payload)
 RESET=TCP(sport=my_s_port, dport=my_d_port, flags="RA", seq=101, ack=my_ack)
 send(ip/RESET)

 if (i%2 == 0):
   time.sleep(secs_to_sleep+5)
 else:
   time.sleep(secs_to_sleep)
