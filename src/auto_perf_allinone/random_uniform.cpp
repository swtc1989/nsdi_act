/*************************************************************************
    > File Name: random.cpp
  > Author: wsun
  > Mail:sunweiflyus@gmail.com
  > Created Time: Sun 16 Oct 2016 02:49:42 PM CDT
  > Comments: You need to change the file if want to change the random parameter range
 ************************************************************************/
#include <iostream>
#include <sys/timeb.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <cmath>
#include "share.h"
#include <string>

static gsl_rng *r ; //for using gsl lib

// For random generator seed
unsigned long get_time()
{
	struct timeb timebuffer;
	ftime(&timebuffer);
	return timebuffer.time * 1000 + timebuffer.millitm;
}

void random_init()
{
	unsigned long tmpseed = get_time() + 1 + getpid();
	srand(tmpseed);
	r = gsl_rng_alloc(gsl_rng_mt19937);
	gsl_rng_set(r, tmpseed);
}

#define UNIFORM_RANGE 256
#define BYTE_RANGE 16
unsigned int random_range_zero(unsigned int range) // [0, range)
{
	unsigned int tmp = gsl_rng_uniform_int(r, range); // int truancated
	return tmp;
}

double random_range_double(void)
{
	double res = gsl_rng_uniform_pos(r) / 10;
	return res;
}

int random_cube_record(struct RANGE_INFO& range_info, struct State_Record& cube_state)
{
	cube_state.cwnd = random_range_zero(range_info.cwnd_range);
	cube_state.ssthresh = random_range_zero(range_info.ssth_range);
	cube_state.srtt = random_range_zero(range_info.rtt_range);
	cube_state.rttvar = random_range_zero(range_info.rtvar_range);
	cube_state.tcp_state = random_range_zero(range_info.state_range);
	return 0;
}

#define Speed_Last 1000000// uniform range[0, 1000000]
double Speed_type(int flag)
{
	double res = 1 + 0.099999 * flag;
	return res;// unit 0.1Mbps, from 1(0.1 Mbps) to 1000,00(10Gbps)// unit is 0.1 Mbps// please change ns data-rate.cc unit;
}

#define Loss_Last 1000000// uniform range[0, 1000000]
double Loss_type(int flag)
{
	double res = 0.00001 * flag ;
	return res ;// from 0 % to 10.0%
}

#define Alpha_Last 1000000// uniform range[0, 1000000]
double Alpha_type(int flag)
{
	double res = 0.00002 * flag;
	return res; //from 0 to 20
}

#define Beta_Last 1000000 // uniform range[0, 1000000]
double Beta_type(int flag)
{
	double res = 0.00008 * flag;
	return res;//from 0 to 80
}

#define Shift_Last 1000000// uniform range[0, 1000000]
double Shift_type(int flag)
{
	double res = 0.001 * flag;
	return res; // [0 - 1000] ms // one way delay, 512*4
}

#define App_Last 3// uniform range, last is 3, previous a bug
int App_speed_type(int flag)
{
	int res = 0;
	switch (flag)
	{
	case 0:
		res = 1;//1 kbps //unit is 1000 in dce-linux.cc file
		break;
	case 1:
		res = 1000;//1 Mbps
		break;
	case 2:
		res = 1000000; //1 Gbps
		break;
	case 3:
		res = 100000000; // 100 Gbps
		break;
	}
	return res;
}

#define Bottleneck_Speed_Last 99999// uniform range
int Bottneck_Speed_type(int flag)
{
	double res = flag + 1;
	return res; // unit Mbps, from 1(0.1 Mbps) to 1000,00(10Gbps)// unit is 0.1 Mbps// please change ns3 data-rate.cc unit if want to change unit;
}

#define Bottleneck_Delay_Last 999// uniform range
int Bottneck_Delay_type(int flag)
{
	double res = 1 + flag;
	return res; // [1 - 1000]/ms
}

#define Router_Queue_Last 190// uniform range
int Router_Queue_type(int flag)
{
	double res = 10 + flag;
	return res;  // [10, 200%]
}

#define Cross_Traffic_Last 90// uniform range
int Cross_Traffic_type(int flag)
{
	double res = 10 + flag;
	return res;  // [10, 90%]
}

int random_flag(int last)
{
	// All integers in the range [0,n-1] are produced with equal probability
	int tmp = gsl_rng_uniform_int(r, last + 1); // int truancated
	return tmp;
}

void random_input_value(struct Test_Parems& test_para)
{
	test_para.speed = Speed_type(random_flag(Speed_Last));
	test_para.Loss_rate = Loss_type(random_flag(Loss_Last));
	test_para.app_speed = App_speed_type(random_flag(App_Last));
	test_para.sftgma.alpha = Alpha_type(random_flag(Alpha_Last));
	test_para.sftgma.beta = Beta_type(random_flag(Beta_Last));
	test_para.sftgma.shift = Shift_type(random_flag(Shift_Last));
}

//previously used for topology 2: multi connections
void random_input_value_2(struct Test_Parems_2& test_para)
{
	test_para.bottneck_speed = Bottneck_Speed_type(random_flag(Bottleneck_Speed_Last));
	test_para.bottneck_delay = Bottneck_Delay_type(random_flag(Bottleneck_Delay_Last));
	test_para.router_queue_size = Router_Queue_type(random_flag(Router_Queue_Last));
	test_para.cross_traffic_size = Cross_Traffic_type(random_flag(Cross_Traffic_Last));
}
