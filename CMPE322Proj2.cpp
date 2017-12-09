#include<stdio.h>
#include<string.h>
#include<iostream>
#include<deque>
#include<utility>
#include"Incoming.h"
#include"Semaphore.h"

using namespace std;

//Constants
const string OUTPUT_FILE = "output.txt";
const int TIME_QUANTUM = 100;

//Semaphores
Semaphore semaphores[10] = {
	Semaphore(1),
	Semaphore(2),
	Semaphore(3),
	Semaphore(4),
	Semaphore(5),
	Semaphore(6),
	Semaphore(7),
	Semaphore(8),
	Semaphore(9),
	Semaphore(10)
};

//Queues
//Queue of incoming tasks is needed so that we know when to add Tasks to the queue
deque<Incoming> incoming_queue;

//RR queue is implemented as a queue of Task objects
deque<Task> task_queue;

void print_time(int time) {
	ofstream fs;
	fs.open(OUTPUT_FILE, fstream::out | fstream::app);
	fs << time << "::";
	fs.close();
}

void print_queue(deque<Task> q) {
	ofstream fs;
	fs.open(OUTPUT_FILE, fstream::out | fstream::app);
	if (q.empty()) {
		fs << "HEAD--TAIL\n";
	} else {
		fs << "HEAD-";
		for(int i = 0; i < q.size(); i++) {
			fs << q.at(i).m_process_name << '-';
		}
		fs << "TAIL\n";
	}
	fs.close();
}

/* "Runs" process for as long as time quantum allows
 * @param current_runtime
 * @param instruction number of instructions to skip
 * @param process_name P1, P2, P3 etc
 * @param filename
 * @return pair<int, int> pair of last line that was executed
 * (-1 means process has exited or entered semaphore wait queue) and total execution time
 */
pair<int, int> run_process(int current_runtime, int instruction, string process_name, string filename) {
	//File operations
	ifstream fs;
	fs.open(filename, fstream::in);

	//Initialize loop variables
	int runtime_total = 0;
	int current_instr = 0;
	string instruction_name;
	int current_instr_runtime;

	while(fs >> instruction_name && fs >> current_instr_runtime) {
		//Skip to current instruction
		if (++current_instr <= instruction) continue;

		//Start execution
		runtime_total += current_instr_runtime;
		cout << instruction_name << "\t" << process_name << "\t" << current_instr_runtime << "\t" << runtime_total << "\t" << current_runtime + runtime_total << '\n';
		if (runtime_total >= TIME_QUANTUM) {
			//Exceed time quantum
			return make_pair(current_instr, runtime_total);
		} else if (instruction_name.compare("exit") == 0) {
			//Finish execution
			return make_pair(-1, runtime_total);
		} else if (instruction_name.compare(0, 5, "waitS") == 0) {
			//Check semaphore
			int semaphore_num = (instruction_name[6] - '0') - 1;
			Semaphore &sem = semaphores[semaphore_num];
			if (!sem.get_lock() && 
					(sem.starvation_queue.empty() || sem.starvation_queue.front().compare(process_name) == 0)) {
				if (!sem.starvation_queue.empty())	sem.starvation_queue.pop_front();
				//Use and lock
				sem.lock();
			} else {
				//Semaphore being used
				sem.starvation_queue.push_back(process_name);
				sem.queue.push_back(Task(current_instr - 1, process_name, filename));
				sem.print_time(current_runtime + runtime_total);
				sem.print_queue();
				return make_pair(-1, runtime_total);
			}
		} else if (instruction_name.compare(0, 5, "signS") == 0) {
			//Release semaphore
			int semaphore_num = (instruction_name[6] - '0') - 1;
			Semaphore &sem = semaphores[semaphore_num];
			sem.unlock();
			//Release 1 process if queue not empty
			if (!sem.queue.empty()) {
				task_queue.push_back(sem.queue.front());
				sem.queue.pop_front();
				sem.print_time(current_runtime + runtime_total);
				sem.print_queue();
				print_time(current_runtime + runtime_total);
				print_queue(task_queue);
			}
		}
	}
	
	fs.close();

	//In case file is faulty
	return make_pair(-1, 0);
}

int main(int argc, const char * argv[]) {

	//Initialize loop variables
	char process_name[5];
	char process_filename[20];
	int process_arrival_time;

	FILE *f = freopen("definition.txt", "r", stdin);

	//Populate incoming_tasks queue
	while(scanf("%s %s %d", process_name, process_filename, &process_arrival_time) == 3) {
		incoming_queue.push_back(Incoming(process_name, process_filename, process_arrival_time));
	}

	fclose(f);

	int time = -TIME_QUANTUM;
	int i = 0;

	cout << "CONSOLE OUTPUT EXPLANATION:\n"
					"instr_name--process--intr_runtime--process_runtime--total_runtime\n";

	//Main scheduling logic
	while(!incoming_queue.empty() || !task_queue.empty()) {
		bool enqueue_running_process = false;
		Task incomplete_process = Task(0, "", ""); //Placeholder
		if (!task_queue.empty()) {
			Task current_task = task_queue.front();
			task_queue.pop_front();
			pair<int, int> current_task_status = run_process(time, current_task.m_current_line, current_task.m_process_name, current_task.m_process_filename);
			if (current_task_status.first == -1) {
				//Finished execution, do nothing
			} else {
				//Set boolean so that we enqueue running process after adding incoming processes
				enqueue_running_process = true;
				incomplete_process = Task(current_task_status.first, current_task.m_process_name, current_task.m_process_filename);
			}
			time += current_task_status.second;
		} else {
			time += TIME_QUANTUM;
		}
		while (!incoming_queue.empty() && time >= incoming_queue.front().m_arrival_time) {
			//Add new process(es) to queue
			Incoming new_task = incoming_queue.front();
			incoming_queue.pop_front();
			task_queue.push_back(Task(0, new_task.m_process_name, new_task.m_process_filename));
		}
		if (enqueue_running_process) {
			//Add incomplete process to task_queue
			task_queue.push_back(incomplete_process);
		}

		print_time(time);
		print_queue(task_queue);
	}

	return 0;
}