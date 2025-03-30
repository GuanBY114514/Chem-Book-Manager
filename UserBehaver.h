#pragma once
#include "stdc++.h"

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

	void Change_Password(std::string _in_old_password) const
	{
		if (_in_old_password == this->password)
		{
			std::string _new_password_first;//首次输入新密码
			std::string _new_password_second;//
		}
		std::cerr << "原密码错误！\n";
		return;
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
