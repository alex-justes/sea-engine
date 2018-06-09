#include <iostream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <iomanip>
#include "Log.h"

void log(int log_level, const std::string& message)
{
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
}