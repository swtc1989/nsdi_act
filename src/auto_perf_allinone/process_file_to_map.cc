/*************************************************************************
    > File Name: process_pcap.cpp
  > Author: wsun
  > Mail:sunweiflyus@gmail.com
  > Created Time: Sat 15 Oct 2016 12:12:52 PM CDT
  > Comments:
 ************************************************************************/
#include "share.h"


int read_from_config(char* filename1, vector<struct Test_Parems>& tmp_vec)
{
	ifstream inputFile(filename1);

	if (!inputFile.is_open())
	{
		cerr << "Unable to open config file:" << filename1 << "\n";
		exit(-1);
	}

	if (inputFile)
	{
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
		}
	}

	return 0 ;
}


extern int read_from_file(char* filename1, COVG_MAP_VEC& trace, Config_Map& map_config,  vector<struct Test_Parems>& test_para_vec);

extern struct Record_average average_record;
extern vector<pair<struct Test_Parems, Record_average> > input_output_relation;
int process_to_map(COVG_MAP_VEC& covg_map_vec, Config_Map& config_map, int start, int end, char* folder, int mode, int diff)
{
	int Tries_num = 1;

	vector<struct Test_Parems> vec_test_para;
	char cmd[512];
	for (int i = start; i <= end; i++)
	{
		vec_test_para.clear();
		snprintf (cmd, 512, "%s/config_%d/input_config.txt", folder, i);

		//To update config.txt
		read_from_config(cmd, vec_test_para); // for cwnd;

		//for (int j = 1; j <= Tries_num; j++)
		{
			snprintf (cmd, 512, "%s/config_%d/%d/messages", folder, i, i + diff);
			//cout << cmd << endl;
			read_from_file(cmd, covg_map_vec, config_map, vec_test_para);
		}
		//if (DEBUG)
		{
			cout << "process folder:" << i << endl;
		}

		if (mode == 1)// need corrleation
			input_output_relation.push_back(make_pair(vec_test_para[0], average_record));
	}

	return 0;
}
