/*************************************************************************
> File Name: share.h
int  > Author: Wei Sun
> Mail:sunweiflyus@gmail.com
> Created Time: Sat 26 Nov 2016 07:53:03 PM CST
> Comments:
************************************************************************/
#ifndef __SHARE_H__
#define __SHARE_H__
#include <utility>
#include <vector>
#include <queue>
#include <map>
#include <set>
#include "fstream"
#include "iostream"

using namespace std;
#define DEBUG 0
#define CWND_RANGE 1024
#define SSTH_RANGE 1024
#define RTT_RANGE 512
#define RTVAR_RANGE 256
#define STATE_RANGE 4 // TCP_CA_CWR cannot cover

typedef vector<struct State_Record> State_Record_Vector;

struct State_Record
{
	//int counter;//used for counter
	int flag; //0:ack, 1: data
	unsigned int cwnd;
	unsigned int ssthresh;
	unsigned int srtt;
	unsigned int rttvar;
	unsigned int tcp_state;
	uint64_t curr_time;

	bool operator==(const State_Record& other) const
	{
		return (cwnd == other.cwnd && ssthresh == other.ssthresh && srtt == other.srtt && rttvar == other.rttvar && tcp_state == other.tcp_state && curr_time == other.curr_time);
	}

	template<class Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & cwnd;
		ar & ssthresh;
		ar & srtt;
		ar & rttvar;
		ar & tcp_state;
		ar & curr_time;
	}

	State_Record()
	{
		curr_time = 0;
	}

	State_Record(int _flag, unsigned int _cwnd, unsigned int _ssthresh, unsigned int _srtt, unsigned int _rttvar, unsigned int _tcp_state, int64_t _curr_time)
	{
		flag = _flag ;
		cwnd = _cwnd;
		ssthresh = _ssthresh;
		srtt = _srtt;
		rttvar = _rttvar;
		tcp_state = _tcp_state;
		curr_time = _curr_time;
	}

	bool operator<(const State_Record& other) const
	{
		if (cwnd < other.cwnd)
			return true;
		else if (cwnd > other.cwnd)
			return false;

		if (ssthresh < other.ssthresh)
			return true;
		else if  (ssthresh > other.ssthresh)
			return false;

		if (srtt < other.srtt)
			return true;
		else if  (srtt > other.srtt)
			return false;

		if (rttvar < other.rttvar)
			return true;
		else if  (rttvar > other.rttvar)
			return false;

		if (tcp_state < other.tcp_state)
			return true;
		else if  (tcp_state > other.tcp_state)
			return false;

		if (curr_time < other.curr_time)
			return true;
		else if  (curr_time > other.curr_time)
			return false;

		return false; //operator== is not used by std::set. Elements a and b are considered equal iff !(a < b) && !(b < a)
	}
};
struct Shift_Gamma
{
	double alpha;
	double beta;
	double shift;

	template<class Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & alpha;
		ar & beta;
		ar & shift;
	}

	bool operator==(const Shift_Gamma& other) const
	{
		return (alpha == other.alpha && beta == other.beta && shift == other.shift);
	}
};

struct Test_Parems
{
	double speed; //unit Mbps
	struct Shift_Gamma sftgma;
	double Loss_rate;
	int app_speed;
	int rng_run; //used for seed
	int64_t curr_time;

	template<class Archive>
	void serialize(Archive &ar, const unsigned int version)
	{
		ar & speed;
		ar & sftgma;
		ar & Loss_rate;
		ar & app_speed;
		ar & rng_run;
		ar & curr_time;
	}
	Test_Parems()
	{
		curr_time = 0; // for feedback 1 it is always 0
	}

	void print()
	{
		cout << "speed:" << speed
		     << " alph:" << sftgma.alpha
		     << " beta:" << sftgma.beta
		     << " shift:" << sftgma.shift
		     << " loss:" << Loss_rate
		     << " app_speed:" << app_speed
		     << " seed:" << rng_run
		     << " time:" << curr_time
		     << "\n";
	}


	bool operator==(const Test_Parems& other) const
	{
		return (speed == other.speed && sftgma == other.sftgma && Loss_rate == other.Loss_rate && app_speed == other.app_speed
		        && rng_run == other.rng_run && curr_time == other.curr_time);
	}

	bool operator<(const Test_Parems& other) const
	{
		if (speed < other.speed)
			return true;
		else if (speed > other.speed)
			return false;

		if (sftgma.shift < other.sftgma.shift)
			return true;
		else if  (sftgma.shift > other.sftgma.shift)
			return false;

		if (sftgma.beta < other.sftgma.beta)
			return true;
		else if  (sftgma.beta > other.sftgma.beta)
			return false;

		if (sftgma.alpha < other.sftgma.alpha)
			return true;
		else if  (sftgma.alpha > other.sftgma.alpha)
			return false;

		if (app_speed < other.app_speed)
			return true;
		else if  (app_speed > other.app_speed)
			return false;

		if (Loss_rate < other.Loss_rate)
			return true;
		else if (Loss_rate > other.Loss_rate)
			return false;

		if (rng_run < other.rng_run)
			return true;
		else if (rng_run > other.rng_run)
			return false;

		return false; //operator== is not used by std::set. Elements a and b are considered equal iff !(a < b) && !(b < a)
	}

};

#endif
