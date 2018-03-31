/*************************************************************************
  > File Name: main.cc
  > Author: wsun
  > Mail:sunweiflyus@gmail.com
  > Created Time: Thu 13 Apr 2017 09:46:38 PM CDT
  > Comments:
 ************************************************************************/
#include <iostream>
#include "share.h"
#include "string.h"
#include "signal.h"
#include "math.h"

#define MAX_CWND 13000

using namespace std;
unsigned int cwnd_max = 0;
int max_incr = 0;
int max_diff = 0;
int choice = 0;
int MAX_RTT = 0;

int checker_rule1(char* filename1, State_Record_Vector& state_vec)
{
	unsigned int before_cwnd, after_cwnd;
	for (int i = 0; i < state_vec.size() - 1; i++)
	{
		cwnd_max = max(state_vec[i].cwnd, cwnd_max);

		if (state_vec[i].cwnd > MAX_CWND)
		{
			cout << "[Voilation0] for cwnd" << state_vec[i].cwnd << ", from " << filename1 << " at line:" << i << " [Reason:] CWND larger than MAX num\n" << endl;
		}
		if ((state_vec[i].tcp_state == 0 || state_vec[i].tcp_state == 1 ) && (state_vec[i + 1].tcp_state == 3 || state_vec[i + 1].tcp_state == 4 ))
		{
			before_cwnd = state_vec[i].cwnd;
		}

		if ((state_vec[i].tcp_state == 3 || state_vec[i].tcp_state == 4 ) && (state_vec[i + 1].tcp_state == 0 || state_vec[i + 1].tcp_state == 1 ))
		{
			after_cwnd = state_vec[i + 1].cwnd;

			if (after_cwnd >  before_cwnd)
			{
				int diff = after_cwnd - before_cwnd;
				if (diff > max_diff)
				{
					max_diff = diff;
					cout << "update max diff" << diff << endl;
				}

				cout << "[Voilation1] for undo before cwnd: " << before_cwnd << ", after cwnd:" << after_cwnd << ", from " << filename1 << " at time:" << state_vec[i + 1].curr_time << " [Reason:] undo bugs\n" << endl;
			}
		}
	}

	return 0;
}

//For cwnd increase per ACK
#define Total_counter 20
#define Each_INTERVAL 2
int checker_rule2(char* filename1, State_Record_Vector& state_vec)
{
	for (int i = 0; i < state_vec.size(); i++)
	{
		int start_cwnd = state_vec[i].cwnd;
		unsigned int start_time = state_vec[i].curr_time;
		int curr_cwnd = state_vec[i].cwnd;
		int ack_counter = 0;
		if (state_vec[i].cwnd < state_vec[i].ssthresh)
			continue;

		if (state_vec[i].flag == 0 || state_vec[i].tcp_state == 3 || state_vec[i].tcp_state == 4 || state_vec[i].ssthresh <= 30 || state_vec[i].srtt > (1000 / 4))//to ignore flag == 0, ack
			continue;

		for (int j = i + 1; j < state_vec.size(); j++)
		{
			if (state_vec[j].cwnd < state_vec[j].ssthresh)// if not CA tcp_state
				break;

			if (state_vec[j].tcp_state == 3 || state_vec[j].tcp_state == 4 )
				break;

			if (state_vec[i].ssthresh <= 30 || state_vec[i].srtt > (1000 / 4))
				break;

			if (state_vec[j].flag == 0 ) //Ack packet
			{
				ack_counter++;
				if (ack_counter % Each_INTERVAL == 0)
				{
					if (state_vec[j].cwnd > curr_cwnd)
					{
						curr_cwnd = state_vec[j].cwnd;
					}
					else
					{
						break;
					}
				}

				if (ack_counter == Total_counter)
				{
					int diff_cwnd = state_vec[j].cwnd - start_cwnd;
					if (diff_cwnd > max_incr)
					{
						max_incr = diff_cwnd;
						cout << "update max incr: " << diff_cwnd << endl;
					}

					max_incr = max(max_incr, diff_cwnd);

					if (diff_cwnd >= Total_counter / Each_INTERVAL)
					{
						cout << "[Voilation2] for " << filename1 << " [Reason:] CWND increase 1 for every other ACK" << " with the increase of ack: " << diff_cwnd  << ", start cwnd:" << start_cwnd << " at simulation time: " << state_vec[i].curr_time << " and final cwnd:" << state_vec[j].cwnd << " at simulation time: " << state_vec[j].curr_time  << endl;
						return 0;//skip this file
					}

					break;
				}
			}
		}

	}

	return 0;
}

