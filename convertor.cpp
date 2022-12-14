#include "convertor.h"

const int sampleRate = 44100; //количество сэмплов в секунду

void mix_converter::help()
{
	cout << "mix plugin:" << endl;
	cout << "mix 2 streams" << endl;
	cout << "example(this should be in config): " << endl;
	cout << "mix $<number of file in command arg> <time1>" << endl;
	cout << "<time1> - start time to mix two streams " << endl;
	cout << endl;
}

short* mix_converter::dowork(args& arg) {
	int begin = stoi(arg.argss[2]) * sampleRate;
	short* additionalStream = arg.addStreams;
	
	for (size_t i = begin; i < arg.sizeofmainstream; i++)
	{
		if (i == arg.sizeofaddstreams) 
			break;
		arg.mainStream[i] = (arg.mainStream[i] + additionalStream[i]) / 2;
	}

	return arg.mainStream;

}


void mute_converter::help() {
	cout << "mute plugin:" << endl;
	cout << "mute stream from start point to end point" << endl;
	cout << "example(this should be in config): " << endl;
	cout << "mute <time1> <time2>" << endl;
	cout << "time1 - start point, time2 - end point" << endl;
	cout << "!this plugin doesn't support additional streams!" << endl;
	cout << endl;
}

short* mute_converter::dowork(args& arg) {
	if (arg.addStreams != 0) 
		throw invalid_argument("Too many arguments. This plugin doesn't support additional streams!");

	short* mainStream = arg.mainStream;

	int begin = std::stoi(arg.argss[1]);
	int end = std::stoi(arg.argss[2]);

	begin *= sampleRate; end *= sampleRate;

	if (begin > arg.sizeofmainstream|| end > arg.sizeofmainstream ) 
		throw invalid_argument("Arguments [time] is out of wav file length.");

	while (begin != end)
	{
		mainStream[begin] = 0;
		begin++;
	}

	return mainStream;
}


void louder_converter::help() {
	cout << "loud plugin:" << endl;
	cout << "make sound louder" << endl;
	cout << "example(this should be in config): " << endl;
	cout << "loud <time1> <time2>" << endl;
	cout << "time1 - start point, time2 - end point" << endl;
	cout << "!this plugin doesn't support additional streams!" << endl;
	cout << endl;
}

short* louder_converter::dowork(args& arg) {
	if (arg.addStreams != 0)
		throw invalid_argument("too many arguments, this plugin doesn't support additional streams");

	short* mainStream = arg.mainStream;

	int begin = std::stol(arg.argss[1]);
	int end = std::stol(arg.argss[2]);

	begin *= sampleRate; end *= sampleRate;

	if (begin > arg.sizeofmainstream || end > arg.sizeofmainstream)
		throw invalid_argument("arguments <time> are out of wav file length.");

	while (begin != end)
	{
		mainStream[begin] *= 1.8;
		begin++;
	}

	return mainStream;
}