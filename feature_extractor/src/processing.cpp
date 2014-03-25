#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fftw3.h>
#include <assert.h>
#include <math.h>
#include <utility>
#include <support.hpp>
#include <vector>


using namespace std;

static precision mean(vector<precision> data){
	precision sum = 0.0;
	for( precision e : data )	
		sum += e;
	return 1.0/(precision)data.size()*sum;
}

static precision sample_std(vector<precision> data, precision sample_mean){
	precision sum = 0.0;
	for(precision e : data){
		sum += (e-sample_mean)*(e-sample_mean);
	}
	return sqrt(1.0/(precision)data.size()*sum);	
}

vector<int> online_VAD(precision* speech, const int length){
	for( int i = 0; i < length; i++){
		
	}

	return vector<int>();
}
vector<int> offline_VAD(precision* speech, const int length){

	vector<precision> speech_vec;
	for(int i = 0; i < length; i++) speech_vec.push_back(speech[i]);

	precision xbar = mean(speech_vec);
	precision xstd = sample_std(speech_vec, xbar);

	for( int i = 0; i < length; i++) {
		
	}

	return vector<int>();
}

pair<precision**,int> frame_speech( precision* speech, const int length, const int frame_length, const int frame_step) {

	// calculate how many frames are possible to make

	int num_frames = 1;

	if( length > frame_length )
		num_frames = 2 + (length-frame_length)/frame_step;

	// if the frame length is not a true divisor of the length, compute padding
	
	int padding = num_frames*frame_step + frame_length - length;

	// TODO: pointer is changed
	speech = (precision*) realloc( speech, (length+padding)*sizeof(precision) );

	if( speech == NULL) {
		perror("Unable to expand size of speech vector\n");
		printf("FIX THIS\n");
	}
	
	memset( speech+length, 0, padding*sizeof(precision) );
	
	printf("Data length was: %d\tPadded data length: %d\n",length,length+padding);
		
	// compute frames
	precision** frames = new precision*[num_frames];
	for(int i = 0; i < num_frames; i++) { 
		frames[i] = new precision[frame_length];
		memset(frames[i],0,sizeof(precision)*frame_length);
	}

	int start = 0;
	int stop = frame_length;

	for( int fnum = 0; fnum < num_frames; fnum++){
		for( int j = start; j < stop; j++){
			frames[fnum][j-start] = speech[j];
		}
		start += frame_step;
		stop += frame_step;
	}

	pair<precision**,int> out(frames, num_frames);

	printf("Frame step: %d\n",frame_step);
	printf("Frame length: %d\n",frame_length);
	printf("Num frames: %d\n",num_frames);
	printf("Frame matrix: (%d,%d)\n",num_frames,frame_length);

	return out;
}

void preemphasis(precision* speech, int length){
	precision alpha = 0.95;	
	for(int i = 1; i < length; i++ ){
		speech[i] = speech[i] - alpha*speech[i];
	}
}

precision** power_spectra(precision** frames, int num_frames, int nFFT){
	
	precision scale = 1.0/((precision)nFFT)*0.001;
	int spectrum_len = nFFT/2+1;

	fftwf_plan my_planf;
	fftw_plan my_plan;

	float* inf = (float*) fftw_malloc(sizeof(float)*nFFT);
	double* in = (double*) fftw_malloc(sizeof(double)*nFFT);
	fftwf_complex* outf = fftwf_alloc_complex(spectrum_len);
	fftw_complex* out = fftw_alloc_complex(spectrum_len);
		

	precision** F = new precision*[num_frames];
	for(int i = 0; i < num_frames; i++) {
		F[i] = new precision[spectrum_len];
		memset(F[i],0,sizeof(precision)*spectrum_len);
	}

	if(sizeof(precision)==sizeof(float))
		my_planf = fftwf_plan_dft_r2c_1d( nFFT, inf, outf, FFTW_ESTIMATE );
	else
		my_plan = fftw_plan_dft_r2c_1d( nFFT, in, out, FFTW_ESTIMATE );


	for( int i = 0; i < num_frames; i++){
		if(sizeof(precision)==sizeof(float)) {
			memcpy(inf,frames[i],sizeof(precision)*nFFT);
			fftwf_execute( my_planf );	
			for( int j = 0; j < spectrum_len; j++) {
				F[i][j] = scale*(outf[j][0]*outf[j][0] + outf[j][1]*outf[j][1] );
				assert(F[i][j]==F[i][j]);
			}
		}
		else {
			memcpy(in,frames[i],sizeof(precision)*nFFT);
			fftw_execute( my_plan );
			for( int j = 0; j < spectrum_len; j++) {
				F[i][j] = scale*(out[j][0]*out[j][0] + out[j][1]*out[j][1] );
				assert(F[i][j]==F[i][j]);
			}
		}
	}

	printf("Spectra dimensions: %d %d\n",num_frames,spectrum_len);

	free(in);
	free(out);

	// compute the power spectra abs( FFT(frames).^2 )*1/nFFT
	return F;

}

