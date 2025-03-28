#pragma once
#include<conio.h>
#define Log_lev Logger::Level

std::string get_password(const char prompt[], bool show_asterisk = true) {
	std::string password;
	std::cout << prompt;

#ifdef _WIN32
	int ch;
	while (true) {
		ch = _getch();
		// ������չ�������緽�����
		if (ch == 0 || ch == 224) {
			_getch();  // ������չ�����ĵڶ����ֽ�
			continue;
		}
		if (ch == '\r')  // �س�����������
			break;
		if (ch == '\b') { // �����˸��
			if (!password.empty()) {
				password.pop_back();
				if (show_asterisk)
					std::cout << "\b \b"; // ���˲������Ǻ�
			}
		}
		else {
			password.push_back(static_cast<char>(ch));
			if (show_asterisk)
				std::cout << '*';
		}
	}
#else
	struct termios oldt, newt;
	tcgetattr(STDIN_FILENO, &oldt); // ���浱ǰ�ն�����
	newt = oldt;
	newt.c_lflag &= ~(ECHO | ICANON); // ���û��Ժ͹淶ģʽ
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	char ch;
	while (read(STDIN_FILENO, &ch, 1) == 1 && ch != '\n') {
		if (ch == 127 || ch == '\b') { // �����˸�
			if (!password.empty()) {
				password.pop_back();
				if (show_asterisk) {
					std::cout << "\b \b";
					std::cout.flush();
				}
			}
		}
		else {
			password.push_back(ch);
			if (show_asterisk) {
				std::cout << '*';
				std::cout.flush();
			}
		}
	}
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // �ָ��ն�����
#endif
	std::cout << std::endl;
	return password;
}

void command_help(Login_User_State _state)
{
	std::cout << "\n��ǰ��������:\n";
	if (_state == Unlogined)
	{
		std::cout << "help  --��ȡ����\nexitpra   --�˳����\nlogin   --��¼�û�\n";
	}
	else if (_state == Local_user)
	{
		std::cout << "borrow   --����\n";
	}
	else if (_state == Administrator)
	{
		std::cout << "view  --�鿴���н������\n";
	}

	std::cout << "\n";

}

bool If_In_Users_List(std::string _name)
{
	for (int i = 0; i < users.size(); i++)
	{
		if (_name == users[i].Get_name())
		{
			Pre_Logined_User = users[i];
			return true;
		}
	}
	return false;
}

bool Is_Password_Correct(std::string _passward)
{
	if (_passward == Pre_Logined_User.Get_Password())
	{
		return true;
	}
	else
	{
		std::cerr << "";
		Pre_Logined_User = User();
		return false;
	}
}

bool Is_Password_Correct(const std::string _real_password, const std::string _password)
{
	if (_real_password == _password)
	{
		return true;
	}
	else
	{
		return false;
	}
}

User Find(std::string _name)
{
	for (int i = 0; i < users.size(); i++)
	{
		if (users[i].Get_name() == _name)
		{
			return users[i];
		}
	}
	return User();
}

void Interact(Login_User_State User_State, std::string commandP)
{
	std::cout << "\n";

	switch (User_State)
	{

	case Unlogined:
//----------------------------------------------------------------------------------
		if (commandP == "login")
		{
			std::string name_tmp;
			std::cout << "�������û���:";
			std::cin >> name_tmp;
			if (If_In_Users_List(name_tmp))
			{
				Pre_Logined_User = Find(name_tmp);
				std::string tmp_password = get_password("����������:", true);
				if (Is_Password_Correct(Pre_Logined_User.Get_Password(), tmp_password))
				{
					Logined_user = name_tmp;
					std::cout << "��½�ɹ�!\n";
					Current_State = User(Local_user).Get_Level();
					log_manager.log(Log_lev::USER_INFO, "User:" + name_tmp + "��½�ɹ�");
				}
				else
				{
					std::cerr << "�������\n";
					log_manager.log(Log_lev::USER_INFO, "User��" + name_tmp + "��¼ʧ�ܣ�ԭ���������");
				}
			}
			else
			{
				std::cout << "�޸��û�����������\n";
				log_manager.log(Log_lev::USER_INFO, "User��" + name_tmp + "��¼ʧ�ܣ�ԭ���޸��û�");
			}
		}
		else
		{
			std::cout << "��������ȷ��ָ��\n";
		}
		break;

//------------------------------------------------------------------------------------
	case Local_user:
		if (commandP == "exit")
		{
			std::cout << "�û��˳��ɹ�\n";
			Logined_user = "";
			Current_State = Unlogined;
			log_manager.log(Log_lev::USER_INFO, "User��" + Logined_User.Get_name() + "�˳��ɹ�");
		}
		else if (commandP == "login")
		{
			std::cout << "���Ѿ���¼��\n";
		}
		else if (commandP == "help")
		{
			command_help(User(Local_user).Get_Level());
		}
		else
		{
			std::cout << "��������ȷ��ָ��\n";
		}
		break;

//--------------------------------------------------------------------------------------
	case Administrator:
		break;
	default:
		break;
	}
	

	std::cout << "\n";
}