#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QTcpServer>
#include <QSslSocket>
#include <QSslKey>
#include <QQueue>

#include "mtcommon.h"

#include "UniqLogger.h"

class NrServerConfig
{
public:
    enum PortBindingPolicyType
    {
        E_BindToSpecificPort,   /*!< bind the server to a single, specific port: \a serverPort  */
        E_BindUsingPortRange    /*!< bint the server to a port range: \a serverMinPort - \a serverMaxPort */
    };
    QHostAddress serverAddress; /*!< local server address to bind */
    int serverPort;             /*!< server port used to listen for incoming connections when E_BindToSpecificPort has been specified */
    int serverMinPort; /*!< min server port used to listen for incoming connections when \a E_BindUsingPortRange has been specified */
    int serverMaxPort; /*!< max server port used to listen for incoming connections when \a E_BindUsingPortRange has been specified */
    PortBindingPolicyType portBindingPolicy;    /*!< port binding policy */
    QString keyfile;    /*!< full abs path to the keyfile to be used when encrypted connection is enabled */
    QString certfile;   /*!< full abs path ceryificate to be used when encrypted connection is enabled */
    QByteArray passphrase;  /*!< passphrase used to decrypt the certificate/key  when encrypted connection is enabled and the key has been pwd protected */
    bool allowUntrustedCerts; /*!< UNUSED at the moment */
    bool ignoreSslErrors;   /*!< if set to true ignore all ssl errors */
    bool disableEncryption; /*!< true: use plaintext connections */
    QString internalLoggerId;
    QString logfile;

    WriterConfig logWriterConfig;
    int allowedInactivitySeconds;   /*!< If set to a positive no. (>0) this is the max no. of secs the connection can be
                                         inactive before it is closed */
    int allowedClientsHardLimit;    /*!< If set to a positive no. (>0) this is the max no. of client: any connection request exceeding the hard limit
                                         will be rejected */
    int allowedClientsSoftLimit;    /*!< If set to a positive no. (>0) this is the soft connection limit: when such no. of connection has been reached
                                         the new conection will emit clientConnectionsExhausting() signal to notify the limit;*/

    explicit NrServerConfig()
        : serverAddress(QHostAddress::Any),
          serverPort(0),
		  serverMinPort(1025),
          serverMaxPort(65535),
          portBindingPolicy(E_BindUsingPortRange),
          passphrase("netresults_testcert_privkey"),
          disableEncryption(false),
          internalLoggerId("NrTcpServer"),
          logfile("sslsrv.log"),
          allowedInactivitySeconds(0),
          allowedClientsHardLimit(0),
          allowedClientsSoftLimit(0) {}
};


class SslServer : public QTcpServer
{
	Q_OBJECT

    NrServerConfig m_ServerConfig;
	QQueue<QSslSocket*> m_sslSocketQ;
    Logger *m_flogger, *m_clogger;

public:
    explicit SslServer(const NrServerConfig &cfg, QObject *parent = 0);

#if QT_VERSION > 0x050000
    void incomingConnection(qintptr handle);
#else
    void incomingConnection(int handle);
#endif

#if QT_VERSION <= 0x040700
    QSslSocket* nextSslPendingConnection();
#else
    // just use QTcpServer::nextPendingConnection()
#endif

	bool listen(const QHostAddress &address, quint16 i_minPort, quint16 i_maxPort);
    bool listen();
    bool listen(const QHostAddress &address, quint16 port=0);

    void setLogLevel(int logLevel);

protected slots:
    void onSocketError(QAbstractSocket::SocketError e);
    void onSslErrors(QList<QSslError> el);

signals:
    void connectedClient();

public slots:

};

#endif // SSLSERVER_H
