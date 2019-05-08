#include "Client.h"

Client::Client(std::string login, int password)
{
	this->login_ = login;
	this->password_ = password;

	std::string otvet = Connect(login_, password_);
	if // если законнектилось- работаем

	/*
	расшифровка ответа, id- часть ответа
	*/

	this->id_ = /* id из ответа */;
}

