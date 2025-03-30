#include "stdc++.h"
#include <windows.h>
#include <WinUser.h>
#include <io.h>
#include <fcntl.h>

enum Login_User_State
{
	Unlogined = -1,
	Local_user,
	L_Administrator
};

#include"UserBehaver.h"
#include"LoadFile.h"

std::string command;										 //当前命令
std::string Logined_user;                                    //当前登录用户名
std::vector<User> users;									 //用户列表
User Logined_User;                                           //当前登录用户
User Pre_Logined_User;                                       //预登录用户
Login_User_State Current_State = Unlogined;                  //当前登录状态
unsigned int num_of_list;                                    //当前用户在用户列表中的位置
int max_borrowed;											 //最大借书数量
Logger log_manager;                                          //日志记录器
bool is_exit = false;                                        //是否退出

#include "interact.h"

int main()
{
	//初始化
	
	// 设置任务栏图标
	HINSTANCE hInstance = GetModuleHandle(nullptr);
	HICON hAppIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));

	// 同时设置控制台窗口图标（可选）
	HWND hConsole = GetConsoleWindow();
	SendMessage(hConsole, WM_SETICON, ICON_SMALL, (LPARAM)hAppIcon);
	SendMessage(hConsole, WM_SETICON, ICON_BIG, (LPARAM)hAppIcon);

	//加载用户信息

	Load_Setting(max_borrowed);
	Load_Users();
	system("color f1");
	std::wcout << "\n\n**************************\n";
	std::wcout << "*欢迎使用化竞图书管理系统*\n";
	std::wcout << "**************************\n\n\n操作台:\n";

	log_manager.log(Logger::Level::ENDL, "");
	log_manager.log(Logger::Level::APP_INFO, "应用启动");

	//交互模块
	std::wcout << ">>>";
	while ((std::cin >> command))
	{
		Interact(Current_State, command);
		std::cout << Logined_user << ">>>";
		if (is_exit)
		{
			break;
		}
	}

	return 0;
}