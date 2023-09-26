#pragma once
#include <utility>
#include <ctime>

#include "Types.hpp"
#include "IO.hpp"
#include "Filesystem/File.hpp"
#include "Threading/SpinLock.hpp"

namespace CrossFire
{

enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
};

inline auto get_level_string(const LogLevel &lvl) -> const char *
{
    switch (lvl) {
    case LogLevel::Debug:
        return "DEBUG";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warning:
        return "WARNING";
    case LogLevel::Error:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

constexpr const char *timestamp_format = "%02d-%02d-%04d|%02d:%02d:%02d";

class Logger final {
    LogLevel level;
    BufferedWriter& writer;
    bool timestamp = false;
    const char *name = nullptr;
    SpinLock lock;

    inline auto log(LogLevel lvl, const Slice<u8> &buffer) -> void
    {
        LockGuard<SpinLock> guard(lock);

        if (lvl >= this->level) {
            if (timestamp) {
                (void)writer.raw_write(Slice<u8>::from_string("["));

                // Get current time
                time_t now = time(nullptr);
                tm *ltm = localtime(&now);

                // Write time to buffer
                char time_buffer[32];
                // Format: MM-DD-YYYY|HH:MM:SS
                (void)sprintf(time_buffer, timestamp_format, ltm->tm_mon + 1,
                              ltm->tm_mday, ltm->tm_year + 1900, ltm->tm_hour,
                              ltm->tm_min, ltm->tm_sec);

                (void)writer.raw_write(Slice<u8>::from_string(time_buffer));

                (void)writer.raw_write(Slice<u8>::from_string("]"));
            }

            if (name != nullptr) {
                (void)writer.raw_write(Slice<u8>::from_string("["));
                (void)writer.raw_write(Slice<u8>::from_string(name));
                (void)writer.raw_write(Slice<u8>::from_string("]"));
            }

            (void)writer.raw_write(Slice<u8>::from_string("["));
            (void)writer.write(get_level_string(lvl));
            (void)writer.raw_write(Slice<u8>::from_string("]: "));
            (void)writer.raw_write(buffer);
            (void)writer.raw_write(Slice<u8>::from_string("\n"));
        }
    }

public:
    explicit Logger(BufferedWriter& writer, LogLevel level = LogLevel::Info)
        : level(level)
        , writer(writer)
    {
    }

    ~Logger() {
        flush();
    }

    inline auto flush() -> void
    {
        writer.flush();
    }

    inline auto set_level(LogLevel lvl) -> void
    {
        level = lvl;
    }

    inline auto set_timestamp(bool stamp) -> void
    {
        timestamp = stamp;
    }

    inline auto set_name(const char *log_name) -> void
    {
        name = log_name;
    }

    inline static auto get_stdout() -> Logger &
    {
        static auto stdout_bwriter = BufferedWriter(FileFactory::get_stdout().writer());
        static auto stdout_logger = Logger(stdout_bwriter);
        return stdout_logger;
    }

    inline static auto get_stderr() -> Logger &
    {
        static auto stderr_bwriter = BufferedWriter(FileFactory::get_stderr().writer());
        static auto stderr_logger = Logger(stderr_bwriter);
        return stderr_logger;
    }

    inline auto debug(const char *message) -> void
    {
        log(LogLevel::Debug, Slice<u8>::from_string(message));
    }

    inline auto info(const char *message) -> void
    {
        log(LogLevel::Info, Slice<u8>::from_string(message));
    }

    inline auto warn(const char *message) -> void
    {
        log(LogLevel::Warning, Slice<u8>::from_string(message));
    }

    inline auto err(const char *message) -> void
    {
        log(LogLevel::Error, Slice<u8>::from_string(message));
    }
};

}