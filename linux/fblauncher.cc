/**
 * SSP: Added for handling facebook login.
 **/

#include <iostream>
#include <string>

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

#include "fblauncher.hh"

using namespace std;

int FBLauncher::launchBrowser(string strUrl)
{
	int retCode	= -1;
	pid_t	pid	= -1;
	int	status	= -1;

	if(strUrl.length() > 0)
	{
		//call system here.
		//
		retCode	= system("/usr/bin/firefox test.html"); /*getBrowserPath().c_str(), strUrl.c_str());*/
		/*pid = fork();

		if(pid == 0)
		{
			retCode	= system("/usr/bin/firefox test.html"); //getBrowserPath().c_str(), strUrl.c_str());
			perror("retCode");
		}
		else
		{
			waitpid(pid, &status, 1);
			retCode	= WIFEXITED(status);

			cout << "waitpid() returned." << status << endl;
		}*/
	}
	else
	{
		cout << "Invalid Url." << endl;
	}

	return retCode;
}
