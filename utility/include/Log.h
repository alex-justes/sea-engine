#ifndef ENGINE_LOG_H
#define ENGINE_LOG_H
#include <string>

#define STATUS_ERROR    0
#define STATUS_STATUS   1
#define STATUS_WARNING  2
#define STATUS_DEBUG    3

#define LOG(status, ...) log(status, format(__VA_ARGS__));

#define LOG_E(...) LOG(STATUS_ERROR, __VA_ARGS__);
#define LOG_S(...) LOG(STATUS_STATUS, __VA_ARGS__);
#define LOG_W(...) LOG(STATUS_WARNING, __VA_ARGS__);

#ifndef NDEBUG
#define MAX_MESSAGE_LENGTH 512
#define LOG_D(fmt, ...) LOG(STATUS_DEBUG, "<%s: %d> " fmt, __FILE__, __LINE__, ##__VA_ARGS__);
#else
#define MAX_MESSAGE_LENGTH 256
#define LOG_D(...) {}
#endif

template<class ... Types>
inline std::string format(const char fmt[], Types&& ... args)
{
    char message[MAX_MESSAGE_LENGTH];
    std::snprintf(message, MAX_MESSAGE_LENGTH, fmt, std::forward<Types>(args)...);
    return std::string(message);
}

void log(int log_level, const std::string& message);

/*{
    constexpr char CLEAR[]  =  "\x1B[0m";
    constexpr char RED[]    =  "\x1B[31m";
    constexpr char YELLOW[] =  "\x1B[33m";
    constexpr char CYAN[]   =  "\x1B[36m";
    std::string color;
    std::string status;
    std::string padding;
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    switch(log_level)
    {
        case STATUS_ERROR:
            color = RED;
            status = "Error";
            padding = "  ";
            break;
        case STATUS_STATUS:
            color = CLEAR;
            status = "Status";
            padding = " ";
            break;
        case STATUS_WARNING:
            color = YELLOW;
            status = "Warning";
            break;
        case STATUS_DEBUG:
            color = CYAN;
            status = "Debug";
            padding = "  ";
            break;
        default:
            break;
    }

    std::stringstream ss;
    ss << color
       << "[" << status << "] "
       << padding
       << std::put_time(std::localtime(&time), "[%Y-%m-%d %T] ")
       << message
       << CLEAR
       << std::endl;
    std::cout << ss.str() ;
}*/

#endif //ENGINE_LOG_H
