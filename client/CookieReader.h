#pragma once
using namespace std ;
class ReadCookie{
    private:
        string randomToken ;
    public:    
        ReadCookie(){      
         randomToken = -1 ;
        } 
       string getRandomToken(); 
       int getCookieFileFromBrowser();
};

