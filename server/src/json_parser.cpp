#include <vector>
#include <iostream>

using namespace std;

static inline char* forward(char* stream){
	while( *stream == ' ' ){
		stream++;
	}
	return stream;
}
static inline char* check_token(char* stream, char tok){
	if(*stream != tok){
		cout << "ERROR: pasing JSON found " << *stream << " expected " << tok << endl;
		exit(1);
	}
	return stream+1;
}
static inline string get_key(char* stream){

	string keyword;

	cout << "Checking for keywords" << endl;

	stream = forward(stream);
	stream = check_token(stream,'"');

	while(*stream != '"' || isalpha(*stream)){
		keyword += *stream;
		stream++;
	}

	stream = check_token(stream,'"');
	stream = forward(stream);	
	
	cout << "Found keyword " << keyword << endl;
	return keyword;
}

static vector<short> get_vector(char* stream){

	int signal;

	vector<short> speech;
	
	stream = forward(stream);
	stream = check_token(stream,'[');
	stream = forward(stream);


	while( sscanf(stream,"%d",&signal) > 0 ){

		if((short)signal == 0)
			speech.push_back((short)1);
		else
			speech.push_back((short)signal);	
		while(isdigit(*stream) || *stream=='.' || *stream=='e' || *stream=='-') { 
			stream++;	
		}
		stream = forward(stream);
		if(*stream==',')
			stream++;
		else
			break;
	}
	cout << endl;

	stream = forward(stream);
	stream = check_token(stream,']');

	cout << "Parsed vector with " << speech.size() << " elements" << endl;

	return speech;
}

vector<short> parse_vector(char* stream){

	vector<short> value;

	stream = forward(stream);
	stream = check_token(stream,'{');
	string keyword = get_key(stream);
	stream += keyword.size()+4;
	stream = forward(stream);

	if( keyword.compare( string("DATA") )==0){

		value = get_vector(stream);
		stream = forward(stream);

	}

	return value;
}

