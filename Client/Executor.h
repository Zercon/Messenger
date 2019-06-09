#pragma once

#include <string.h>
#include <iostream>
#include <PNet/IncludeMe.h>
#include <libs/Packet.h>
#include <list>
#include <vector>

class Executor
{
public:
	Executor()
	{
		this->ident = 1;
		talking_flag = false;
	};
	~Executor() {};

	//������ �����
	PNet::PResult AuthoriseOut(std::string login, std::string password, PNet::Socket socket); //����������������

	PNet::PResult RegistrationOut(std::string login, std::string password, PNet::Socket socket); //����������

	PNet::PResult PingOut(PNet::Socket socket); //ping

	PNet::PResult ContactsCheckOut(PNet::Socket socket);	//����������� ������ ���������

	PNet::PResult HistoryCheckOut(std::string login, PNet::Socket socket);	//������� ���������

	PNet::PResult ConversationOut(std::string login, std::string message, PNet::Socket socket);	//��������� ��������� ������������ �� ������ ������ ���������

	bool AuthoriseIn(Packet packet); //����������������

	bool RegistrationIn(Packet packet); //����������

	PNet::PResult PingIn(Packet packet); //ping

	PNet::PResult ContactsCheckIn(Packet packet);	//����������� ������ ���������

	PNet::PResult HistoryCheckIn(int expected_login, Packet packet);	//������� ���������

	PNet::PResult ConversationIn(Packet packet); //g������� ���������

	PNet::PResult WaitCheck(Packet packet); //��������� ������� ���������� ���� ������ � ������ ��������

	bool IfWait();
	/*
	PNet::PResult MessageRead(int id);	//�������� ������������� � �������� ���������
	//PNet::PResult GetMessage();			//�������� ��������� �� ������� ������������ �� ������ ������ ���������
	PNet::PResult Find(int id);			//����� ������ ������������
	PNet::PResult Add();
	PNet::PResult Remove(int id);		//������� ����������� ������������ � ���� ������ ���������
	PNet::PResult HistoryDelete(int id);	//������� ������� ��������� � ���������� ������������� � ���� ������ ���������

	*/




	void identIncrement()
	{
		this->ident += 1;
	}

	int ident;

	std::vector<std::string> contacts;
	std::vector<std::vector<std::string>> hranilishe;
	std::list<int> wait;
	bool talking_flag;

private:

};