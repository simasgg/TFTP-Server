#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4819)
#include "server.h"
#define socklen_t int
using namespace std;

#define PORT 69
#define PACKET_SIZE 516

#define OP_ACK 4
#define OP_RRQ 1
#define OP_WRQ 2
#define OP_DATAPACKET 3
#define OP_ERROR 5



server::server() {
	mySocket = -1;
}

server::~server() {
	if (mySocket != -1)
		closesocket(mySocket);
}

void server::Init(wxListBox* l) {
	m_list1 = l;
	WSADATA WinSockData;
	int iWsaStartup;
	iWsaStartup = WSAStartup(MAKEWORD(2, 2), &WinSockData);
	if (iWsaStartup != 0) {
		cout << "WSAStartUp Failed" << endl;
	}
}

void server::EndProgram() {
	int iWsaCleanup;
	iWsaCleanup = WSACleanup();
	if (iWsaCleanup == SOCKET_ERROR) {
		cout << "CleanUp Fun Failed & Error No-> " << WSAGetLastError() << endl;
	}
}


void server::setFilename(string fn) {
	filename = fn;
}

void server::printList(string str) {
	m_list1->AppendString(str);
	m_list1->SetSelection(m_list1->GetCount() - 1);
	wxYield();
}

void server::start(string host, string myport, wxButton* b) {
	m_btn1 = b;
	int iBind;
	struct sockaddr_in server;
	struct hostent* by_name;

	m_btn1->Enable(false);
	//convert string to char array
	char* cstr = new char[host.size() + 1];
	strcpy(cstr, host.c_str());
	int len = sizeof(server);

	memset(&server, 0, sizeof(server));
	by_name = gethostbyname(cstr);
	server.sin_family = AF_INET;
	if (myport.empty())
		server.sin_port = htons(PORT);
	else
		server.sin_port = htons(stoi(myport));	//convert string to int

	memcpy(&server.sin_addr.S_un.S_addr, by_name->h_addr_list[0], 4);

	mySocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (mySocket == -1) {
		printList("socket: errno = " + errno);
		return;
	}
	printList("Server Socket Creation Success");

	iBind = ::bind(mySocket, (struct sockaddr*)&server, sizeof(server));
	if (iBind == -1) {
		printList("Unable to bind listening address");
		closesocket(mySocket);
		return;
	}
	printList("Binding Success");
	server.sin_port = htons(0);
	my_function_to_send_and_recieve(mySocket);
	closesocket(mySocket);
	m_btn1->Enable(true);
	delete[]cstr;
	return;
}

void server::send_error_message(int mySocket, int hex_num, const char* message, struct sockaddr_in* client)
{
	char error_datagram[PACKET_SIZE];
	int size = 0;
	error_datagram[0] = 0x00;  // Opcode == 5.
	error_datagram[1] = 0x05;
	error_datagram[2] = 0x00;  // ErrorCode == 0 (not defined).
	error_datagram[3] = hex_num;
	strcpy(&error_datagram[4], message);

	size = strlen(message) + 5;
	int i = 4;

	printList("<<<Error>>> number: " + hex_num);
	printList(message);

	sendto(mySocket, error_datagram, size, 0, (struct sockaddr*)client, sizeof(struct sockaddr_in));
	closesocket(mySocket);
	exit(0);
}

