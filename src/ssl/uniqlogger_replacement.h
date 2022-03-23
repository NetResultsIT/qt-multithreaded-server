#ifndef UNQL_REPLACEMENT_INCS
#define UNQL_REPLACEMENT_INCS
namespace UNQL {
const QString LOG_INFO = "";
const QString LOG_WARNING = "";
const QString LOG_CRITICAL = "";
const QString LOG_ERROR = "";
const QString eom = "";
const QString EOM = "";
}
class Logger {
public:
    Logger& operator<<(QString) { return *this; }
    void setVerbosityAcceptedLevel(int) {}
};
#endif
