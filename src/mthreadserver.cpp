#include "mthreadserver.h"

#define MAX_INACTIVITY_MILLISECS 1000

QMultiThreadedServer::QMultiThreadedServer(const NrServerConfig& i_rSrvConf, quint16 numberOfThreads, QObject* parent)
    : QObject(parent),
      m_connectedClients(0),
      m_srvConf(i_rSrvConf)
{
    m_pSslServer = new SslServer(i_rSrvConf, nullptr, this);
    m_pTStampCheckerTimer = new QTimer(this);
    m_pTPool = new NRThreadPool(numberOfThreads, "MTSrvPool", this);

    bool b = (bool) connect (m_pSslServer, SIGNAL(connectedClient()), this, SLOT(onNewClientConnection()), Qt::QueuedConnection);
    b &= (bool) connect (m_pTStampCheckerTimer, SIGNAL(timeout()), this, SLOT(checkClientInactivity()), Qt::QueuedConnection);
    Q_ASSERT(b); Q_UNUSED(b);
}


QMultiThreadedServer::~QMultiThreadedServer()
{
    MTSDBG << Q_FUNC_INFO;
    m_pSslServer->close();
    m_pSslServer->deleteLater();
    m_pSslServer = NULL;
}


int
QMultiThreadedServer::connectedClients()
{
    int cc;

    muxConnCount.lock();
    cc = m_connectedClients;
    muxConnCount.unlock();

    return cc;
}

void QMultiThreadedServer::setThreadAssignementPolicy(NRThreadPool::ThreadAssignmentPolicy i_threadAssignPolicy)
{
    m_pTPool->setPolicy(i_threadAssignPolicy);
}

QMap<int, int> QMultiThreadedServer::getConnectionStats()
{
    return m_pTPool->threadAllocationMap();
}

void
QMultiThreadedServer::onClientConnectionEncrypted()
{
    QSslSocket *sslsock = (QSslSocket*) sender();
    NrServerWorker *wo = NULL;

    muxMap.lock();
    if (m_Socket2WorkerPtrMap.contains(sslsock)) {
        wo = m_Socket2WorkerPtrMap[sslsock];
    }
    else {
        //FIXME - LOG
    }
    muxMap.unlock();

    MTSDBG << "socket is now encrypted: " << sslsock->isEncrypted();
    bool b = connect(sslsock, SIGNAL(readyRead()), wo, SLOT(handleClientData()));
    Q_ASSERT(b); Q_UNUSED(b);

    if (sslsock->bytesAvailable() > 0 )
    {
        MTSDBG << "Data already available in the socket during connection: "<< QString::number(sslsock->bytesAvailable()) << " bytes";
        QTimer::singleShot(0, wo, SLOT(handleClientData()));
    }

}

void
QMultiThreadedServer::onNewClientConnection()
{
    QTcpSocket *sock = m_pSslServer->nextPendingConnection();
    qDebug() << Q_FUNC_INFO << "New connection:" << sock;

    int cc;
    muxConnCount.lock();
    cc = m_connectedClients;
    muxConnCount.unlock();

    if (m_srvConf.allowedClientsHardLimit > 0 && cc >= m_srvConf.allowedClientsHardLimit) {
        sock->abort();
        sock->deleteLater();
        emit clientRejected();
        return;
    }

    if (m_srvConf.allowedClientsSoftLimit > 0 && cc >= m_srvConf.allowedClientsSoftLimit) {
        emit clientConnectionsExhausting(cc);
    }

    NrServerWorker *wo = getNewWorkerPointer(sock);
    sock->setParent(wo);

    muxMap.lock();
    m_Socket2LastTStampMap.insert(sock, QDateTime::currentDateTime());
    m_Socket2WorkerPtrMap.insert(sock, wo);
    muxMap.unlock();

    if (m_srvConf.allowedInactivitySeconds > 0) {
        bool b = false;
        b = connect(sock, SIGNAL(readyRead()), this, SLOT(updateClientDataTimestamp()), Qt::QueuedConnection);
        Q_ASSERT(b); Q_UNUSED(b);
        b = connect(sock, SIGNAL(bytesWritten(qint64)), this, SLOT(updateClientDataTimestamp()));
        Q_ASSERT(b); Q_UNUSED(b);
    }
    if (m_srvConf.disableEncryption == true) {
        connect(sock, SIGNAL(readyRead()), wo, SLOT(handleClientData()));
    }
    else {
        QSslSocket *ssock = dynamic_cast<QSslSocket*>(sock);
        if (ssock) {
            bool b = false;
            b = connect(ssock, SIGNAL(encrypted()), this, SLOT(onClientConnectionEncrypted()));
            Q_ASSERT(b); Q_UNUSED(b);
        }
        else {
            //FIXME - LOG
        }
    }
    /*
     * Note: the QueuedConnection is needed!
     */
    bool b = false;
    b = connect(sock, SIGNAL(disconnected()), this, SLOT(onClientDisconnection_private()), Qt::QueuedConnection);
    Q_ASSERT(b); Q_UNUSED(b);

    /*
    QThread *aWorkerThread = new QThread();
    aWorkerThread->setObjectName("NrWorkerThread");
    wo->moveToThread(aWorkerThread);

    b = connect(wo, SIGNAL(destroyed()), aWorkerThread, SLOT(quit()));
    Q_ASSERT(b); Q_UNUSED(b);
    b = connect(aWorkerThread, SIGNAL(finished()), aWorkerThread, SLOT(deleteLater()));
    Q_ASSERT(b); Q_UNUSED(b);

    aWorkerThread->start();
    */

    m_pTPool->runObject(wo);

    emit clientConnected(wo);

    muxConnCount.lock();
    m_connectedClients++;
    muxConnCount.unlock();
}



