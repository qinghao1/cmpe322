#include<stdio.h>
#include<iostream>
#include<deque>
#include<utility>
#include<string.h>
#include"Task.h"
#include"Incoming.h"

//Constants
const char INPUT_FILE[30] = "definition.txt";
const char OUTPUT_FILE[30] = "output.txt";
const int TIME_QUANTUM = 100;

// void debug(char* s) {
// 	FILE *f = freopen(OUTPUT_FILE, "a", stdout);
// 	printf("DEBUG %s\n", s);
// 	fclose(f);
// }

// void debug(int d) {
// 	FILE *f = freopen(OUTPUT_FILE, "a", stdout);
// 	printf("DEBUG %d\n", d);
// 	fclose(f);
// }

void print_time(int time) {
	if (time == 0) {
		//Write new file
		FILE *f = freopen(OUTPUT_FILE, "w", stdout);
		printf("%d::", time);
		fclose(f);
	} else {
		//Append
		FILE *f = freopen(OUTPUT_FILE, "a", stdout);
		printf("%d::", time);
		fclose(f);
	}
}

void print_queue(std::deque<Task> q) {
	FILE *f = freopen(OUTPUT_FILE, "a", stdout);
	if (q.empty()) {
		printf("%s", "HEAD--TAIL\n");
	} else {
		printf("%s", "HEAD-");
		for(int i = 0; i < q.size(); i++) {
			printf("%s-", q.at(i).m_process_name);
		}
		printf("%s", "TAIL\n");
	}
	fclose(f);
}

/* "Runs" process for as long as time quantum allows
 * @param instruction number of instructions to skip
 * @param filename
 * @return std::pair<int, int> pair of last line that was executed (-1 means process has exited) and total execution time if it exited
 */
std::pair<int, int> run_process(int instruction, const char *filename) {

	FILE *f = freopen(filename, "r", stdin);

	//Initialize loop variables
	int runtime_total = 0;
	int current_instr = 0;
	char instruction_name[100]; //Use big value of 100
	int current_instr_runtime;

	while(scanf("%s\t%d", instruction_name, &current_instr_runtime) == 2) {
		//Skip to current instruction
		if (++current_instr <= instruction) continue;

		runtime_total += current_instr_runtime;
		if (strcmp(instruction_name, "exit") == 0) {
			//Finish execution
			return std::make_pair(-1, runtime_total);
		} else if (runtime_total > TIME_QUANTUM) {
			//Exceed time quantum
			return std::make_pair(current_instr, runtime_total);
		}
	}

	fclose(f);
}

int main(int argc, const char * argv[]) {
	//Queue of incoming tasks is needed so that we know when to add Tasks to the queue
	std::deque<Incoming> incoming_queue;

	//RR queue is implemented as a queue of Task objects
	std::deque<Task> task_queue;

	//Initialize loop variables
	char process_name[5];
	char process_filename[20];
	int process_arrival_time;

	FILE *f = freopen(INPUT_FILE, "r", stdin);

	//Populate incoming_tasks queue
	while(scanf("%s %s %d", process_name, process_filename, &process_arrival_time) == 3) {
		incoming_queue.push_back(Incoming(process_name, process_filename, process_arrival_time));
	}

	fclose(f);

	int time = -100;

	//Main scheduling logic
	while(!incoming_queue.empty() || !task_queue.empty()) {
		if (!task_queue.empty()) {
			Task current_task = task_queue.front();
			task_queue.pop_front();
			std::pair<int, int> current_task_status = run_process(current_task.m_current_line, current_task.m_process_filename);
			if (current_task_status.first == -1) {
				//Finished execution, do nothing
			} else {
				//Add incomplete process to task_queue
				task_queue.push_back(Task(current_task_status.first, current_task.m_process_name, current_task.m_process_filename));
			}
			time += current_task_status.second;
		} else {
			time += 100;
		}
		while (!incoming_queue.empty() && time > incoming_queue.front().m_arrival_time) {
			//Add new process(es) to queue
			Incoming new_task = incoming_queue.front();
			incoming_queue.pop_front();
			task_queue.push_back(Task(0, new_task.m_process_name, new_task.m_process_filename));
		}
		print_time(time);
		print_queue(task_queue);
	}

	return 0;
}