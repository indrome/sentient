#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <assert.h>
#include <vector>

using namespace std;

#define MIN(a,b) ((a < b )?a:b)

static inline float dist_L2(float a, float b){
	return fabs(a-b);
}

static inline float minf(float a, float b){
	return (a < b)?a:b;
}

float dtw_dist(vector<float> s, vector<float> t, int w){

	int N = s.size();
	int M = t.size();
	w = ((float)w > fabs(M-N)) ? w : (int)fabs(M-N); 
	float** DTW = (float**)malloc(sizeof(float*)*(N+1));

	// alloc and init mem
	for( int i = 0; i < N; i++ ){
		DTW[i] = (float*)malloc(sizeof(float)*(M+1));
		for(int j = 0; j < N; j++)
				DTW[i][j] = FLT_MAX;
	}
	DTW[0][0] = 0.0f;

	// compute distance matrix
	for( int i = 1; i < N; i++){
		int start = (1 > i-w) ? 1 : i-w;
		int stop = (M < i+w) ? M : i+w;
		for( int j = start; j < stop; j++) {
			float cost = dist_L2(s[i-1],t[j-1]);
			DTW[i][j] = cost + MIN( MIN( DTW[i-1][j], DTW[i][j-1]), DTW[i-1][j-1] );
		}
	}
	
	return DTW[M-1][N-1];
}
