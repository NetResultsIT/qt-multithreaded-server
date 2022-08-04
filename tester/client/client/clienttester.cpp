#include "clienttester.h"

#include "echoclient.h"

#include <QDebug>
#include <QThread>
#include <unistd.h>

ClientTester::ClientTester(QObject *parent) : QObject(parent)
{
    this->setObjectName("ECHO Tester");
    for(int i = 0; i < NUM_CLIENTS; i++) {
        QThread *t = new QThread();
        t->setObjectName("echo client thread " + QString::number(i));
        t->start();
        EchoClient *ec = new EchoClient(i);
        ec->setObjectName("echo client " + QString::number(i));
        connect(ec, SIGNAL(clientfinished(int)), this, SLOT(onClientStop(int)));
        ec->moveToThread(t);
        ec->go();
        //if we don't sleep a bit we might exhaust server available connections
        usleep(10000);
    }
}


void
ClientTester::onClientStop(int i)
{
    qDebug() << Q_FUNC_INFO << "client stopped" << i;
    m_StoppedClientsVector.append(i);
    qDebug() << "clients stopped so far: " << m_StoppedClientsVector.count() << " out of " << NUM_CLIENTS;
    /*qDebug() << m_StoppedClientsVector;
    QVector<int> v = m_StoppedClientsVector;
    qSort(v);
    qDebug() << v;
    qDebug() << "------";*/
    EchoClient * ec = dynamic_cast<EchoClient*>(sender());
    if (!ec) {
        qDebug() << "failed to cast to echo client";
        return;
    }
    ec->thread()->exit();
    ec->deleteLater();
}
