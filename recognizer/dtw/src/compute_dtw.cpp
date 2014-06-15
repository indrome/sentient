#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

#ifdef _PROFILE 
#include <gperftools/profiler.h>
#endif

#include "dtw.hpp"

using namespace std;

void usage(char* argv[]){
	cout << "Usage: " << argv[0] << " FILE1 FILE2" << endl;
}

static int fpeek(FILE* const fp){
	const int c = getc(fp);
	return c == EOF ? EOF : ungetc(c, fp);
}

static void check_token(FILE* fp,char tok){
	char c = fgetc(fp); if(c!=tok) {
		cout << "ERROR: expected " << tok << " found " << c << endl;
		exit(1);
	}

}

static vector<float> parse_vector(FILE* fp, string fmt){
	check_token(fp,'[');
	float feature;
	vector<float> row;

	while(fscanf(fp,fmt.c_str(), &feature) > 0 ){
		row.push_back(feature);	

		char c = fgetc(fp);
		if(c == ' ') c = fgetc(fp);
		if(c == ']') break;

	}

	//cout << "Parsed row: " << row.size() << " elements" << endl;
	return row;
}


vector<float> get_vector_from_json(const char* fname){

	FILE* fp = fopen(fname,"r");
	if(!fp){
		perror("ERROR opening file");
		exit(1);
	}

	check_token(fp,'{'); 
	check_token(fp,'"');

	// Get key
	string s("");
	while( isalpha(fpeek(fp)) ){
		s += fgetc(fp);
	}
	check_token(fp,'"');	
	check_token(fp,':');	


	check_token(fp,'[');	
	vector<float> features;
	vector<float> row = parse_vector(fp,"%f"); 

	while( row.size() > 0 ){

		//features.push_back( row );
		for( auto e : row ){
			features.push_back( e );
		}

		char c = fgetc(fp);
		if(c==',') row = parse_vector(fp,"%f");
		else break;

	}


	return features;
}

int main(int argc, char* argv[]) {

	if(argc < 3){
		usage(argv);
		exit(1);
	}

	int num_files_in = argc-2;

	char* fname1 = argv[1];
	vector< vector<float> >  feature_vectors;
	vector< string > feature_names;

	for(int i = 2; i < num_files_in+2; i++) {

		auto v = get_vector_from_json( argv[i] );	
		feature_vectors.push_back( v );
		feature_names.push_back( (string)argv[i] );

	}

	#ifdef _PROFILE
	ProfilerStart("/tmp/prof.out");
	#endif

	auto v1 = get_vector_from_json(fname1);

	for( int i = 0; i < feature_names.size(); i++ ){ 
		auto v2 = feature_vectors[i];
		auto f2 = feature_names[i];
		float dist = dtw_dist(v1,v2,32);
		printf("%s %f\n",f2.c_str(), dist);
	}

	#ifdef _PROFILE
	ProfilerStop();
	#endif




	return 1;
}
