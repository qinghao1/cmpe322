#include<string.h>
struct Incoming {
	std::string m_process_name;
	std::string m_process_filename;
	int m_arrival_time;
	Incoming(std::string process_name, std::string process_filename, int arrival_time) {
		m_process_name = process_name;
		m_process_filename = process_filename;
		m_arrival_time = arrival_time;
	}
};