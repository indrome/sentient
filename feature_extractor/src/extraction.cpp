#include <support.hpp>
#include <processing.hpp>
#include <sndfile.h>
#include <math.h>
#include <string.h>
#include <utility>

using namespace std;

int main(int argc, char* argv[]) {

	if(argc != 2){
		printf("Usage: %s FILE.wav",argv[0]);
		return -1;
	}

	char filename[256];
	strncpy(filename,argv[1],255);

	printf("Presicion %s\n", (sizeof(precision)>4) ? "double" : "float");


	int Hz = 16000;

	print_wav_info( filename );

	// ------------------
	// Read data, convert, preemphasis 
	// ------------------
	SF_INFO info = get_wav_info( filename );
	precision* speech = read_wav_data( filename );
	precision* speech_cpy = new precision[info.frames*info.channels];
	memcpy(speech_cpy,speech,sizeof(precision)*info.frames*info.channels);


	// ------------------
	// Frame Speech
	// ------------------
	int frame_length = 0.025*info.samplerate;
	int frame_step = 0.01*info.samplerate;

	pair<precision**,int> result = frame_speech(speech, info.frames*info.channels, frame_length, frame_step);
	precision** frames = result.first;
	int num_frames = result.second;

	// ------------------
	// Power Spectra
	// ------------------
	int nFFT = 2<<(int)floorf(log2(frame_length));
	frames = padd_frames(frames,num_frames,frame_length,nFFT);
	precision** S = power_spectra(frames,num_frames,nFFT);
	int nFFT_out = nFFT/2+1;
	//int filter_length = nFFT/2+1;

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

	// ------------------
	// Write result 
	// ------------------
	write_json(speech, info.frames*info.channels, MFCC, num_frames, num_coeffs);


	return 1;
}
