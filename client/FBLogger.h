#pragma once
#include <fstream>
#include <string>

using namespace std;

class FBLogger
{
	string mFileName;
	ofstream	oFile;

	void Open();
	void Close();

public:
	FBLogger(string fileName)
	{
		mFileName	= fileName;
	}


	void Write(string buffer);
	void Write(char *buffer);
};
