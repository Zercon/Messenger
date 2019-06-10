#include "Simulator.h"

using namespace std;

void Input(std::string & login, std::string & password)
{
	cout << "enter login: ";
	cin >> login;
	cout << "enter password: ";
	cin >> password;
}

bool Choise()
{
	while (true)
	{
		cout << "1: log in" << endl <<
				"2: registrate new accout" << endl;
		string s;
		cin >> s;
		if (s == "1") return true;
		else
			if (s == "2") return false;
			else cout << "wrong input, try again" << endl;
	}
}

bool AnotherChoise()
{
	while (true)
	{
		cout << "1: continue talking to chosen user" << endl <<
				"2: return to contacts" << endl;
		string s;
		cin >> s;
		if (s == "1") return false;
		else
			if (s == "2") return true;
			else cout << "wrong input, try again" << endl;
	}
}

int UserChoise(Executor execute)
{
	while (true)
	{
		cout << "enter id of user you want to talk to: ";
		int choise;
		cin >> choise;
		if ((choise > 0) && (choise <= execute.contacts.size()))
		{
			execute.talking_flag = true;
			return choise - 1;
		}
		else cout << "wrong id, try again" << endl;
	}
}

void InputMessage(std::string &message)
{
	cout << ": ";
	cin >> message;
}

void PrintContacts(Executor execute)
{
	for(int i=0; i<execute.contacts.size()-1;i++)
	{
		cout << "id " << i + 1 << ": " << execute.contacts[i] << endl;
	}
}


