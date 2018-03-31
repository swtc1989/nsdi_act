/*************************************************************************
    > File Name: feedback.cc
  > Author: wsun
  > Mail:sunweiflyus@gmail.com
  > Created Time: Sat 15 Apr 2017 11:16:54 PM CDT
  > Comments:
 ************************************************************************/
#include "share.h"
extern int execute_ns3_2(int mode);
extern int total_execution;
extern void prepare_before_config_vec(vector<struct Test_Parems>& vec_test_para);
extern int granularity;
extern int read_from_file(char* filename1, COVG_MAP_VEC& trace, Config_Map& map_config,  vector<struct Test_Parems>& test_para_vec);

int get_app_speed(int app_speed)
{
	int tmp_id;

	if (app_speed >= 1 && app_speed < 1000) // for double
	{
		tmp_id  = 1;
	}
	else if (app_speed >= 1000 && app_speed < 1000000)
	{
		tmp_id  = 2;
	}
	else if (app_speed >= 1000000 && app_speed < 100000000) // for double
	{
		tmp_id  = 3;
	}
	else if (app_speed <= 100000000) // for double
	{
		tmp_id  = 4;
	}
	else
	{
		cout << "app_speed:" << app_speed << "error report, app_speed \n";
	}
	return tmp_id;
}

int get_App_speed_type(int flag)
{
	int res = 0;
	switch (flag)
	{
	case 1:
		res = 1;//1 kbps //unit is 1000 in dce-linux.cc file
		break;
	case 2:
		res = 1000;//1 Mbps
		break;
	case 3:
		res = 1000000; //1 Gbps
		break;
	case 4:
		res = 100000000; // 100 Gbps
		break;
	default:
		cout << "app_flag:" << flag << "error report, app_flag \n";
	}
	return res;
}


int find_empty_area_N(State_Record& empty_state, struct Grans_coverage_map& tmp_map)
{
	Cube_State_Map::iterator it ;
	int counter = FIND_EMPTY_LIMITE;
	struct State_Record tmp(0, 0, 0, 0, 0, 0);
	while (counter > 0)
	{
		tmp.cwnd = random_range_zero(tmp_map.range_info.cwnd_range) + 1;// 0 - 1023 in coverage map
		tmp.ssthresh = random_range_zero(tmp_map.range_info.ssth_range) + 1;
		tmp.srtt = random_range_zero(tmp_map.range_info.rtt_range) + 1;
		tmp.rttvar = random_range_zero(tmp_map.range_info.rtvar_range) + 1;
		tmp.tcp_state = random_range_zero(tmp_map.range_info.state_range);//0, 1, 2, 3

		// here empty point needs a mapping operation to be searched in coverage map; as the mapping is done when inserting point into coverage map;

		struct State_Record coverage_tmp(0, 0, 0, 0, 0, 0);
		state_granularity_mapping(tmp, tmp_map.granularity, coverage_tmp); // here granularity 1

		it = tmp_map.coverage_map.find(coverage_tmp);
		if (it == tmp_map.coverage_map.end()) //Got an empty vector
		{
			empty_state = tmp;
			if (DEBUG)
			{
				cout << "Find empty cube:";
				empty_state.print();
			}
			return 0;
		}
		counter--;
	}

	if (DEBUG)
	{
		cout << "cannot find empty cube\n";
	}
	return -1; // its saturated
}

