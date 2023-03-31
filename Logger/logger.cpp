#include "logger.h"

Logger::Logger()
{
    qDebug() << "Logger::Logger";
    QDir dir; // Initialize to the desired dir if 'path' is relative

    if (!dir.exists("haha"))
    {
        qDebug() << "Creating Log Dir";
        dir.mkpath("haha"); // You can check the success if needed
    }
    qDebug() << "opening directory " << Logger::m_dirname  << "/" << Logger::m_filename;
    m_file.open(QIODevice::WriteOnly); // Or QIODevice::ReadWrite
}

Logger *Logger::getInstance()
{
    if(!m_instance)
        m_instance = new Logger();
    return m_instance;
}

void Logger::write_log(std::string msg)
{
    qDebug() << msg.c_str();
}


#if defined(IS_TESTING)
QString Logger::m_dirname = "test";
QString Logger::m_filepref = "test_";
#else
QString Logger::m_dirname = "log";
QString Logger::m_filepref = "log_";
#endif // IS_TESTING
Logger *Logger::m_instance = nullptr;
uint8_t Logger::m_logindex = 1;
// QString Logger::m_filename = Logger::m_filepref + QDateTime::currentDateTime().toString("YYYY-DD-dd") + "_" + Logger::m_logindex + ".log";
QString Logger::m_filename = "";
// QFile Logger::m_file(Logger::m_dirname + "/" + Logger::m_filename);
QFile Logger::m_file("");
