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
#define TARGET_RANGE 1024
#define RTT_RANGE 512
#define RTVAR_RANGE 256
#define STATE_RANGE 4 // TCP_CA_CWR cannot cover
#define FIND_EMPTY_LIMITE 5
#define NO_INC_LIMITE 5
#define Pearson_limite 0.5 // 99% confidence interval given 10000 times
#define TRIES_Interval 1
enum Input_type {speed = 0, loss, alpha, beta, shift, app_speed, Input_type_end};
enum Input_type_2 {access_speed, access_delay, bottneck_speed, bottneck_delay, router_queue_size, cross_traffic_size};
enum Output_type {cwnd = 0, ssth, srtt, rttvar, state, target, Output_type_end};

struct single_input_all_output
{
	enum Input_type inputType;
	double _cwnd;
	double _ssth;
	double _srtt;
	double _rttvar;
	double _state;
	double _target;

	void clear()
	{
		_cwnd = 0;
		_ssth = 0;
		_srtt = 0;
		_rttvar = 0;
		_state = 0;
		_target = 0;
	}

	double GetTypeValue(enum Output_type type)
	{
		switch (type)
		{
			case cwnd:
				return _cwnd;
			case ssth:
				return _ssth;
			case srtt:
				return _srtt;
			case rttvar:
				return _rttvar;
			case state:
				return _state;
			case target:
				return _target;
		}
		return 0;
	}
};

struct INPUT_ELEMENT
{
	enum Input_type input_type;
	int id; // for the other dimension
	template<class Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & input_type;
			ar & id;
		}

	bool operator==(const INPUT_ELEMENT& other) const
	{
		return (input_type == other.input_type && id == other.id);
	}

	bool operator<(const INPUT_ELEMENT& other) const
	{
		if (input_type < other.input_type)
			return true;
		else if (input_type > other.input_type)
			return false;

		if (id < other.id)
			return true;
		else if  (id > other.id)
			return false;

		return false;
	}
};

typedef map<pair<enum Input_type, enum Output_type>, int > INPUT_OUT_MAP;
typedef map<pair<enum Input_type_2, enum Output_type>, int > INPUT_OUT_MAP_2;

struct RANGE_INFO
{
	unsigned    int cwnd_range;
	unsigned    int ssth_range;
	unsigned    int rtt_range;
	unsigned    int rtvar_range;
	unsigned    int state_range;
	unsigned    int target_range;
	uint64_t    total;

	bool operator==(const RANGE_INFO& other) const
	{
		return (cwnd_range == other.cwnd_range && ssth_range == other.ssth_range && rtt_range == other.rtt_range && rtvar_range == other.rtvar_range && state_range == other.state_range && total == other.total && target_range == other.target_range);
	}

	template<class Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & cwnd_range;
			ar & ssth_range;
			ar & rtt_range;
			ar & rtvar_range;
			ar & state_range;
			ar & target_range;
			ar & total;
		}
};

struct Record_average
{
	double cwnd_aver;
	double ssth_aver;
	double rtt_aver;
	double rttvar_aver;
	double state_aver;
	double target_aver;

