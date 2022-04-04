#ifndef CLIENTTESTER_H
#define CLIENTTESTER_H

#include <QObject>
#include <QVector>

class ClientTester : public QObject
{
    Q_OBJECT
    QVector<int> m_StoppedClientsVector;
public:
    explicit ClientTester(QObject *parent = 0);

signals:

public slots:
    void onClientStop(int);
};

#endif // CLIENTTESTER_H
