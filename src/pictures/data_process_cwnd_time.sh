#########################################################################
# File Name: data_process.sh
# Author: wsun
# mail: sunweiflyus@gmail.com
# Created Time: Fri 16 Dec 2016 12:14:54 PM CST
# Comments: This script is used to process messages file to generate data file for gnuplot
#########################################################################
#!/bin/bash
#sudo grep -Eo 'rtt:[0-9]+' client_message.txt | grep -Eo '[0-9]+' > rtt.txt

#sudo grep 'a,c' messages > tmp_messages
cp messages  tmp_messages
sudo grep -Eo ',c:[0-9]+' tmp_messages | grep -Eo '[0-9]+' > cwnd.txt
sudo grep -Eo ',u:[0-9]+' tmp_messages | grep -Eo '[0-9]+' > ts.txt
paste cwnd.txt ts.txt > cwnd_ts.txt
#sudo grep -Eo 'ssthresh:[0-9]+' client_message.txt | grep -Eo '[0-9]+' > ssth.txt
#sudo grep -Eo 'ca_state:[0-9]+' client_message.txt | grep -Eo '[0-9]+' > ca_state.txt
#sudo grep -Eo 'tcp_state:[0-9]+' client_message.txt | grep -Eo '[0-9]+' > tcp_state.txt
#sed -i 's/2147483647/100/g' ssth.txt
#grep -Eo '0:[0-9]+' output_delays.txt  > client_send_packet_delays.txt
#grep -Eo '1:[0-9]+' output_delays.txt  > server_send_packet_delays.txt
#grep -Eo '2:[0-9]+' output_delays.txt  > ack_arrival_packet_delays.txt
#gnuplot rtt.gpl 
#gnuplot state.gpl 
#gnuplot cwnd_onlyAck.gpl 
#gnuplot delays.gpl 
#mv client_message.txt client-side-perPacket-details.txt
#mv output_delays.txt network-side-perPacket-delays.txt

#rm *.gpl
#rm *.sh
