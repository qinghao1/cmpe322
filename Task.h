#include<string.h>
struct Task {
	int m_current_line;
	char m_process_name[5];
	char m_process_filename[20];
	Task(int current_line, const char *process_name, const char *process_filename) {
		m_current_line = current_line;
		strcpy(m_process_name, process_name);
		strcpy(m_process_filename, process_filename);
	}
};