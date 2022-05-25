#include "log.h"
namespace sylar
{
    
Logger::Logger(conse std::string& name)
    m_name(name){
}

void Logger::Log(LogLevel::Level level, LogEvent::ptr event){
    if(level >= m_level){
        for(auto &i : m_appenders){
            i->Log(level, event);
        }
    }
}

void Logger::debug(LogEvent::ptr event){
    debug(LogLevel::DEBUG, event);
}

void Logger::info(LogEvent::ptr event){
    debug(LogLevel::INFO, event);
}

void Logger::warn(LogEvent::ptr event){
    debug(LogLevel::WARN, event);
}  

void Logger::error(LogEvent::ptr event){
    debug(LogLevel::ERROR, event);
}

void Logger::fatal(LogEvent::ptr event){
    debug(LogLevel::FATAL, event);
}

void Logger::addAppender(LogAppender::ptr appender){
    Logger::m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender){    
    for(auto i = Logger::m_appenders.begin(),
        e = Logger::m_appenders.end(); i != e; ++i){
        if(*i == appender){
            Logger::m_appenders.erase(appender);
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
    m_filename(name){
}

FileLogAppender::Log(LogLevel::Level level, LogEvent::ptr event){
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
