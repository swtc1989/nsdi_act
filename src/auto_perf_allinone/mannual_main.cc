/*************************************************************************
    > File Name: main.cc
  > Author: wsun
  > Mail:sunweiflyus@gmail.com
  > Created Time: Thu 13 Apr 2017 09:46:38 PM CDT
  > Comments:
 ************************************************************************/
#include <iostream>
#include "share.h"
#include "random_uniform.cpp"
#include "granularity.cc"
#include "feedback.cc"
#include "process_file_to_map.cc"
#include "pearson.cc"
#include "string.h"
#include "signal.h"
#include "math.h"

int manual_seed = 0;
int total_execution = 0;
int MAIL_MODE = 0;
using namespace std;

//globel variable
COVG_MAP_VEC covg_map_vec;
Config_Map config_map;
int Finish_exeuction = 0;
struct RANGE_INFO range_info;
struct Record_average average_record;
vector<pair<struct Test_Parems, Record_average> > input_output_relation;

int index_i = 0;
int granularity = 1;

int convert_string_to_elem(string& line, COVG_MAP_VEC& trace, Config_Map& map_config, vector<struct Test_Parems>& test_para_vec)
{
	if (line[0] != 'd' && line[0] != 'a' ) // sanity check
		return 1;

	unsigned int cwnd, ssthresh, srtt, rttvar, state ;
	int64_t curr_time;
	int c_left, s_left, ca_left, r_left, o_left, left, u_left;

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

	if (ssthresh == 2147483647) return 0; // remove inital slow start
	if (state == 4) state = 3; // substitute State Recovery(3) with State Loss(4) for state(0-3)
	else if (state == 3) state = 2; // substitute State CWR(2) with State Recovery(3) for state(0-3)
	average_record.cwnd_aver += (cwnd - average_record.cwnd_aver) * 1.0 / (index_i + 1);
	average_record.ssth_aver += (ssthresh - average_record.ssth_aver) * 1.0 / (index_i + 1);
	average_record.rtt_aver += (srtt - average_record.rtt_aver) * 1.0 / (index_i + 1);
	average_record.rttvar_aver += (rttvar - average_record.rttvar_aver) * 1.0 / (index_i + 1);
	average_record.state_aver += (state - average_record.state_aver) * 1.0 / (index_i + 1);
	index_i++;

	if (cwnd > 0 && cwnd <= CWND_RANGE && ssthresh > 0 && ssthresh <= SSTH_RANGE && srtt > 0 && srtt <= RTT_RANGE && rttvar > 0 && rttvar <= RTVAR_RANGE && state >= 0 && state < STATE_RANGE && curr_time > 0)  //ssthresh at least 2
	{
		struct State_Record tmp(cwnd, ssthresh, srtt, rttvar, state, curr_time);
		insert_state(tmp, trace, test_para_vec, map_config);
	}
	return 0;
}

#define COVG_LIMIT_RANDOM 5
#define COVG_LIMIT_FEEDBACK1 2
#define COVG_LIMIT_FEEDBACK2 1
int total_files = 0;
int prev_coverage_size = 1;
int read_from_file(char* filename1, COVG_MAP_VEC& trace, Config_Map& map_config,  vector<struct Test_Parems>& test_para_vec)
{
	string line1;
	ifstream file1(filename1);

	if (!file1.is_open())
	{
		cerr << "Unable to open file:" << filename1 << "\n";
		return -2; //for exception case
	}
	index_i = 0;
	memset(&average_record, 0, sizeof(average_record));

	while (getline(file1, line1))
	{
		convert_string_to_elem(line1, trace, map_config, test_para_vec);
	}

	total_files++;

	cout << "Current files:" << total_files << endl;

	if (total_files % 5000 == 0)
	{
		double inc_per = (trace[0].coverage_map.size() - prev_coverage_size) * 100.0 / prev_coverage_size; //Percentage

		cout << "[Coverage:] every 1000 times:" << trace[0].coverage_map.size()  << " ,prev_coverage_size:" << prev_coverage_size << ", growth %: " << inc_per << " , total files:" << total_files << endl;
		prev_coverage_size = trace[0].coverage_map.size();

		cal_coverage_AllGrans (covg_map_vec);
	}

	return 0 ;
}


