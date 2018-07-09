#ifndef ECHOWORKER_H
#define ECHOWORKER_H

#include <serverworker.h>

class EchoWorker : public NrServerWorker
{
    int m_numBytes;
public:
    EchoWorker(QTcpSocket *i_sock);
    virtual void handleClientData();

    int getNum() const;
};

#endif // ECHOWORKER_H
