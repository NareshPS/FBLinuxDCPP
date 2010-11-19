/**
 * SSP: Class for launching facebook.
 **/

#pragma once
#include <iostream>
#include <string>

using namespace std;

class FBLauncher
{
	string fbPage;
public:
	FBLauncher()
	{
		fbPage	= "ConnectFB.html";
	}
	string getFBAppUrl(){return fbPage;};
	string getBrowserPath() const { return "/usr/bin/firefox";}
	int launchBrowser(string strUrl);
};
