#include<string.h>
struct Incoming {
	char m_process_name[5];
	char m_process_filename[20];
	int m_arrival_time;
	Incoming(const char *process_name, const char *process_filename, int arrival_time) {
		strcpy(m_process_name, process_name);
		strcpy(m_process_filename, process_filename);
		m_arrival_time = arrival_time;
	}
};