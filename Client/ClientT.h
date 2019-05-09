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

	//клиент может
	std::string Connect(std::string login, int password);		//подключиться к серверу
	void ListCheck();			//просмотреть свой список контактов
	void HistoryCheck(int id);	//просмотреть историю с пользователем из своего списка контактов
	void Conversation(int id);	//отправить сообщение пользователю из своего списка контактов
	void MessageRead(int id);	//получить подтверждение о доставке сообщения
	void GetMessage();			//получить сообщение от другого пользователя из своего списка контактов
	void Find(int id);			//найти конкретного пользователя в своём списке контактов
	void Remove(int id);		//удалить конкретного пользователя в своём списке контактов
	void HistoryDelete(int id);	//удалить историю сообщений с конкретным пользователем в своём списке контактов
	
	// заменить все войды на норм возвращаемые значения (прочитать, что нужно сокетам)


private:
	std::string login_;
	int password_;
	int id_; //???!!! <-----------------------

};

#endif