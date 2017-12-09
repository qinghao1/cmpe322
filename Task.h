struct Task {
	int m_current_line;
	std::string m_process_name;
	std::string m_process_filename;
	Task(int current_line, std::string process_name, std::string process_filename) {
		m_current_line = current_line;
		m_process_name = process_name;
		m_process_filename = process_filename;
	}
};