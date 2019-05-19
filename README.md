# Automated Testing Platform for TCP Congestion control (ACT)

# Overview
The implementation of ACT has four main modules: ns3, DCE, Linux kernel, and ase_brain.
For ns3-dce, system manipulate the packet delay in p2p channel according to given random
distribution. It takes as input an input parameter configuration (i.e., input.txt).

For Linux kernel, it is a inherently user-space library used by DCE for running a real kernel
stack. Some probing points are added here to print some target state variable in related kernel functions. Do not forget to copy a new bin_dce (cp liblinux.so ../ns-3-dce/build/bin_dce/)
every after a new compilation of kernel source.

For ns3-dce, it has customized testing scripts. You could configure different typologies, algorithms in the script. More scripts example could be referred in example folder.

For ase_brain, it automatically generates new test input parameter configuration for verb—ns3-
dce— simulation. It also processes and analyzes the log traces generate by the simulation to


# Installation
Following are all steps building the whole system, recall that the manual is build based on
ubuntu 14.04. We need to install customized ns3, Linux kernel stack, DCE, three modules.
Customized modules needed to be patched before installation.

Step 1. Get and install all updates and essentials

sudo apt-get update

sudo apt-get -y install build-essential

Step 2. Install required dependences for DCE-ASE

sudo apt-get install -y gcc g++ python python-dev mercurial bzr cmake 

unzip p7zip-full autoconf git cvs unrar-free libssl-dev flex bison pkg-config 

libdb-dev libgsl0ldbl gsl-bin libgsl0-dev

Step 3. Build a new directory for all system files, example uses ACT

cd ~/

mkdir ACT

cd ACT/

git clone https://github.com/swtc1989/nsdi_act.git

Step 4. Install customized ns3 (NS3 module)

cd ~/ACT

hg clone http://code.nsnam.org/ns-3.25

cd ns-3.25/

patch -p1 < ../act/patch/ns_3.25.patch

sudo ./waf configure --enable-examples -d optimized --prefix=$HOME/ACT/build 

--includedir=$HOME/ACT/include/ns-3.25

sudo ./waf

sudo ./waf install

cd ..

Step 5. Install customized kernel (kernel module)

cd ~/ACT

git clone https://github.com/thehajime/net-next-sim.git

cd net-next-sim

git checkout sim-ns3-3.10.0-branch

patch -p1 < ../act/patch/linux_kernel.patch

make defconfig OPT=no ARCH=sim

make library OPT=no ARCH=sim

cd ..

Step 6. Install ns3-dce (DCE modular)

cd ~/ACT

hg clone http://code.nsnam.org/ns-3-dce -r dce-1.8

cd ns-3-dce

patch -p1 < ../act/patch/dce.patch

./waf configure --with-ns3=$HOME/ACT/build --enable-opt

--enable-kernel-stack =$HOME/ACT/net-next-sim/arch --prefix=$HOME/ACT/build

sudo ./waf

sudo ./waf install

sudo cp ../act/src/ip build/bin_dce/

sudo cp ../net-next-sim/liblinux.so build/bin_dce/

cd ..

By now all components finish installed. Check out your main folder, there should
have four distinct folders: act, net-next-sim, ns-3.25, ns-3-dce.

Step 7. Run system for a test!
System generates binary allinone_main under ACT/ns-3-dce/build the output file log.txt 
is under the same directory. Outputs from ns3 are under /tmp/output.
Attention: output files will be overwritten every time!

cd ~/ACT/act/src/auto_perf_allinone/

make main

cd ~/ACT/ns-3-dce/build

sudo ./allinone_main