void prepare_before_config_vec(vector<struct Test_Parems>& vec_test_para)
{
	system("rm -r /tmp/output");
	system("mkdir /tmp/output");
	ofstream output_file("/tmp/input_config.txt");

	total_execution++;
	if (output_file)
	{
		output_file << vec_test_para.size() << "\n";
		for (unsigned int i = 0; i < vec_test_para.size(); i++)
		{
			output_file << vec_test_para[i].speed
			            << " " << vec_test_para[i].sftgma.alpha
			            << " " << vec_test_para[i].sftgma.beta
			            << " " << vec_test_para[i].sftgma.shift
			            << " " << vec_test_para[i].Loss_rate
			            << " " << vec_test_para[i].app_speed
			            << " " << vec_test_para[i].rng_run
			            << " " << vec_test_para[i].curr_time
			            << "\n";
			if (DEBUG)
				cout << vec_test_para[i].speed
				     << " " << vec_test_para[i].sftgma.alpha
				     << " " << vec_test_para[i].sftgma.beta
				     << " " << vec_test_para[i].sftgma.shift
				     << " " << vec_test_para[i].Loss_rate
				     << " " << vec_test_para[i].app_speed
				     << " " << vec_test_para[i].rng_run
				     << " " << vec_test_para[i].curr_time
				     << "\n";

		}
		output_file.close();
	}
	else
	{
		cout << "canot output config vec\n";
		return ;
		//exit(-1);
	}

	system("cp /tmp/input_config.txt /tmp/output/");

}


int execute_ns3_2(int mode) // without given arguments;
{

	if (total_execution % 5000 == 0)//to clean up ns3 memory 
	{
		system("sudo sync && echo 3 | sudo tee /proc/sys/vm/drop_caches");
	}

	char cmd[512];
	snprintf (cmd, 512, "sudo sh dce.sh dce-linux %d", total_execution);

	if (DEBUG) cout << cmd << endl;
	int res = system(cmd);
	if (res < 0 || res == 127)
	{
		cerr << "cannot execute the ns3 script sucessfully, retval:"<< res << endl;
		return -2;
	}

	return 0;
}

#define TRIES_PER 1
int try_per_config(int i, int mode, struct Test_Parems test_para) //test input i
{
	char mvcmd[256] = {0};
	vector<struct Test_Parems> test_para_vec;

	int res = 0;

	test_para_vec.push_back(test_para);

	//before
	prepare_before_config_vec(test_para_vec);
	for (int j = 1; j <= TRIES_PER; j++)
	{
		res = execute_ns3_2(0); //mode 0
	}
	if (res == -2)
	{
		total_execution--; //offsetting this execution
		return -2; // used for skp exception
	}

	snprintf (mvcmd, 256, "/tmp/output/%d/messages", total_execution);
	res = read_from_file(mvcmd, covg_map_vec, config_map, test_para_vec);

	//after
	snprintf (mvcmd, 256, "mv /tmp/output /tmp/output_all/config_%d", i);
	system(mvcmd);

	if (DEBUG)
	{
		cout << "Input Test_Parems:" << endl;
		test_para.print();
	}

	return res;
}

int cmd_init_random()
{
	system("sudo rm -r /tmp/output_all");
	system("sudo rm -r /tmp/input_config.txt");
	system("sudo mkdir /tmp/output_all");

	return 0;
}

int cmd_init_feedback() // Todo: change mode
{
	system("sudo rm -r /tmp/input_config.txt");
	system("sudo rm -r /tmp/output");
	system("sudo rm -r /tmp/output_feedback1");
	system("sudo rm -r /tmp/output_feedback2");
	system("sudo mkdir /tmp/output_feedback1");
	system("sudo mkdir /tmp/output_feedback2");

	return 0;
}

