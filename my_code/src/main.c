#include "./include/codexion.h"

int main(int argc, char *argv) {
	// we create the structure for the simulation
	t_sim sim;

	// we fill all the structure with zeros
	memset(&sim, 0, sizeof(t_sim));

	if (argc < 7) {
		printf("Arguments: coders_number time_burnout time_compile time_debug time_refactor required_compiles");
		return 1;
	}

	sim.number_of_coders = argv[1];
	sim.time_to_burnout = argv[2];
	sim.time_to_compile = argv[3];
	sim.time_to_debug = argv[4];
	sim.time_to_refactor = argv[5];
	sim.number_of_compiles_required = argv[6];

	// we initialize the locks
	pthread_mutex_init(&sim.stop_mutex, NULL);
	pthread_mutex_init(&sim.print_mutex, NULL);

	sim.coders = (t_coder *)malloc(sizeof(t_coder) * sim.number_of_coders);

	sim.start_time = get_time_in_ms();

	return 0;
}
