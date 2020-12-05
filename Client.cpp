#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <string>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable: 4996)

SOCKET Connection;

enum Packet {
	P_ChatMessage,
	P_Test
};

//Обработка приходящих пакетов
bool ProcessPacket(Packet packettype) {
	switch (packettype) {
	case P_ChatMessage:
	{
		int msgSize;
		recv(Connection, (char*)&msgSize, sizeof(int), NULL);
		char* msg = new char[msgSize = 1];
		msg[msgSize] = '\0';
		recv(Connection, msg, msgSize, NULL);
		std::cout << msg << std::endl;
		delete[] msg;
		break;
	}
	case P_Test:
		std::cout << "Тестовый пакет для проверки соединения" << std::endl;
		break;
	default:
		std::cout << "Незнакомый пакет: " << packettype << std::endl;
		break;
	}
	return true;
}

//Принятие отправленного сервером сообщения
void ClientHandler() {
	Packet packettype;
	while (true) {
		recv(Connection, (char*)&packettype, sizeof(Packet), NULL);

		if (!ProcessPacket(packettype)) {
			break;
		}
	}
	closesocket(Connection);
}
int main()
{
	WSAData wsadata;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsadata) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}


	SOCKADDR_IN addr;
	//Размер структуры 
	int addrSize = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	//Сокет соединением с сервером
	Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) != 0)
	{
		std::cout << "Failed connect to server! \n" << std::endl;
		return 1;
	}
	std::cout << "Connection succed! \n" << std::endl;
	
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);


	//Считывание вводимых пользователем символов
	std::string msg1;
	while (true) {
		std::getline(std::cin, msg1);
		int msgSize = msg1.size();
		Packet packettype = P_ChatMessage;;
		send(Connection, (char*)&packettype, sizeof(Packet), NULL);
		send(Connection, (char*)msgSize, sizeof(msg1), NULL);
		send(Connection, msg1.c_str(), msgSize, NULL);
		Sleep(10);
	}


	system("pause");
	return 0;
}
