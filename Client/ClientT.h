#pragma once
#ifndef CLIENT_H
#define CLIENT_H

#include <string.h>
#include <iostream>

class Client
{
public:
	Client(std::string login, int password);
	~Client() {};

	//������ �����
	std::string Connect(std::string login, int password);		//������������ � �������
	void ListCheck();			//����������� ���� ������ ���������
	void HistoryCheck(int id);	//����������� ������� � ������������� �� ������ ������ ���������
	void Conversation(int id);	//��������� ��������� ������������ �� ������ ������ ���������
	void MessageRead(int id);	//�������� ������������� � �������� ���������
	void GetMessage();			//�������� ��������� �� ������� ������������ �� ������ ������ ���������
	void Find(int id);			//����� ����������� ������������ � ���� ������ ���������
	void Remove(int id);		//������� ����������� ������������ � ���� ������ ���������
	void HistoryDelete(int id);	//������� ������� ��������� � ���������� ������������� � ���� ������ ���������
	
	// �������� ��� ����� �� ���� ������������ �������� (���������, ��� ����� �������)


private:
	std::string login_;
	int password_;
	int id_; //???!!! <-----------------------

};

#endif