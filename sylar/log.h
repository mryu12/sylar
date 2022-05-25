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

//日志事件
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();
    const char* getFile(){ return m_file; }
    int32_t getLine(){ return m_line; }
    int32_t getElpase(){ return m_elpase; }
    int32_t getThreadId(){ return m_threadid; }
    int32_t getFiberId(){ return m_fiberid; }
    uint64_t getTime() { return m_time; }
    std::string getComtent( return m_content; )
private:
    const char* m_file = nullptr;
    int32_t m_line = 0;
    int32_t m_elpase = 0;
    int32_t m_threadid = 0;
    int32_t m_fiberid = 0;
    uint64_t m_time = 0;
    std::string m_content;
};

// 日志级别
class LogLevel{
public:
    enum Level {
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };
};

//日志输出格式
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    std::string format(LogEvent::ptr event);
    LogFormatter(const std::string& pattern);
    void init();

private:
    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        virtual ~FormatItem();
        virtual void format(std::ostream& os, LogEvent::ptr) = 0;
    };

private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
};

//日志输出地
class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender();
    virtual void Log(LogLevel::Level, LogEvent::ptr event) = 0;

    void setFormatter(LogFormatter::ptr mater) { m_format = mater; }
    LogFormatter::ptr getFormatter() const { return m_format; }

protected:
    LogLevel::Level m_level;
    LogFormatter::ptr m_format;
};

//日志器
class Logger{
public:
    typedef std::shared_ptr<Logger> ptr;
    Logger(const std::string& name = "root");
    void Log(LogLevel::Level level, LogEvent::ptr event);

    void debug(LogLevel::Level level, LogEvent::ptr event);
    void info(LogLevel::Level level, LogEvent::ptr event);
    void warn(LogLevel::Level level, LogEvent::ptr event);
    void error(LogLevel::Level level, LogEvent::ptr event);
    void fatal(LogLevel::Level level, LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);

    LogLevel::Level getLevel(){return m_level;}
    void setLevel(LogLevel::Level level){m_level = level;}

private:
    std::string m_name;
    LogLevel::Level m_level;
    std::list<LogAppender::ptr> m_appenders;
};

//输出到控制台的Appender
class StdoutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void Log(LogLevel::Level level, LogEvent::ptr event) override;
};

//输出到控制台的Appender
class FileLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& name);
    void Log(LogLevel::Level level, LogEvent::ptr event) override;
    bool reopen();

private:
    const std::string m_filename;
    std::fstream m_filestream;
};

} // namespace sylar


#endif