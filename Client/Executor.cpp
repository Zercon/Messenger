#include "Executor.h"

using namespace std;

static bool debug = true;

PNet::PResult Executor::WaitCheck(Packet packet) //проверяем ожидали ли мы такой ответ от сервера
{
	auto it = wait.begin();
	while (it != wait.end())
	{
		if (*it == packet.getIdent())
		{
			wait.erase(it); //мы приняли ответ на ожидаемый сигнал, далее мы будем его обрабатывать, значит мы его больше ждать не будем, так что убираем его из листа ожиданий
			if (wait.size() == 0)
			{
				this->ident = 1;
				if(debug) cout << "(debug is on): " << "ident reseted" << endl;
			}
			return PNet::PResult::P_Success;
		}
		it++;
	}
	return PNet::PResult::P_GenericError;
}

bool Executor::IfWait()
{
	if (this->wait.size() != 0)
		return true;
	else
		return false;
}

PNet::PResult Executor::AuthoriseOut(std::string login, std::string password, PNet::Socket socket)
{
	Packet packet(2 /*authorisation code*/, 9, this->ident, 0);
	packet.makeStringItem(login.data(), login.size());
	packet.makeStringItem(password.data(), password.size());

	if (socket.SendAll((char*)packet.data(), packet.getLength()) == PNet::PResult::P_Success)
	{
		this->wait.push_back(this->ident);
		identIncrement();
		return PNet::PResult::P_Success;
	}
	else
	{
		return PNet::PResult::P_GenericError;
	}
}

bool Executor::AuthoriseIn(Packet packet) //пришёл ответ от сервера об авторизации
{
	switch (packet.getCodItem(1))
	{
	case 1:
		std::cout << "successfully loged in" << std::endl;
		return true;
		break;
	case 2:
		std::cout << "incorrect login or password" << std::endl;
		return false;
		break;
	case 3: 
		std::cerr << "error" << std::endl;
		return false;
		break;
	default:
		std::cerr << "unknown response from server in authorising" << std::endl;
		return false;
	}	
}

bool Executor::RegistrationIn(Packet packet)
{
	switch (packet.getCodItem(1))
	{
	case 1:
		std::cout << "account successfully created" << std::endl;
		return true;
		break;
	case 2:
		std::cout << "this login is already taken" << std::endl;
		return false;
		break;
	case 3:
		std::cerr << "error" << std::endl;
		return false;
		break;
	default:
		std::cerr << "unknown response from server in registration" << std::endl;
		return false;
	}
}

PNet::PResult Executor::ContactsCheckIn(Packet packet)
{
	int ic = packet.getItemCount();
	hranilishe.resize(ic);
	for (int i = 0; i < ic-1; i++) //мы тут вроде как с 0 начинаем счёт, так что ic - 1
	{
		contacts.push_back(packet.getStringItem(i));
		if (debug) cout << "(debug is on): " << "id " << i + 1 << ": " << contacts[i] << endl;
	}

	return PNet::PResult::P_Success; //?????????????????????????????????? what do we return???
}

PNet::PResult Executor::HistoryCheckIn(int expected_login, Packet packet)
{
	int amount;
	switch (packet.getCodItem(1))
	{
	case 1:
		std::cout << "request approved" << std::endl;
		amount = packet.getCodItem(2);
		for (int i = 0; i < amount; i++)
		{
			hranilishe[expected_login].push_back(packet.getStringItem(i));
			if (debug) cout << "(debug is on): " << hranilishe[expected_login].back() << endl;
		}

		return PNet::PResult::P_Success;
		break;
	case 2:
		std::cout << "user not found" << std::endl;
		return PNet::PResult::P_GenericError;
		break;
	case 3:
		std::cerr << "error" << std::endl;
		return PNet::PResult::P_GenericError;
		break;
	default:
		std::cerr << "unknown response from server in history receiving" << std::endl;
		return PNet::PResult::P_GenericError;
	}
		

	//int ic = packet.getItemCount();
	


	return PNet::PResult(); //ЧТО МЫ ТУТ ВОЗВРАЩАЕМ???7
}

PNet::PResult Executor::RegistrationOut(std::string login, std::string password, PNet::Socket socket)
{
	Packet packet(1 /*registration code*/, 9, this->ident, 0);
	packet.makeStringItem(login.data(), login.size());
	packet.makeStringItem(password.data(), password.size());

	if (socket.SendAll((char*)packet.data(), packet.getLength()) == PNet::PResult::P_Success)
	{
		this->wait.push_back(this->ident);
		identIncrement();
		return PNet::PResult::P_Success;
	}
	else
	{
		return PNet::PResult::P_GenericError;
	}
}

PNet::PResult Executor::PingOut(PNet::Socket socket)
{
	Packet packet(3 /*ping code*/, 9, this->ident, 0);
	if (socket.SendAll((char*)packet.data(), packet.getLength()) == PNet::PResult::P_Success)
	{
		this->wait.push_back(this->ident);
		identIncrement();
		return PNet::PResult::P_Success;
	}
	else
	{
		return PNet::PResult::P_GenericError;
	}
}

PNet::PResult Executor::ContactsCheckOut(PNet::Socket socket)
{
	Packet packet(4 /*Contacts code*/, 9, this->ident, 0);
	if (socket.SendAll((char*)packet.data(), packet.getLength()) == PNet::PResult::P_Success)
	{
		this->wait.push_back(this->ident);
		identIncrement();
		return PNet::PResult::P_Success;
	}
	else
	{
		return PNet::PResult::P_GenericError;
	}
}

PNet::PResult Executor::HistoryCheckOut(std::string login, PNet::Socket socket)
{
	Packet packet(5 /*history check code*/, 9, this->ident, 0);
	packet.makeStringItem(login.data(), login.size());

	if (socket.SendAll((char*)packet.data(), packet.getLength()) == PNet::PResult::P_Success)
	{
		this->wait.push_back(this->ident);
		identIncrement();
		return PNet::PResult::P_Success;
	}
	else
	{
		return PNet::PResult::P_GenericError;
	}
}

PNet::PResult Executor::ConversationOut(std::string login, std::string message, PNet::Socket socket)
{
	Packet packet(6 /*message send code*/, 9, this->ident, 0);
	packet.makeStringItem(login.data(), login.size());
	packet.makeStringItem(message.data(), message.size());

	if (socket.SendAll((char*)packet.data(), packet.getLength()) == PNet::PResult::P_Success)
	{
		this->wait.push_back(this->ident);
		identIncrement();
		return PNet::PResult::P_Success;
	}
	else
	{
		return PNet::PResult::P_GenericError;
	}
}
