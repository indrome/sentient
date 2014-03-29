#include <sndfile.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <support.hpp>

void write_json(precision* speech, int speech_len, precision** MFCC, int num_features, int feature_len){

	FILE* fp = fopen("features.json","w");

	if(!fp){
		perror("Could not open file");
		return;
	}
	
	fprintf(fp,"{\"features\":[");
	for(int i = 0; i < num_features; i++) {
		fprintf(fp,"[");
		for( int j = 0; j < feature_len; j++) {
			fprintf(fp,"%f,",MFCC[i][j]); 
		}
		fseek(fp,-1,SEEK_CUR);
		fprintf(fp," ],");
	}
	fseek(fp,-1,SEEK_CUR);
	fprintf(fp,"]}");

	fclose(fp);

	printf("Wrote features to file: features.json\n");

}


SF_INFO get_wav_info(const char* filename){

	SF_INFO info;
	SNDFILE *sf;

	info.format = 0;
	sf = sf_open(filename,SFM_READ,&info);

	if( sf == NULL ){
		printf("Failed to open file: %s\n",filename);
	}

	return info;
}

precision** padd_frames(precision** frames, int num_frames, int frame_length, int nFFT){
	
	assert( nFFT > frame_length );

	for(int i = 0; i < num_frames; i++){
		precision* row = frames[i];
		frames[i] = new precision[nFFT];
		memset(frames[i],0,sizeof(precision)*nFFT);
		memcpy(frames[i],row,sizeof(precision)*frame_length);
		free(row);
	}
	return frames;	
}

void print_wav_info(const char *filename){
	
	SF_INFO info;
	SNDFILE *sf;

	info.format = 0;
	sf = sf_open(filename,SFM_READ,&info);

	if( sf == NULL ){
		printf("Failed to open file: %s\n",filename);
		return;
	}

	printf("Frames:\t\t%d\n",(int)info.frames);
	printf("Sample rate:\t%d\n",info.samplerate);
	printf("Channels:\t%d\n",info.channels);

	
}

precision* read_wav_data(const char* filename){

	SF_INFO info;
	SNDFILE *sf;

	info.format = 0;
	sf = sf_open(filename,SFM_READ,&info);

	if( sf == NULL ){
		printf("Failed to open file: %s\n",filename);
		return NULL;
	}

	int num_samples = info.frames*info.channels;
	short* buffer = new short[num_samples];

	int read_count = sf_read_short(sf,buffer,num_samples);
	printf("Read %d bytefrom %s\n",read_count,filename);

	sf_close(sf);


	// convert to precision
	precision* out = new precision[num_samples];
	precision max = 0.0;
	for(int i = 0; i < num_samples; i++){
		out[i] = (precision)buffer[i];
		max = (max < out[i]) ? out[i] : max;
	}
	for(int i = 0; i < num_samples; i++){
		out[i] *= 1.0/max;
	}

	free( buffer );

	return out;
}

void write_speech(precision* speech, int num_elements){
	FILE* fp = fopen("speech.data","w");
	if(!fp){
		perror("Could not write file for result?");
		exit(1);
	}
	for(int i = 0; i < num_elements; i++){
		fprintf(fp,"%f\n",speech[i]);
	}
	printf("Write matrix flat to speech.data\n");
	fclose(fp);
}

void write_matrix_flat(const char* fname, precision** Mat, const int M, const int N){
	
	FILE* fp = fopen(fname,"w");
	if(!fp){
		perror("Could not write file for result?");
		exit(1);
	}
	
	for(int i = 0; i < M; i++){
		for(int j = 0; j < N; j++){
			fprintf(fp,"%f\n",Mat[i][j]);
		}
	}
	
	printf("Write matrix flat to %s\n",fname);
	fclose(fp);
}	
void write_matrix_flat(const char* fname, short** Mat, const int M, const int N){
	
	FILE* fp = fopen(fname,"w");
	if(!fp){
		perror("Could not write file for result?");
		exit(1);
	}
	
	for(int i = 0; i < M; i++){
		for(int j = 0; j < N; j++){
			fprintf(fp,"%d\n",Mat[i][j]);
		}
	}
	
	printf("Write matrix flat to %s\n",fname);
	fclose(fp);
}	
