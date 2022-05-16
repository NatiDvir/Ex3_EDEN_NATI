#define _CRT_SECURE_NO_WARNINGS
#include "UtilsFunc.h"
#include <fstream>

void initializeWinsock() {

	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		throw exception("WebServer: Error at WSAStartup()");
	}
}

string getMethod(string msg){
	return msg.substr(0, msg.find_first_of(' ')); // Get name of method in string.
}

requestType convertStrToEnumType(string& type) //
{
    if (type == "GET") return requestType::GET_TYPE;
    else if (type == "POST") return requestType::POST_TYPE;
    else if (type == "PUT") return requestType::PUT_TYPE;
    else if (type == "DELETE") return requestType::DELETE_TYPE;
    else if (type == "HEAD") return requestType::HEAD_TYPE;
    else if (type == "TRACE") return requestType::TRACE_TYPE;
    else if (type == "OPTIONS") return requestType::OPTIONS_TYPE;
}

string handleResponseGet(string request)
{
	string fileName = getFullFileDir(request);

	string statusRes = OK_RES;
	string bodyRes = "";
	int fileSize = 0;

	ifstream myFile(fileName);

	if (!myFile)
	{
		statusRes = NOT_FOUND_RES;
	}
	else
	{
		myFile.seekg(0, ios::end);
		fileSize = myFile.tellg();

		myFile.seekg(0, ios::beg);

		while (myFile.good())
		{
			string line = "";
			getline(myFile, line);
			bodyRes += (line + "\n");
		}

		myFile.close();
	}

	return createResponse(statusRes, "text/html", to_string(bodyRes.length()), bodyRes, false);
}

string getFullFileDir(string request){
	
	// Get file's name
	string fileName = request.substr(request.find_first_of('/') + 1, request.find_first_of('?') - request.find_first_of('/'));
	fileName = fileName.substr(0, fileName.size() - 6); 
	// Get language name
	string lang = request.substr(request.find_first_of('?') + 6);
	lang = lang.substr(0, lang.find_first_of(' '));

	// C:\\temp\\fileName_language.html
	string fullFileName = START_PATH + fileName + "_" + lang + ".html";
	return fullFileName;
}

//string getFullFileDir(string req)
//{
//	string fileName = getFileName(req);
//	string lang = getLangFromReq(req);
//	string fullFileName = "C:\\temp\\" + fileName + "_" + lang + ".html";
//
//	return fullFileName;
//}

//string getFileName(string req)
//{
//	string ans = req.substr(req.find_first_of('/') + 1, req.find_first_of('?') - req.find_first_of('/'));
//
//	ans = ans.substr(0, ans.size() - 6);
//	return ans;
//}
//
//
//
//string getLangFromReq(string req)
//{
//	string reqString(req);
//
//	string fromLangStrToEnd = reqString.substr(reqString.find_first_of('?') + 6);
//
//	return fromLangStrToEnd.substr(0, fromLangStrToEnd.find_first_of(' '));
//}

string createResponse(string status, string type, string len, string body, bool isOptions) {

	time_t rawtime;
	time(&rawtime);
	string date(ctime(&rawtime)); // get the date for the response
	string respond = "HTTP/1.1 ";
	respond += (status + "\r\n");
	respond += "Host: Web Server\r\n";
	respond += ("Date: " + date);
	respond += (string("Content-Type: ") + type + "\r\n");
	respond += (string("Content-Length: ") + len+ "\r\n");

	if (isOptions)
	{
		respond += ("Allow: GET,POST,PUT,HEAD,TRACE, OPTIONS, DELETE\r\n");
	}

	respond += "\r\n"; //need to be a blank line separates header & body
	respond += body;

	return respond;

}

string handleResponsePost(string req)
{
	cout << "The data from the POST req is: " << getBodyFromReq(req) << "." << endl;
	return createResponse(NO_CONTENT_RES, "text/html", "0", "", false);
}

string getBodyFromReq(string req)
{
	for (int i = 0; i < req.size() - 3; i++)
	{
		if (req[i] == '\r' && req[i + 1] == '\n' && req[i + 2] == '\r' && req[i + 3] == '\n') //the header ends with \r\n\r\n
		{
			return req.substr(i + 4); //extract the body from the req
		}
	}
	return ""; // no body found
}

string handleResponsePut(string req)
{
	string fileName = START_PATH + getFileNameForPutAndDelete(req);
	string status = OK_RES;

	ifstream tmpFile(fileName);
	if (!tmpFile)
		status = CREATED_FILE_RES;

	ofstream myFile(fileName);

	if (myFile.is_open())
	{
		if (getBodyFromReq(req) == "") //body is Empty
			status = NO_CONTENT_RES;

		myFile << getBodyFromReq(req);

		myFile.close();
	}

	return createResponse(status, "text/html", "0", "", false);
}

string getFileNameForPutAndDelete(string req)
{
	string fromFileNameToEnd = req.substr(req.find_first_of('/') + 1);
	return fromFileNameToEnd.substr(0, fromFileNameToEnd.find_first_of(' '));
}


string handleResponseDelete(string request)
{
	string fileName = START_PATH + getFileNameForPutAndDelete(request);
	string status = NO_CONTENT_RES;

	if (remove(fileName.c_str()) != 0)
		status = SERVER_ERROR;

	return createResponse(status, "text/html", "0", "", false);
}

string handleResponseHead(string request)
{
	string fileName = getFullFileDir(request);
	ifstream myFile(fileName);
	string status = OK_RES;
	int fileSize = 0;

	if (!myFile)
	{
		status = NOT_FOUND_RES;
	}
	else
	{
		myFile.seekg(0, ios::end);
		fileSize = myFile.tellg();
		myFile.close();
	}

	return createResponse(status, "text/html", to_string(fileSize), "", false);
}

string handleResponseTrace(string request){
	return createResponse(OK_RES, "massage/http", to_string(request.size()), request, false);
}

string handleResponseOptions(string request)
{
	return createResponse(OK_RES, "httpd/unix-directory", "0", "", true);
}