	template<class Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & cwnd_aver;
			ar & ssth_aver;
			ar & rtt_aver;
			ar & rttvar_aver;
			ar & state_aver;
			ar & target_aver;
		}

	bool operator==(const Record_average& other) const
	{
		return (cwnd_aver == other.cwnd_aver && ssth_aver == other.ssth_aver && rtt_aver == other.rtt_aver && rttvar_aver == other.rttvar_aver && state_aver == other.state_aver 
				&& target_aver == other.target_aver);
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
#define SPEED_UP 100000
#define SPEED_LOW 1

#define LOSS_UP 10
#define LOSS_LOW 0

#define ALPHA_UP 20
#define ALPHA_LOW 0

#define BETA_UP 80
#define BETA_LOW 0

#define SHIFT_UP 1000
#define SHIFT_LOW 0

#define APP_UP 100000000 // 100Gbps
#define APP_LOW 1 // unit kbps

struct Test_Parems_Limite
{
	vector<double> speed_inc_output_vec; //unit Mbps
	vector<double> speed_dec_output_vec; //unit Mbps

	vector<double> alpha_inc_output_vec;
	vector<double> alpha_dec_output_vec;

	vector<double> beta_inc_output_vec;
	vector<double> beta_dec_output_vec;

	vector<double> shift_inc_output_vec;
	vector<double> shift_dec_output_vec;

	vector<double> loss_rate_inc_output_vec;
	vector<double> loss_rate_dec_output_vec;

	vector<double> app_speed_inc_output_vec;
	vector<double> app_speed_dec_output_vec;
};

struct Test_Parems_2
{
	int access_speed; //unit Mbps
	int access_delay; //unit ms

	int bottneck_speed; //unit Mbps
	int bottneck_delay; //unit ms

	int router_queue_size; //unit % BDP
	int cross_traffic_size; //unit % Bottneck Traffic

	int rng_run; //used for seed
	int64_t curr_time;

	template<class Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & access_speed;
			ar & access_delay;
			ar & bottneck_speed;
			ar & bottneck_delay;
			ar & router_queue_size;
			ar & cross_traffic_size;
			ar & rng_run;
			ar & curr_time;
		}

	bool operator==(const Test_Parems_2& other) const
	{
		return (access_speed == other.access_speed && access_speed == other.access_speed &&
				bottneck_speed == other.bottneck_speed && bottneck_delay == other.bottneck_delay &&
				router_queue_size == other.router_queue_size && cross_traffic_size == other.cross_traffic_size
				&& rng_run == other.rng_run && curr_time == other.curr_time);
	}

	bool operator<(const Test_Parems_2& other) const
	{
		if (access_speed < other.access_speed)
			return true;
		else if (access_speed > other.access_speed)
			return false;

		if (access_delay < other.access_delay)
			return true;
		else if  (access_delay > other.access_delay)
			return false;

		if (bottneck_speed < other.bottneck_speed)
			return true;
		else if  (bottneck_speed > other.bottneck_speed)
			return false;

		if (bottneck_delay < other.bottneck_delay)
			return true;
		else if  (bottneck_delay > other.bottneck_delay)
			return false;

		if (router_queue_size < other.router_queue_size)
			return true;
		else if (router_queue_size > other.router_queue_size)
			return false;

		if (cross_traffic_size < other.cross_traffic_size)
			return true;
		else if (cross_traffic_size > other.cross_traffic_size)
			return false;

		if (rng_run < other.rng_run)
			return true;
		else if (rng_run > other.rng_run)
			return false;

		return false; //operator== is not used by std::set. Elements a and b are considered equal iff !(a < b) && !(b < a)
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

struct State_Record
{
	//int counter;//used for counter
	unsigned int cwnd;
	unsigned int ssthresh;
	unsigned int srtt;
	unsigned int rttvar;
	unsigned int tcp_state;
	unsigned int target;
	uint64_t curr_time;

	bool operator==(const State_Record& other) const
	{
		return (cwnd == other.cwnd && ssthresh == other.ssthresh && srtt == other.srtt && rttvar == other.rttvar && tcp_state == other.tcp_state && curr_time == other.curr_time 
			    && target == other.target);
	}

	void print()
	{
		cout << "cwnd:" << cwnd
			<< " ssth:" << ssthresh
			<< " srtt:" << srtt
			<< " rttvar:" << rttvar
			<< " ca_state:" << tcp_state
			<< " bic_target:" << target
			<< " switching_time:" << curr_time
			<< "\n";
	}

	template<class Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & cwnd;
			ar & ssthresh;
			ar & srtt;
			ar & rttvar;
			ar & tcp_state;
			ar & target;
			ar & curr_time;
		}

	State_Record()
	{
		cwnd = 0;
		ssthresh = 0;
		srtt = 0;
		rttvar = 0;
		tcp_state = 0;
		target = 0;
		curr_time = 0;
	}

	State_Record(unsigned int _cwnd, unsigned int _ssthresh, unsigned int _srtt, unsigned int _rttvar, unsigned int _tcp_state, unsigned int _target, int64_t _curr_time)
	{
		cwnd = _cwnd;
		ssthresh = _ssthresh;
		srtt = _srtt;
		rttvar = _rttvar;
		tcp_state = _tcp_state;
		curr_time = _curr_time;
		target = _target;
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

		if (target < other.target)
			return true;
		else if  (target > other.target)
			return false;

		if (curr_time < other.curr_time)
			return true;
		else if  (curr_time > other.curr_time)
			return false;

		return false; //operator== is not used by std::set. Elements a and b are considered equal iff !(a < b) && !(b < a)
	}
};

struct Test_Stack_elem
{
	struct State_Record upbound;
	struct State_Record lowbound;
	vector<struct State_Record> empty_vec;
	int prev_coverd_size;
	int limit_flag;
};

typedef set<struct State_Record> State_Space_Per;
typedef map<struct State_Record, int> State_Space_Map;
typedef map<int, vector<struct Test_Parems> > Config_Map;
typedef map<int, int> Coarse_Map;
typedef map<struct State_Record, Coarse_Map> Cube_State_Map;
typedef map<struct Test_Parems_2, vector<struct Test_Parems_2> > Coarse_Map_2;
typedef map<struct State_Record, Coarse_Map_2> Cube_State_Map_2;

struct Grans_coverage_map
{
	int granularity;
	Cube_State_Map coverage_map;
	struct RANGE_INFO range_info;

	bool operator==(const Grans_coverage_map& other) const
	{
		return (granularity == other.granularity && coverage_map == other.coverage_map && range_info == other.range_info);
	}

	template<class Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & granularity;
			ar & coverage_map;
			ar & range_info;
		}

};

typedef vector<Grans_coverage_map> COVG_MAP_VEC;

template< typename T >
class mycomparison2
{
	bool reverse;
	public:
	mycomparison2(const bool& revparam = true) //false, return largest
	{
		reverse = revparam;
	}

	bool operator() (const T lhs, const T rhs) const
	{
		if (reverse) return (lhs.config_id > rhs.config_id);
		else return (lhs.config_id < rhs.config_id);
	}
};

typedef struct State_Record PAIR_DISTANCE; // no need to be pair, use config_id to represent the distance
typedef pair<struct State_Record, struct State_Record> PAIR_STATE;
typedef vector<PAIR_DISTANCE> PAIR_DISTANCE_VEC;
typedef std::priority_queue <PAIR_DISTANCE, PAIR_DISTANCE_VEC, mycomparison2<PAIR_DISTANCE> > Normal_DISTANCE_PQ; //  default is less than comparison, return the largest one
typedef map<struct Normalize_Record, Normal_DISTANCE_PQ> Normalize_Map;

#endif
