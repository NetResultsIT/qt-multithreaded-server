#include "serverworker.h"

#include <QThread> // just to get the thread id for debugging

NrServerWorker::NrServerWorker(QTcpSocket *i_sock)
    : m_cachedRemoteAddress( i_sock->peerAddress().toString() ),
      m_cachedRemotePort( i_sock->peerPort() )
{
    m_sock = i_sock;
}


NrServerWorker::~NrServerWorker()
{
    MTSDBG << Q_FUNC_INFO << "current thread: " << QThread::currentThread() << "socket thread:" << m_sock->thread();
    disconnect(m_sock,0,0,0);

    m_sock->abort();
    m_sock->setParent(0);
    m_sock->deleteLater();
}

void
NrServerWorker::abortSocket()
{
    MTSDBG << Q_FUNC_INFO << "current thread: " << QThread::currentThread() << "socket thread:" << m_sock->thread();
    m_sock->abort();
}




void
NrServerWorker::handleClientData()
{
    MTSDBG << Q_FUNC_INFO <</* "parent thread: " << parent()->thread() << */"current thread: " << QThread::currentThread();
    MTSDBG << "Received client data: " << m_sock->readAll();
}

