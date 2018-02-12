/*************************************************************************
    > File Name: granularity.cc
  > Author: wsun
  > Mail:sunweiflyus@gmail.com
  > Created Time: Mon 06 Feb 2017 10:13:46 PM CST
  > Comments:This file is used to for granularity
 ************************************************************************/
#ifndef __GRANULARITY_H__
#define __GRANULARITY_H__
#include "share.h"
#include <algorithm>
int cal_coverage_AllGrans (COVG_MAP_VEC& map_vec)
{
	cout << "map_vec.size:" << map_vec.size() << endl;
	for (int i = 0; i < map_vec.size(); i++)
	{
		double coverage = map_vec[i].coverage_map.size() * 1.0 /
		                  map_vec[i].range_info.total;
		cout << "I:" << i <<  ", Grans:" << map_vec[i].granularity << " ,covered size:" << map_vec[i].coverage_map.size() << " ,total:" << map_vec[i].range_info.total << ", coverage:" << coverage << endl;
	}

	return 0;
}

int state_granularity_mapping(struct State_Record& old_record, int granularity, struct State_Record& new_record)
{
	new_record.cwnd = (old_record.cwnd - 1) / granularity;
	new_record.ssthresh = (old_record.ssthresh - 1) / granularity;
	new_record.srtt = (old_record.srtt - 1) / granularity;
	new_record.rttvar = (old_record.rttvar - 1) / granularity;
	new_record.tcp_state = old_record.tcp_state ; // no grain for states
	return 0;
}

extern int GetId(enum Input_type input_type, struct Test_Parems tmp);

int insert_state(struct State_Record& tmp, COVG_MAP_VEC& map_vec, vector<struct Test_Parems>& test_para_vec, Config_Map& config_map)
{
	int granularity = 0;

	int id = GetId(Input_type_end, test_para_vec[0]);
	// To updae all coverage_map with different granularities

	if (test_para_vec.size() > 1 && test_para_vec[test_para_vec.size() - 2].curr_time >= tmp.curr_time)
	{
		return 0;
	}
	else
	{
		test_para_vec[test_para_vec.size() - 1].curr_time = tmp.curr_time; //To add new switching time for next new input
	}

	for (int i = 0; i < map_vec.size(); i++)
	{
		granularity = map_vec[i].granularity;
		int cwnd = (tmp.cwnd - 1) / granularity;
		int ssthresh = (tmp.ssthresh - 1) / granularity;
		int srtt = (tmp.srtt - 1) / granularity;
		int rttvar = (tmp.rttvar - 1) / granularity;
		int state = tmp.tcp_state ; // no grain for states
		struct State_Record tmp_cube(cwnd, ssthresh, srtt, rttvar, state, 0);
		Cube_State_Map::iterator it = map_vec[i].coverage_map.find(tmp_cube);

		if (it != map_vec[i].coverage_map.end())
		{
			Coarse_Map::iterator it_coarse = it->second.find(id);

			if (it_coarse == it->second.end())
			{
				int config_id = config_map.size();
				it->second.insert(make_pair(id, config_id));
				config_map[config_id] = test_para_vec;
			}
		}
		else // to create a new cube
		{
			Coarse_Map coarse_map;
			int config_id = config_map.size();
			coarse_map.insert(make_pair(id, config_id));
			config_map[config_id] = test_para_vec;
			map_vec[i].coverage_map.insert(make_pair(tmp_cube, coarse_map));
		}
	}
	return 0;
}

void print_pattern(Cube_State_Map& mymap)
{
	ofstream output_file("./output.txt");

	if (output_file)
	{
		for (Cube_State_Map::iterator map_it = mymap.begin(); map_it != mymap.end(); ++map_it)
		{
			output_file << map_it->first.cwnd
			            << " " << map_it->first.ssthresh
			            << " " << map_it->first.srtt
			            << " " << map_it->first.rttvar
			            << " " << map_it->first.tcp_state
			            << endl;
		}
	}
	else
	{
		cerr << "brain: cannot open output file  sucessfully, please check the sudo permission required" << endl;
		exit(-1);
	}

	return ;
}

typedef pair<State_Record, State_Space_Per> MAP_PAIR;
bool cmp_by_value(const MAP_PAIR & lhs, const MAP_PAIR & rhs)
{
	return lhs.second.size() < rhs.second.size();
}

int cal_range (struct RANGE_INFO & range_info, int granularity)
{
	range_info.cwnd_range = CWND_RANGE / granularity;
	range_info.ssth_range = SSTH_RANGE / granularity;
	range_info.rtt_range = granularity >= RTT_RANGE ? 1 : RTT_RANGE / granularity;
	range_info.rtvar_range = granularity >= RTVAR_RANGE ? 1 : RTVAR_RANGE / granularity;

	range_info.state_range = STATE_RANGE; // / (tcp_state_granularity); no grain for states
	range_info.total = range_info.cwnd_range * range_info.ssth_range * range_info.rtt_range * range_info.rtvar_range * range_info.state_range;
	return 0;
}

double cal_coverage (struct RANGE_INFO & range_info, Cube_State_Map & map)
{
	double coverage = map.size() * 1.0 / range_info.total;
	cout << "size:" << map.size() << ", total:" << range_info.total << ", coverage:" << coverage << endl;
	return coverage;

}

double cal_coverage_1d (int choice2, struct RANGE_INFO & range_info, Cube_State_Map & map)
{
	int total = 0;
	switch (choice2)
	{
	case 1:
		total = range_info.cwnd_range;
		break;
	case 2:
		total = range_info.ssth_range;
		break;
	case 3:
		total = range_info.rtt_range;
		break;
	case 4:
		total = range_info.rtvar_range;
		break;
	case 5:
		total = range_info.state_range;
		break;
	}

	double coverage = map.size() * 1.0 / total;
	cout << "covered size:" << map.size() << ", total:" << total << ", coverage:" << coverage << endl;

	return coverage;
}

double cal_coverage_2d (int choice, int choice2, struct RANGE_INFO& range_info, Cube_State_Map& map)
{
	int total = 0, total2 = 0;

	switch (choice)
	{
	case 1:
		total = range_info.cwnd_range;
		break;
	case 2:
		total = range_info.ssth_range;
		break;
	case 3:
		total = range_info.rtt_range;
		break;
	case 4:
		total = range_info.rtvar_range;
		break;
	case 5:
		total = range_info.state_range;
		break;
	}

	switch (choice2)
	{
	case 1:
		total2 = range_info.cwnd_range;
		break;
	case 2:
		total2 = range_info.ssth_range;
		break;
	case 3:
		total2 = range_info.rtt_range;
		break;
	case 4:
		total2 = range_info.rtvar_range;
		break;
	case 5:
		total2 = range_info.state_range;
		break;
	}

	double coverage = map.size() * 1.0 / (total * total2);
	cout << "covered size:" << map.size() << ", total:" << total*total2 << ", coverage:" << coverage << endl;

	return coverage;
}

#endif
