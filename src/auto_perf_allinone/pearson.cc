/*************************************************************************
    > File Name: pearson.cc
  > Author: wsun
  > Mail:sunweiflyus@gmail.com
  > Created Time: Wed 19 Apr 2017 09:25:39 PM CDT
  > Comments:
 ************************************************************************/
#include "share.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_statistics.h>

double Person_coefficient(vector<double>& x, vector<double>& y, int n)
{
	gsl_vector_const_view gsl_x = gsl_vector_const_view_array( &x[0], x.size() );
	gsl_vector_const_view gsl_y = gsl_vector_const_view_array( &y[0], y.size() );

	double pearson = gsl_stats_correlation( (double*) gsl_x.vector.data, 1,
	                                        (double*) gsl_y.vector.data, 1,
	                                        n );

	return pearson;
}

struct Person_elems
{
	vector<double> x_speed;
	vector<double> x_loss;
	vector<double> x_alpha;
	vector<double> x_beta;
	vector<double> x_shift;
	vector<double> x_app_speed;

	vector<double> y_cwnd;
	vector<double> y_ssth;
	vector<double> y_srtt;
	vector<double> y_rttvar;
	vector<double> y_state;
};

int GetId(enum Input_type input_type, struct Test_Parems tmp)
{
	int id = 0;

	if (input_type != speed)
	{
		int tmp_id;

		if (tmp.speed > -0.1 && tmp.speed <= 25000)
		{
			tmp_id = 1;
		}
		else if (tmp.speed > 25000 && tmp.speed <= 50000)
		{
			tmp_id = 2;
		}
		else if (tmp.speed > 50000 && tmp.speed <= 75000)
		{
			tmp_id = 3;
		}
		else if (tmp.speed > 75000 && tmp.speed <= 100000.1)
		{
			tmp_id = 4;
		}
		else
		{
			cout << "error report, speed \n";
			exit(-1);
		}

		id = 10 * id;
		id += tmp_id;
	}

	if (input_type != loss)
	{
		int tmp_id;
		if (tmp.Loss_rate > -0.1 && tmp.Loss_rate <= 2.5)
		{
			tmp_id  = 1;
		}
		else if (tmp.Loss_rate > 2.5 && tmp.Loss_rate <= 5.0)
		{
			tmp_id  = 2;
		}
		else if (tmp.Loss_rate > 5.0 && tmp.Loss_rate <= 7.5) // for double;
		{
			tmp_id  = 3;
		}
		else if (tmp.Loss_rate > 7.5 && tmp.Loss_rate <= 10.01) // for double;
		{
			tmp_id  = 4;
		}
		else
		{
			cout << "error report, Loss_rate \n";
			exit(-1);
		}

		id = 10 * id;
		id += tmp_id;
	}

	if (input_type != alpha)
	{
		int tmp_id;

		if (tmp.sftgma.alpha > -0.1 && tmp.sftgma.alpha <= 5)// for double
		{
			tmp_id = 1;
		}
		else if (tmp.sftgma.alpha > 5 && tmp.sftgma.alpha <= 10)
		{
			tmp_id = 2;
		}
		else if (tmp.sftgma.alpha > 10 && tmp.sftgma.alpha <= 15) // for double
		{
			tmp_id = 3;
		}
		else if (tmp.sftgma.alpha > 15 && tmp.sftgma.alpha <= 21) // for double
		{
			tmp_id = 4;
		}
		else
		{
			cout << "error report, alpha \n";
			exit(-1);
		}

		id = 10 * id;
		id += tmp_id;
	}

	if (input_type != beta)
	{
		int tmp_id;
		if (tmp.sftgma.beta > -0.1 && tmp.sftgma.beta <= 20 )// for double
		{
			tmp_id  = 1;
		}
		else if (tmp.sftgma.beta > 20 && tmp.sftgma.beta <= 40)
		{
			tmp_id  = 2;
		}
		else if (tmp.sftgma.beta > 40 && tmp.sftgma.beta <= 60) // for double
		{
			tmp_id  = 3;
		}
		else if (tmp.sftgma.beta > 60 && tmp.sftgma.beta <= 80.01) // for double
		{
			tmp_id  = 4;
		}
		else
		{
			cout << "error report, beta \n";
			exit(-1);
		}

		id = 10 * id;
		id += tmp_id;
	}

	if (input_type != shift)
	{
		int tmp_id;

		if (tmp.sftgma.shift > -0.1 && tmp.sftgma.shift <= 250)// for double
		{
			tmp_id  = 1;
		}
		else if (tmp.sftgma.shift > 250 && tmp.sftgma.shift <= 500)
		{
			tmp_id  = 2;
		}
		else if (tmp.sftgma.shift > 500 && tmp.sftgma.shift <= 750) // for double
		{
			tmp_id  = 3;
		}
		else if (tmp.sftgma.shift > 750 && tmp.sftgma.shift <= 1001) // for double
		{
			tmp_id  = 4;
		}
		else
		{
			cout << "error report, shfit \n";
			exit(-1);
		}

		id = 10 * id;
		id += tmp_id;
	}

	if (input_type != app_speed)
	{
		int tmp_id;

		if (tmp.app_speed >= 1 && tmp.app_speed < 25000000) // for double
		{
			tmp_id  = 1;
		}
		else if (tmp.app_speed >= 25000000 && tmp.app_speed < 50000000)
		{
			tmp_id  = 2;
		}
		else if (tmp.app_speed >= 50000000 && tmp.app_speed < 75000000) // for double
		{
			tmp_id  = 3;
		}
		else if (tmp.app_speed <= 100000000) // for double
		{
			tmp_id  = 4;
		}
		else
		{
			cout << "app_speed:" << tmp.app_speed << "error report, app_speed \n";
			exit(-1);
		}

		// for 1000, 10000, 100000 cases
		/*
		if (tmp.app_speed >= 1 && tmp.app_speed < 1000) // for double
		{
		    tmp_id  = 1;
		}
		else if (tmp.app_speed >= 1000 && tmp.app_speed < 1000000)
		{
		    tmp_id  = 2;
		}
		else if (tmp.app_speed >= 1000000 && tmp.app_speed < 100000000) // for double
		{
		    tmp_id  = 3;
		}
		else if (tmp.app_speed <= 100000000) // for double
		{
		    tmp_id  = 4;
		}
		else
		{
		    cout << "app_speed:" << tmp.app_speed << "error report, app_speed \n";
		    exit(-1);
		}
		*/

		id = 10 * id;
		id += tmp_id;
	}
	return id;
}