int get_input_output_relation(Test_Parems_Limite& test_limit, Output_type output, INPUT_OUT_MAP& input_output_map)
{
	pair<enum Input_type, enum Output_type> pair_enum ;

	pair_enum = make_pair(speed, output);
	INPUT_OUT_MAP::iterator it = input_output_map.find(pair_enum);
	if (it == input_output_map.end())
	{
		cout << "Error Speed no correleation found !!!\n";
		//exit(-1);
	}

	if ( it-> second == 1)
	{
		test_limit.speed_inc_output_vec.push_back(SPEED_UP);
		test_limit.speed_dec_output_vec.push_back(SPEED_LOW);
	}
	else if (it->second == -1)
	{
		test_limit.speed_inc_output_vec.push_back(SPEED_LOW);
		test_limit.speed_dec_output_vec.push_back(SPEED_UP);
	}
	else
	{
		test_limit.speed_inc_output_vec.push_back(SPEED_UP);
		test_limit.speed_inc_output_vec.push_back(SPEED_LOW);
		test_limit.speed_dec_output_vec.push_back(SPEED_UP);
		test_limit.speed_dec_output_vec.push_back(SPEED_LOW);
	}

	pair_enum = make_pair(loss, output);
	it = input_output_map.find(pair_enum);
	if (it == input_output_map.end())
	{
		cout << "Error Loss no correleation found !!!\n";
		exit(-1);
	}
	if ( it-> second == 1)
	{
		test_limit.loss_rate_inc_output_vec.push_back(LOSS_UP);
		test_limit.loss_rate_dec_output_vec.push_back(LOSS_LOW);
	}
	else if (it->second == -1)
	{
		test_limit.loss_rate_inc_output_vec.push_back(LOSS_LOW);
		test_limit.loss_rate_dec_output_vec.push_back(LOSS_UP);
	}
	else
	{
		test_limit.loss_rate_inc_output_vec.push_back(LOSS_UP);
		test_limit.loss_rate_inc_output_vec.push_back(LOSS_LOW);
		test_limit.loss_rate_dec_output_vec.push_back(LOSS_UP);
		test_limit.loss_rate_dec_output_vec.push_back(LOSS_LOW);
	}

	pair_enum = make_pair(alpha, output);
	it = input_output_map.find(pair_enum);

	if (it == input_output_map.end())
	{
		cout << "Error alpha no correleation found !!!\n";
		exit(-1);
	}

	if ( it-> second == 1)
	{
		test_limit.alpha_inc_output_vec.push_back(ALPHA_UP);
		test_limit.alpha_dec_output_vec.push_back(ALPHA_LOW);
	}
	else if (it->second == -1)
	{
		test_limit.alpha_inc_output_vec.push_back(ALPHA_LOW);
		test_limit.alpha_dec_output_vec.push_back(ALPHA_UP);
	}
	else
	{
		test_limit.alpha_inc_output_vec.push_back(ALPHA_UP);
		test_limit.alpha_inc_output_vec.push_back(ALPHA_LOW);
		test_limit.alpha_dec_output_vec.push_back(ALPHA_UP);
		test_limit.alpha_dec_output_vec.push_back(ALPHA_LOW);
	}

	pair_enum = make_pair(beta, output);
	it = input_output_map.find(pair_enum);

	if (it == input_output_map.end())
	{
		cout << "Error beta no correleation found !!!\n";
		//exit(-1);
	}

	if ( it-> second == 1)
	{
		test_limit.beta_inc_output_vec.push_back(BETA_UP);
		test_limit.beta_dec_output_vec.push_back(BETA_LOW);
	}
	else if (it->second == -1)
	{
		test_limit.beta_inc_output_vec.push_back(BETA_LOW);
		test_limit.beta_dec_output_vec.push_back(BETA_UP);
	}
	else
	{
		test_limit.beta_inc_output_vec.push_back(BETA_UP);
		test_limit.beta_inc_output_vec.push_back(BETA_LOW);
		test_limit.beta_dec_output_vec.push_back(BETA_UP);
		test_limit.beta_dec_output_vec.push_back(BETA_LOW);
	}

	pair_enum = make_pair(shift, output);
	it = input_output_map.find(pair_enum);

	if (it == input_output_map.end())
	{
		cout << "Error shift no correleation found !!!\n";
		//exit(-1);
	}

	if ( it-> second == 1)
	{
		test_limit.shift_inc_output_vec.push_back(SHIFT_UP);
		test_limit.shift_dec_output_vec.push_back(SHIFT_LOW);
	}
	else if (it->second == -1)
	{
		test_limit.shift_inc_output_vec.push_back(SHIFT_LOW);
		test_limit.shift_dec_output_vec.push_back(SHIFT_UP);
	}
	else
	{
		test_limit.shift_inc_output_vec.push_back(SHIFT_UP);
		test_limit.shift_inc_output_vec.push_back(SHIFT_LOW);
		test_limit.shift_dec_output_vec.push_back(SHIFT_UP);
		test_limit.shift_dec_output_vec.push_back(SHIFT_LOW);
	}

	pair_enum = make_pair(app_speed, output);
	it = input_output_map.find(pair_enum);

	if (it == input_output_map.end())
	{
		cout << "Error app_speed no correleation found !!!\n";
		//exit(-1);
	}

	if ( it-> second == 1)
	{
		test_limit.app_speed_inc_output_vec.push_back(APP_UP);
		test_limit.app_speed_dec_output_vec.push_back(APP_LOW);
	}
	else if (it->second == -1)
	{
		test_limit.app_speed_inc_output_vec.push_back(APP_LOW);
		test_limit.app_speed_dec_output_vec.push_back(APP_UP);
	}
	else
	{
		test_limit.app_speed_inc_output_vec.push_back(APP_UP);
		test_limit.app_speed_inc_output_vec.push_back(APP_LOW);
		test_limit.app_speed_dec_output_vec.push_back(APP_UP);
		test_limit.app_speed_dec_output_vec.push_back(APP_LOW);
	}
	return -1;
}


