#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctime>
#include <InetAddress.h>
using namespace std;

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		cout << "usage:./client ip port" << endl;
		return -1;
	}

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("socket() failed");
		return -1;
	}

	char buf[1024];
	sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons(atoi(argv[2]));
	// InetAddress servaddr(argv[1], atoi(argv[2]));

	if (connect(sockfd, (sockaddr *)&servaddr, sizeof(servaddr)) != 0)
	{
		cout << "connect " << argv[1] << ":" << argv[2] << " failed!" << endl;
		close(sockfd);
		return -1;
	}
	cout << "connection successful!" << endl;

	// 获取当前时间
	time_t currentTime = time(0);
	cout << "开始时间：" << ctime(&currentTime); // 将变量传递给 ctime

	// 发送数据
	for (int i = 0; i < 100; i++)
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "hello world ->%d", i);
		// cout << "please input:";
		// cin.getline(buf, sizeof(buf));
		char tmpbuf[1024];
		memset(tmpbuf, 0, sizeof(tmpbuf));
		int len = strlen(buf);
		memcpy(tmpbuf, &len, 4);
		memcpy(tmpbuf + 4, buf, len);

		send(sockfd, tmpbuf, len + 4, 0);
	}

	// 接受服务端返回的数据
	for (int i = 0; i < 100; i++)
	{
		int len;
		// 先接收数据长度，再接收数据
		recv(sockfd, &len, 4, 0);
		memset(buf, 0, sizeof(buf));
		recv(sockfd, buf, len, 0);

		// cout << "recv: " << buf << endl;
	}
	// 获取当前时间
	currentTime = time(0);						 // 将时间存储在变量中
	cout << "结束时间：" << ctime(&currentTime); // 将变量传递给 ctime
	close(sockfd);								 // 关闭socket
	return 0;
}