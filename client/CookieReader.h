#pragma once
using namespace std ;
class CookieReader{
    private:
        string randomToken;
    public:    
       string getRandomToken(); 
       int getCookieFileFromBrowser();
};

