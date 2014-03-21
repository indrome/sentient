#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>

#include <aquila/ml/Dtw.h>

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

static vector<double> parse_vector(FILE* fp, string fmt){
	check_token(fp,'[');
	float feature;
	vector<double> row;

	while(fscanf(fp,fmt.c_str(), &feature) > 0 ){
		row.push_back(feature);	

		char c = fgetc(fp);
		if(c == ' ') c = fgetc(fp);
		if(c == ']') break;

	}

	//cout << "Parsed row: " << row.size() << " elements" << endl;
	return row;
}


Aquila::DtwDataType get_vector_from_json(const char* fname){

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

	//cout << "Read key: " << s << endl;

	check_token(fp,'[');	
	Aquila::DtwDataType features;
	vector< double > row = parse_vector(fp,"%f"); 

	while( row.size() > 0 ){

		//for( int i = 0; i < row.size(); i++ ){
		//	features.push_back( row[i] );
		//}
		features.push_back( row );

		char c = fgetc(fp);
		if(c==',') row = parse_vector(fp,"%f");
		else break;

	}

	//cout << "Value vector has " << features.size() << " elements" << endl;

	return features;
}

int main(int argc, char* argv[]) {

	if(argc != 3){
		usage(argv);
		exit(1);
	}

	char* fname1 = argv[1];
	char* fname2 = argv[2];

	auto v1 = get_vector_from_json(fname1);
	auto v2 = get_vector_from_json(fname2);

	Aquila::Dtw dtw;
	double dist = dtw.getDistance(v1,v2);

	cout << dist << endl;


	return 1;
}
