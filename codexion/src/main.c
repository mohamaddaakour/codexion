#include "../include/codexion.h"

int main(int argc, char **argv) {
	// we create the structure for the simulation
	t_sim sim;

	// we fill all the structure with zeros
	memset(&sim, 0, sizeof(t_sim));

	if (argc != 7) {
		printf("Arguments: coders_number time_burnout time_compile time_debug time_refactor required_compiles");
		return 1;
	}

	sim.number_of_coders = atoi(argv[1]);
	sim.time_to_burnout = atoi(argv[2]);
	sim.time_to_compile = atoi(argv[3]);
	sim.time_to_debug = atoi(argv[4]);
	sim.time_to_refactor = atoi(argv[5]);
	sim.number_of_compiles_required = atoi(argv[6]);

	// we initialize the locks
	pthread_mutex_init(&sim.stop_mutex, NULL);
	pthread_mutex_init(&sim.print_mutex, NULL);

	// we allocate an array of coders and dongles
	sim.dongles = (t_dongle *)malloc(sizeof(t_dongle) * sim.number_of_coders);
	sim.coders = (t_coder *)malloc(sizeof(t_coder) * sim.number_of_coders);

	sim.start_time = get_time_in_ms();

	for (int i = 0; i < sim.number_of_coders; i++) {
		pthread_mutex_init(&sim.dongles[i].dongle_mutex, NULL);
	}

	for (int i = 0; i < sim.number_of_coders; i++) {
		sim.coders[i].coder_id = i + 1;
		sim.coders[i].sim = &sim;
		sim.coders[i].last_compile_start = sim.start_time;

		sim.coders[i].left = &sim.dongles[i];
		sim.coders[i].right = &sim.dongles[(i + 1) % sim.number_of_coders];

		// we initialize the lock
		pthread_mutex_init(&sim.coders[i].last_compile_mutex, NULL);
		pthread_mutex_init(&sim.coders[i].compile_count_mutex, NULL);
	}

	// we create a thread for each coder
	for (int i = 0; i < sim.number_of_coders; i++) {
		pthread_create(&sim.coders[i].coder_thread, NULL, coder_routine, &sim.coders[i]);
	}

	// we create a thread for the monitor
	pthread_create(&sim.monitor, NULL, monitor_routine, &sim);

	pthread_join(sim.monitor, NULL);

	// like this we linked all the threads together and with this the program will
	// continue to the end, without join the program may stop without finishing
	for (int i = 0; i < sim.number_of_coders; i++) {
		pthread_join(sim.coders[i].coder_thread, NULL);
	}

	// we destroyed the locks because when we reach this means we finished
	// the program
	for (int i = 0; i < sim.number_of_coders; i++) {
		pthread_mutex_destroy(&sim.coders[i].last_compile_mutex);
		pthread_mutex_destroy(&sim.coders[i].compile_count_mutex);
		pthread_mutex_destroy(&sim.dongles[i].dongle_mutex);
	}

	// we freed the array of coders
	free(sim.coders);
	free(sim.dongles);

	pthread_mutex_destroy(&sim.stop_mutex);
	pthread_mutex_destroy(&sim.print_mutex);

	return 0;
}
