#pragma once
#include "UtilsFunc.h"

using namespace std;

class Request
{
public:
	string msg;
	int sendSubType;
	string data;

	Request(int _sendSubType, string _msg, string _data = nullptr);
	void setData(string _data);

	Request(int _sendSubType, string _msg, string _data)
	{
		sendSubType = _sendSubType;
		msg = _msg;
		data = _data;
	}

	void setData(string _data) {
		data = _data;
	}


};
