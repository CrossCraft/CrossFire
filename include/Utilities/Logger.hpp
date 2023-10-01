#pragma once
#include <utility>
#include <ctime>

#include "Types.hpp"
#include "IO.hpp"
#include "Filesystem/File.hpp"
#include "Threading/SpinLock.hpp"

namespace CrossFire
{

/**
 * @brief The LogLevel enum represents the log level.
 */
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
};

/**
 * @brief Get the log level string.
 * @param lvl The log level.
 * @return The log level string.
 */
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

/**
 * @brief The Logger class is a simple logger.
 */
class Logger final {
    LogLevel level;
    BufferedWriter &writer;
    bool timestamp = false;
    const char *name = nullptr;
    SpinLock lock;

    /**
     * @brief Log a message.
     * @param lvl The log level.
     * @param buffer The buffer to log.
     */
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
    /**
     * @brief Construct a new Logger object.
     * @param writer A BufferedWriter to write to.
     * @param level The log level.
     */
    explicit Logger(BufferedWriter &writer, LogLevel level = LogLevel::Debug)
        : level(level)
        , writer(writer)
    {
    }

    ~Logger()
    {
        flush();
    }

    /**
     * @brief Flush the logger.
     */
    inline auto flush() -> void
    {
        writer.flush();
    }

    /**
     * @brief Set the log level.
     * @param lvl The log level.
     */
    inline auto set_level(LogLevel lvl) -> void
    {
        level = lvl;
    }

    /**
     * @brief Set whether or not to timestamp the log.
     * @param stamp Whether or not to timestamp the log.
     */
    inline auto set_timestamp(bool stamp) -> void
    {
        timestamp = stamp;
    }

    /**
     * @brief Set the name of the logger.
     * @param log_name The name of the logger.
     */
    inline auto set_name(const char *log_name) -> void
    {
        name = log_name;
    }

    /**
     * @brief Get the stdout logger.
     * @return The stdout logger.
     */
    inline static auto get_stdout() -> Logger &
    {
        static auto stdout_bwriter =
            BufferedWriter(FileFactory::get_stdout()->writer());
        static auto stdout_logger = Logger(stdout_bwriter);
        return stdout_logger;
    }

    /**
     * @brief Get the stderr logger.
     * @return The stderr logger.
     */
    inline static auto get_stderr() -> Logger &
    {
        static auto stderr_bwriter =
            BufferedWriter(FileFactory::get_stderr()->writer());
        static auto stderr_logger = Logger(stderr_bwriter);
        return stderr_logger;
    }

    /**
     * @brief Log a message.
     * @param message The message to log.
     */
    inline auto debug(const char *message) -> void
    {
        log(LogLevel::Debug, Slice<u8>::from_string(message));
    }

    /**
     * @brief Log a message.
     * @param message The message to log.
     */
    inline auto info(const char *message) -> void
    {
        log(LogLevel::Info, Slice<u8>::from_string(message));
    }

    /**
     * @brief Log a message.
     * @param message The message to log.
     */
    inline auto warn(const char *message) -> void
    {
        log(LogLevel::Warning, Slice<u8>::from_string(message));
    }

    /**
     * @brief Log a message.
     * @param message The message to log.
     */
    inline auto err(const char *message) -> void
    {
        log(LogLevel::Error, Slice<u8>::from_string(message));
    }
};

}