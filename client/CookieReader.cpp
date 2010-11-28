#include<iostream>
//#include<cstring>
#include<Python.h>
#include"CookieReader.h"
using namespace std ;
#define PythonFunc "getCookieData"
#define PythonObj  "read_sql"
/*class CookieReader{
    private:
        string randomToken ;
    public:    
        CookieReader(){      
         randomToken = -1 ;
        } 
       string getRandomToken(); 
       int getCookieFileFromBrowser();
};*/

// Read Cookie 
string CookieReader::getRandomToken(){
    getCookieFileFromBrowser();
    return randomToken ;
}

//Get Fresh Cookie info 
int CookieReader::getCookieFileFromBrowser(){

  PyObject *pName, *pModule, *pDict, *pFunc;
    PyObject *pArgs, *pValue;
    Py_Initialize();
    pName = PyString_FromString(PythonObj);
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);
    if (pModule != NULL) {
        pFunc = PyObject_GetAttrString(pModule,PythonFunc);
        /* pFunc is a new reference */

        if (pFunc && PyCallable_Check(pFunc)) {
            pArgs = PyTuple_New(0);
            pValue = PyObject_CallObject(pFunc, pArgs);
            Py_DECREF(pArgs);
            if (pValue != NULL) {

                randomToken = PyString_AsString(pValue); 
                Py_DECREF(pValue);
            }
            else {
                Py_DECREF(pFunc);
                Py_DECREF(pModule);
                PyErr_Print();
                fprintf(stderr,"Call failed\n");
                return 1;
            }
        }
        else {
            if (PyErr_Occurred())
                PyErr_Print();
            fprintf(stderr, "Cannot find function \"%s\"\n", PythonFunc);
        }
        Py_XDECREF(pFunc);
        Py_DECREF(pModule);
    }
    else {
        PyErr_Print();
        fprintf(stderr, "Failed to load \"%s\"\n", PythonObj);
        return 1;
    }
    Py_Finalize();
    return 0;
}

/*int main(){
    CookieReader reader ;
    //reader.getCookieFileFromBrowser();  
    cout<<"\n Reader  is reading "<<reader.getRandomToken();
}*/

