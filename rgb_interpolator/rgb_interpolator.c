/*
	version 0.1
	all arrays and variables, exept *arr_len should has "byte" type, 
*/

//returns value from an array, with checking for arr_pos accuracy
int get_colour_src(int src_arr[], int arr_len, int arr_pos)
{
	if (!arr_len) return 0;
	if (arr_pos < 0)  return src_arr[0];
	if (arr_pos >= arr_len)  return src_arr[arr_len-1];
	return src_arr[arr_pos];
}

//returns float casted to (int) interpolated value, according to float position in source array
int get_interpolated_colour(int one_channel_arr[], int arr_len, float i_arr_pos)
{

	int low  = (int) i_arr_pos;   // lower  colour position in current iteration 
	int high = (int) (i_arr_pos+1);  
	int colour_low  = get_colour_src(one_channel_arr, arr_len, low); // lower colour from source arr.
	int colour_high = get_colour_src(one_channel_arr, arr_len, high);

	float coef = i_arr_pos - low; //  coef < 1, is used for finding interpolated value between two basic

// calculating interpolated value itself
	if (coef <= 0) return colour_low;
	if (coef >= 1) return colour_high;
	
	return (int) (colour_low + coef*(colour_high - colour_low));
}

void RGB_interpolator(int source_arr[][3], int s_arr_len, int interpolated_arr[][3], int i_arr_len, float multiplier)
{
	//split two dimention array, into 3 separate arrays by collum
	int s_arr_1st[s_arr_len]; //red channel
	int s_arr_2nd[s_arr_len]; //green channel
	int s_arr_3rd[s_arr_len]; //blue channel

// copy values to 3 separate arrays
	for (int i = 0; i < s_arr_len; ++i)
	{
		s_arr_1st[i] = source_arr[i][0];
		s_arr_2nd[i] = source_arr[i][1];
		s_arr_3rd[i] = source_arr[i][2];

	}

//getting interpolateg values for 3 channels separatelly, and writing it into an resulting array
	for (int i = 0; i < i_arr_len; ++i)
	{
		interpolated_arr[i][0] = get_interpolated_colour(s_arr_1st, s_arr_len, i/multiplier);
		interpolated_arr[i][1] = get_interpolated_colour(s_arr_2nd, s_arr_len, i/multiplier);
		interpolated_arr[i][2] = get_interpolated_colour(s_arr_3rd, s_arr_len, i/multiplier);
	}
}
