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
 *
 */
class QMultiThreadedServer : public QObject
{
    Q_OBJECT

    QMutex muxMap;
    QMutex muxConnCount;

    QMap<QTcpSocket*, QDateTime> m_Socket2LastTStampMap;
    QMap<QTcpSocket*, NrServerWorker*> m_Socket2WorkerPtrMap;

    QTimer *m_pTStampCheckerTimer;
    int m_connectedClients;

    NRThreadPool *m_pTPool;

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
    /*!
     * \brief QMultiThreadedServer
     * \param i_rSrvConf    [in] The mt server configuration
     * \param numberOfThreads the number of threads to be used to handle incoming connection, the default (0) will spawn a number of thread equal to the number
     * virtual cores present on the machine
     */
    explicit QMultiThreadedServer(const NrServerConfig &i_rSrvConf, quint16 numberOfThreads=0, QObject*parent=nullptr);
    virtual ~QMultiThreadedServer();

    /*!
     * \brief connectedClients
     * \return Returns the no. of connected clients
     */
    int connectedClients();

    /*!
     * \brief Set the thread assignement policy of the thread pool
     * \param i_threadAssignPolicy The new thread assignement policy.
     */
    void setThreadAssignementPolicy(NRThreadPool::ThreadAssignmentPolicy i_threadAssignPolicy);

    /*
     *  Returns the map with the connection to thread allocation
     */
    QMap<int, int> getConnectionStats();

public slots:
    /*!
     * \brief listen
     *   Start the server listening on the configured port
     *   Before calling this method all connection to the mt server will be dropped/rejected
     * \return  true on success; false otherwise
     */
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
     * \brief clientRejected: @@FIXME
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
                                 QObject* parent=NULL,
                                 quint16 i_numberOfThreads=0);
    virtual ~MultiThreadedServer();
};

//Now include the template implementation
#include "mthreadserver.tpp"

#endif
