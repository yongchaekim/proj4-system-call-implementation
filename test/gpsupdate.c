include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <linux/kernel.h>
#include "gps.h"

#define __NR_set_gps_location 380

int main(int argc, char** argv)
{
	if(argc < 6)
	{
		printf("Some input for GPS locations are missing\n");
		exit(1);
	}

        struct gps_location loc;

	loc.lat_integer = atoi(argv[1]);
	loc.lat_fractional = atoi(argv[2]);
	loc.lng_integer = atoi(argv[3]);
	loc.lng_fractional = atoi(argv[4]);
	loc.accuracy = atoi(argv[5]);

        if(syscall(__NR_set_gps_location, &loc))
	{
		printf("Unsuccessful in setting GPS location\n");
		exit(1);
	}

	printf("Successfully added new GPS location\n");
        return 0;
}