//For checking the interval time between bic update
#define Total_counter 20
#define Each_INTERVAL 2
int checker_rule3(char* filename1, State_Record_Vector& state_vec)
{
	for (int i = 0; i < state_vec.size(); i++)
	{
		if (state_vec[i].flag == 2)
		{
			int prev_cwnd = state_vec[i + 1].cwnd;
			int prev_srtt = state_vec[i + 1].srtt;
			int64_t start_time = state_vec[i].curr_time;
			int64_t prev_cnt = state_vec[i].cwnd;

			int prev_ssth = state_vec[i + 1].ssthresh;

			if (prev_ssth > prev_cwnd || prev_cnt < 10 || prev_cwnd < 10)
				continue;


			for (int j = i + 1; j < state_vec.size(); j++)
			{
				if (state_vec[j].flag == 2) // only for bic_update, ssth:target`
				{
					int curr_cwnd = state_vec[j - 1].cwnd;
					int64_t curr_start_time = state_vec[j].curr_time;
					int curr_cnt = state_vec[j].cwnd;
					int curr_target = state_vec[j].ssthresh;
					int curr_ssth = state_vec[j - 1].ssthresh; //flag is not 2, which is not bic_update record

   /*               //if (curr_cnt == 0 && curr_target > 1000)*/
					if (curr_target <= 2 * curr_cwnd)
					{
						break;
					}

					int curr_srtt = state_vec[j + 1].srtt;
					int linux_srtt = 2 * 4 * max(curr_srtt, prev_srtt);
					//MAX_RTT = 0;
					int rtt_limit = linux_srtt; //max(MAX_RTT, linux_srtt);

					{
						int64_t diff = curr_start_time - start_time;
						diff = diff / 1000000; // with ms unit
						if (diff > rtt_limit)// || curr_cnt == 0)
						{
							cout << "[Voilation] for " << filename1 << " [Reason:] the interval between two bic_updates is larger much than 2 * current RTT" << ":" << rtt_limit << ", diff:" << diff << " , first bic_update at simulation time:" << start_time << ", second at:" << curr_start_time << " cnt:" << curr_cnt << " cwnd:" << curr_cwnd << " Bic_target:" << curr_target << "." << endl;
						}
					}
					break;
				}

				if (state_vec[j].tcp_state == 3 || state_vec[j].tcp_state == 4 )
					break;

				if (state_vec[j].ssthresh > state_vec[j].cwnd)
					break;

			}
		}
	}
	return 0;
}

int convert_string_to_elem(string& line, State_Record_Vector& state_vec)
{
	int flag = -1;

	if (line[0] == '[' && line[1] == 'b')
	{
		flag = 2;
		int cnt_left, cnt_right, u_left, target_right;
		for (unsigned int i = 0; i < line.size(); i++)
		{
			if (i < line.size() - 2 && line[i] == 'n' && line[i + 1] == 't')
			{
				cnt_left = i + 3;
			}
			else if (i < line.size() - 2 && line[i] == 'u' && line[i + 1] == ':')
			{
				u_left = i + 2;
			}
			else if (i < line.size() - 2 && line[i] == 'c' && line[i + 1] == 'n')
			{
				cnt_right = i - 2;
			}
			else if (i < line.size() - 2 && line[i] == 'e' && line[i + 1] == 't')
			{
				target_right = i  + 3;
			}
		}

		int64_t curr_time = stol(line.substr(u_left));
		int cnt = stol(line.substr(cnt_left, cnt_right));
		int target = stol(line.substr(target_right));

		struct State_Record tmp(flag, cnt, target, 0, 0, 0, curr_time);
		state_vec.push_back(tmp);
		return 0;
	}


	if (line[0] != 'd' && line[0] != 'a' ) // sanity check
		return 1;
	if (line[0] == 'a') flag = 0;
	else if (line[0] == 'd') flag = 1;


	unsigned int cwnd, ssthresh, srtt, rttvar, state ;
	int64_t curr_time;
	int c_left, s_left, ca_left, r_left, o_left, u_left;

	for (unsigned int i = 0; i < line.size(); i++)
	{
		if (i < line.size() - 2 && line[i] == 'c' && line[i + 1] == ':')
		{
			c_left = i + 2;
		}
		else if (i < line.size() - 2 && line[i] == 's' && line[i + 1] == ':')
		{
			s_left = i + 2;
		}
		else if (i < line.size() - 2 && line[i] == 'a' && line[i + 1] == ':')
		{
			ca_left = i + 2;
		}
		else if (i < line.size() - 2 && line[i] == 'r' && line[i + 1] == ':')
		{
			r_left = i + 2;
		}
		else if (i < line.size() - 2 && line[i] == 'o' && line[i + 1] == ':')
		{
			o_left = i + 2;
		}
		else if (i < line.size() - 2 && line[i] == 'u' && line[i + 1] == ':')
		{
			u_left = i + 2;
		}
	}

	cwnd = stol(line.substr(c_left, s_left - 3));
	ssthresh = stol(line.substr(s_left, ca_left - 4));
	state = stol(line.substr(ca_left, r_left - 3));
	srtt = stol(line.substr(r_left, o_left - 3));
	rttvar = stol(line.substr(o_left, u_left - 3));
	curr_time = stol(line.substr(u_left));
	struct State_Record tmp(flag, cwnd, ssthresh, srtt, rttvar, state, curr_time);
	state_vec.push_back(tmp);

	return 0;
}

