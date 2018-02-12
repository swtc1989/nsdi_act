#########################################################################
# File Name: dce.sh
# Author: wsun
# mail: sunweiflyus@gmail.com
# Created Time: Mon 17 Oct 2016 03:28:30 PM CDT
# Comments:this script is used to run ns3-dce, then record the output log traces for analysis use
#########################################################################
#!/bin/bash

if [ $# -eq 2 ]
then
	echo "run the ns3 script: $1"
else
	echo "invalid argument please pass six argument: binary, Rngrun(int), speed(int), loss_rate(double), alpha(double), beta(double), shift(double)"
	exit 1;
fi
cd ..
sudo rm -rf files-*;
sudo chmod 777 -R /tmp/output;
sudo ./waf  --run $1  --command-template="%s --RngRun=$2 " ||  exit $?;

sudo mkdir /tmp/output/$2 || echo "cannot mkdir output: $2"
sudo mv files-1/var/log/messages /tmp/output/$2 || echo "cannot mv messages for $2"
