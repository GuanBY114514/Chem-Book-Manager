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
		std::cout << "login  --��¼\n";
		std::cout << "exitpra  --�˳�����\n";
		std::cout << "about  --����Ӧ�ó���\n";
		std::cout << "help  --�鿴����\n";
	}
	else if (_state == Local_user)
	{
		std::cout << "borrow   --����\n";
		std::cout << "exit  --�˳���¼\n";
		std::cout << "return  --����\n";
		std::cout << "view  --�鿴�������\n";
		std::cout << "help  --�鿴����\n";
	}
	else if (_state == L_Administrator)
	{
		std::cout << "view  --�鿴���н������\n";
		std::cout << "borrow  --����\n";
		std::cout << "exit  --�˳���¼\n";
		std::cout << "help  --�鿴����\n";
		std::cout << "return  --����\n";
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
	std::cout << "��ǰ�û�" << _current_user.Get_name() << "�������:\n";
	if (tmp.size() == 0)
	{
		std::cout << "��ǰ�û�û�н���\n";
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
			std::cout << "�������û���:";
			std::cin >> name_tmp;
			if (If_In_Users_List(name_tmp))
			{
				Pre_Logined_User = Find(name_tmp);
				std::string tmp_password = get_password("����������:", true);
				if (Is_Password_Correct(Pre_Logined_User.Get_Password(), tmp_password))
				{
					Logined_user = name_tmp;
					Logined_User = Pre_Logined_User;
					std::cout << "��½�ɹ�!\n";
					Current_State = Logined_User.Get_Level();
					num_of_list = Find_num(name_tmp);
					log_manager.log(Log_lev::USER_INFO, "User:" + name_tmp + "��½�ɹ�");
				}
				else
				{
					std::cerr << "�������\n";
					Pre_Logined_User = User();
					log_manager.log(Log_lev::USER_INFO, "User��" + name_tmp + "��¼ʧ�ܣ�ԭ���������");
				}
			}
			else
			{
				std::cout << "�޸��û�����������\n";
				log_manager.log(Log_lev::USER_INFO, "User��" + name_tmp + "��¼ʧ�ܣ�ԭ���޸��û�");
			}
		}
		else if (commandP == "help")
		{
			command_help(User(Unlogined).Get_Level());
		}
		else if (commandP == "exitpra")
		{
			std::cout << "����˳��ɹ�\n";
			log_manager.log(Log_lev::APP_INFO, "������˳�");
			Logined_user = "";
			Current_State = Unlogined;
			Logined_User = User();
			is_exit = true;
			return;
		}
		else if (commandP == "about")
		{
			MessageBoxW(NULL, TEXT("����ͼ�����ϵͳ\n�汾��v1.0\n���ߣ�2401 GuanBY"), TEXT("����"), MB_OK);
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
			Logined_User = User();
		}
		else if (commandP == "login")
		{
			std::cout << "���Ѿ���¼��\n";
		}
		else if (commandP == "help")
		{
			command_help(User(Local_user).Get_Level());
		}
		else if (commandP == "borrow")
		{
			std::string book_name;
			std::cout << "������Ҫ�������:";
			std::cin >> book_name;
			if (Logined_User.Get_Borrowed_Book_List().size() >= max_borrowed)
			{
				std::cerr << "���Ѿ�����" << max_borrowed << "���飬�޷��ٽ�\n";
				log_manager.log(Log_lev::USER_INFO, "User��" + Logined_User.Get_name() + "����ʧ�ܣ�ԭ�򣺽��������Ѵ�����");
			}
			else
			{

				std::cout << "����ɹ�\n";
				Logined_User.Borrow_Book(book_name);
				users[num_of_list] = Logined_User;
				log_manager.log(Log_lev::USER_INFO, "User��" + Logined_User.Get_name() + "����ɹ�");
				Rewrite_User_File(users);

			}
		}
		else if (commandP == "return")
		{
			std::cout << "\n��ǰ�������:\n";
			View_Current_User_Borrowed_Book(Logined_User);
			std::cout << "\n����������Ż��������黹 ���:x ����:n\n�����룺";
			std::string command_l;
			std::cin >> command_l;
			if (command_l == "x")
			{
				int num;
				std::cout << "���������:";
				std::cin >> num;
				num--;
				if (num >= Logined_User.Get_Borrowed_Book_List().size())
				{
					std::cerr << "��������ȷ�����\n";
					log_manager.log(Log_lev::USER_INFO, "User��" + Logined_User.Get_name() + "����ʧ�ܣ�ԭ����Ŵ���");
				}
				else
				{
					Logined_User.Return_Book(num);
					users[num_of_list] = Logined_User;
					Rewrite_User_File(users);
					std::cout << "����ɹ�\n";
					log_manager.log(Log_lev::USER_INFO, "User��" + Logined_User.Get_name() + "����ɹ�");
				}
			}
			else if (command_l == "n")
			{
				std::string book_name;
				std::cout << "������Ҫ��������:";
				std::cin >> book_name;
				if (Logined_User.Return_Book(book_name))
				{
					users[num_of_list] = Logined_User;
					Rewrite_User_File(users);
					std::cout << "����ɹ�\n";
					log_manager.log(Log_lev::USER_INFO, "User��" + Logined_User.Get_name() + "����ɹ�");
				}
				else
				{
					std::cerr << "δ�ҵ�����\n";
					log_manager.log(Log_lev::USER_INFO, "User��" + Logined_User.Get_name() + "����ʧ�ܣ�ԭ��δ�ҵ�����");
				}
			}
			else
			{
				std::cerr << "��������ȷ��ָ��\n";
			}
		}
		else if(commandP=="view")
		{
			View_Current_User_Borrowed_Book(Logined_User);
		}
		else
		{
			std::cout << "��������ȷ��ָ��\n";
		}
		break;

//--------------------------------------------------------------------------------------
	case L_Administrator:
		if (commandP == "view")
		{
			std::string comd;
			std::cout << "���������ļ��л��ǿ���̨ �ļ���file ����̨:com\n�����룺";
			std::cin >> comd;
			if (comd == "file")
			{
				std::cout << "�ļ����ɳɹ�\n";
				Administrator(Logined_User).View_All_Borrowed_Book_In_HTML_Table(Get_All_Borrowed_Book()); 
				//�����ļ�
				system("start borrowed_book.html");
				log_manager.log(Log_lev::FILE_OUT, "User��" + Logined_User.Get_name() + "�ɹ������ļ�borrowed_book.html");
			}
			else if (comd == "con")
			{
				Administrator(Logined_User).View_All_Borrowed_Book_In_Console(Get_All_Borrowed_Book());
			}
			else
			{
				std::cout << "��������ȷ��ָ��\n";
			}
		}
		else if (commandP == "exit")
		{
			std::cout << "�û��˳��ɹ�\n";
			log_manager.log(Log_lev::USER_INFO, "User��" + Logined_User.Get_name() + "�˳��ɹ�");
			Logined_user = "";
			Current_State = Unlogined;
		}
		else if (commandP == "borrow")
		{
			std::string book_name;
			std::cout << "������Ҫ�������:";
			std::cin >> book_name;
			std::cout << "����ɹ�\n";
			Logined_User.Borrow_Book(book_name);
			users[num_of_list] = Logined_User;
			Rewrite_User_File(users);
			log_manager.log(Log_lev::USER_INFO, "User��" + Logined_User.Get_name() + "����ɹ�");

			
		}
		else if (commandP == "login")
		{
			std::cout << "���Ѿ���¼��\n";
		}
		else if (commandP == "return")
		{
			std::string book_name;
			std::cout << "������Ҫ��������:";
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
			std::cout << "��������ȷ��ָ��\n";
		}
		break;
	default:
		break;
	}
	

	std::cout << "\n";
}