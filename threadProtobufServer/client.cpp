//////////////////////////////////////////////////////////////////////////////
// Created by zhouzzz on 2018/6/18.
// Description : �ͻ�����
//////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <iostream>
#include <winsock2.h>
#include <thread>
#include "msg.pb.h"
#pragma comment(lib,"ws2_32.lib")
using namespace word3;
static int number;

//�������Է���������Ϣ��Ҳ����ʱ������ת���Ŀͻ��˵���Ϣ��clientid = 0����������clientid = number������,clientid = ��������Ŀͻ���
void recvFromServer(SOCKET clientsocket, char *buf)
{
	sendmsg smsg;
	while (1) {
		memset(buf, '\0', sizeof(buf));
		if (recv(clientsocket, buf, 1024, 0) <= 0)
		{
			printf("�ر�����!\n");
			break;
			closesocket(clientsocket);
		}
		smsg.ParseFromArray(buf, 1024);//�����л�
		if (smsg.clientid() == 0) std::cout << "������" << ":";
		else if (smsg.clientid() == number) std::cout << "��" << ":";
		else std::cout << "�ͻ���" << smsg.clientid() << ":";
		std::cout << smsg.msg() << "\n";
	}
}
int main()
{
	SOCKET clientsocket;
	SOCKADDR_IN serveraddr;
	SOCKADDR_IN clientaddr;
	char buf[1024];
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);	//��ʼ��WS2_32.DLL
										//�����׽���
	if ((clientsocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0)
	{
		printf("�׽���socket����ʧ��!\n");
		system("pause");
		return -1;
	}
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9102);
	serveraddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	//��������
	printf("����������...\n");
	if (connect(clientsocket, (SOCKADDR *)&serveraddr, sizeof(serveraddr)) != 0)
	{
		printf("����ʧ��!\n");
		system("pause");
		return -1;
	}
	printf("���ӳɹ�!\n");
	//��������ʱ�����ܵ���������һ����Ϣ������Ϣ���б��ͻ��˵�id����ȡ����ֵ��number
	if (recv(clientsocket, buf, 1024, 0) <= 0)
	{
		printf("�ر�����!\n");
		return -1;
		closesocket(clientsocket);
	}
	sendmsg smsg;
	smsg.ParseFromArray(buf, 1024);
	std::cout << "������" << ":" << smsg.msg() << "\n";
	number = smsg.clientid();//id��ֵ��number
	smsg.Clear();
	std::thread task03(recvFromServer, clientsocket, buf);//����������Ϣ�߳�
	task03.detach();
	//�������ݣ���Ϊֻ��Ҫ�������ݸ������������Է�����Ϣ����ֱ�ӷ������߳�
	sendmsg smsgs;
	char ss[1024];
	memset(ss, '\0', sizeof(ss));
	printf("����������:\n");
	while (1) {
		scanf("%s", buf);
		smsgs.set_clientid(number);
		smsgs.set_msg(buf);
		smsgs.SerializeToArray(ss, 1024);
		if (send(clientsocket, ss, strlen(ss) + 1, 0) <= 0)
		{
			printf("���ʹ���!\n");
			break;
		}
	}
	closesocket(clientsocket);
	WSACleanup();    //�ͷ�WS2_32.DLL
	system("pause");
	return 0;

}