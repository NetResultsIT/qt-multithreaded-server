#ifndef __NR_WORK_THREAD_H__
#define __NR_WORK_THREAD_H__

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QHostAddress>

#include "mtcommon.h"

class NrServerWorker : public QObject
{
    Q_OBJECT
    QString m_cachedRemoteAddress;
    int m_cachedRemotePort;

protected:
    QTcpSocket *m_sock;

public:
    explicit NrServerWorker(QTcpSocket *i_sock);
    virtual ~NrServerWorker();

    inline QString getClientAddress() const { return m_cachedRemoteAddress; }
    inline int getClientPort() const { return m_cachedRemotePort; }

public slots:
    virtual void handleClientData();
    /*
     * @@FIXME: shoudl this be public?
     */
    void abortSocket();
};

#endif
