#ifndef UNQL_REPLACEMENT_INCS
#define UNQL_REPLACEMENT_INCS
namespace UNQL {
const QString LOG_INFO = "";
const QString LOG_WARNING = "";
const QString LOG_CRITICAL = "";
const QString eom = "";
}
class Logger {
public:
    Logger& operator<<(QString) { return *this; }
    void setVerbosityAcceptedLevel(int) {}
};
#endif
