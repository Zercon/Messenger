#pragma once
#include <iostream>
#include <string>
#include "Executor.h"


void Input(std::string &login, std::string &password);

bool Choise(); //true if log in

bool AnotherChoise(); //true if return to contacts

int UserChoise(Executor execute);

void InputMessage(std::string &message);

void PrintContacts(Executor execute);