int Push_map(Person_elems& total_person, struct Test_Parems test, Record_average& record)
{
	total_person.x_speed.push_back(test.speed);
	total_person.x_loss.push_back(test.Loss_rate);
	total_person.x_alpha.push_back(test.sftgma.alpha);
	total_person.x_beta.push_back(test.sftgma.beta);
	total_person.x_shift.push_back(test.sftgma.shift);
	total_person.x_app_speed.push_back(test.app_speed);

	total_person.y_cwnd.push_back(record.cwnd_aver);
	total_person.y_ssth.push_back(record.ssth_aver);
	total_person.y_srtt.push_back(record.rtt_aver);
	total_person.y_rttvar.push_back(record.rttvar_aver);
	total_person.y_state.push_back(record.state_aver);
	return 0;
}


int pearson_corrleation_update(struct single_input_all_output& input_output_struct, INPUT_OUT_MAP& map)
{
	double pearson_res = 0 ;
	pair<Input_type, Output_type> pair_enum;
	for (int i = 0; i < Output_type_end; i++)
	{
		pair_enum = make_pair(input_output_struct.inputType, (Output_type)i);
		pearson_res = input_output_struct.GetTypeValue((Output_type)i);
		if (std::isnan(pearson_res) || abs(pearson_res) < Pearson_limite)
		{
			//if (DEBUG)
			cout << "Input type:" << input_output_struct.inputType << " , Output type:" << i << " with correlation: 0" << endl;
			map.insert(make_pair(pair_enum, 0));
		}
		else
		{
			//if (DEBUG)
			{
				if (pearson_res > 0 )
					cout << "Input type:" << input_output_struct.inputType << " , Output type:" << i << " with correlation: 1" << endl;
				else
					cout << "Input type:" << input_output_struct.inputType << " , Output type:" << i << " with correlation: -1" << endl;
			}

			map.insert(make_pair(pair_enum, pearson_res > 0 ? 1 : -1));
		}

	}
	return 0;
}

