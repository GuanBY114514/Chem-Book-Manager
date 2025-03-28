#pragma once
#pragma warning(disable:4996)
#pragma warning(disable:6031)
#include <string>
#include <chrono>
#include <iomanip>
#include <mutex>

class Logger {
public:
    enum class Level { USER_INFO, WARNING, ERROR, BORROW, RETURN, APP_INFO };

    // ���캯����Ĭ����־�ļ�Ϊ��ǰĿ¼��temp.log��
    explicit Logger(const std::string& filename = "temp.log")
        : log_file(filename, std::ios::app)
    {
        if (!log_file.is_open()) {
            throw std::runtime_error("�޷�����־�ļ�: " + filename);
        }
    }

    // ���������Զ��ر��ļ�
    ~Logger() {
        if (log_file.is_open()) {
            log_file.close();
        }
    }

    // д����־���̰߳�ȫ�汾��
    void log(Level level, const std::string& message) {
        std::lock_guard<std::mutex> lock(log_mutex);  // �Զ�����
        if (log_file.is_open()) {
            log_file << get_timestamp() << " "
                << level_to_string(level) << ": "
                << message << std::endl;  // ʹ��endlȷ������д��
        }
    }

private:
    std::ofstream log_file;
    std::mutex log_mutex;  // ���ڶ��߳�ͬ��

    // ��ȡ��ǰʱ���
    std::string get_timestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&time);

        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    // ��־����ת�ַ���
    std::string level_to_string(Level level) {
        switch (level) {
        case Level::USER_INFO:    return "[USER-INFO]  ";
        case Level::WARNING:      return "[WARNING]      ";
        case Level::ERROR:        return "[ERROR]          ";
        case Level::BORROW:       return "[BORROW]        ";
        case Level::RETURN:       return "[RETURN]        ";
        case Level::APP_INFO:     return "[APP-INFO]    ";
        default:                  return "[UNKNOWN]      ";
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

void Load_Setting(int& s) {  // �޸�Ϊ���ò���
    std::cout << "Loading Settings...\n";
    std::ifstream fin("settings.txt");
    if (!fin.is_open()) {
        std::cerr << "Error: Could not open settings file\n";
        log_manager.log(Logger::Level::ERROR, "�޷��������ļ�");
        return;
    }

    int tmp;
    if (!(fin >> tmp)) {
        std::cerr << "Error: Failed to read integer from file\n";
        log_manager.log(Logger::Level::ERROR, "�޷���ȡ����");
        return;
    }

    s = tmp;  // ��������ȷ�޸��ⲿ����
    std::cout << "Settings loaded successfully\n";
}

void Load_Users()
{
    std::cout << "Loading User Information...\n";

    std::ifstream fin("user.txt");

    if (!fin.is_open())
    {
        std::cerr << "�޷���'user.txt',����Ŀ¼\n";
        log_manager.log(Logger::Level::ERROR, "�޷����û��ļ�");
        return;
    }

    if (!(fin >> Users_num))
    {
        std::cerr << "�޷�������ȡ����\n";
        log_manager.log(Logger::Level::ERROR, "�޷���ȡ�û��ļ�");
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
            level = Administrator;
        }

        users.push_back(User(_name, _password, brw_book, book_brw_list, level));
    }
    std::cout << "Load Is Successful\n";
}
