
#include <iostream>
#include <string>
#include <vector>

using namespace std;

typedef struct arguments {
	short* mainStream;
	short* addStreams;
	int sizeofaddstreams;
	int sizeofmainstream;
	int numberofaddstreams;
	vector<string> argss;
}args;

class base_convertor {
public:

	virtual void help() = 0;
	virtual short* dowork(args& arg) = 0;
	virtual ~base_convertor() = default;
	
};

class mute_converter : public base_convertor {
public:
	mute_converter() = default;
	~mute_converter() = default;
	short* dowork(args& arg);
	void help();
};

class mix_converter : public base_convertor {
public:
	mix_converter() = default;
	~mix_converter() = default;
	short* dowork(args & arg);
	void help();
};

class louder_converter : public base_convertor {
public:
	louder_converter() = default;
	~louder_converter() = default;
	short* dowork(args & arg);
	void help();
};