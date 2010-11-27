#include <iostream>
#include <string>

#include "FBLogger.h"

using namespace std;

int
main(int argc, char *argv[])
{
	FBLogger	fbLogger("./test.txt");

	fbLogger.Write("hi");

	return 0;
}
