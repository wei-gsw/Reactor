#include <iostream>
#include <vector>
#include <functional>
#include <cstdlib> // for atoi
#include <signal.h>
#include "EchoServer.h"
using namespace std;

EchoServer *echoServer;

void stop(int sig)
{ // �ź�2��15��������ֹͣ����
	cout << "sig=" << sig << endl;
	echoServer->stop();
	cout << "stop server" << endl;
	delete echoServer;
	exit(0);
}

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		cout << "usage:./tcpepoll ip port" << endl;
		return -1;
	}

	
	signal(SIGTERM, stop);
	signal(SIGINT, stop);
	echoServer= new EchoServer(argv[1], atoi(argv[2]), 3, 2);

	// �����¼�ѭ��
	echoServer->start();
	return 0;
}
