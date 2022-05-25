#include "log.h"
namespace sylar
{
    
Logger::Logger(const std::string& name)
    :m_name(name){
}

void Logger::Log(LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        for(auto &i : m_appenders){
            i->Log(level, event);
        }
    }
}

void Logger::debug(LogLevel::Level level, LogEvent::ptr event){
    debug(LogLevel::DEBUG, event);
}

void Logger::info(LogLevel::Level level, LogEvent::ptr event){
    debug(LogLevel::INFO, event);
}

void Logger::warn(LogLevel::Level level, LogEvent::ptr event){
    debug(LogLevel::Level::WARN, event);
}  

void Logger::error(LogLevel::Level level, LogEvent::ptr event){
    debug(LogLevel::ERROR, event);
}

void Logger::fatal(LogLevel::Level level, LogEvent::ptr event){
    debug(LogLevel::FATAL, event);
}

void Logger::addAppender(LogAppender::ptr appender){
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

void StdoutLogAppender::Log(LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        std::cout << m_format->format(event);
    }
}

FileLogAppender::FileLogAppender(const std::string& name)
    :m_filename(name){
}

void FileLogAppender::Log(LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        m_filestream << m_format->format(event);
    }
}

bool FileLogAppender::reopen(){
    if(m_filestream){
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !!m_filestream;
}

} // namespace sylar
