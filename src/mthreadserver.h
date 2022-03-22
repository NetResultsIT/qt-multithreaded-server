#ifndef NR_MTHREAD_SRV_INCS
#define NR_MTHREAD_SRV_INCS

#include <QObject>
#include <QMap>
#include <QTimer>
#include <QDateTime>
#include <QMutex>

#include "sslserver.h"
#include "serverworker.h"
#include "nrthreadpool.h"


/*!
 * \brief The QMultiThreadedServer class
 */
class QMultiThreadedServer : public QObject
{
    Q_OBJECT

    QMutex muxMap;
    QMutex muxConnCount;

    QMap<QTcpSocket*, QDateTime> m_Socket2LastTStampMap;
    QMap<QTcpSocket*, NrServerWorker*> m_Socket2WorkerPtrMap;

    QTimer *m_pTStampCheckerTimer;
    NRThreadPool *m_pTPool;
    Logger *m_pLogger;

    int m_connectedClients;

private slots:
    void updateClientDataTimestamp();
    void onClientDisconnection_private();
    void checkClientInactivity();

protected:
    NrServerConfig m_srvConf;   /*!< mt server configuration */
    SslServer *m_pSslServer;

    virtual NrServerWorker * getNewWorkerPointer(QTcpSocket*) = 0;

    /*!
     * \brief decreaseConnCounter
     */
    void decreaseConnCounter();

protected slots:
    virtual void onNewClientConnection();
    virtual void onClientDisconnection();
    virtual void onClientError();
    virtual void onClientConnectionEncrypted();

public:
    explicit QMultiThreadedServer(const NrServerConfig &i_rSrvConf, quint16 numberOfThreads=0, Logger*i_plog=nullptr, QObject*parent=nullptr);
    virtual ~QMultiThreadedServer();

    int connectedClients();
    void setThreadAssignementPolicy(NRThreadPool::ThreadAssignmentPolicy i_threadAssignPolicy);
    QMap<int, int> getConnectionStats();

public slots:
    bool listen();

signals:
    /*!
     * \brief clientConnected: emitted when a new client connects
     *  \param The worker associated with the connection
     */
    void clientConnected(NrServerWorker *);
    /*!
     * \brief clientDisconnected: emitted when a client disconnect from the mt server
     *  \param the worker associated with the connection
     */
    void clientDisconnected(NrServerWorker *);
    /*!
     * \brief clientRejected: @@FIXME
     */
    void clientRejected();
    /*!
     * \brief clientConnectionsExhausting: @@FIXME
     */
    void clientConnectionsExhausting(int);
};


/*
 * Now the template class: we cannot template a QObject class directly
 */
template <class T = NrServerWorker>
class MultiThreadedServer : public QMultiThreadedServer
{
protected:
    NrServerWorker* getNewWorkerPointer(QTcpSocket*);
public:
    explicit MultiThreadedServer(const NrServerConfig &srvconf,
                                 Logger* i_pLogger=nullptr,
                                 QObject* parent=nullptr,
                                 quint16 i_numberOfThreads=0);
    virtual ~MultiThreadedServer();
};

//Now include the template implementation
#include "mthreadserver.tpp"

#endif
