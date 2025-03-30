#pragma once
#pragma warning(disable:4996)
#pragma warning(disable:6031)
#include <string>
#include <chrono>
#include <iomanip>
#include <mutex>

class Logger {
public:
    enum class Level { USER_INFO, WARNING, L_ERROR, BORROW, RETURN, APP_INFO, FILE_OUT };

    // 构造函数（默认日志文件为当前目录的temp.log）
    explicit Logger(const std::string& filename = "temp.log")
        : log_file(filename, std::ios::app)
    {
        if (!log_file.is_open()) {
            throw std::runtime_error("无法打开日志文件: " + filename);
        }
    }

    // 析构函数自动关闭文件
    ~Logger() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }

    // 写入日志（线程安全版本）
    void log(Level level, const std::string& message) {
        std::lock_guard<std::mutex> lock(log_mutex);  // 自动加锁
        if (log_file.is_open()) {
            log_file << get_timestamp() << " "
                << level_to_string(level) << ": "
                << message << std::endl;  // 使用endl确保立即写入
        }
    }

private:
    std::ofstream log_file;
    std::mutex log_mutex;  // 用于多线程同步

    // 获取当前时间戳
    std::string get_timestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    // 日志级别转字符串
    std::string level_to_string(Level level) {
        switch (level) {
        case Level::USER_INFO:    return "[USER-INFO]    ";
        case Level::WARNING:      return "[WARNING]        ";
        case Level::L_ERROR:        return "[ERROR]          ";
        case Level::BORROW:       return "[BORROW]          ";
        case Level::RETURN:       return "[RETURN]          ";
        case Level::APP_INFO:     return "[APP-INFO]      ";
        case Level::FILE_OUT:     return "[FILE-OUT]       ";
        default:                  return "[UNKNOWN]        ";
        }
    }
};

FILE* file_string;
extern size_t Users_num;
extern std::vector<User> users;
extern Logger log_manager;

void Exit()
{
	freopen("CON", "r", stdin);
}

std::string get_current_path_win() {
    char buffer[MAX_PATH] = { 0 };
    // 获取可执行文件完整路径
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string path(buffer);
    // 去除文件名，保留目录
    size_t pos = path.find_last_of("\\/");
    return (pos != std::string::npos) ? path.substr(0, pos) : "";
}

void Load_Setting(int& s) {  // 修改为引用参数
    std::cout << "Loading Settings...\n";
    std::ifstream fin("settings.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Could not open settings file\n";
        log_manager.log(Logger::Level::L_ERROR, "无法打开设置文件");
        return;
    }

    int tmp;
    if (!(fin >> tmp)) {
        std::cerr << "Error: Failed to read integer from file\n";
        log_manager.log(Logger::Level::L_ERROR, "无法读取数据");
        return;
    }

    s = tmp;  // 现在能正确修改外部变量
    std::cout << "Settings loaded successfully\n";
}

void Load_Users()
{
    std::cout << "Loading User Information...\n";

    std::ifstream fin("user.txt");

    if (!fin.is_open())
    {
        std::cerr << "无法打开'user.txt',请检查目录\n";
        log_manager.log(Logger::Level::L_ERROR, "无法打开用户文件");
        return;
    }

    if (!(fin >> Users_num))
    {
        std::cerr << "无法正常读取数据\n";
        log_manager.log(Logger::Level::L_ERROR, "无法读取用户文件");
    }

    for (int i = 0; i < Users_num; i++)
    {
        std::string _name;
        std::string _password;
        int brw_book = 0;
        std::vector<Book> book_brw_list;
        int tmp = 0;
        Login_User_State level = Unlogined;

        fin >> _name >> _password >> brw_book;
        for (int i = 0; i < brw_book; i++)
        {
            std::string brw_book_name;
            time_t brw_time = 0;
            fin >> brw_book_name >> brw_time;
            book_brw_list.push_back({ brw_book_name, brw_time });
        }
        fin >> tmp;
        if (tmp == 0)
        {
            level = Local_user;
        }
        else
        {
            level = L_Administrator;
        }

        users.push_back(User(_name, _password, brw_book, book_brw_list, level));
    }
    std::cout << "Load Is Successful\n";
}

std::vector<Book_With_Person> Get_All_Borrowed_Book()
{
	std::vector<Book_With_Person> result;
	for (int i = 0; i < Users_num; i++)
	{
		std::vector<Book> tmp = users[i].Get_Borrowed_Book_List();
		for (int j = 0; j < tmp.size(); j++)
		{
			result.push_back({ tmp[j], users[i].Get_name() });
		}
	}
	return result;
}

/*-------------------------------------------------------------------------------*/
std::string escape_html(const std::string& s) {
    std::string result;
    for (char c : s) {
        switch (c) {
        case '&':  result += "&amp;";  break;
        case '<':  result += "&lt;";   break;
        case '>':  result += "&gt;";   break;
        case '"':  result += "&quot;"; break;
        case '\'': result += "&apos;"; break;
        default:   result += c;       break;
        }
    }
    return result;
}

// 时间戳转字符串
std::string time_to_str(time_t t) {
    struct tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string generate_book_table(
    const std::vector<Book_With_Person>& books
) {

    std::ostringstream html;
    html << "<table border=\"1\">\n"
        << "<tr><th>序号</th><th>借书名称</th><th>借书时间</th><th>借书人</th></tr>\n";

    for (size_t i = 0; i < books.size(); ++i) {
        const auto& book = books[i];
        html << "<tr>"
            << "<td>" << i + 1 << "</td>"
            << "<td>" << escape_html(book.name) << "</td>"
            << "<td>" << time_to_str(book.borrowed_book_time) << "</td>"
            << "<td>" << escape_html(book.person) << "</td>"
            << "</tr>\n";
    }

    html << "</table>";
    return html.str();
}

void Rewrite_User_File(std::vector<User> _users)
{
	std::ofstream fout("user.txt");
	if (!fout.is_open())
	{
		std::cerr << "无法打开文件\n";
		log_manager.log(Logger::Level::L_ERROR, "无法打开文件");
		return;
	}
	fout << _users.size() << std::endl;
	for (int i = 0; i < _users.size(); i++)
	{
		fout << _users[i].Get_name() << " " << _users[i].Get_Password() << " " << _users[i].Get_Borrowed_Book_List().size() << std::endl;
		std::vector<Book> tmp = _users[i].Get_Borrowed_Book_List();
		for (int j = 0; j < tmp.size(); j++)
		{
			fout << tmp[j].name << " " << tmp[j].borrowed_book_time << std::endl;
		}
		if (_users[i].Get_Level() == Local_user)
		{
			fout << 0 << std::endl;
		}
		else
		{
			fout << 1 << std::endl;
		}
	}
	std::cout << "文件重写成功\n";
	log_manager.log(Logger::Level::APP_INFO, "文件重写成功");
}