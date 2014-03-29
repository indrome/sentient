#include <sndfile.h>
#define precision double

void write_json(precision* speech, int speech_len, precision** MFCC, int num_features, int feature_len);
SF_INFO get_wav_info(const char* filename);
precision** padd_frames(precision** frames, int num_frames, int frame_length, int nFFT);
void print_wav_info(const char *filename);
precision* read_wav_data(const char* filename);
void write_speech(precision* speech, int num_elements);
void write_matrix_flat(const char* fname, precision** Mat, const int M, const int N);
void write_matrix_flat(const char* fname, short** Mat, const int M, const int N);

