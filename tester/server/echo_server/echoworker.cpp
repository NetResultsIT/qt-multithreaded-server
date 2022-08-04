#include "echoworker.h"

EchoWorker::EchoWorker(QTcpSocket *i_sock)
    : NrServerWorker(i_sock),
      m_numBytes(0)
{
    qDebug() << Q_FUNC_INFO << "Created an echo worker for connection coming from "
             << i_sock->peerAddress() << ":" << i_sock->peerPort();
}


void
EchoWorker::handleClientData()
{
    qDebug() << Q_FUNC_INFO <</* "parent thread: " << parent()->thread() << */"current thread: " << QThread::currentThread();
    QString s = m_sock->readAll();
    m_numBytes += s.length();
    qDebug() << "Received client data: " << s;
    qDebug() << "Sending it back...";
    m_sock->write(s.toUtf8());
    m_sock->flush();
}

int
EchoWorker::getNum() const
{ return m_numBytes; }
