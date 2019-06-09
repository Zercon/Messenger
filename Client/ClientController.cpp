#include "ClientController.h"
#include <PNet\IncludeMe.h>
#include <thread>
#include "Simulator.h"

using namespace PNet;

static bool debug = true;

ClientController::~ClientController()
{
	/*
	this->threadOut->join();
	this->threadIn->join();
	*/
}

void ClientController::DataOut()
{
	Socket socket;
	if (socket.Create() == PResult::P_Success)
	{
		std::cout << "Socket successfully created." << std::endl;
		if (socket.Connect(IPEndpoint("127.0.0.1", 4790)) == PResult::P_Success) // «ƒ≈—№ —ќ≈ƒ»Ќя≈ћ—я — —≈–¬≈–ќћ, 1 ј–√”ћ≈Ќ“- ј…ѕ», 2- ѕќ–“, »—ѕќЋ№«”≈“—я ѕќƒ Ћё„≈Ќ»≈ IPv4
		{
			std::cout << "Successfully connected to server!" << std::endl;

			bool authorised = false;

			if (debug)
			{
				std::cout << "(debug is on): " << std::endl;
				system("pause");
			}

			while (true)
			{
				system("cls");

				std::string login_;
				std::string password_;
				std::string message_;
				int expected_login;
				Packet packet;

				Executor execute;
				if (!authorised)
				{
					//как бы отправили сообщение с кодом 1 или 2
					if (Choise()) 
					{
						Input(login_, password_);
						//если авторизаци€ клик
						if (execute.AuthoriseOut(login_, password_, socket) != PResult::P_Success)
						{
							std::cerr << "Failed to send authorisation request to server" << std::endl;
							if (debug)
							{
								std::cout << "(debug is on): " << std::endl;
								system("pause");
							}
							continue;
						}
					}
					else
					{
						Input(login_, password_);
						//если регистраци€
						if (execute.RegistrationOut(login_, password_, socket) != PResult::P_Success)
						{
							std::cerr << "Failed to send registration request to server" << std::endl;
							if (debug)
							{
								std::cout << "(debug is on): " << std::endl;
								system("pause");
							}
							continue;
						}
					}


					//как бы принимаем сообщение с кодом 1 или 2 (костыль)
					if (socket.RecvAll(&packet, sizeof(Packet)) != PNet::PResult::P_Success)
					{
						std::cerr << "Failed to accept the packet from server" << std::endl;
						if (debug)
						{
							std::cout << "(debug is on): " << std::endl;
							system("pause");
						}
						continue;
					}

					/*if (execute.WaitCheck(packet) != PNet::PResult::P_Success) //возможно это надо убрать
					{
						std::cerr << "Received unexpected response from server" << std::endl;
						continue;
					}*/

					switch (packet.getCod())
					{
					case 129: //registration
						authorised = execute.RegistrationIn(packet);
						break;
					case 130: //authorisation
						authorised = execute.AuthoriseIn(packet);
						break;
					default:
						std::cerr << "Failed to recognize the packet" << std::endl;
						if (debug)
						{
							std::cout << "(debug is on): " << std::endl;
							system("pause");
						}
					}
				}
				else
				{
					/*
					//пинг
					if (execute.PingOut(socket) != PResult::P_Success)
					{
						std::cerr << "Failed to send ping to server" << std::endl;
					}
					*/
					//отправл€ем запрос контактов
					if (execute.ContactsCheckOut(socket) != PResult::P_Success)
					{
						std::cerr << "Failed to send contacts request to server" << std::endl;
						if (debug)
						{
							std::cout << "(debug is on): " << std::endl;
							system("pause");
						}
						continue;
					}
					//принимаем пакет с контактами (костыль)
					if (socket.RecvAll(&packet, sizeof(Packet)) != PResult::P_Success)
					{
						std::cerr << "Failed to accept the packet from server" << std::endl;
						if (debug)
						{
							std::cout << "(debug is on): " << std::endl;
							system("pause");
						}
						continue;
					}
					execute.ContactsCheckIn(packet); //(костыль)
					PrintContacts(execute);
					//истори€ с пользователем
					expected_login = UserChoise(execute); // понимаем с каким пользователем хоти общатьс€

					do
					{
						system("cls");

						if (execute.HistoryCheckOut(execute.contacts[expected_login], socket) != PResult::P_Success)
						{
							std::cerr << "Failed to send history request to server" << std::endl;
							if (debug)
							{
								std::cout << "(debug is on): " << std::endl;
								system("pause");
							}
							continue;
						}
						//принимаю пакет с историей сообщ (костыль)
						if (socket.RecvAll(&packet, sizeof(Packet)) != PResult::P_Success)
						{
							std::cerr << "Failed to accept the packet from server" << std::endl;
							if (debug)
							{
								std::cout << "(debug is on): " << std::endl;
								system("pause");
							}
							continue;
						}
						execute.HistoryCheckIn(expected_login, packet); //костыль

						//отправить сообщ
						InputMessage(message_);

						if (execute.ConversationOut(execute.contacts[expected_login], message_, socket) != PResult::P_Success)
						{
							std::cerr << "Failed to send message to server" << std::endl;
							if (debug)
							{
								std::cout << "(debug is on): " << std::endl;
								system("pause");
							}
							continue;
						}
						//мы ничего не принимаем, потому что мы прост в следующий раз всю историю заново выведем (костыль)

						if (AnotherChoise()) execute.talking_flag = false;
					} while (execute.talking_flag);
				}
			 			  			  			 		   
				/*
				//принимаем пакеты от серва
				if (execute.IfWait())
				{
					if (!authorised)
					{
						Packet packet;
						if (socket.RecvAll(&packet, sizeof(Packet)) != PNet::PResult::P_Success)
						{
							std::cerr << "Failed to accept the packet from server" << std::endl;
							continue;
						}

						if (execute.WaitCheck(packet) != PNet::PResult::P_Success)
						{
							std::cerr << "Received unexpected response from server" << std::endl;
							continue;
						}

						switch (packet.getCod())
						{
						case 129: //registration
							authorised = execute.RegistrationIn(packet);
							break;
						case 130: //authorisation
							authorised = execute.AuthoriseIn(packet);
							break;
						default:
							std::cerr << "Failed to recognize the packet" << std::endl;
						}
					}
					else
					{
						Packet packet;
						if (socket.RecvAll(&packet, sizeof(Packet)) != PResult::P_Success)
						{
							std::cerr << "Failed to accept the packet from server" << std::endl;
							continue;
						}
						/*
						if (execute.WaitCheck(packet) != PNet::PResult::P_Success)
						{
							std::cerr << "Received unexpected response from server" << std::endl;
							continue;
						}
						*/ //не может работать, т.к. вход€щие сообщени€ не могут ожидатьс€ клиентом, они просто приход€т, когда их отослали
				/*
						switch (packet.getCod())
						{
						case 131: //ping
							break;
						case 132: //contacts
							execute.ContactsCheckIn(packet);
							break;
						case 133: //history
							execute.HistoryCheckIn(execute.contacts[expected_login], packet);
							break;
						case 134: //message

							break;
						default:
							std::cerr << "Failed to recognize the packet" << std::endl;
						}
					}

				}
				*/
			}
		}
		else
		{
			std::cerr << "Failed to connect to server." << std::endl;
		}
		socket.Close();
	}
	else
	{
		std::cerr << "Socket failed to create." << std::endl;
	}
}


