#include "processing.hpp"
#include <stdio.h>
#include <sndfile.h>
#include <math.h>
#include <string.h>
#include <utility>
#include <assert.h>
#include <vector>

using namespace std;

precision** mfcc( precision* speech, int sample_rate, int speech_frames, int Hz, int channels){
	
	assert(channels == 1);
	
	// ------------------
	// Frame Speech
	// ------------------
	int frame_length = 0.025*sample_rate;
	int frame_step = 0.01*sample_rate;

	pair<precision**,int> result = frame_speech(speech, speech_frames*channels, frame_length, frame_step);
	precision** frames = result.first;
	int num_frames = result.second;

	// ------------------
	// Power Spectra
	// ------------------
	int nFFT = 2<<(int)floorf(log2(frame_length));
	frames = padd_frames(frames,num_frames,frame_length,nFFT);
	precision** S = power_spectra(frames,num_frames,nFFT);
	int nFFT_out = nFFT/2+1;

	// ------------------
	// Triangle filterbank 
	// ------------------
	int num_filters = 20;
	precision** H = triangle_filterbank(num_filters, nFFT, nFFT_out, Hz); 

	// ------------------
	// Compute MFCCs
	// ------------------
	precision** M = matmul(S, H, num_frames, nFFT_out, num_filters, nFFT_out);
	precision** D = dct(M, num_frames, num_filters);
	lifter(D, num_frames, num_filters);

	int num_coeffs = 13;
	precision** MFCC = extract_coefficients(D,num_frames, num_filters, num_coeffs,S,nFFT_out);

	write_json(speech, speech_frames*channels, MFCC, num_frames, num_coeffs);

	return MFCC;
}
