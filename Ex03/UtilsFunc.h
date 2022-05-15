#pragma once
#include <string>
#include <iostream>
#include <time.h>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <list>
using namespace std;
//***************************************************************************************************************//
const int PORT = 8080;
const int MAX_SOCKETS = 60;	// amount of possible clients to handle simultanious 
const int EMPTY = 0;	// for recv
const int LISTEN = 1;	// for recv
const int RECEIVE = 2;	// for recv
const int IDLE = 3;	// for send 
const int SEND = 4;	// for send
const int TWO_MIN = 120000;
const int MAX_SIZE = 1024;
//************************************const for impliments handler CRUD methods**********************************//
const string START_PATH = "C:\\temp\\";
const string NOT_FOUND_RES = "404 NOT FOUND";
const string NO_CONTENT_RES = "204 No Content";
const string OK_RES = "200 OK";
const string CREATED_FILE_RES = "201 Created";
const string NO_CONTENT_RES = "204 No content";
//***************************************************************************************************************//
enum class requestType
{
	GET_TYPE,
	POST_TYPE,
	PUT_TYPE,
	DELETE_TYPE,
	HEAD_TYPE,
	TRACE_TYPE,
	OPTIONS_TYPE
};

void initializeWinsock();
string getMethod(string msg);
requestType convertStrToEnumType(string& type);
string handleResponseGet(string request);
string handleResponsePost(string request);
