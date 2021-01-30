#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>

#pragma warning(disable: 4996)

SOCKET Connections[100];
int counter = 0;

enum Packet {
	P_ChatMessage,
	P_Test
};

bool ProcessPacket(int index, Packet packettype) {
	switch (packettype)
	{
	case P_ChatMessage:
	{
		int msgSize;
		recv(Connections[index], (char*)&msgSize, sizeof(int), NULL);
		char* msg = new char[msgSize + 1];
		msg[msgSize] = '\0';
		recv(Connections[index], msg, msgSize, NULL);
		for (int i = 0; i < counter; i++) {
			if (i == index) {
				continue;
			}
			Packet msgtype = P_ChatMessage;
			send(Connections[i], (char*)&msgtype, sizeof(Packet), NULL);
			send(Connections[i], (char*)&msgSize, sizeof(int), NULL);
			send(Connections[i], msg, msgSize, NULL);
		}
		delete[] msg;
		break;
	}
	default:
		std::cout << "Íåçíàêîìûé ïàåêò: " << packettype << std::endl;
		break;
	}
	return true;
}


void ClientHandler(int index)
{
	Packet packettype;
	while (true) {
		recv(Connections[index], (char*)&packettype, sizeof(Packet), NULL);
		
		if (!ProcessPacket(index, packettype)) {
			break;
		}
	}
	closesocket(Connections[index]);
}
int main()
{
	
	WSAData wsadata;
	
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsadata) != 0) {
		std::cout << "Error!!! \n" << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;

	int addrSize = sizeof(addr);

	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;
	 
	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);

	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);


	SOCKET nConnect;

	for (int i = 0; i < 100; i++)
	{
		nConnect = accept(sListen, (SOCKADDR*)&addr, &addrSize);

		if (nConnect == 0) {
			std::cout << "Error connection with client! \n" << std::endl;
		}
		else {
			std::cout << "Client has been connected! \n" << std::endl;
			std::string msg = "Some random message for client to check connection";
			int msgSize = msg.size();
			Packet msgtype = P_ChatMessage;
			send(nConnect, (char*)&msgtype, sizeof(Packet), NULL);
			send(nConnect, (char*)&msgSize , sizeof(int), NULL);
			send(nConnect, msg.c_str() , msgSize, NULL);

			Connections[i] = nConnect;
			counter++;

			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL);
		

			Packet testpacket = P_Test;
			send(nConnect, (char*)&testpacket, sizeof(Packet), NULL);
		}
	}

	system("pause");
	return 0;
}
