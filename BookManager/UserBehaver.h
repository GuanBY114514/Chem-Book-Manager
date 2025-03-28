#pragma once
#include<stdc++.h>
#include"UserBehaver.h"

struct Book
{
	std::string name;
	time_t borrowed_book;
};


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
	}

	std::string Get_name()
	{
		return name;
	}

	void Borrow_book(std::string book_name,int max_b)
	{
		if (borrowed_book >= max_b)
		{
			printf("你已经借了%d本书了，上限是%d本书，请先归还再借阅", borrowed_book, max_b);
			return;
		}
		borrowed_book_list.push_back({ book_name ,borrowed_book });
		borrowed_book++;
	}

	void Return_book()
	{

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

private:
	std::string name;
	std::vector<Book> borrowed_book_list;
	std::string password;
	int borrowed_book;
	Login_User_State level;
};

class Administrator : public User
{

};