precision** triangle_filterbank(int num_filters, int nFFT, int filter_length, int Hz) {

	precision f_low = 0.0;
	precision f_hi = 8000.0;
	precision m_low = 2595.0*log( 1 + f_low/700.0 );
	precision m_hi  = 2595.0*log( 1 + f_hi /700.0 );
	precision delta_m = (m_hi-m_low)/((precision)num_filters+1.0);

	precision m[num_filters+2]; memset(m,0,sizeof(precision)*num_filters+2);
	precision h[num_filters+2]; memset(h,0,sizeof(precision)*num_filters+2);
	precision f[num_filters+2]; memset(f,0,sizeof(precision)*num_filters+2);

	// compute mel-frequency 
	for( int i = 0; i < num_filters+2; i++) {
		m[i] = m_low + delta_m*i; 
		h[i] = 700.0*(exp(m[i]/2595.0)-1.0);
		f[i] = floor((nFFT+1)*h[i]/(precision)Hz+1.0 )-1;
	}

	precision** H = new precision*[num_filters];
	for( int i = 0; i < num_filters; i++){
		H[i] = new precision[filter_length];
		memset(H[i],0,sizeof(precision)*filter_length);
	}

	for( int j = 0; j < num_filters; j++){
		for( int i = (int)f[j]; i <= (int)f[j+1]; i++){ 
			precision a = ((precision)i - f[j])/(f[j+1]-f[j]); 
			H[j][i] = a;
		}
		for( int i = (int)f[j+1]; i <= (int)f[j+2]; i++){
			precision a = (f[j+2]-(precision)i)/(f[j+2]-f[j+1]);
			H[j][i] = a;
		}
	}

	printf("Triangle filter dimensions: (%d,%d)\n",num_filters,filter_length);

	return H; 
} 
	
precision** matmul(precision** S, precision** H, const int Srows, const int Scols, const int Hrows, const int Hcols) { 
	// Normally, it is required that M x N * N x P
	// Since, H is transposed:
	assert(Scols == Hcols);

	// Normally C is M x P
	// Since H is transposed: C is M x N
	precision** M = new precision*[Srows];
	for( int i = 0; i < Srows; i++){
		M[i] = new precision[Hrows];
		memset(M[i],0,sizeof(precision)*Hrows);
	}

	// Pretend H is transposed.
	for(int i = 0; i < Srows; i++){
		for(int j = 0; j < Hrows; j++) {
			precision c = 0.0;
			for(int k = 0; k < Scols; k++) {
				c += S[i][k]*H[j][k];	
			}
			if( c < 1e-30 ) c = 1e-30;
			M[i][j] = log2(c);
			assert(M[i][j] == M[i][j]);
		}
	}
	printf("MFCC matrix is: (%d,%d)\n",Srows,Hrows);
	return M;
}

precision** dct(precision** M, const int SM, const int SN){

	fftwf_plan my_planf;
	fftw_plan my_plan;

	float* inf = new float[SN];  memset(inf,0,sizeof(float)*SN);
	double* in = new double[SN];  memset(in,0,sizeof(double)*SN);
	float* outf = new float[SN]; memset(outf,0,sizeof(float)*SN);
	double* out = new double[SN]; memset(out,0,sizeof(double)*SN);

	precision** D = new precision*[SM];
	for(int i = 0; i < SM; i++){
		D[i] = new precision[SN]; 
		memset(D[i],0,sizeof(precision)*SN); 
	}

	my_planf = fftwf_plan_r2r_1d(SN, inf, outf, FFTW_REDFT10, FFTW_ESTIMATE );
	my_plan = fftw_plan_r2r_1d(SN, in, out, FFTW_REDFT10, FFTW_ESTIMATE );

	for(int i = 0; i < SM; i++){
		if(sizeof(precision)==sizeof(float)) {
			memcpy(inf,M[i],sizeof(float)*SN);
			fftwf_execute( my_planf );
			for( int j = 0; j < SN; j++) {
				D[i][j] = (j == 0) ? 1.0/sqrt(2.0)*out[j] : sqrt(2.0/(precision)SN)*out[j];
				assert(D[i][j]==D[i][j]);
			}
		}
		else {
			memcpy(in,M[i],sizeof(double)*SN);
			fftw_execute( my_plan );

			for( int j = 0; j < SN; j++) {
				D[i][j] = (j == 0) ? 1.0/sqrt(2.0)*out[j] : sqrt(2.0/(precision)SN)*out[j];
				assert(D[i][j]==D[i][j]);
			}
		}
	}

	return D;
	
}

void lifter(precision** D, const int SM, const int SN){

	precision L = 22.0;

	precision* lifty = new precision[SN];
	for( int i = 0; i < SN; i++){
		lifty[i] = 1.0+(L-1.0)/2.0*sin(M_PI*(precision)i/(L-1.0));
	}
	
	for(int i = 0; i < SM; i++){
		for(int j = 0; j < SN; j++){
			D[i][j] = D[i][j]*lifty[j];
			if( !isfinite(D[i][j]) )
				D[i][j] = 0.0f;
			assert(D[i][j]==D[i][j]);
		}
	}
}

precision** extract_coefficients(precision** D, int rows, int cols, int num_coeffs, precision** S, int SM){
	
	precision** MFCC = new precision*[rows];
	for(int i = 0; i < rows; i++){
		MFCC[i] = new precision[num_coeffs];

		MFCC[i][0] = 0.0;
		for(int k = 0; k < SM; k++){ 
			MFCC[i][0] += S[i][k];
		}
		MFCC[i][0] = log(MFCC[i][0]*1e10);

		for( int j = 1; j < num_coeffs; j++)
			MFCC[i][j] = D[i][j];
	}
	return MFCC;
}
