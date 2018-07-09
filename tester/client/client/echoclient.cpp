#include "echoclient.h"

#include <QtGlobal>
#include <QDateTime>
#include <QThread>
#include <unistd.h>

#define DEFAULT_SRV_PORT 9876

EchoClient::EchoClient(int id, QObject *parent) : QObject(parent)
{
    m_id = id;

    s = new QSslSocket(this);
    s->connectToHostEncrypted("localhost", DEFAULT_SRV_PORT);
    connect(s, SIGNAL(encrypted()), this, SLOT(doStuff()));
    s->ignoreSslErrors();
}


void
EchoClient::doStuff()
{
    QTcpSocket *s = dynamic_cast<QTcpSocket*>(sender());
    if (s) {
        qsrand(QDateTime::currentDateTime().currentMSecsSinceEpoch());
        int num = qrand() % MAX_SENTENCES;
        qDebug() << Q_FUNC_INFO << QThread::currentThread() << "client " << m_id << "Randomized sentences" << ++num;
        for (int j=1; j<=MAX_SENTENCES; j++) {
            QString b;
            b.fill(QString::number(m_id).at(0), j);
            s->write(b.toUtf8());
            m_totalBytes += b.length();
            //qDebug() << "client "<< m_id << "sent" << b;
            usleep(500000);
        }
    }
    emit clientfinished(m_id);
}



EchoClient::~EchoClient()
{
    qDebug() << Q_FUNC_INFO << m_id << "written total bytes" << m_totalBytes;
    s->abort();
}
