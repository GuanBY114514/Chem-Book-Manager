#include<stdc++.h>

enum Login_User_State
{
	Unlogined = -1,
	Local_user,
	Administrator
};

#include"UserBehaver.h"
#include"LoadFile.h"

std::string command;
std::string Logined_user;
std::vector<User> users;
User Logined_User;                                           //当前登录用户
User Pre_Logined_User;                                       //预登录用户
Login_User_State Current_State = Unlogined;                  //当前登录状态
size_t Users_num;                                            //用户数量
Logger log_manager;                                          //

#include "interact.h"

int main()
{
	//初始化
	int max_borrowed;
	Load_Setting(max_borrowed);
	Load_Users();
	system("color f1");
	std::cout << "\n\n**************************\n";
	std::cout << "*欢迎使用化竞图书管理系统*\n";
	std::cout << "**************************\n\n\n操作台:\n";

	log_manager.log(Logger::Level::APP_INFO, "应用启动");

	//Main Function
	std::cout << ">>>";
	while (std::getline(std::cin,command))
	{
		Interact(Current_State, command);
		std::cout << Logined_user << ">>>";
	}

	return 0;
}