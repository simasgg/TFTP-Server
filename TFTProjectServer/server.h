#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <string.h>
#include <thread>
#include <ctime>
#include "wx/wx.h"
#include "wx/dirdlg.h"
#include "wx/filedlg.h"
#include <wx/wfstream.h>
#include "wx/glcanvas.h"
#define socklen_t int


class server
{
private:
	wxListBox* m_list1 = nullptr;
	wxButton* m_btn1 = nullptr;
	int mySocket;
	std::string filename;
	bool flag = false;
public:
	server();
	~server();
	void Init(wxListBox* l);
	void EndProgram();
	void my_function_to_send_and_recieve(int mySocket);
	void my_function_to_recieve(int mySocket, char* file_name, struct sockaddr_in* client);
	void start(std::string host, std::string myport, wxButton* b);
	void send_error_message(int mySocket, int hex_num, const char* message, struct sockaddr_in* client);
	void printList(std::string str);
	void setFilename(std::string fn);
};