void QMultiThreadedServer::decreaseConnCounter()
{
    muxConnCount.lock();
    m_connectedClients--;
    Q_ASSERT(m_connectedClients>=0);
    muxConnCount.unlock();
}


void
QMultiThreadedServer::onClientDisconnection_private()
{
    NrServerWorker *woptr;
    QTcpSocket *sock = dynamic_cast<QTcpSocket*>(sender());
    if (sock) {
        disconnect(sock,0,0,0);
        MTSDBG << Q_FUNC_INFO << "Disconnected socket from " << sock->peerAddress().toString() << sock->peerPort()
                 << QThread::currentThread();;

        //remove the tstamp entry and worker object
        muxMap.lock();
        m_Socket2LastTStampMap.remove(sock);
        woptr = m_Socket2WorkerPtrMap[sock];
        m_Socket2WorkerPtrMap.remove(sock);
        muxMap.unlock();

        //sock->close();

        decreaseConnCounter();
        emit clientDisconnected(woptr);
    }
}


void
QMultiThreadedServer::updateClientDataTimestamp()
{
    QTcpSocket *sock = dynamic_cast<QTcpSocket*>(sender());
    if (sock) {
        MTSDBG << Q_FUNC_INFO << "Activity on socket from " << sock->peerAddress().toString() << sock->peerPort();

        muxMap.lock();
        m_Socket2LastTStampMap[sock] = QDateTime::currentDateTime();
        muxMap.unlock();
    }
}


void
QMultiThreadedServer::onClientDisconnection()
{
    QTcpSocket *sock = dynamic_cast<QTcpSocket*>(sender());
    if (sock) {
        MTSDBG << "Disconnected socket from " << sock->peerAddress().toString() << sock->peerPort();
    }
}


void
QMultiThreadedServer::onClientError()
{
    MTSDBG << Q_FUNC_INFO;
}


void
QMultiThreadedServer::checkClientInactivity()
{
    QDateTime curtime = QDateTime::currentDateTime();
    QList<QTcpSocket*> list;

    QMap<QTcpSocket*, QDateTime>::iterator it;
    muxMap.lock();
    for (it = m_Socket2LastTStampMap.begin(); it != m_Socket2LastTStampMap.end();)
        if ((it.value()).secsTo(curtime) > m_srvConf.allowedInactivitySeconds) {
            QTcpSocket *sock = it.key();
            list.append(sock);
            it = m_Socket2LastTStampMap.erase(it);
            /*  We do not need to decrease the connCounter her, it will be triggered by our sock-close */
        }
        else
            ++it;
    muxMap.unlock();

    foreach (QTcpSocket* s, list) {
        MTSDBG << "Calling from thread " << QThread::currentThread() << " abort() on socket" << s << " whose thread is " << s->thread();
        muxMap.lock();
        NrServerWorker *woptr = m_Socket2WorkerPtrMap[s];
        muxMap.unlock();
        /*
         *  using invokeMethod to guarantee the method is callen on the right thread
         */
        QMetaObject::invokeMethod(woptr, "abortSocket");
    }
}


bool
QMultiThreadedServer::listen()
{
    if (m_srvConf.allowedInactivitySeconds > 0)
    {
        m_pTStampCheckerTimer->start(MAX_INACTIVITY_MILLISECS);
    }
    return m_pSslServer->listen();
}
