#pragma once
#define Log_lev Logger::Level


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
		std::cout << "change_password  --修改密码\n";
	}
	else if (_state == L_Administrator)
	{
		std::cout << "view  --查看所有借书情况\n";
		std::cout << "borrow  --借书\n";
		std::cout << "exit  --退出登录\n";
		std::cout << "help  --查看帮助\n";
		std::cout << "return  --还书\n";
		std::cout << "adduser  --添加用户\n";
		std::cout << "deluser  --删除用户\n";
		std::cout << "change_password  --修改密码\n";
		std::cout << "change_max_borrowed  --修改最大借书数量\n";
		std::cout << "change_color  --修改颜色\n";
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
					log_manager.log(Log_lev::USER_INFO, "User：" + name_tmp + "登陆成功");
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
				log_manager.log(Log_lev::BORROW, "User：" + Logined_User.Get_name() + "借书失败，原因：借书数量已达上限");
			}
			else
			{

				std::cout << "借书成功\n";
				Logined_User.Borrow_Book(book_name);
				users[num_of_list] = Logined_User;
				log_manager.log(Log_lev::BORROW, "User：" + Logined_User.Get_name() + "借书成功");
				Rewrite_User_File(users);

			}
		}
		else if (commandP == "return")
		{
			std::cout << "\n当前借书情况:\n";
			View_Current_User_Borrowed_Book(Logined_User);
			if (Logined_User.Get_Borrowed_Book_List().size() == 0)
			{
				std::cout << "当前没有借书\n";
				return;
			}
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
					log_manager.log(Log_lev::RETURN, "User：" + Logined_User.Get_name() + "还书失败，原因：序号错误");
				}
				else
				{
					Logined_User.Return_Book(num);
					users[num_of_list] = Logined_User;
					Rewrite_User_File(users);
					std::cout << "还书成功\n";
					log_manager.log(Log_lev::RETURN, "User：" + Logined_User.Get_name() + "还书成功");
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
					log_manager.log(Log_lev::RETURN, "User：" + Logined_User.Get_name() + "还书成功");
				}
				else
				{
					std::cerr << "未找到该书\n";
					log_manager.log(Log_lev::RETURN, "User：" + Logined_User.Get_name() + "还书失败，原因：未找到该书");
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
		else if (commandP == "change_password")
		{
			std::string old_password;
			old_password = get_password("请输入原密码:", true);
			int flag = Logined_User.Change_Password(old_password);
			if (flag == 0)
			{
				std::cout << "修改成功\n";
				users[num_of_list] = Logined_User;
				Rewrite_User_File(users);
				log_manager.log(Log_lev::USER_INFO, "User：" + Logined_User.Get_name() + "修改密码成功");
			}
			else if (flag == 1)
			{
				std::cerr << "两次输入密码不一致\n";
				log_manager.log(Log_lev::USER_INFO, "User：" + Logined_User.Get_name() + "修改密码失败，原因：两次输入密码不一致");
			}
			else
			{
				std::cerr << "原密码错误\n";
				log_manager.log(Log_lev::USER_INFO, "User：" + Logined_User.Get_name() + "修改密码失败，原因：原密码错误");
			}
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
			std::cout << "请输入在文件中还是控制台 文件：file 控制台:con\n请输入：";
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
			log_manager.log(Log_lev::BORROW, "User：" + Logined_User.Get_name() + "借书成功");
		}
		else if (commandP == "change_password")
		{
			std::string old_password;
			old_password = get_password("请输入原密码:", true);
			int flag = Logined_User.Change_Password(old_password);
			if (flag == 0)
			{
				std::cout << "修改成功\n";
				users[num_of_list] = Logined_User;
				Rewrite_User_File(users);
				log_manager.log(Log_lev::USER_INFO, "User：" + Logined_User.Get_name() + "修改密码成功");
			}
			else if (flag == 1)
			{
				std::cerr << "两次输入密码不一致\n";
				log_manager.log(Log_lev::USER_INFO, "User：" + Logined_User.Get_name() + "修改密码失败，原因：两次输入密码不一致");
			}
			else
			{
				std::cerr << "原密码错误\n";
				log_manager.log(Log_lev::USER_INFO, "User：" + Logined_User.Get_name() + "修改密码失败，原因：原密码错误");
			}
		}
		else if (commandP == "login")
		{
			std::cout << "你已经登录了\n";
		}
		else if (commandP == "return")
		{
			std::cout << "\n当前借书情况:\n";
			View_Current_User_Borrowed_Book(Logined_User);
			if (Logined_User.Get_Borrowed_Book_List().size() == 0)
			{
				std::cout << "当前没有借书\n";
				return;
			}
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
					log_manager.log(Log_lev::RETURN, "User：" + Logined_User.Get_name() + "还书失败，原因：序号错误");
				}
				else
				{
					Logined_User.Return_Book(num);
					users[num_of_list] = Logined_User;
					Rewrite_User_File(users);
					std::cout << "还书成功\n";
					log_manager.log(Log_lev::RETURN, "User：" + Logined_User.Get_name() + "还书成功");
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
					log_manager.log(Log_lev::RETURN, "User：" + Logined_User.Get_name() + "还书成功");
				}
				else
				{
					std::cerr << "未找到该书\n";
					log_manager.log(Log_lev::RETURN, "User：" + Logined_User.Get_name() + "还书失败，原因：未找到该书");
				}
			}
			else
			{
				std::cerr << "请输入正确的指令\n";
			}
			Rewrite_User_File(users);
		}
		else if (commandP == "help")
		{
			command_help(User(L_Administrator).Get_Level());
		}
		else if (commandP == "adduser")
		{
			std::string name;
			std::string password;
			std::cout << "请输入用户名:";
			std::cin >> name;
			std::cout << "请输入密码:";
			std::cin >> password;
			users.push_back(User(name, password, 0, {}, Local_user));
			Rewrite_User_File(users);
			std::cout << "添加用户成功\n";
			log_manager.log(Log_lev::USER_INFO, "User：" + Logined_User.Get_name() + "添加用户" + name + "成功");
		}
		else if (commandP == "deluser")
		{
			std::string name;
			std::cout << "请输入要删除的用户名:";
			std::cin >> name;
			for (int i = 0; i < users.size(); i++)
			{
				if (users[i].Get_name() == name)
				{
					users.erase(users.begin() + i);
					Rewrite_User_File(users);
					std::cout << "删除用户成功\n";
					log_manager.log(Log_lev::USER_INFO, "User：" + Logined_User.Get_name() + "删除用户" + name + "成功");
					return;
				}
			}
			std::cerr << "未找到该用户\n";
			log_manager.log(Log_lev::USER_INFO, "User：" + Logined_User.Get_name() + "删除用户失败，原因：未找到该用户");
		}
		else if (commandP == "change_max_borrowed")
		{
			std::cout << "请输入新的最大借书数量:";
			std::cin >> max_borrowed;
			std::cout << "修改成功\n";
			Rewrite_Setting_File(max_borrowed, color);
			log_manager.log(Log_lev::APP_INFO, "User：" + Logined_User.Get_name() + "修改最大借书数量成功");
		}
		else if (commandP == "change_color")
		{
			std::string color;
			std::cout << "请输入新的颜色:";
			std::cin >> color;
			system(("color " + color).c_str());
			std::cout << "修改成功\n";
			Rewrite_Setting_File(max_borrowed, color);
			log_manager.log(Log_lev::APP_INFO, "User：" + Logined_User.Get_name() + "修改颜色成功");
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