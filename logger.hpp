#pragma once

enum LogLevel {
    LOG_ERROR = 0,
    LOG_WARN  = 1,
    LOG_INFO  = 2,
    LOG_DEBUG = 3
};

int  start_logger(const char *log_filename, LogLevel level);
void log_msg(LogLevel level, const char *fmt, ...);
void stop_logger();

