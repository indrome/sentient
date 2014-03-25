#include <utility>
#include <vector>
#include <support.hpp>

using namespace std;

vector<int> VAD(precision* speech, const int length);
pair<precision**,int> frame_speech( precision* speech, const int length, const int frame_length, const int frame_step) ;
void preemphasis(precision* speech, int length);
precision** power_spectra(precision** frames, int num_frames, int nFFT);
precision** triangle_filterbank(int num_filters, int nFFT, int filter_length, int Hz);
precision** matmul(precision** S, precision** H, const int SM, const int SN, const int HM, const int HN);
precision** dct(precision** M, const int SM, const int SN);
void lifter(precision** D, const int SM, const int SN);
precision** extract_coefficients(precision** D, int rows, int cols, int num_coeffs, precision** S, int SM);