void server::my_function_to_send_and_recieve(int mySocket) {
	double t1, t2;
	FILE* pFile = NULL;
	bool flag = false;
	char  buf[PACKET_SIZE], file_name[PACKET_SIZE];
	struct sockaddr_in client;

	time_t now = time(0);
	char* dt = ctime(&now);

	int client_length = sizeof(client);
	memset(&file_name, 0, sizeof(file_name));
	memset(&buf, 0, sizeof(buf));
	int i = 2, byte_count = 0, block = 0, j, my_size;
	printList("Waiting for client...");

	int bytes_recv = recvfrom(mySocket, buf, PACKET_SIZE, 0, (struct sockaddr*)&client, &client_length);
	if (bytes_recv == -1) {
		printList("sendto: errno = " + errno);
	}
	char* strAdd = inet_ntoa(client.sin_addr);
	string str1(strAdd);
	string str2 = to_string(ntohs(client.sin_port));
	printList("Connection Accepted from " + str1 + " on port " + str2 + " ---> ");

	//nuo antro buf indekso sukam iki nulinio baito, kuris mums parodo ties kur baigiasi failo vardas
	while (buf[i] != 0) i++;
	memcpy(&file_name, &buf[2], i - 2);		//failo vardas

	//jei opcode(buf[1]) 1 tai readinam ir siunciam, jei 2 - tai recievinam ir writinam
	if (buf[1] == OP_WRQ) {
		printList("Using local port " + to_string(ntohs(client.sin_port)));
		my_function_to_recieve(mySocket, file_name, &client);				// <-----------to recieve (WRQ)
		return;
	}
	string str3(file_name);
	printList("Using local port " + to_string(ntohs(client.sin_port)));
	printList("Write request for file <" + str3 + ">");

	if (flag == true) {
		filename = filename + "\\" + str3;
		std::replace(str3.begin(), str3.end(), '\\', '/');
	}
	else
		filename = str3;

	//string>char
	char* filenamechar = new char[filename.size() + 1];
	strcpy(filenamechar, filename.c_str());

	i = j = 0;
	t1 = clock();
	while (true) {
		memset(&buf, 0, sizeof(buf));
		if (!pFile) {
			if ((pFile = fopen(filenamechar, "rb")) == NULL)
				send_error_message(mySocket, 0x01, "Could not open file", &client);
		}

		if (flag == false) {
			fseek(pFile, 0, SEEK_END);
			//susiskaiciuoju failo dydi baitais
			my_size = ftell(pFile);
			flag = true;
			fseek(pFile, 0, SEEK_SET);
		}

		int to_send = PACKET_SIZE;
		if ((to_send - 4 > 0)) {
			if (my_size < 516)
				to_send = my_size + 4;
			my_size = my_size - 512;
			byte_count += to_send - 4;
			block++;
			fread(&buf[4], 1, to_send - 4, pFile);
			if (ferror(pFile)) {
				send_error_message(mySocket, 0x03, "Disk full or allocation exceeded", &client);
			}
		}
		//Opcode 3=DATA
		buf[0] = 0;
		buf[1] = OP_DATAPACKET;
		//skaiciuoju block number kuri paskui siunciu kartu su DATA packetu
		i++;
		buf[2] = static_cast<unsigned short>(j);
		buf[3] = static_cast<unsigned short>(i);
		if (buf[3] == -1) {
			j++;
		}

		//DATA block (512 Bytes)
		int send_count = sendto(mySocket, buf, to_send, 0, (const struct sockaddr*)&client, sizeof(struct sockaddr_in));
		if (send_count == -1) {
			printList("sendtoDATA: errno = " + errno);
			break;
		}

		//--------------ACK block
		bytes_recv = recvfrom(mySocket, buf, 4, 0, (struct sockaddr*)&client, &client_length);
		if (bytes_recv == -1) {
			printList("recvfromACK: errno = " + errno);
			break;
		}

		if (buf[1] != OP_ACK)
			send_error_message(mySocket, 0x00, "Could not read file", &client);

		printList(to_string(byte_count) + " bytes (" + to_string(block) + " blocks) sent");
		if (send_count - 4 != 512) {
			break;
		}

	}
	t2 = clock();
	printList("***Transfered " + to_string(byte_count) + " bytes in " + to_string((t2 - t1) / CLK_TCK) + " sec***");
	fclose(pFile);
	delete[]filenamechar;
}

void server::my_function_to_recieve(int mySocket, char* file_name, struct sockaddr_in* client) {
	double t1, t2;
	char  buf[PACKET_SIZE];
	FILE* pFile = NULL;
	int block = 0, byte_count = 0;
	string str4(file_name);

	int client_length = sizeof(*client);
	memset(&buf, 0, sizeof(buf));

	if (flag == true) {
		filename = filename + "\\" + str4;
		std::replace(str4.begin(), str4.end(), '\\', '/');
	}
	else
		filename = str4;

	//string>char
	char* filenamechar = new char[filename.size() + 1];
	strcpy(filenamechar, filename.c_str());

	printList("Write request for file <" + str4 + ">");
	buf[0] = 0;
	buf[1] = OP_ACK;
	buf[2] = 0;
	buf[3] = 0;
	// BLOCK 0
	int bytes_sent = sendto(mySocket, buf, 4, 0, (const struct sockaddr*)client, sizeof(struct sockaddr_in));
	if (bytes_sent == -1) {
		printList("sendtoACKfirst: errno = " + errno);
	}

	t1 = clock();
	while (true) {
		memset(&buf, 0, sizeof(buf));
		//---------------DATA block (512 Bytes)
		int bytes_recv = recvfrom(mySocket, buf, PACKET_SIZE, 0, (struct sockaddr*)client, &client_length);
		if (bytes_recv == -1) {
			printList("sendto: errno = " + errno);
		}

		if (!pFile) {
			if ((pFile = fopen(filenamechar, "wb")) == NULL)
			{
				send_error_message(mySocket, 0x01, "Could not open file", client);
			}
		}

		//1 block = 512 bytes
		int block_number = ((unsigned short)buf[2] << 8) | (unsigned short)buf[3];
		if ((bytes_recv - 4 > 0) && (block_number > block)) {
			byte_count += bytes_recv - 4;
			block++;
			fwrite(&buf[4], 1, bytes_recv - 4, pFile);
			if (ferror(pFile)) {
				send_error_message(mySocket, 0x03, "Disk full or allocation exceeded", client);
			}
		}

		buf[0] = 0;
		buf[1] = OP_ACK;
		//---------------ACK block
		int send_count = sendto(mySocket, buf, 4, 0, (const struct sockaddr*)client, sizeof(struct sockaddr_in));
		if (send_count == -1) {
			printList("sendtoACKothers: errno = " + errno);
		}

		if (send_count != 4)
			send_error_message(mySocket, 0x04, "Illegal TFTP operation", client);

		printList(to_string(byte_count) + " bytes (" + to_string(block) + " blocks) recieved");

		fflush(stdout);

		if (bytes_recv - 4 != 512) {
			break;
		}
	}
	t2 = clock();
	printList("***Transfered " + to_string(byte_count) + " bytes in " + to_string((t2 - t1) / CLK_TCK) + " sec***");
	fclose(pFile);
	delete[]filenamechar;
}