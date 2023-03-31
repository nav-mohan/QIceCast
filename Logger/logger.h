#if !defined(LOGGER_H)
#define LOGGER_H
#include <string>
#include <QString>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QDateTime>

class Logger {

public:
    static Logger *getInstance();
    void write_log(std::string msg);

private:
    Logger();
    static QString m_dirname;
    static QString m_filename;
    static QString m_filepref;
    static uint8_t m_logindex;
    static Logger *m_instance;
    static QFile m_file;
};

#endif // LOGGER_H