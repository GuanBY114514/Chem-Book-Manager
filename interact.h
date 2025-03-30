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
		std::cout << "login  --登录\n";
		std::cout << "exitpra  --退出程序\n";
		std::cout << "about  --关于应用程序\n";
		std::cout << "help  --查看帮助\n";
	}
	else if (_state == Local_user)
	{
		std::cout << "borrow   --借书\n";
		std::cout << "exit  --退出登录\n";
		std::cout << "return  --还书\n";
		std::cout << "view  --查看借书情况\n";
		std::cout << "help  --查看帮助\n";
	}
	else if (_state == L_Administrator)
	{
		std::cout << "view  --查看所有借书情况\n";
		std::cout << "borrow  --借书\n";
		std::cout << "exit  --退出登录\n";
		std::cout << "help  --查看帮助\n";
		std::cout << "return  --还书\n";
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

int Find_num(std::string _name)
{
	for (int i = 0; i < users.size(); i++)
	{
		if (users[i].Get_name() == _name)
		{
			return i;
		}
	}
	return -1;
}

extern int max_borrowed;

void View_Current_User_Borrowed_Book(User _current_user)
{
	std::vector<Book> tmp = _current_user.Get_Borrowed_Book_List();
	std::cout << "当前用户" << _current_user.Get_name() << "借书情况:\n";
	if (tmp.size() == 0)
	{
		std::cout << "当前用户没有借书\n";
		return;
	}
	for (int i = 0; i < tmp.size(); i++)
	{
		std::cout << i + 1 << " " << tmp[i].name << " " << time_to_str(tmp[i].borrowed_book_time) << std::endl;
	}
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
					Logined_User = Pre_Logined_User;
					std::cout << "登陆成功!\n";
					Current_State = Logined_User.Get_Level();
					num_of_list = Find_num(name_tmp);
					log_manager.log(Log_lev::USER_INFO, "User:" + name_tmp + "登陆成功");
				}
				else
				{
					std::cerr << "密码错误\n";
					Pre_Logined_User = User();
					log_manager.log(Log_lev::USER_INFO, "User：" + name_tmp + "登录失败，原因：密码错误");
				}
			}
			else
			{
				std::cout << "无该用户，请检查输入\n";
				log_manager.log(Log_lev::USER_INFO, "User：" + name_tmp + "登录失败，原因：无该用户");
			}
		}
		else if (commandP == "help")
		{
			command_help(User(Unlogined).Get_Level());
		}
		else if (commandP == "exitpra")
		{
			std::cout << "软件退出成功\n";
			log_manager.log(Log_lev::APP_INFO, "软件已退出");
			Logined_user = "";
			Current_State = Unlogined;
			Logined_User = User();
			is_exit = true;
			return;
		}
		else if (commandP == "about")
		{
			MessageBoxW(NULL, TEXT("化竞图书管理系统\n版本：v1.0\n作者：2401 GuanBY"), TEXT("关于"), MB_OK);
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
			Logined_User = User();
		}
		else if (commandP == "login")
		{
			std::cout << "你已经登录了\n";
		}
		else if (commandP == "help")
		{
			command_help(User(Local_user).Get_Level());
		}
		else if (commandP == "borrow")
		{
			std::string book_name;
			std::cout << "请输入要借的书名:";
			std::cin >> book_name;
			if (Logined_User.Get_Borrowed_Book_List().size() >= max_borrowed)
			{
				std::cerr << "你已经借了" << max_borrowed << "本书，无法再借\n";
				log_manager.log(Log_lev::USER_INFO, "User：" + Logined_User.Get_name() + "借书失败，原因：借书数量已达上限");
			}
			else
			{

				std::cout << "借书成功\n";
				Logined_User.Borrow_Book(book_name);
				users[num_of_list] = Logined_User;
				log_manager.log(Log_lev::USER_INFO, "User：" + Logined_User.Get_name() + "借书成功");
				Rewrite_User_File(users);

			}
		}
		else if (commandP == "return")
		{
			std::cout << "\n当前借书情况:\n";
			View_Current_User_Borrowed_Book(Logined_User);
			std::cout << "\n请输入以序号还是书名归还 序号:x 书名:n\n请输入：";
			std::string command_l;
			std::cin >> command_l;
			if (command_l == "x")
			{
				int num;
				std::cout << "请输入序号:";
				std::cin >> num;
				num--;
				if (num >= Logined_User.Get_Borrowed_Book_List().size())
				{
					std::cerr << "请输入正确的序号\n";
					log_manager.log(Log_lev::USER_INFO, "User：" + Logined_User.Get_name() + "还书失败，原因：序号错误");
				}
				else
				{
					Logined_User.Return_Book(num);
					users[num_of_list] = Logined_User;
					Rewrite_User_File(users);
					std::cout << "还书成功\n";
					log_manager.log(Log_lev::USER_INFO, "User：" + Logined_User.Get_name() + "还书成功");
				}
			}
			else if (command_l == "n")
			{
				std::string book_name;
				std::cout << "请输入要还的书名:";
				std::cin >> book_name;
				if (Logined_User.Return_Book(book_name))
				{
					users[num_of_list] = Logined_User;
					Rewrite_User_File(users);
					std::cout << "还书成功\n";
					log_manager.log(Log_lev::USER_INFO, "User：" + Logined_User.Get_name() + "还书成功");
				}
				else
				{
					std::cerr << "未找到该书\n";
					log_manager.log(Log_lev::USER_INFO, "User：" + Logined_User.Get_name() + "还书失败，原因：未找到该书");
				}
			}
			else
			{
				std::cerr << "请输入正确的指令\n";
			}
		}
		else if(commandP=="view")
		{
			View_Current_User_Borrowed_Book(Logined_User);
		}
		else
		{
			std::cout << "请输入正确的指令\n";
		}
		break;

