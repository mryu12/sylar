#ifndef _SYLAR_LOG_H_
#define _SYLAR_LOG_H_

#include <memory>
#include <stdint.h>
#include <string>
#include <list>
#include <fstream>
#include <sstream>
#include <iostream>
#include <ostream>
#include <vector>

namespace sylar
{

    class Logger;
    //日志事件
    class LogEvent
    {
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent(const char* filr, int32_t m_line, uint32_t elpase,
                uint32_t thread_id, uint32_t fiber_id, uint64_t time);
        const char *getFile() { return m_file; }
        int32_t getLine() { return m_line; }
        int32_t getElpase() { return m_elpase; }
        int32_t getThreadId() { return m_threadId; }
        int32_t getFiberId() { return m_fiberId; }
        uint64_t getTime() { return m_time; }
        std::string getContent() { return m_ss.str(); }
        std::stringstream& getSS(){ return m_ss; }

    private:
        const char *m_file = nullptr;
        int32_t m_line = 0;
        int32_t m_elpase = 0;
        int32_t m_threadId = 0;
        int32_t m_fiberId = 0;
        uint64_t m_time = 0;
        std::stringstream m_ss;
    };

    // 日志级别
    class LogLevel
    {
    public:
        enum Level
        {
            UNKNOW = 0,
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5
        };
        
        static const char* ToString(LogLevel::Level level);
    };

    //日志输出格式
    class LogFormatter
    {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
        LogFormatter(const std::string &pattern);
        void init();

    public:
        class FormatItem
        {
        public:
            typedef std::shared_ptr<FormatItem> ptr;
            virtual ~FormatItem(){};
            virtual void format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr) = 0;
        };

    private:
        std::string m_pattern;
        std::vector<FormatItem::ptr> m_items;
    };

    //日志输出地
    class LogAppender
    {
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        virtual ~LogAppender(){};
        virtual void log(std::shared_ptr<Logger>logger, LogLevel::Level level, LogEvent::ptr event) = 0;

        void setFormatter(LogFormatter::ptr mater) { m_format = mater; }
        LogFormatter::ptr getFormatter() const { return m_format; }

    protected:
        LogLevel::Level m_level;
        LogFormatter::ptr m_format;
    };

    //日志器
    class Logger : public std::enable_shared_from_this<Logger>
    {
    public:
        typedef std::shared_ptr<Logger> ptr;
        Logger(const std::string &name = "root");
        void Log(LogLevel::Level level, LogEvent::ptr event);

        void debug(LogLevel::Level level, LogEvent::ptr event);
        void info(LogLevel::Level level, LogEvent::ptr event);
        void warn(LogLevel::Level level, LogEvent::ptr event);
        void error(LogLevel::Level level, LogEvent::ptr event);
        void fatal(LogLevel::Level level, LogEvent::ptr event);

        void addAppender(LogAppender::ptr appender);
        void delAppender(LogAppender::ptr appender);

        LogLevel::Level getLevel() { return m_level; }
        void setLevel(LogLevel::Level level) { m_level = level; }

        const std::string& getName() const { return m_name; }

    private:
        std::string m_name;
        LogLevel::Level m_level;
        std::list<LogAppender::ptr> m_appenders;
        LogFormatter::ptr m_formatter;
    };

    //输出到控制台的Appender
    class StdoutLogAppender : public LogAppender
    {
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        void log(std::shared_ptr<Logger>logger, LogLevel::Level level, LogEvent::ptr event) override;
    };

    //输出到控制台的Appender
    class FileLogAppender : public LogAppender
    {
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
        FileLogAppender(const std::string &name);
        void log(std::shared_ptr<Logger>logger, LogLevel::Level level, LogEvent::ptr event) override;
        bool reopen();

    private:
        const std::string m_filename;
        std::fstream m_filestream;
    };

} // namespace sylar

#endif