int pearson_corrleation_struct(enum Input_type input_type, struct Person_elems& total_person, struct single_input_all_output& input_output_struct)
{
	input_output_struct.inputType = input_type;
	input_output_struct.clear();
	double pearson_res;

	if (input_type == speed)
	{
		pearson_res = Person_coefficient(total_person.x_speed, total_person.y_cwnd, total_person.x_speed.size());
		if (DEBUG) cout << "Pearson corrleation between speed and cwnd:" << pearson_res << endl;

		if (std::isnan(pearson_res))
		{
			input_output_struct._cwnd = 0 ;
		}
		else
		{
			input_output_struct._cwnd = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_speed, total_person.y_ssth, total_person.x_speed.size());

		if (DEBUG) cout << "Pearson corrleation between speed and ssthresh:" << pearson_res << endl;

		if (std::isnan(pearson_res))
		{
			input_output_struct._ssth = 0 ;
		}
		else
		{
			input_output_struct._ssth = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_speed, total_person.y_srtt, total_person.x_speed.size());

		if (DEBUG) cout << "Pearson corrleation between speed and rtt:" << pearson_res << endl;

		if (std::isnan(pearson_res))
		{
			input_output_struct._srtt = 0 ;
		}
		else
		{
			input_output_struct._srtt = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_speed, total_person.y_rttvar, total_person.x_speed.size());
		if (DEBUG) cout << "Pearson corrleation between speed and rttvar:" << pearson_res << endl;

		if (std::isnan(pearson_res))
		{
			input_output_struct._rttvar = 0 ;
		}
		else
		{
			input_output_struct._rttvar = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_speed, total_person.y_state, total_person.x_speed.size());
		if (DEBUG) cout << "Pearson corrleation between speed and state:" << pearson_res << endl;

		if (std::isnan(pearson_res))
		{
			input_output_struct._state = 0;
		}
		else
		{
			input_output_struct._state = pearson_res;
		}
	}

	if (input_type == loss)
	{
		pearson_res = Person_coefficient(total_person.x_loss, total_person.y_cwnd, total_person.x_speed.size());
		if (DEBUG) cout << "Pearson corrleation between Loss rate and _cwnd:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._cwnd = 0 ;
		}
		else
		{
			input_output_struct._cwnd = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_loss, total_person.y_ssth, total_person.x_speed.size());
		if (DEBUG) cout << "Pearson corrleation between Loss rate and ssthresh:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._ssth = 0 ;
		}
		else
		{
			input_output_struct._ssth = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_loss, total_person.y_srtt, total_person.x_speed.size());
		if (DEBUG)cout << "Pearson corrleation between Loss rate and rtt:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._srtt = 0 ;
		}
		else
		{
			input_output_struct._srtt = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_loss, total_person.y_rttvar, total_person.x_speed.size());
		if (DEBUG)  cout << "Pearson corrleation between Loss rate and _rttvar:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._rttvar = 0 ;
		}
		else
		{
			input_output_struct._rttvar = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_loss, total_person.y_state, total_person.x_speed.size());
		if (DEBUG) cout << "Pearson corrleation between Loss rate and _state:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._state = 0;
		}
		else
		{
			input_output_struct._state = pearson_res;
		}

	}

	if (input_type == alpha)
	{
		pearson_res = Person_coefficient(total_person.x_alpha, total_person.y_cwnd, total_person.x_speed.size());
		if (DEBUG)  cout << "Pearson corrleation between alpha and _cwnd:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._cwnd = 0 ;
		}
		else
		{
			input_output_struct._cwnd = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_alpha, total_person.y_ssth, total_person.x_speed.size());
		if (DEBUG)  cout << "Pearson corrleation between alpha and ssthresh:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._ssth = 0 ;
		}
		else
		{
			input_output_struct._ssth = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_alpha, total_person.y_srtt, total_person.x_speed.size());
		if (DEBUG)  cout << "Pearson corrleation between alpha and rtt:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._srtt = 0 ;
		}
		else
		{
			input_output_struct._srtt = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_alpha, total_person.y_rttvar, total_person.x_speed.size());
		if (DEBUG)  cout << "Pearson corrleation between alpha and _rttvar:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._rttvar = 0 ;
		}
		else
		{
			input_output_struct._rttvar = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_alpha, total_person.y_state, total_person.x_speed.size());
		if (DEBUG)  cout << "Pearson corrleation between alpha and _state:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._state = 0;
		}
		else
		{
			input_output_struct._state = pearson_res;
		}
	}

	if (input_type == beta)
	{
		pearson_res = Person_coefficient(total_person.x_beta, total_person.y_cwnd, total_person.x_speed.size());
		if (DEBUG)cout << "Pearson corrleation between beta and _cwnd:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._cwnd = 0 ;
		}
		else
		{
			input_output_struct._cwnd = pearson_res ;
		}


		pearson_res = Person_coefficient(total_person.x_beta, total_person.y_ssth, total_person.x_speed.size());
		if (DEBUG)cout << "Pearson corrleation between beta and ssthresh:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._ssth = 0 ;
		}
		else
		{
			input_output_struct._ssth = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_beta, total_person.y_srtt, total_person.x_speed.size());
		if (DEBUG)cout << "Pearson corrleation between beta and rtt:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._srtt = 0 ;
		}
		else
		{
			input_output_struct._srtt = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_beta, total_person.y_rttvar, total_person.x_speed.size());
		if (DEBUG)cout << "Pearson corrleation between beta and _rttvar:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._rttvar = 0 ;
		}
		else
		{
			input_output_struct._rttvar = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_beta, total_person.y_state, total_person.x_speed.size());
		if (DEBUG)cout << "Pearson corrleation between beta and _state:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._state = 0;
		}
		else
		{
			input_output_struct._state = pearson_res;
		}
	}

	if (input_type == shift)
	{
		pearson_res = Person_coefficient(total_person.x_shift, total_person.y_cwnd, total_person.x_speed.size());
		if (DEBUG)cout << "Pearson corrleation between shift and _cwnd:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._cwnd = 0 ;
		}
		else
		{
			input_output_struct._cwnd = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_shift, total_person.y_ssth, total_person.x_speed.size());
		if (DEBUG)cout << "Pearson corrleation between shift and ssthresh:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._ssth = 0 ;
		}
		else
		{
			input_output_struct._ssth = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_shift, total_person.y_srtt, total_person.x_speed.size());
		if (DEBUG)cout << "Pearson corrleation between shift and rtt:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._srtt = 0 ;
		}
		else
		{
			input_output_struct._srtt = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_shift, total_person.y_rttvar, total_person.x_speed.size());
		if (DEBUG) cout << "Pearson corrleation between shift and _rttvar:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._rttvar = 0 ;
		}
		else
		{
			input_output_struct._rttvar = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_shift, total_person.y_state, total_person.x_speed.size());
		if (DEBUG) cout << "Pearson corrleation between shift and _state:" << pearson_res << endl;
		if (std::isnan(pearson_res))
		{
			input_output_struct._state = 0;
		}
		else
		{
			input_output_struct._state = pearson_res;
		}

	}

	if (input_type == app_speed)
	{
		pearson_res = Person_coefficient(total_person.x_app_speed, total_person.y_cwnd, total_person.x_app_speed.size());
		if (DEBUG) cout << "Pearson corrleation between app_speed and _cwnd:" << pearson_res << endl;

		if (std::isnan(pearson_res))
		{
			input_output_struct._cwnd = 0 ;
		}
		else
		{
			input_output_struct._cwnd = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_app_speed, total_person.y_ssth, total_person.x_app_speed.size());

		if (DEBUG) cout << "Pearson corrleation between app_speed and ssthresh:" << pearson_res << endl;

		if (std::isnan(pearson_res))
		{
			input_output_struct._ssth = 0 ;
		}
		else
		{
			input_output_struct._ssth = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_app_speed, total_person.y_srtt, total_person.x_app_speed.size());

		if (DEBUG) cout << "Pearson corrleation between app_speed and rtt:" << pearson_res << endl;

		if (std::isnan(pearson_res))
		{
			input_output_struct._srtt = 0 ;
		}
		else
		{
			input_output_struct._srtt = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_app_speed, total_person.y_rttvar, total_person.x_app_speed.size());
		if (DEBUG) cout << "Pearson corrleation between app_speed and _rttvar:" << pearson_res << endl;

		if (std::isnan(pearson_res))
		{
			input_output_struct._rttvar = 0 ;
		}
		else
		{
			input_output_struct._rttvar = pearson_res ;
		}

		pearson_res = Person_coefficient(total_person.x_app_speed, total_person.y_state, total_person.x_app_speed.size());
		if (DEBUG) cout << "Pearson corrleation between app_speed and _state:" << pearson_res << endl;

		if (std::isnan(pearson_res))
		{
			input_output_struct._state = 0;
		}
		else
		{
			input_output_struct._state = pearson_res;
		}
	}
	if (DEBUG) cout << "sample size:" << total_person.x_app_speed.size() << endl;
	return 0;
}