int total_files = 0;
int counterexample = 1;
int read_from_config(char* filename1, vector<struct Test_Parems>& tmp_vec)
{
	ifstream inputFile(filename1);
	int res = 0;

	if (!inputFile.is_open())
	{
		cerr << "Unable to open config file:" << filename1 << "\n";
		exit(-1);
	}

	if (inputFile)
	{
		MAX_RTT = 0;
		int counter;//speed, rng_run,•
		//double alpha, beta, shift, Loss_rate;
		// read the elements in the file into a vector
		inputFile >> counter;
		for (int i = 0; i < counter ; i ++)
		{
			struct Test_Parems new_tmp;
			inputFile >> new_tmp.speed;
			inputFile >> new_tmp.sftgma.alpha;
			inputFile >> new_tmp.sftgma.beta;
			inputFile >> new_tmp.sftgma.shift;
			inputFile >> new_tmp.Loss_rate;
			inputFile >> new_tmp.app_speed;
			inputFile >> new_tmp.rng_run;
			inputFile >> new_tmp.curr_time;
			tmp_vec.push_back(new_tmp);

			if (tmp_vec[i].app_speed < 1000)
			{
				//cout << "found filename: " << filename1 << " low speed: " << tmp_vec[i].app_speed << endl ;
				counterexample++;
				res = 1;
			}
			//return 0;

			if (i == 1)
			{
				if (tmp_vec[i - 1].app_speed < 1000 && tmp_vec[i].app_speed > 10000)
				{
					//cout << "found filename: " << filename1 << " low speed: " << tmp_vec[i - 1].app_speed << " high speed: " << tmp_vec[i].app_speed << endl ;
					counterexample++;
					res = 1;
				}
			}

			int aver = 2 * (new_tmp.sftgma.alpha * new_tmp.sftgma.beta + new_tmp.sftgma.shift);
			MAX_RTT = max(MAX_RTT, aver);
		}
	}

	return res ;
}

int read_from_file(char* filename1)
{
	string line1;
	ifstream file1(filename1);
	State_Record_Vector state_vec;

	if (!file1.is_open())
	{
		cerr << "Unable to open file:" << filename1 << "\n";
		return -2; //for exception case
	}

	while (getline(file1, line1))
	{
		convert_string_to_elem(line1, state_vec);
	}

	if (choice == 1 ) checker_rule1(filename1, state_vec);

	if (choice == 2 ) checker_rule2(filename1, state_vec);

	if (choice == 3) checker_rule3(filename1, state_vec);

	total_files++;
	cout << "total_files:" << total_files << endl;
	return 0 ;
}


int main (int argc, char* argv[])
{
	if (argc != 5)
	{
		// Tell the user how to run the program
		std::cerr << "Usage: " << argv[0] << " config from num, to num, check files folder path, rule choice(1:rule1, 2:rule2, 3:rule3)" << std::endl;
		return 1;
	}

	int from = stoi(argv[1]);
	int to = stoi(argv[2]);
	choice = stoi(argv[4]);
	char cmd[512];
	int res = 0;
	vector<struct Test_Parems> vec_test_para;
	for (int i = from; i <= to ; i++)
	{
		vec_test_para.clear();
		snprintf (cmd, 512, "%s/config_%d/input_config.txt", argv[3], i);
		res = read_from_config(cmd, vec_test_para); // for cwnd;
		//if (res == 0) continue ; //skip this file
		snprintf (cmd, 256, "%s/config_%d/%d/messages", argv[3], i, i);
		read_from_file(cmd);
		//process each file one by one
	}
	//cout << "cwnd_max:" << cwnd_max << " cwnd_max_increase:" << max_incr << " cwnd_max_diff:" << max_diff << endl;
	return 0;
}
