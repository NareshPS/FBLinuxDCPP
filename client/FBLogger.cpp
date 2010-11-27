#include <iostream>
#include <fstream>
#include <string>

#include "FBLogger.h"

using namespace std;


void FBLogger::Open()
{
	oFile.open(mFileName.c_str(), ios::out|ios::app);

	if(oFile.fail())
	{
		throw "Open Failed";
	}
}

void FBLogger::Write(string buffer)
{
	if(oFile.is_open())
	{
		oFile << buffer << endl;
	}
	else
	{
		Open();
		Write(buffer);
		Close();
	}
}

void FBLogger::Write(char *buffer)
{
	if(buffer)
	{
		Write(string(buffer));
	}
}

void FBLogger::Close()
{
	if(oFile.is_open())
	{
		oFile.close();
	}
}