/*
void ClientController::DataIn()
{
	while (true)
	{
		Socket socket;
		if (socket.Create() == PResult::P_Success)
		{
			std::cout << "Socket successfully created." << std::endl;
			if (socket.Listen(IPEndpoint("0.0.0.0", 4790)) == PResult::P_Success) //кусок на прин€тие данных
			{
				std::cout << "Socket successfully listening on port 4790." << std::endl;
				Socket newConnection;
				if (socket.Accept(newConnection) == PResult::P_Success)
				{
					std::cout << "New connection accepted." << std::endl;
					
					while (true)
					{
						Packet packet;
						if (newConnection.RecvAll(&packet, sizeof(Packet)) != PNet::PResult::P_Success)
						{
							std::cerr << "Failed to accept the packet from server" << std::endl;
						}
						else
						{
							uint8_t cod = packet.getCod();

							switch (cod)
							{
							case 129: //registration

								break;
							case 130: //authorisation

								break;
							case 131: //ping

								break;
							case 132: //contacts

								break;
							case 133: //history

								break;
							case 134: //message

								break;
							default:
								std::cerr << "Failed to recognize the packet" << std::endl;



							}


						}













					}
				}
				else
				{
					std::cerr << "Failed to accept new connection." << std::endl;
				}

			}
			else
			{
				std::cerr << "Failed to listen on port 4790." << std::endl;
			}
			socket.Close();
		}
		else
		{
			std::cerr << "Socket failed to create." << std::endl;
		}
	}
}
*/

