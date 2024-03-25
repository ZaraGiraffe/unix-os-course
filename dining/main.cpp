#include <iostream>
#include <pthread.h>
#include <deque>
#include <vector>
#include <algorithm>
#include <unistd.h>

#define N_PHIL 5
#define N_PORTIONS 20

#define right_fork(i) ((i + 1) % N_PHIL)
#define left_fork(i) (i)
#define right_phil(i) ((i + 1) % N_PHIL)
#define left_phil(i) ((i - 1 + N_PHIL) % N_PHIL)


pthread_mutex_t fork_mutex[N_PHIL];

std::deque<int> priority;
pthread_mutex_t priority_mutex;

std::vector<int> eating_order;
pthread_mutex_t eating_order_mutex;

pthread_t phil_thread[N_PHIL];


struct phil_params {
	int phil_id;
	int num_portions;
} thread_phil_params[N_PHIL];


void init_dinner() {
	for (int i = 0; i < N_PHIL; i++) {
		thread_phil_params[i].phil_id = i;
		thread_phil_params[i].num_portions = N_PORTIONS;
	}
}


void eat(int phil_id) {
	// the function that represents eating

	pthread_mutex_lock(&eating_order_mutex);
	pthread_mutex_lock(&priority_mutex);

	eating_order.push_back(phil_id);
	priority.erase(std::find(priority.begin(), priority.end(), phil_id));
	priority.push_back(phil_id);
	sleep(0.75); // eating simulation
	
	pthread_mutex_unlock(&eating_order_mutex);
	pthread_mutex_unlock(&priority_mutex);
} 


void* philosopher_func(void* params) {
	// the function that every philosopher does 
	
	struct phil_params* prms = (struct phil_params*)params;
	int phil_id = prms->phil_id;
	int num_portions = prms->num_portions;

	pthread_mutex_lock(&priority_mutex);
	priority.push_back(phil_id);
	pthread_mutex_unlock(&priority_mutex);

	int cnt_portions = 0;
	while (cnt_portions < num_portions) {
		
		// if the philosopher is the first in priority
		if (priority.front() == phil_id) {
			pthread_mutex_lock(&fork_mutex[left_fork(phil_id)]);
			pthread_mutex_lock(&fork_mutex[right_fork(phil_id)]);
			eat(phil_id);
			cnt_portions++;
			pthread_mutex_unlock(&fork_mutex[left_fork(phil_id)]);
			pthread_mutex_unlock(&fork_mutex[right_fork(phil_id)]);
		}

		// if philosopher is NOT the first in priority
		else {
			if (priority.front() == left_phil(phil_id) || priority.front() == right_phil(phil_id)) 
				continue;
			int try_left = pthread_mutex_trylock(&fork_mutex[left_fork(phil_id)]);
			int try_right = pthread_mutex_trylock(&fork_mutex[right_fork(phil_id)]);
			if (try_left == 0 && try_right == 0) {
				eat(phil_id);
				cnt_portions++;
				pthread_mutex_unlock(&fork_mutex[left_fork(phil_id)]);
				pthread_mutex_unlock(&fork_mutex[right_fork(phil_id)]);
			}
			else {
				if (try_left == 0) 
					pthread_mutex_unlock(&fork_mutex[left_fork(phil_id)]);
				if (try_right == 0) 
					pthread_mutex_unlock(&fork_mutex[right_fork(phil_id)]);
			}
		}
		
	}
	
	pthread_mutex_lock(&priority_mutex);
	priority.erase(std::find(priority.begin(), priority.end(), phil_id));
	pthread_mutex_unlock(&priority_mutex);
	
}


int main(int argc, char *argv[]) {
	
	init_dinner();

	for (int i = 0; i < N_PHIL; i++) {
		int err = pthread_create(&phil_thread[i], NULL, philosopher_func, &thread_phil_params[i]);
		// std::cout << "the thread " << i << " is created " << err << std::endl;
	}
	
	for (int i = 0; i < N_PHIL; i++) {
		int err = pthread_join(phil_thread[i], NULL);	
		// std::cout << "the thread " << i << " is joined " << err << std::endl;	
	}
	
	std::cout << "Here is the order of dinning: ";
	for (int i = 0; i < eating_order.size(); i++)  
		std::cout << eating_order[i] << " ";
	std::cout << std::endl;
}





