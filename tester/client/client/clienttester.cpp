#include "clienttester.h"

#include "echoclient.h"

#include <QDebug>
#include <QThread>

ClientTester::ClientTester(QObject *parent) : QObject(parent)
{
    for(int i = 0; i < NUM_CLIENTS; i++) {
        QThread *t = new QThread(this);
        t->start();
        EchoClient *ec = new EchoClient(i);
        connect(ec, SIGNAL(clientfinished(int)), this, SLOT(onClientStop(int)));
        ec->moveToThread(t);
        //ec->doStuff();
    }
}


void
ClientTester::onClientStop(int i)
{
    qDebug() << Q_FUNC_INFO << "client stopped" << i;
    EchoClient * ec = dynamic_cast<EchoClient*>(sender());
    if (!ec) {
        qDebug() << "failed to cast to echo client";
        return;
    }
    //ec->deleteLater();
}
