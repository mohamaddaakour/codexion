#include "../include/codexion.h"

// this function will give us the current time in milleseconds
long long get_time_in_ms() {
	// this is a built in structure in the sys/time.h library
	struct timeval tv;

	// this function to enable taking tv_sec and the tv_usec
	gettimeofday(&tv, NULL);

	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

// to get a precise sleep because usleep or sleep alone are not accurate
void precise_sleep(long long ms) {
	long long start_time = get_time_in_ms();

	while ((get_time_in_ms() - start_time) < ms) {
		usleep(100);
	}
}