//--------------------------------------------------------------------------------------
	case L_Administrator:
		if (commandP == "view")
		{
			std::string comd;
			std::cout << "请输入在文件中还是控制台 文件：file 控制台:com\n请输入：";
			std::cin >> comd;
			if (comd == "file")
			{
				std::cout << "文件生成成功\n";
				Administrator(Logined_User).View_All_Borrowed_Book_In_HTML_Table(Get_All_Borrowed_Book()); 
				//生成文件
				system("start borrowed_book.html");
				log_manager.log(Log_lev::FILE_OUT, "User：" + Logined_User.Get_name() + "成功生成文件borrowed_book.html");
			}
			else if (comd == "con")
			{
				Administrator(Logined_User).View_All_Borrowed_Book_In_Console(Get_All_Borrowed_Book());
			}
			else
			{
				std::cout << "请输入正确的指令\n";
			}
		}
		else if (commandP == "exit")
		{
			std::cout << "用户退出成功\n";
			log_manager.log(Log_lev::USER_INFO, "User：" + Logined_User.Get_name() + "退出成功");
			Logined_user = "";
			Current_State = Unlogined;
		}
		else if (commandP == "borrow")
		{
			std::string book_name;
			std::cout << "请输入要借的书名:";
			std::cin >> book_name;
			std::cout << "借书成功\n";
			Logined_User.Borrow_Book(book_name);
			users[num_of_list] = Logined_User;
			Rewrite_User_File(users);
			log_manager.log(Log_lev::USER_INFO, "User：" + Logined_User.Get_name() + "借书成功");

			
		}
		else if (commandP == "login")
		{
			std::cout << "你已经登录了\n";
		}
		else if (commandP == "return")
		{
			std::string book_name;
			std::cout << "请输入要还的书名:";
			std::cin >> book_name;
			Logined_User.Return_Book(book_name);
			users[num_of_list] = Logined_User;
			Rewrite_User_File(users);
		}
		else if (commandP == "help")
		{
			command_help(User(L_Administrator).Get_Level());
		}
		else
		{
			std::cout << "请输入正确的指令\n";
		}
		break;
	default:
		break;
	}
	

	std::cout << "\n";
}