int generate_new_test_para_vec_1D(int feedback_mode, Output_type output, struct State_Record original_empty_set, COVG_MAP_VEC& covg_map_vec, Config_Map& map_config, vector<vector<struct Test_Parems> > & new_test_para_vec, INPUT_OUT_MAP& input_output_map)
{
	if (DEBUG)
	{
		cout << "Output_type:" << output << endl;
	}
	int index = 0, uprange = 0 , lowrange = -1;
	int i = 0;
	Cube_State_Map::iterator it ;
	struct State_Record empty_set(0, 0, 0, 0, 0, 0);
	struct State_Record empty_set_record(0, 0, 0, 0, 0, 0);//to remember the mapping empty set given granularity
	bool upbound_limit = false;
	bool lowbound_limit = false;
	State_Record low_state, up_state;

	cout << "Target empty_set:";
	original_empty_set.print();

	for (i = 0; i < covg_map_vec.size() ; i++)
	{
		state_granularity_mapping(original_empty_set, covg_map_vec[i].granularity, empty_set_record);
		int output_counter = Output_type_end; //for each granularity, we need to try all output types
		int int_output;
		if (DEBUG)
		{
			cout << "Mapping empty_set:";
			empty_set_record.print();
		}

		while (output_counter > 0)// try output type from Output_type_end to 1, before changing to a larger granularity
		{
			output_counter--;
			int_output = (int)output;
			int_output++;
			int_output %= Output_type_end;
			output = (Output_type) int_output;
			empty_set = empty_set_record; //to reset for each output type

			switch (output)
			{
			case cwnd:
				uprange = covg_map_vec[i].range_info.cwnd_range;
				index = empty_set.cwnd;
				break;
			case ssth:
				uprange = covg_map_vec[i].range_info.ssth_range;
				index = empty_set.ssthresh;
				break;
			case srtt:
				uprange = covg_map_vec[i].range_info.rtt_range;
				index = empty_set.srtt;
				break;
			case rttvar:
				uprange = covg_map_vec[i].range_info.rtvar_range;
				index = empty_set.rttvar;
				break;
			case state:
				uprange = covg_map_vec[i].range_info.state_range;
				index = empty_set.tcp_state;
				break;
			default:
				cout << "No matching output type!!!" << endl;
				exit(-1);
			}
			if (DEBUG)
			{
				cout << "uprange:" << uprange << endl;
			}
			// to looking for upbound state
			int upbound = uprange ;
			for (int uprange_i = index + 1 ; uprange_i < uprange ; uprange_i++) // -1 and up are low and high bounds;
			{
				switch (output)
				{
				case cwnd:
					empty_set.cwnd = uprange_i;
					break;
				case ssth:
					empty_set.ssthresh = uprange_i;
					break;
				case srtt:
					empty_set.srtt = uprange_i;
					break;
				case rttvar:
					empty_set.rttvar = uprange_i;
					break;
				case state:
					empty_set.tcp_state = uprange_i;
					break;
				default:
					cout << "No matching output type!!!" << endl;
					exit(-1);
				}
				if (DEBUG)
				{
					cout << "uprange empty_set:";
					empty_set.print();
				}
				it = covg_map_vec[i].coverage_map.find(empty_set);

				if (it != covg_map_vec[i].coverage_map.end())
				{
					upbound = uprange_i;
					break;
				};
			}

			up_state = empty_set;

			// to looking for lowbound state
			int lowbound = -1;
			for (int low_i = index - 1 ; low_i > lowbound ; low_i--) // 0 and total+1 are low and high bounds;
			{
				switch (output)
				{
				case cwnd:
					empty_set.cwnd = low_i;
					break;
				case ssth:
					empty_set.ssthresh = low_i;
					break;
				case srtt:
					empty_set.srtt = low_i;
					break;
				case rttvar:
					empty_set.rttvar = low_i;
					break;
				case state:
					empty_set.tcp_state = low_i;
					break;
				default:
					cout << "No matching output type!!!" << endl;
					exit(-1);

				}
				if (DEBUG)
				{
					cout << "lowrange empty_set:";
					empty_set.print();
				}
				it = covg_map_vec[i].coverage_map.find(empty_set);

				if (it != covg_map_vec[i].coverage_map.end())
				{
					lowbound = low_i;
					break;
				};
			}

			low_state = empty_set;

			upbound_limit = false;
			lowbound_limit = false;
			if (lowbound == lowrange) lowbound_limit = true; // = -1
			if (upbound == uprange) upbound_limit = true; // range_

			if (!lowbound_limit || !upbound_limit)
			{
				//if (DEBUG)
				{
					if (lowbound_limit || upbound_limit)
						cout << "[One Limit] FIND candidate points at granularity:" << covg_map_vec[i].granularity << endl;
					else
						cout << "[No Limit] FIND candidate points at granularity:" << covg_map_vec[i].granularity << endl;
				}
				break;
			};
		}

		if (!lowbound_limit || !upbound_limit)
			break; // To exit the main loop
	}

	if (upbound_limit && lowbound_limit)
	{
		//if(DEBUG)
		cout << "cannot find candidate points\n";
		return -1;
	}

	Cube_State_Map::iterator it1, it2;
	it1 = covg_map_vec[i].coverage_map.find(up_state);
	it2 = covg_map_vec[i].coverage_map.find(low_state);
	Test_Parems_Limite test_limit;
	get_input_output_relation(test_limit, output, input_output_map);

	Config_Map::iterator it_config;

	if (upbound_limit)  // To increase output
	{
		if (DEBUG)
			cout << "upbound_limit feedback" << endl;
		double a1 = random_range_double();
		double a2 = 1 - a1;
		int rnd = random_range_zero(it2->second.size());
		Coarse_Map::iterator it2_space = it2->second.begin();
		advance(it2_space, rnd);

		it_config = map_config.find(it2_space->second);
		struct Test_Parems new_test_para = it_config->second[0];

		int config_index = 0;
		if (feedback_mode == 2)
		{
			config_index = it_config->second.size() - 1; // cross over from the last one
		}

		new_test_para.speed = a1 * it_config->second[config_index].speed + a2 * test_limit.speed_inc_output_vec[random_range_zero(test_limit.speed_inc_output_vec.size())];
		a1 = random_range_double();
		a2 = 1 - a1;

		new_test_para.sftgma.alpha = a1 * it_config->second[config_index].sftgma.alpha + a2 * test_limit.alpha_inc_output_vec[random_range_zero(test_limit.alpha_inc_output_vec.size())];
		a1 = random_range_double();
		a2 = 1 - a1;

		new_test_para.sftgma.beta = a1 * it_config->second[config_index].sftgma.beta + a2 * test_limit.beta_inc_output_vec[random_range_zero(test_limit.beta_inc_output_vec.size())];
		a1 = random_range_double();
		a2 = 1 - a1;

		new_test_para.sftgma.shift = a1 * it_config->second[config_index].sftgma.shift + a2 * test_limit.shift_inc_output_vec[random_range_zero(test_limit.shift_inc_output_vec.size())];
		a1 = random_range_double();
		a2 = 1 - a1;

		new_test_para.Loss_rate = a1 * it_config->second[config_index].Loss_rate + a2 * test_limit.loss_rate_inc_output_vec[random_range_zero(test_limit.loss_rate_inc_output_vec.size())];
		a1 = random_range_double();
		a2 = 1 - a1;
		
		new_test_para.app_speed = a1 * it_config->second[config_index].app_speed + a2 * test_limit.app_speed_inc_output_vec[random_range_zero(test_limit.app_speed_inc_output_vec.size())];

		new_test_para.rng_run = total_execution + 1;
		vector<struct Test_Parems> tmp_vector;
		if (feedback_mode == 1)
		{
			tmp_vector.push_back(new_test_para);    //only has one test config
			new_test_para_vec.push_back(tmp_vector);
		}
		else if (feedback_mode == 2) // keep all old test, and add one more new test;
		{
			tmp_vector.insert(tmp_vector.end(), it_config->second.begin(), it_config->second.end());
			tmp_vector.push_back(new_test_para);
			new_test_para_vec.push_back(tmp_vector);
		}
	}
	else if (lowbound_limit)// To decrease output
	{
		if (DEBUG)
			cout << "lowbound_limit feedback" << endl;

		{
			double a1 = random_range_double();
			double a2 = 1 - a1;
			int rnd = random_range_zero(it1->second.size());
			Coarse_Map::iterator it1_space = it1->second.begin();
			advance(it1_space, rnd);
			it_config = map_config.find(it1_space->second);
			struct Test_Parems new_test_para = it_config->second[0];
			int config_index = 0;
			if (feedback_mode == 2)
			{
				config_index = it_config->second.size() - 1; // cross over from the last one
			}

			new_test_para.speed = a1 * it_config->second[config_index].speed + a2 * test_limit.speed_dec_output_vec[random_range_zero(test_limit.speed_dec_output_vec.size())];
			a1 = random_range_double();
			a2 = 1 - a1;
			new_test_para.sftgma.alpha = a1 * it_config->second[config_index].sftgma.alpha + a2 * test_limit.alpha_dec_output_vec[random_range_zero(test_limit.alpha_dec_output_vec.size())];

			a1 = random_range_double();
			a2 = 1 - a1;
			new_test_para.sftgma.beta = a1 * it_config->second[config_index].sftgma.beta + a2 * test_limit.beta_dec_output_vec[random_range_zero(test_limit.beta_dec_output_vec.size())];

			a1 = random_range_double();
			a2 = 1 - a1;
			new_test_para.sftgma.shift = a1 * it_config->second[config_index].sftgma.shift + a2 * test_limit.shift_dec_output_vec[random_range_zero(test_limit.shift_dec_output_vec.size())];

			a1 = random_range_double();
			a2 = 1 - a1;
			new_test_para.Loss_rate = a1 * it_config->second[config_index].Loss_rate + a2 * test_limit.loss_rate_dec_output_vec[random_range_zero(test_limit.loss_rate_dec_output_vec.size())];

			a1 = random_range_double();
			a2 = 1 - a1;
			new_test_para.app_speed = a1 * it_config->second[config_index].app_speed + a2 * test_limit.app_speed_dec_output_vec[random_range_zero(test_limit.app_speed_dec_output_vec.size())];

			new_test_para.rng_run = total_execution + 1;

			vector<struct Test_Parems> tmp_vector;
			if (feedback_mode == 1)
			{
				tmp_vector.push_back(new_test_para);    //only has one test config
				new_test_para_vec.push_back(tmp_vector);
			}
			else if (feedback_mode == 2)
			{
				//new_feedback2
				tmp_vector.insert(tmp_vector.end(), it_config->second.begin(), it_config->second.end());
				tmp_vector.push_back(new_test_para);
				new_test_para_vec.push_back(tmp_vector);
			}
		}
	}
	else
	{
		if (feedback_mode == 2) return -1;//

		Coarse_Map::iterator it_coarse_tmp1, it_coarse_tmp2;
		vector<pair<vector<struct Test_Parems>, vector<struct Test_Parems> > > tmp_test_parems_vec;

		if (DEBUG)
			cout << "No limite feedback" << endl;


		int rnd1, rnd2;
		{
			rnd1 = random_range_zero(it1->second.size());
			rnd2 = random_range_zero(it2->second.size());
			it_coarse_tmp1 = it1->second.begin();
			it_coarse_tmp2 = it2->second.begin();
			advance(it_coarse_tmp1, rnd1);
			advance(it_coarse_tmp2, rnd2);

			Config_Map::iterator it_config1, it_config2;
			it_config1 = map_config.find(it_coarse_tmp1->second);
			it_config2 = map_config.find(it_coarse_tmp2->second);

			tmp_test_parems_vec.push_back(make_pair(it_config1->second, it_config2->second));

			if (tmp_test_parems_vec[0].first.size() == 1 && tmp_test_parems_vec[0].second.size() == 1)
			{
				
				struct Test_Parems new_test_para;
				new_test_para = (random_range_zero(2) > 0) ? tmp_test_parems_vec[0].first[0] : tmp_test_parems_vec[0].second[0]; // half first, half second;
				double a1 = random_range_double();
				double a2 = 1 - a1;
				new_test_para.Loss_rate = a1 * tmp_test_parems_vec[0].first[0].Loss_rate + a2 * tmp_test_parems_vec[0].second[0].Loss_rate;
				a1 = random_range_double();
				a2 = 1 - a1;

				new_test_para.sftgma.beta = a1 * tmp_test_parems_vec[0].first[0].sftgma.beta + a2 * tmp_test_parems_vec[0].second[0].sftgma.beta;
				a1 = random_range_double();
				a2 = 1 - a1;

				new_test_para.sftgma.alpha = a1 * tmp_test_parems_vec[0].first[0].sftgma.alpha + a2 * tmp_test_parems_vec[0].second[0].sftgma.alpha;
				a1 = random_range_double();
				a2 = 1 - a1;
				new_test_para.sftgma.shift = a1 * tmp_test_parems_vec[0].first[0].sftgma.shift + a2 * tmp_test_parems_vec[0].second[0].sftgma.shift;

				a1 = random_range_double();
				a2 = 1 - a1;

				new_test_para.app_speed = a1 * tmp_test_parems_vec[0].first[0].app_speed + a2 * tmp_test_parems_vec[0].second[0].app_speed;
				new_test_para.rng_run = total_execution + 1;

				vector<struct Test_Parems> tmp_vector;
				tmp_vector.push_back(new_test_para);
				new_test_para_vec.push_back(tmp_vector);
			}
			else
			{
				return -1; //cannot generate a test put;
			}
		}
	}
	return 0;
}

