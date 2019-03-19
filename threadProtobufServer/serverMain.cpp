//////////////////////////////////////////////////////////////////////////////
// Created by zhouzzz on 2018/6/18.
// Description : ��������
//////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <winsock2.h>
#include <thread>
#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include "msg.pb.h"
#pragma comment(lib,"ws2_32.lib")
using namespace word3;
static int connectCount = 0;//��ǰ���ӿͻ��˵�����
std::vector <SOCKET> clients;//�������������ӵĿͻ��˷���������

void delClient(SOCKET clientsoc)//���ͻ��˶Ͽ�����ʱ����������ɾ���ͻ���
{
	for (int i = 0; i < (int)clients.size(); i++)
	{
		if (clients[i] == clientsoc)
		{
			clients.erase(clients.begin() + i);
			break;
		}
	}
}
void sendMsg(char *buf)//�����пͻ��˷�����Ϣ
{
	std::vector<SOCKET>::iterator it;
	for (it = clients.begin(); it != clients.end(); it++) {
		if (send(*it, buf, strlen(buf) + 1, 0) <= 0)
		{
			printf("���ʹ���!\n");
			break;
		}
	}
}
void listenThread(char *buf, SOCKET clientsoc, SOCKADDR_IN clientaddr, int number)//�����̣߳������ͻ��˵���������
{
	printf("----------------------------------------------------------\n");
	//printf("���ӳɹ����ͻ���ip:%s\n�ͻ��˱��%d,��ǰ��������%d\n", inet_ntoa(clientaddr.sin_addr), number, connectCount);//inet_ntoa:����ip���ַ�����ʽ���ͻ��˵ı�Ű������Ⱥ�˳��ֵ
	printf("----------------------------------------------------------\n");
	memset(buf, '\0', sizeof(buf));
	//�ͻ��˸�������ʱ������һ�����ݣ����ͻ��˵ı�Ÿ��߿ͻ���
	sendmsg smsg;
	char ss[1024];
	memset(ss, '\0', sizeof(ss));
	smsg.set_clientid(number);
	char tt[16] = "��ӭ�����ͻ���";//�̶���ʽ��һ�������ַ�ռ2��char�����Ҫ��һ��charΪ������'\0'
	tt[14] = number + '0';
	smsg.set_msg(tt);
	smsg.SerializeToArray(ss, 1024);//���л�
	if (send(clientsoc, ss, strlen(ss) + 1, 0) <= 0)
	{
		printf("���ʹ���!\n");
		return;
	}
	//�����пͻ��˷��ͷ���������Ϣ
	memset(ss, '\0', sizeof(ss));
	while (1) {
		//scanf("%s", buf);
		printf("�����пͻ��˷��ͷ���������Ϣ: %s \n", buf);
		int x = 0;
		smsg.set_clientid(x);
		smsg.set_msg(buf);
		smsg.SerializeToArray(buf, 1024);
		sendMsg(buf);
	}
}

void recvFromClient(SOCKET clientsoc, char *buf, int number)//����һ���ͻ��˵���Ϣ��ת�������пͻ���
{
	while (1)
	{
		if (recv(clientsoc, buf, 1024, 0) <= 0)
		{
			printf("�ͻ���%d�ر�����!\n", number);
			delClient(clientsoc);
			connectCount--;
			closesocket(clientsoc);
			return;
		}
		sendMsg(buf);
	}
}
int main()
{
	SOCKET serversoc;
	SOCKET clientsoc;
	SOCKADDR_IN serveraddr;
	SOCKADDR_IN clientaddr;
	char buf[1024];
	int len;
	static int listenCount = 0;

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);	//��ʼ��WS2_32.DLL
										//�����׽���
	if ((serversoc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0)
	{
		printf("�׽���socket����ʧ��!\n");
		return -1;
	}
	//����Э�飬IP���˿�
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9102);
	serveraddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//���׽���
	if (bind(serversoc, (SOCKADDR *)&serveraddr, sizeof(serveraddr)) != 0)
	{
		printf("�׽��ְ�ʧ��!\n");
		return -1;
	}
	printf("��ʼ����...\n");
	//��������
	if (listen(serversoc, 1) != 0)
	{
		printf("����ʧ��!\n");
		return -1;
	}
	len = sizeof(SOCKADDR_IN);
	//��������
	while (1) {
		if ((clientsoc = accept(serversoc, (SOCKADDR *)&clientaddr, &len)) <= 0)
		{
			printf("��������ʧ��!\n");
			return -1;
		}
		else {
			listenCount++;
			connectCount++;
			clients.push_back(clientsoc);//���������ϵĿͻ��˼�������
			std::thread task(listenThread, buf, clientsoc, clientaddr, listenCount);//ÿ���пͻ�����������ʱ���Ϳ���һ���߳̽�������
			task.detach();//detach()������ɶ�����join()�����߳���ɶ���
			std::thread task02(recvFromClient, clientsoc, buf, listenCount);//����һ���̼߳����ÿͻ��˵���Ϣ
			task02.detach();
		}
	}
	WSACleanup();     //�ͷ�WS2_32.DLL
	system("pause");
	return 0;
}