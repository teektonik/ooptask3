#include <Windows.h>
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <string>
#include <regex>
#include <thread>

#include "convertor.h"

using namespace std;

#pragma pack(push, 1)
typedef struct wavfiles {
	char	chunkId[4];
	int		chunkSize;
	int		format;
	int		subchunk1Id;
	int		subchunk1Size;
	short	audioFormat;
	short	numChannels;
	int		sampleRate;
	int		byteRate;
	short	blockAlign;
	short	bitsPerSample;
	int		subchunk2Id;
	int		subchunk2Size;
}wave;

#pragma pack(pop)

//структура которая описывает тэги
typedef struct chunk_s
{
	int id;
	int size;
} chunk;

#pragma pack(push, 1)
typedef struct _Wav_s
{
	wave wav_header;
	short* data;
} _Wav;
#pragma pack(pop)


class read_config {
public:
	string config_file;
	vector<string> lines;
	vector<args> _converterArgs;

	read_config(string& c) {
		config_file = c;
	}
	void parse_config(){
		args* newArgument = new args();
		ifstream fin(config_file);
		string curr;
		newArgument->numberofaddstreams = 0;
		while (!fin.eof()) {
			
			getline(fin, curr);
			
			if (curr.empty() || curr.front() == '#') {
				continue;
			}
			int end = 0;
			int start;
			while ((start = curr.find_first_not_of(' ', end)) != string::npos)
			{
				end = curr.find(' ', start);
				lines.push_back(curr.substr(start, end - start));
			}
			for (auto& x : lines)
				if (x.front() == '$') newArgument->numberofaddstreams++;

			newArgument->mainStream= nullptr;
			newArgument->argss = lines;
			newArgument->addStreams = nullptr;	//TO DO
			newArgument->sizeofaddstreams = 0;
			newArgument->sizeofmainstream = 0;

			this->_converterArgs.push_back(*newArgument);

			lines.clear();

		}
	}

};

class read_files {
public:
	string files;
	_Wav wav_data;
	read_files(string& f) {
		files = f;
	}
	unsigned long long int readfile(string fileName)
	{
		const unsigned long long int _data = 1635017060;

		ifstream fin(fileName, ios_base::binary);
		if (!fin.is_open())
			return 0;

		//узнаю размер файла		
		int file_size = 0;
		fin.seekg(0, ios::end);
		file_size = fin.tellg();
		fin.seekg(0);
		//читаю заголовок 
		fin.read(reinterpret_cast<char*>(&wav_data.wav_header), sizeof(wave));

	


		//обхожу мусор 
		int _trashSize = 0; chunk _tempChunk;
		if (wav_data.wav_header.subchunk2Id != _data)
		{
			while (!fin.eof())
			{
				fin.read(reinterpret_cast<char*>(&_tempChunk), sizeof(chunk));

				if (_tempChunk.id == _data)
				{
					wav_data.wav_header.subchunk2Id = _tempChunk.id;
					wav_data.wav_header.subchunk2Size = _tempChunk.size;
					break;
				}

				if (_tempChunk.size == _data)
				{
					fin.read(reinterpret_cast<char*>(&wav_data.wav_header.subchunk2Size), sizeof(int));
					wav_data.wav_header.subchunk2Id = _tempChunk.size;
					break;
				}
				_trashSize += sizeof(chunk);
			}
		}
		unsigned long long int dataSize = 1LL * file_size - sizeof(wave) - _trashSize;
		wav_data.wav_header.subchunk2Size = file_size;
		wav_data.data = new short[dataSize];

		fin.read(reinterpret_cast<char*>(wav_data.data), 1LL * dataSize);
		fin.close();
		

		return dataSize;
	}

private:
	//wave wav_data;
	
};


int main(int argc, char** argv) {

	bool help=false; string config;
	vector<string> files;
	for (size_t i = 1; i < argc; i++)
	{
		if (argv[i] == string("-h")) {
			help = true;
		}
		else if (argv[i] == string("-c")) {
			config = string(argv[i + 1]);
			i++;
		}
		else {
			files.push_back(string(argv[i]));
		}

	}
	if (help) {
		base_convertor* h = new mute_converter();
		h->help();
		h = new louder_converter();
		h->help();
		h = new mix_converter();
		h->help();
		Sleep(1000);
	}
	if (config.empty()) {
		throw invalid_argument("no config!");
	}
	read_config config1(config);
	config1.parse_config();
	if (files.empty()) {
		throw invalid_argument("no files!");
	}
	read_files main_stream(files[0]);

	int sizem = main_stream.readfile(files[0]);
	
	for (auto comm : config1._converterArgs)
	{
		if (comm.argss[0] == string("mute"))
		{
			args a;
			a.addStreams = comm.addStreams;
			a.mainStream = main_stream.wav_data.data;
			a.sizeofmainstream = sizem;
			a.argss = comm.argss;
			a.numberofaddstreams = comm.numberofaddstreams;
			base_convertor* conv = new mute_converter();
			main_stream.wav_data.data = conv->dowork(a);
		}
		else if (comm.argss[0] == string("mix"))
		{
			args b;
			comm.argss[1].erase(comm.argss[1].begin());
			int k = stoi(comm.argss[1]);
			
			b.mainStream = main_stream.wav_data.data;
			int sizea = main_stream.readfile(files[k-1]);
			b.sizeofmainstream = sizem;
			b.addStreams = main_stream.wav_data.data;
			b.sizeofaddstreams = sizea;
			b.argss = comm.argss;
			base_convertor* conv = new mix_converter();
			main_stream.wav_data.data = conv->dowork(b);
		}
		else if (comm.argss[0] == string("loud"))
		{
			args a;
			a.addStreams = comm.addStreams;
			a.mainStream = main_stream.wav_data.data;
			a.sizeofmainstream = sizem;
			a.argss = comm.argss;
			a.numberofaddstreams = comm.numberofaddstreams;
			base_convertor* conv = new louder_converter();
			main_stream.wav_data.data = conv->dowork(a);
		}
		else {
			throw invalid_argument("bad config!");
		}
	}
	ofstream fout("out.wav", ios::binary);
	//wave to_write { 1179011410 , 21895392, 1163280727, 544501094, 16, 65537, 44100, 88200,  }
	fout.write(reinterpret_cast<char*>(&main_stream.wav_data.wav_header), sizeof(wave));
	fout.write(reinterpret_cast<char*>(main_stream.wav_data.data), sizem);

	return 0;
}