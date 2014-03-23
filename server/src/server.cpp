#include <stdlib.h>
#include <sndfile.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <server.hpp>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <string>
#include <vector>
#include <map>
#include <iostream>

#define RECV_BUFF_SIZE 1024

using namespace std;

int host_socket;


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


void receive( int socket ){

	cout << "Worker thread started" << endl;
	
	int num_read = 0;
	char buffer[RECV_BUFF_SIZE]; bzero(buffer,RECV_BUFF_SIZE);

	char* payload = new char[RECV_BUFF_SIZE];
	int payload_size = 0;
	int payload_type = 0;

	while((num_read = recv(socket,buffer,RECV_BUFF_SIZE,0))>0){

		if( payload_type == 0){
			char type[] = {(char)buffer[0],(char)buffer[1],'\0'};
			payload_type = atoi(type);

			payload = (char*) realloc( payload, payload_size+num_read-2);
			memcpy(payload+payload_size,buffer+2,num_read-2);
			payload_size += num_read-2;

			// 10 STATIC 
			// 20 DYNAMIC
			cout << "Payload type: " << payload_type << endl;
			
		}
		else {
			payload = (char*) realloc( payload, payload_size+num_read);
			memcpy(payload+payload_size,buffer,num_read);
			payload_size += num_read;
		}
	}

	cout << "Read totally: " << payload_size << " bytes" << endl;

	FILE* fp = fopen("sample.json","w");
	for(int i = 0; i < payload_size; i++)
		fprintf(fp,"%c",payload[i]);

	vector<short> speech = parse_vector(payload);
	
	SF_INFO info;
	info.channels = 1;
	info.samplerate = 16000;
	info.frames = 1;
	info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	SNDFILE* sf = sf_open("sample.wav",SFM_WRITE,&info);

	if( sf == NULL ){
		printf("Failed to open file: sample.wav\n");
		puts(sf_strerror(NULL));
		exit(1);
	}
	int wrote_count = sf_write_short( sf, speech.data(), (size_t)speech.size());
	sf_close(sf);

	
	system("../extraction/bin/extraction ./sample.wav");
	system("../scripts/test_command.sh ./features.json");

}

void setup_server(string host_addr, int host_port){
	
	host_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(host_socket < 0){
		perror("ERROR opening socket");
		exit(1);
	}

	sockaddr_in addr; bzero(&addr, sizeof(sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(host_port);
	//addr.sin_addr.s_addr = INADDR_ANY; 
	addr.sin_addr.s_addr = inet_addr(host_addr.c_str()); 

	if( bind(host_socket, (struct sockaddr*) &addr, sizeof(addr)) < 0){
		perror("ERROR on binding");
		exit(1);
	}

	cout << "Server setup on addres: " << host_addr << ":" << host_port << endl;

}

void loop_listen(){

	struct sockaddr_in client_socket; 
	socklen_t client_socket_size = sizeof(struct sockaddr_in);
	int new_socket;
	vector<thread> threads;

	listen(host_socket, 5);

	cout << "Server started" << endl; 
	cout << "Waiting for connections" << endl;

	while( 1 ){

		new_socket = accept(host_socket, (struct sockaddr*) &client_socket, &client_socket_size);	
		threads.push_back( thread(receive,new_socket) );
		//receive(new_socket);	

	}


}

int main(int argc, char* argv[]){

	string host_addr;
	int host_port;

	if(argc != 3){
		usage(argv);
		return -1;
	}

	host_addr = string(argv[1]);
	host_port = atoi(argv[2]);

	setup_server(host_addr, host_port);	

	loop_listen();

	return 1;

}

void usage( char* argv[] ){
	cout << "Usage: " << argv[0] << " HOST_ADDRES HOST_PORT" << endl; 
}

