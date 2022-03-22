#include "echoclient.h"

#include <QtGlobal>
#include <QDateTime>
#include <QThread>
#include <unistd.h>

#define DEFAULT_SRV_PORT 9876

EchoClient::EchoClient(int id, QObject *parent)
    : QObject(parent)
    , m_id(id)
{
    qDebug() << "Created echo client #" << id;
    connect(this, &EchoClient::startRequested, this, &EchoClient::priv_go);
}
void EchoClient::go()
{
    //qDebug() << Q_FUNC_INFO << QThread::currentThread();
    emit startRequested();
}

void EchoClient::priv_go()
{
    //qDebug() << Q_FUNC_INFO << QThread::currentThread();
    qDebug() << "Connecting client " << m_id << " to echo server...";
    s = new QSslSocket();
    connect(s, SIGNAL(encrypted()), this, SLOT(doStuff()));
    connect(s, SIGNAL(readyRead()), this, SLOT(receiveData()));
    connect(s, &QSslSocket::disconnected, this, &EchoClient::onDisconnected);
    s->connectToHostEncrypted("localhost", DEFAULT_SRV_PORT);
    s->ignoreSslErrors();
}


void EchoClient::doStuff()
{
    QTcpSocket *s = dynamic_cast<QTcpSocket*>(sender());
    if (s) {
        qDebug() << "Client " << m_id << " successufly connected over ssl to echo server...";
        qsrand(QDateTime::currentDateTime().currentMSecsSinceEpoch());
        int num = qrand() % MAX_SENTENCES;
        qDebug() << Q_FUNC_INFO << QThread::currentThread() << "client " << m_id << "Randomized sentences" << ++num;
        for (int j=1; j<=MAX_SENTENCES; j++) {
            QString b;
            b.fill(QString::number(m_id).at(0), j);
            b.prepend(QString::number(m_id) + " - ");
            qint64 wb = s->write(b.toUtf8());
            m_totalBytes += b.length();
            qDebug() << "client "<< m_id << "sent" << b << "bytes written: " << wb;
            s->flush();
            //usleep(500000);
        }
    }
    emit clientfinished(m_id);
}


void EchoClient::receiveData()
{
    QString msg = s->readAll();
    qDebug() << "Received data: " << msg;
}


void EchoClient::onDisconnected()
{
    qDebug() << "client" << m_id << "disconnected from echo server";
}


EchoClient::~EchoClient()
{
    qDebug() << Q_FUNC_INFO << m_id << "written total bytes" << m_totalBytes;
    s->abort();
}


