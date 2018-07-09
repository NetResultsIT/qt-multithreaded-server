#ifndef CLIENTTESTER_H
#define CLIENTTESTER_H

#include <QObject>

class ClientTester : public QObject
{
    Q_OBJECT
public:
    explicit ClientTester(QObject *parent = 0);

signals:

public slots:
    void onClientStop(int);
};

#endif // CLIENTTESTER_H
