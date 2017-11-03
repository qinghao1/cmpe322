#include<stdio.h>
#include<iostream>
#include<deque>
#include<utility>
#include"Task.h"
#include"Incoming.h"

//Constants
const char INPUT_FILE[30] = "definition.txt";
const char OUTPUT_FILE[30] = "output.txt";
const int TIME_QUANTUM = 100;
const int INF = 0x3f3f3f3f;

/* "Runs" process for as long as time quantum allows
 * @param instruction number of instructions to skip
 * @param filename
 * @return std::pair<int, int> pair of last line that was executed (-1 means process has exited) and total execution time if it exited
 */
std::pair<int, int> run_process(int instruction, const char *filename) {

	FILE *f = freopen(filename, "r", stdin);

	//Initialize loop variables
	int runtime_total = 0;
	int current_instr = instruction;
	char instruction_name[100]; //Use big value of 100
	int current_instr_runtime;

	for(int i = 0; i < instruction; i++) scanf(""); //Skip to current instruction

	while(scanf("%s %d", instruction_name, &current_instr_runtime) == 2) {
		current_instr++;

		runtime_total += current_instr_runtime;
		if (strncmp(instruction_name, "exit", 5)) {
			//Finish execution
			return std::make_pair(-1, runtime_total);
		} else if (runtime_total + current_instr_runtime > TIME_QUANTUM) {
			//Can't run current instruction within deadline
			return std::make_pair(current_instr, runtime_total);
		}
	}

	fclose(f);
}

void debug(char* s) {
	FILE *f = freopen(OUTPUT_FILE, "a", stdout);
	printf("%s\n", s);
	fclose(f);
}

void debug(int d) {
	FILE *f = freopen(OUTPUT_FILE, "a", stdout);
	printf("%d\n", d);
	fclose(f);
}

void print_time(int time) {
	FILE *f = freopen(OUTPUT_FILE, "a", stdout);
	printf("%d::", time);
	fclose(f);
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

	//Loop variables
	int time = 0;
	int task_arrival_time = INF;
	int task_completion_time = INF;
	bool task_arrival = false;
	bool task_completion = false;
	std::pair<int, int> current_process_status;
	Task current_task = Task(0, "", "");

	//Main scheduling logic
	while(!incoming_queue.empty() || !task_queue.empty()) {
		debug(time);
		//Check task arrival
		task_arrival = !incoming_queue.empty() && time > incoming_queue.front().m_arrival_time;

		if (!task_queue.empty()) {
			current_task = task_queue.front();
			task_queue.pop_front();
			current_process_status = run_process(current_task.m_current_line, current_task.m_process_filename);
			if (current_process_status.first == -1) {
				task_completion = true;
				int time_taken_current_process = current_process_status.second;
				task_completion_time = time + time_taken_current_process;
			} else {
				//Push new task into RR queue
				task_queue.push_back(Task(current_process_status.first, current_task.m_process_name, current_task.m_process_filename));
			}
		}

		if(task_arrival) {
			Incoming new_task = incoming_queue.front();
			incoming_queue.pop_front();
			//Add new tasks to task queue
			debug(888);
			task_queue.push_back(Task(0, new_task.m_process_name, new_task.m_process_filename));
			task_arrival_time = new_task.m_arrival_time;
		}

		//Handle new task and task completion events
		if (task_arrival && task_completion) {
			debug(1111);
			if (task_arrival_time < task_completion_time) {
				print_time(task_arrival_time);
				print_queue(task_queue);

				print_time(task_completion_time);
				print_queue(task_queue);
			} else {
				print_time(task_completion_time);
				print_queue(task_queue);

				print_time(task_arrival_time);
				print_queue(task_queue);
			}
			task_completion = task_arrival = false;
			task_arrival_time = task_completion_time = INF;
		} else if (task_completion) {
			debug(2222);
			print_time(task_completion_time);
			print_queue(task_queue);
			task_completion = false;
			task_completion_time = INF;
		} else if (task_arrival) {
			debug(3333);
			print_time(task_arrival_time);
			print_queue(task_queue);
			task_arrival = false;
			task_arrival_time = INF;
		} else {
			debug(4444);
			//Nothing happens
			print_time(time);
			print_queue(task_queue);
		}
		time += 100;
	}
	return 0;
}