#include "log.h"
#include <map>
#include <memory>
#include <functional>

namespace sylar
{
    
const char* LogLevel::ToString(LogLevel::Level level){
    switch(level){
#define XX(name) \
    case LogLevel::name: \
        return #name; \
        break;
    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);
#undef XX
    default:
        return "UNKNOW";
    }
    return "UNKNOW";
}

class MessageFormatItem : public LogFormatter::FormatItem{
public:
    MessageFormatItem(const std::string& str = ""){}
    void format(std::ostream& os, std::shared_ptr<Logger>logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getContent();
    }
};

class LevelFormatItem : public LogFormatter::FormatItem{
public:
    LevelFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger>logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << LogLevel::ToString(level);
    }
};

class ElpaseFormatItem : public LogFormatter::FormatItem{
public:
    ElpaseFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger>logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getElpase();
    }
};

class NameFormatItem : public LogFormatter::FormatItem{
public:
    NameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger>logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << logger->getName();
    }
};

class ThreadIDFormatItem : public LogFormatter::FormatItem{
public:
    ThreadIDFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger>logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getThreadId();
    }
};

class FiberIDFormatItem : public LogFormatter::FormatItem{
public:
    FiberIDFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger>logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getFiberId();
    }
};

class DateTimeFormatItem : public LogFormatter::FormatItem{
public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
        :m_format(format) {
        if(m_format.empty()) {
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }
    void format(std::ostream& os, std::shared_ptr<Logger>logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getTime();
    }
private:
    std::string m_format;
};

class FileNameFormatItem : public LogFormatter::FormatItem{
public:
    FileNameFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger>logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getFile();
    }
};

class LineFormatItem : public LogFormatter::FormatItem{
public:
    LineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger>logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << event->getLine();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem{
public:
    NewLineFormatItem(const std::string& str = "") {}
    void format(std::ostream& os, std::shared_ptr<Logger>logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << std::endl;
    }
};

class StringFormatItem : public LogFormatter::FormatItem{
public:
    StringFormatItem(const std::string& str)
        : m_string(str){}
    void format(std::ostream& os, std::shared_ptr<Logger>logger, LogLevel::Level level, LogEvent::ptr event) override{
        os << m_string;
    }
private:
    std::string m_string;
};

LogEvent::LogEvent(const char* file, int32_t line, uint32_t elpase,
                uint32_t thread_id, uint32_t fiber_id, uint64_t time)
    : m_file(file),
    m_line(line),
    m_elpase(elpase),
    m_threadId(thread_id),
    m_fiberId(fiber_id),
    m_time(time) {
}
    

Logger::Logger(const std::string& name)
    : m_name(name),
    m_level(LogLevel::DEBUG){
    m_formatter.reset(new LogFormatter("%d [%p] %f %l %m %n"));
}

void Logger::Log(LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        auto self = shared_from_this();
        for(auto &i : m_appenders){
            i->log(self, level, event);
        }
    }
}

void Logger::debug(LogLevel::Level level, LogEvent::ptr event){
    Log(LogLevel::DEBUG, event);
}

void Logger::info(LogLevel::Level level, LogEvent::ptr event){
    Log(LogLevel::INFO, event);
}

void Logger::warn(LogLevel::Level level, LogEvent::ptr event){
    Log(LogLevel::Level::WARN, event);
}  

void Logger::error(LogLevel::Level level, LogEvent::ptr event){
    Log(LogLevel::ERROR, event);
}

void Logger::fatal(LogLevel::Level level, LogEvent::ptr event){
    Log(LogLevel::FATAL, event);
}

void Logger::addAppender(LogAppender::ptr appender){
    if(!appender->getFormatter()){
        appender->setFormatter(m_formatter);
    }
    Logger::m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender){    
    for(auto i = Logger::m_appenders.begin(),
        e = Logger::m_appenders.end(); i != e; ++i){
        if(*i == appender){
            Logger::m_appenders.erase(i);
        }
    }
    return;
}

void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        std::cout << m_format->format(logger, level, event);
    }
}

FileLogAppender::FileLogAppender(const std::string& name)
    :m_filename(name){
}

void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        m_filestream << m_format->format(logger, level,  event);
    }
}

bool FileLogAppender::reopen(){
    if(m_filestream){
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !!m_filestream;
}

LogFormatter::LogFormatter(const std::string& pattern)
    : m_pattern(pattern){
}

std::string LogFormatter::format(std::shared_ptr<Logger>logger, LogLevel::Level level, LogEvent::ptr event){
    std::stringstream ss;
    for(auto i : m_items){
        i->format(ss, logger, level, event);
    }
    return ss.str();
}

// 解析日志
// %XXX %XXX{XXX} %%
void LogFormatter::init(){
    std::vector<std::tuple<std::string, std::string, int> > vec;
    std::string nstr;
    for(size_t i = 0; i < m_pattern.size() ; ++i){
        if(m_pattern[i] != '%'){
            nstr.append(1, m_pattern[i]);
            continue;;
        }
        if((i + 1) < m_pattern.size()){
            if(m_pattern[i + 1] == '%'){
                nstr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < m_pattern.size()){
            if(isspace(m_pattern[n])){
                break;
            }
            if(fmt_status == 0){
                if(m_pattern[n] == '{'){
                    str = m_pattern.substr(i + 1, n - i - 1);
                    fmt_status = 1;
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            }
            if(fmt_status == 1){
                if(m_pattern[n] == '}'){
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    fmt_status = 2;
                    break;
                }
            }
        }

        if(fmt_status == 0){
            if(!nstr.empty()){
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
            }
            str = m_pattern.substr(i + 1, n - i + 1);
            vec.push_back(std::make_tuple(str, "", 0));
            i = n;
        }else if(fmt_status == 1){
            std::cout << "pattern parse error: " << m_pattern << "-" << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }else if(fmt_status == 2){
            if(!nstr.empty()){
                vec.push_back(std::make_tuple(nstr, "", 0));
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n;
        }
    }

    if(!nstr.empty()){
        vec.push_back(std::make_tuple(nstr, "", 0));
    }

    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt){ return FormatItem::ptr(new C(fmt));}} 
        XX(m, MessageFormatItem),           //m:消息
        XX(p, LevelFormatItem),             //p:日志级别
        XX(r, ElpaseFormatItem),            //r:累计毫秒数
        XX(c, NameFormatItem),              //c:日志名称
        XX(t, ThreadIDFormatItem),          //t:线程id
        XX(n, NewLineFormatItem),           //n:换行
        XX(d, DateTimeFormatItem),          //d:时间
        XX(f, FileNameFormatItem),          //f:文件名
        XX(l, LineFormatItem),              //l:行号
        // XX(T, TabFormatItem),               //T:Tab
        XX(F, FiberIDFormatItem),           //F:协程id
        // XX(N, ThreadNameFormatItem),        //N:线程名称
#undef XX
    };

    for(auto& i : vec){
        if(std::get<2>(i) == 0){
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        }else{
            auto it = s_format_items.find(std::get<0>(i));
            if(it == s_format_items.end()){
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
            }else{
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
        std::cout << std::get<0>(i) << " - " << std::get<1>(i) << " - " << std::get<2>(i) << std::endl;
    }

}


} // namespace sylar
