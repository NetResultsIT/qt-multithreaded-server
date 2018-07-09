#ifndef ECHOCLIENT_H
#define ECHOCLIENT_H

#include <QObject>
#include <QSslSocket>

#define NUM_CLIENTS 100
#define MAX_SENTENCES 50

class EchoClient : public QObject
{
    Q_OBJECT
    int m_id;
    int m_totalBytes;
    QSslSocket *s;

public:
    explicit EchoClient(int id, QObject *parent = 0);
    ~EchoClient();

signals:
    void clientfinished(int);

public slots:
    void doStuff();
};

#endif // ECHOCLIENT_H
