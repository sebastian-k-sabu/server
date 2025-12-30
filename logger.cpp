#include "logger.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

static int g_log_fd = -1;
static LogLevel g_log_level;

static const char *level_str(LogLevel lvl) {
    switch (lvl) {
        case LOG_ERROR: return "ERROR";
        case LOG_WARN:  return "WARN";
        case LOG_INFO:  return "INFO";
        case LOG_DEBUG: return "DEBUG";
        default:        return "UNK";
    }
}

static void write_timestamp_and_level(int file_fd, LogLevel lvl) {
    char buf[128];
    time_t now = time(NULL);
    struct tm tm;

    localtime_r(&now, &tm);

    int len = strftime(buf, sizeof(buf),
                       "[%Y-%m-%d %H:%M:%S] ",
                       &tm);
    write(file_fd, buf, len);
    write(file_fd, level_str(lvl), strlen(level_str(lvl)));
    write(file_fd, " ", 1);
}

int start_logger(const char *log_filename, LogLevel level) {
    int log_pipe[2];
    if (pipe(log_pipe) < 0)
        return -1;

    g_log_level = level;

    pid_t pid = fork();
    if (pid < 0)
        return -1;

    if (pid == 0) {
        // ---------------- LOGGER PROCESS ----------------
        close(log_pipe[1]); // close write end

        int file_fd = open(log_filename,
                           O_WRONLY | O_CREAT | O_APPEND,
                           0644);
        if (file_fd < 0)
            _exit(1);

        char read_buf[512];
        char line_buf[1024];
        int line_len = 0;

        int n;
        while ((n = read(log_pipe[0], read_buf, sizeof(read_buf))) > 0) {

            for (int i = 0; i < n; i++) {
                line_buf[line_len++] = read_buf[i];

                // full log record detected
                if (read_buf[i] == '\n') {
                    LogLevel lvl = (LogLevel)line_buf[0];

                    write_timestamp_and_level(file_fd, lvl);
                    write(file_fd, line_buf + 1, line_len - 1);

                    line_len = 0; // reset for next record
                }
            }
        }

        close(file_fd);
        close(log_pipe[0]);
        _exit(0);
    }

    // ---------------- PARENT / WORKERS ----------------
    close(log_pipe[0]);
    g_log_fd = log_pipe[1];
    return g_log_fd;
}

void log_msg(LogLevel level, const char *fmt, ...) {
    if (g_log_fd < 0 || level > g_log_level)
        return;

    char buf[512];
    buf[0] = (char)level;  // first byte = log level

    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buf + 1, sizeof(buf) - 1, fmt, args);
    va_end(args);

    write(g_log_fd, buf, len + 1);
}

void stop_logger() {
    if (g_log_fd >= 0) {
        close(g_log_fd);
        g_log_fd = -1;
    }

    while (wait(NULL) > 0);
}