int purely_mannual_testing(int mode) // 0: old mode, 1: new mode
{
	struct Test_Parems test_para;
	vector<double> speed, delay, loss, alpha, beta, app_speed;

	speed.push_back(1);//0.1 Mbps
	speed.push_back(10);//1 Mbps
	speed.push_back(100);//10 Mbps
	speed.push_back(1000);//100 Mbps
	speed.push_back(10000);//1000 Mbps
	speed.push_back(100000);//10000 Mbps

	delay.push_back(1);// 1ms
	delay.push_back(10);// 10ms
	delay.push_back(100);// 100ms
	delay.push_back(1000);// 1000ms

	loss.push_back(0); //0
	loss.push_back(0.0001); //0.0001%
	loss.push_back(0.001); //0.001%
	loss.push_back(0.01); //0.01%
	loss.push_back(0.1); //0.1%
	loss.push_back(1); //1%
	loss.push_back(10); //10%

	alpha.push_back(0);//0
	alpha.push_back(1);//1
	alpha.push_back(10);//10


	beta.push_back(0);//0
	beta.push_back(1);//1
	beta.push_back(10);//10
	
	app_speed.push_back(1);//1kb
	app_speed.push_back(10);//10kb
	app_speed.push_back(100);//100kb
	app_speed.push_back(1000);//1M
	app_speed.push_back(10000);//10M
	app_speed.push_back(100000);//100M
	app_speed.push_back(1000000);//1000M
	app_speed.push_back(10000000);//10000M

	test_para.rng_run = manual_seed;//changed for different experiments;
	for (int i = 0; i < speed.size(); i++)
	for (int j = 0; j < delay.size(); j++)
	for (int k = 0; k < loss.size(); k++)
	for (int l = 0; l < alpha.size(); l++)
	for (int m = 0; m < beta.size(); m++)
	for (int n = 0; n < app_speed.size(); n++)
	{
		test_para.speed = speed[i];
		test_para.sftgma.shift = delay[j];
		test_para.Loss_rate = loss[k];
		test_para.sftgma.alpha = alpha[l];
		test_para.sftgma.beta = beta[m];
		test_para.app_speed = app_speed[n];
		try_per_config(total_execution + 1, mode, test_para);
	}

	return 0;
}

void sigint_handler(int sig)
{
	printf("killing process %d\n", getpid());
	exit(0);
}

void exit_handler()
{
	//After feedback coverage
	cout << "[At Last] 5d coverage:" << endl;
	cal_coverage_AllGrans(covg_map_vec);

	if (MAIL_MODE) system ("sudo mail -s \"Experiment Finished Mention \" unlcsewsun@gmail.com < /dev/null ");
}

int init_coverage_map_vec()
{
	int granularity = 0;
	for (int i = 0; i < 10; i ++)
	{
		Grans_coverage_map coverage;
		granularity = pow(2, i);
		if (DEBUG) std::cout << "coverage map initialization, granularity:" << granularity  << endl;

		struct RANGE_INFO range_info;
		cal_range(range_info, granularity);
		coverage.granularity = granularity;
		coverage.range_info = range_info;
		covg_map_vec.push_back(coverage);
	}

	return 0;
}

int main (int argc, char* argv[])
{
	if (argc != 3)
	{
		// Tell the user how to run the program
		std::cerr << "Usage: " << argv[0] << " Manual_seed, mail mode(0, 1)" << std::endl;
		return 1;
	}

	signal(SIGINT, sigint_handler);
	atexit(exit_handler);
	freopen("log.txt", "w", stdout);
	int mode = 0;
	manual_seed = stoi(argv[1]);
	MAIL_MODE = stoi(argv[2]);
	random_init();
	cout << "free up system memory for this experiment !\n";
	system("sudo sync && echo 3 | sudo tee /proc/sys/vm/drop_caches");
	if (MAIL_MODE) system ("sudo mail -s \"Experiment Begin Mention \" unlcsewsun@gmail.com < /dev/null ");

	if (mode == 0) // purely random only
	{
		if (DEBUG) cout << "Mannual random start !!" << endl ;
		cmd_init_random();
		purely_mannual_testing(0);
	}

	cout << "total_execution:" << total_execution << endl;

	exit(0);
}
