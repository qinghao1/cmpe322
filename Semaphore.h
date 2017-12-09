#include<stdio.h>
#include<iostream>
#include<deque>
#include<utility>
#include<string.h>
#include <fstream>
#include"Task.h"

struct Semaphore {
	int num;
	bool m_lock;
	std::deque<std::string> starvation_queue;
	std::deque<Task> queue;
	std::string output_filename;

	Semaphore(int n) {
		num = n;
		m_lock = false;
		output_filename = "output_" + std::to_string(num) + ".txt";
	}

	void lock() {
		m_lock = true;
	}

	void unlock() {
		m_lock = false;
	}

	bool get_lock() {
		return m_lock;
	}

	void print_queue() {
		std::ofstream fs;
		fs.open(output_filename, std::fstream::out | std::fstream::app);
		if (queue.empty()) {
			fs << "HEAD--TAIL\n";
		} else {
			fs << "HEAD-";
			for(int i = 0; i < queue.size(); i++) {
				fs << queue.at(i).m_process_name << '-';
			}
			fs << "TAIL\n";
		}
		fs.close();
	}

	void print_time(int time) {
		std::ofstream fs;
		fs.open(output_filename, std::fstream::out | std::fstream::app);
		fs << time << "::";
		fs.close();
	}
};