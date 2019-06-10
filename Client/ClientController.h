#pragma once
#include "Executor.h"

class ClientController
{
public:
	ClientController() {};
	~ClientController();

	void DataOut();
	void DataIn();

private:
	/*
	std::shared_ptr<std::thread> threadOut;
	std::shared_ptr<std::thread> threadIn;
	*/
};
