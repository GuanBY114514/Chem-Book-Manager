#pragma once
#include "stdc++.h"
#include <conio.h>

struct Book
{
	std::string name;
	time_t borrowed_book_time;
};

struct Book_With_Person
{
	std::string name;
	time_t borrowed_book_time;
	std::string person;

	Book_With_Person(Book book, std::string person)
	{
		this->name = book.name;
		this->borrowed_book_time = book.borrowed_book_time;
		this->person = person;
	}
};

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

extern std::string time_to_str(time_t t);
extern std::string generate_book_table(const std::vector<Book_With_Person>& books);


class User
{
public:
	User()
	{
		borrowed_book = 0;
		level = Unlogined;
	}

	User(std::vector<Book> book_list_read, int borrowed_book_num, Login_User_State _level)
	{
		this->borrowed_book_list = book_list_read;
		this->borrowed_book = borrowed_book_num;
		this->level = _level;
	}

	User(std::string _name, std::string _password, int borr_num_of_book, std::vector<Book> _book_list, Login_User_State _level)
	{
		this->name = _name;
		this->borrowed_book = borr_num_of_book;
		this->borrowed_book_list = _book_list;
		this->level = _level;
		this->password = _password;
	}//For Read From File

	User(Login_User_State _level)
	{
		this->level = _level;
		this->borrowed_book = 0;
		this->name = "";
		this->password = "";
		this->borrowed_book_list = {};
	}

	std::string Get_name()const
	{
		return name;
	}

	void Borrow_Book(std::string book_name)
	{
		borrowed_book_list.push_back({ book_name , time(0)});
		borrowed_book++;
		
	}

	bool Return_Book(std::string book_name)
	{
		for (int i = 0; i < borrowed_book_list.size(); i++)
		{
			if (borrowed_book_list[i].name == book_name)
			{
				borrowed_book_list.erase(borrowed_book_list.begin() + i);
				borrowed_book--;
				return true;
			}
		}
		std::cerr << "未找到该书\n";
		return false;
	}

	bool Return_Book(int num)
	{
		if (num >= borrowed_book_list.size())
		{
			return false;
		}
		borrowed_book_list.erase(borrowed_book_list.begin() + num);
		borrowed_book--;
		return true;
	}

	std::string Get_Password() const
	{
		return this->password;
	}

	Login_User_State Get_Level() const
	{
		return this->level;
	}

	bool Judge_Is_Password_Right(std::string _in_password) const
	{
		if (_in_password == this->password)
		{
			return true;
		}
		return false;
	}

	int Change_Password(std::string _in_old_password)
	{
		if (_in_old_password == this->password)
		{
			std::string _new_password_first;//首次输入新密码
			std::string _new_password_second;//第二次输入新密码
			_new_password_first = get_password("请输入新密码:", true);
			_new_password_second = get_password("请再次输入新密码:", true);
			if (_new_password_first == _new_password_second)
			{
				this->password = _new_password_first;
				return 0;
			}
			else
			{
				return 1;
			}
		}
		return -1;
	}

	std::vector<Book> Get_Borrowed_Book_List() const
	{
		return borrowed_book_list;
	}

private:
	std::string name;
	std::vector<Book> borrowed_book_list;
	std::string password;
	int borrowed_book;
	Login_User_State level;
};

class Administrator : public User
{
public:
	Administrator() = default;
	Administrator(std::string _name, std::string _password, int borr_num_of_book, std::vector<Book> _book_list, Login_User_State _level)
		:User(_name, _password, borr_num_of_book, _book_list, _level)
	{	}
	Administrator(std::vector<Book> book_list_read, int borrowed_book_num, Login_User_State _level)
		:User(book_list_read, borrowed_book_num, _level)
	{	}
	Administrator(Login_User_State _level)
		:User(_level)
	{	}

	Administrator(const User& _user)
		:User(_user)
	{
	}
	~Administrator() = default;

	void View_All_Borrowed_Book_In_Console(const std::vector<Book_With_Person> _borrowed_book_list)
	{
		for (int i = 0; i < _borrowed_book_list.size(); i++)
		{
			std::cout << _borrowed_book_list[i].name << " " << time_to_str(_borrowed_book_list[i].borrowed_book_time) <<" "<<_borrowed_book_list[i].person << std::endl;
		}
	}

	void View_All_Borrowed_Book_In_HTML_Table(const std::vector<Book_With_Person> _borrowed_book_list)
	{
		std::string html = generate_book_table(_borrowed_book_list);
		std::ofstream fout("borrowed_book.html");
		fout << html;
		fout.close();
	}
};
