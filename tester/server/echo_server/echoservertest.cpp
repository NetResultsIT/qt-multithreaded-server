#include "echoservertest.h"
#include <QThread>

EchoServerTest::EchoServerTest(QObject *parent) : QObject(parent)
{
    NrServerConfig cfg;
    cfg.serverAddress = "0.0.0.0";
    cfg.serverPort = 9876;
    cfg.portBindingPolicy = NrServerConfig::E_BindToSpecificPort;
    cfg.certfile = "test_cacert.pem";
    cfg.keyfile = "test_privkey.pem";
    cfg.allowedInactivitySeconds = 50;

    m_pServer = new MultiThreadedServer<EchoWorker>(cfg);

    bool b;
    b  = (bool) connect(m_pServer, SIGNAL(clientConnected(NrServerWorker*)), this, SLOT(onNewConnection(NrServerWorker*)));
    b &= (bool) connect(m_pServer, SIGNAL(clientDisconnected(NrServerWorker*)), this, SLOT(onDisconnection(NrServerWorker*)));
    //b &= (bool) connect(m_statusTimer, SIGNAL(timeout()), this, SLOT(logCurrentStatus()));
    b &= m_pServer->listen();
    Q_ASSERT(b);
}


void
EchoServerTest::onNewConnection(NrServerWorker *)
{
    qDebug() << Q_FUNC_INFO << "new client connected";
}




void
EchoServerTest::onDisconnection(NrServerWorker *i_pSW)
{
    qDebug() << Q_FUNC_INFO << QThread::currentThread();
    if (i_pSW) {
        qDebug() << "client pointer ok... disconnecting";
        EchoWorker *ewP = (EchoWorker*)i_pSW;
        qDebug() << " ---- Handler received " << ewP->getNum() << " messages";
    }
    i_pSW->deleteLater();
}