int generate_new_test_para_vec_N(int feedback_mode, struct State_Record & empty_set, COVG_MAP_VEC & map_vec, Config_Map & map_config, vector<vector<struct Test_Parems> > & new_test_para_vec, INPUT_OUT_MAP & input_output_map)
{
	int i = random_range_zero(5);// 5 d space
	switch (i)
	{
	case cwnd:
		return generate_new_test_para_vec_1D(feedback_mode, cwnd, empty_set, map_vec, map_config, new_test_para_vec, input_output_map);
	case ssth:
		return generate_new_test_para_vec_1D(feedback_mode, ssth, empty_set, map_vec, map_config, new_test_para_vec, input_output_map);
	case srtt:
		return generate_new_test_para_vec_1D(feedback_mode, srtt, empty_set, map_vec, map_config, new_test_para_vec, input_output_map);
	case rttvar:
		return generate_new_test_para_vec_1D(feedback_mode, rttvar, empty_set, map_vec, map_config, new_test_para_vec, input_output_map);
	case state:
		return generate_new_test_para_vec_1D(feedback_mode, state, empty_set, map_vec, map_config, new_test_para_vec, input_output_map);
	default:
		cout << "No matching output type!!!" << endl;
		exit(-1);
	}

	return 0;//should not execute this
}

