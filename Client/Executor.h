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

	//клиент может
	PNet::PResult AuthoriseOut(std::string login, std::string password, PNet::Socket socket); //авторизироваться

	PNet::PResult RegistrationOut(std::string login, std::string password, PNet::Socket socket); //зарегаться

	PNet::PResult PingOut(PNet::Socket socket); //ping

	PNet::PResult ContactsCheckOut(PNet::Socket socket);	//просмотреть список контактов

	PNet::PResult HistoryCheckOut(std::string login, PNet::Socket socket);	//история сообщений

	PNet::PResult ConversationOut(std::string login, std::string message, PNet::Socket socket);	//отправить сообщение пользователю из своего списка контактов

	bool AuthoriseIn(Packet packet); //авторизироваться

	bool RegistrationIn(Packet packet); //зарегаться

	PNet::PResult PingIn(Packet packet); //ping

	PNet::PResult ContactsCheckIn(Packet packet);	//просмотреть список контактов

	PNet::PResult HistoryCheckIn(int expected_login, Packet packet);	//история сообщений

	PNet::PResult ConversationIn(Packet packet); //gпринять сообщение

	PNet::PResult WaitCheck(Packet packet); //проверить наличие пришедшего кода ответа в списке ожидания

	bool IfWait();
	/*
	PNet::PResult MessageRead(int id);	//получить подтверждение о доставке сообщения
	//PNet::PResult GetMessage();			//получить сообщение от другого пользователя из своего списка контактов
	PNet::PResult Find(int id);			//найти нового пользователя
	PNet::PResult Add();
	PNet::PResult Remove(int id);		//удалить конкретного пользователя в своём списке контактов
	PNet::PResult HistoryDelete(int id);	//удалить историю сообщений с конкретным пользователем в своём списке контактов

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