#define Pearson_Sample_Size 10
int pearson_corrleation(vector<pair<struct Test_Parems, Record_average> > input_output_relation, INPUT_OUT_MAP& total_input_output_map)
{
	map<struct INPUT_ELEMENT, struct Person_elems> in_out_map;
	map<struct INPUT_ELEMENT, struct Person_elems>::iterator it;

	for (int i = 0 ; i < input_output_relation.size(); i++)
	{
		INPUT_ELEMENT input;
		if (DEBUG)
		{
			cout << "relation i th:" << i << endl;
			input_output_relation[i].first.print();
		}

		for (int j = 0; j < Input_type_end; j++)
		{
			input.input_type = (Input_type) j;
			input.id = GetId(input.input_type, input_output_relation[i].first);
			//To get id for the interval construct
			it = in_out_map.find(input);

			if (it == in_out_map.end())
			{
				struct Person_elems elems;
				Push_map(elems, input_output_relation[i].first, input_output_relation[i].second);
				in_out_map.insert(make_pair(input, elems));
			}
			else
			{
				Push_map(it->second, input_output_relation[i].first, input_output_relation[i].second);
			}
		}
	}

	INPUT_ELEMENT input;
	struct single_input_all_output single_corrleation;
	struct single_input_all_output total_single_corrleation;
	int counter = 0;
	//
	for (int m = 0 ; m < Input_type_end; m++)
	{
		counter = 0;
		single_corrleation.clear();
		total_single_corrleation.clear();

		input.input_type = (Input_type) m;
		for (int i = 1; i <= 4 ; i++)
			for (int j = 1; j <= 4 ; j++)
				for (int k = 1; k <= 4 ; k++)
					for (int l = 1; l <= 4 ; l++)
						for (int n = 1; n <= 4 ; n++)
						{
							input.id = i * 10000 + j * 1000 + k * 100 + l * 10 + n;
							if (DEBUG) cout << "process type:" << m << " id:" <<  input.id << endl;
							it = in_out_map.find(input);
							if (it != in_out_map.end())
							{
								if (it->second.x_speed.size() >= Pearson_Sample_Size)
								{
									pearson_corrleation_struct(input.input_type, it->second, single_corrleation);
									total_single_corrleation._cwnd += single_corrleation._cwnd;
									total_single_corrleation._ssth += single_corrleation._ssth;
									total_single_corrleation._srtt += single_corrleation._srtt;
									total_single_corrleation._rttvar += single_corrleation._rttvar;
									total_single_corrleation._state += single_corrleation._state;
									counter++ ;
								}
								else
								{
									if (DEBUG) cout << "sample size:" << it->second.x_speed.size() << " too small to calulate correlation!" << endl;
								}
							}
							else
							{
								if (DEBUG) cout << "id cannot find " << endl;
							}
						}

		total_single_corrleation._cwnd /= counter;
		total_single_corrleation._ssth /= counter;
		total_single_corrleation._srtt /= counter;
		total_single_corrleation._rttvar /= counter;
		total_single_corrleation._state /= counter;

		total_single_corrleation.inputType = input.input_type;
		//if (DEBUG)
		cout << "Update Type: " << m << ", "
		     << " samples: " << counter
		     << " correlation cwnd: " << total_single_corrleation._cwnd
		     << " correlation ssth: " << total_single_corrleation._ssth
		     << " correlation srtt: " << total_single_corrleation._srtt
		     << " correlation rttvar: " << total_single_corrleation._rttvar
		     << " correlation state: " << total_single_corrleation._state
		     << endl;

		pearson_corrleation_update(total_single_corrleation, total_input_output_map);
	}

	return 0;
}
