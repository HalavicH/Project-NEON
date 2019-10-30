#include "stdio.h"
#include "rgb_interpolator.c"


#define SARRLEN 5
#define MULTIPLIER 4.8
#define IARLEN  (int)(SARRLEN*MULTIPLIER+1)


int main(int argc, char const *argv[])
{
	// float multiplier = 4.8; // interpolation multiplier

	int source_arr[SARRLEN][3] = {
		{0,0,0},
		{20,0,0},
		{30,153,0},
		{40,255,0},
		{50,255,0}}; 
	int interpolated_arr[IARLEN][3];

	RGB_interpolator(source_arr, SARRLEN, interpolated_arr, IARLEN, MULTIPLIER);

	for (int i = 0; i < IARLEN; ++i)
	{
		printf("%d iter:  %d, ",i+1, interpolated_arr[i][0]);
		printf("%d, ", interpolated_arr[i][1]);
		printf("%d\n", interpolated_arr[i][2]);
	}

	return 0;
}