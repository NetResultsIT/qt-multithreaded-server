#ifndef ECHOSERVERTEST_H
#define ECHOSERVERTEST_H

#include <QObject>
#include <mthreadserver.h>
#include <echoworker.h>
class EchoServerTest : public QObject
{
    Q_OBJECT

    MultiThreadedServer<EchoWorker> *m_pServer;

public:
    explicit EchoServerTest(QObject *parent = 0);

signals:

public slots:
    void onNewConnection(NrServerWorker*);
    void onDisconnection(NrServerWorker*);
};

#endif // ECHOSERVERTEST_H