int total_folder_feedback = 0 ;
int feedback_random_N(int feedback_mode, COVG_MAP_VEC & map_vec, Config_Map& map_config, INPUT_OUT_MAP & input_output_map)
{
	char mvcmd[256] = {0};
	struct State_Record empty_set;
	int res;

	vector<vector<struct Test_Parems> > new_test_para_vec;
	while (true)
	{
		new_test_para_vec.clear();
		if (find_empty_area_N(empty_set, map_vec[0]) == -1)//to find an empty state given granularity 1
		{
			if (DEBUG) cout << "After feedback(Saturated cannot find empty area):" ;
			return -1 ; // saturated almost impossible for 5 d
		}

		if (DEBUG)
		{
			cout << "Empty set:" << empty_set.cwnd
			     << " " << empty_set.ssthresh
			     << " " << empty_set.srtt
			     << " " << empty_set.rttvar
			     << " " << empty_set.tcp_state
			     << endl;
		}

		res = generate_new_test_para_vec_N(feedback_mode, empty_set, map_vec, map_config, new_test_para_vec, input_output_map);
		if (res == -1) continue;//cannot generate new test inputs, for feedback 2

		for (unsigned int m = 0; m < TRIES_Interval && m < new_test_para_vec.size(); m++)
		{
			prepare_before_config_vec(new_test_para_vec[m]);
			res = execute_ns3_2(0);
			if (res == -2 )
			{
				total_execution--; //offsetting this execution
				break; //for exception
			}
			snprintf (mvcmd, 256, "/tmp/output/%d/messages", total_execution);
			res = read_from_file(mvcmd, map_vec, map_config, new_test_para_vec[m]);

			//after
			snprintf (mvcmd, 256, "mv /tmp/output /tmp/output_feedback%d/config_%d", feedback_mode, total_execution);
			total_folder_feedback++;
			system(mvcmd);

			if (res >= 2 && feedback_mode == 1)
			{
				cout << " feedback1 switching at: " << total_execution << endl;
				return -1;
			}

			if (res == 3 && feedback_mode == 2)
			{
				cout << " feedback2 switching at: " << total_execution << endl;
				return -1;
			}
		}
	}
	return 0;
}
