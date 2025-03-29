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
		// 处理扩展按键（如方向键）
		if (ch == 0 || ch == 224) {
			_getch();  // 忽略扩展按键的第二个字节
			continue;
		}
		if (ch == '\r')  // 回车键结束输入
			break;
		if (ch == '\b') { // 处理退格键
			if (!password.empty()) {
				password.pop_back();
				if (show_asterisk)
					std::cout << "\b \b"; // 回退并擦除星号
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
	tcgetattr(STDIN_FILENO, &oldt); // 保存当前终端设置
	newt = oldt;
	newt.c_lflag &= ~(ECHO | ICANON); // 禁用回显和规范模式
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	char ch;
	while (read(STDIN_FILENO, &ch, 1) == 1 && ch != '\n') {
		if (ch == 127 || ch == '\b') { // 处理退格
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
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // 恢复终端设置
#endif
	std::cout << std::endl;
	return password;
}

void command_help(Login_User_State _state)
{
	std::cout << "\n当前可用命令:\n";
	if (_state == Unlogined)
	{
		std::cout << "help  --获取帮助\nexitpra   --退出软件\nlogin   --登录用户\n";
	}
	else if (_state == Local_user)
	{
		std::cout << "borrow   --借书\n";
	}
	else if (_state == Administrator)
	{
		std::cout << "view  --查看所有借书情况\n";
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
			std::cout << "请输入用户名:";
			std::cin >> name_tmp;
			if (If_In_Users_List(name_tmp))
			{
				Pre_Logined_User = Find(name_tmp);
				std::string tmp_password = get_password("请输入密码:", true);
				if (Is_Password_Correct(Pre_Logined_User.Get_Password(), tmp_password))
				{
					Logined_user = name_tmp;
					std::cout << "登陆成功!\n";
					Current_State = User(Local_user).Get_Level();
					log_manager.log(Log_lev::USER_INFO, "User:" + name_tmp + "登陆成功");
				}
				else
				{
					std::cerr << "密码错误\n";
					log_manager.log(Log_lev::USER_INFO, "User：" + name_tmp + "登录失败，原因：密码错误");
				}
			}
			else
			{
				std::cout << "无该用户，请检查输入\n";
				log_manager.log(Log_lev::USER_INFO, "User：" + name_tmp + "登录失败，原因：无该用户");
			}
		}
		else
		{
			std::cout << "请输入正确的指令\n";
		}
		break;

//------------------------------------------------------------------------------------
	case Local_user:
		if (commandP == "exit")
		{
			std::cout << "用户退出成功\n";
			Logined_user = "";
			Current_State = Unlogined;
			log_manager.log(Log_lev::USER_INFO, "User：" + Logined_User.Get_name() + "退出成功");
		}
		else if (commandP == "login")
		{
			std::cout << "你已经登录了\n";
		}
		else if (commandP == "help")
		{
			command_help(User(Local_user).Get_Level());
		}
		else
		{
			std::cout << "请输入正确的指令\n";
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