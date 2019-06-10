#include <PNet\IncludeMe.h>
#include <iostream>
#include "Executor.h"
#include <thread>
#include "ClientController.h"

using namespace PNet;

int main()
{
	if (Network::Initialize())
	{
		std::cout << "Winsock api successfully initialized." << std::endl;
				//отправление сообщений

				ClientController ccontrol;

				//std::thread thread1(&ClientController::DataIn, std::ref(ccontrol));
				//std::thread thread2(&ClientController::DataOut, std::ref(ccontrol));
				//thread1.join();
				//thread2.join();

				ccontrol.DataOut();







				
				/*
				std::string buffer = "Hello world from client!";

				while (true)
				{
					uint32_t bufferSize = buffer.size();
					bufferSize = htonl(bufferSize); //convert from host to network byte order - all integers sent over a stream should be in network byte order
					int result = socket.SendAll(&bufferSize, sizeof(uint32_t));
					if (result != PResult::P_Success)
						break;

					result = socket.SendAll(buffer.data(), buffer.size());
					if (result != PResult::P_Success)
						break;

					std::cout << "Attempting to send chunk of data..." << std::endl;
					Sleep(500);
				}
				*/
			
		
	}
	Network::Shutdown();
	system("pause");
	return 0;
}