#include "sslserver.h"

/*!
  \class
  \brief a QTcpServer based SSL server
  This class derives from QTcpServer, overloading the incomingConnection method in order to handle SSL connections from tcp clients.
  It also provides a different Queue to handle SSL connections because Qt < 4.7 do not support the addition of a socket to the pendingConnections list
  */
SslServer::SslServer(const NrServerConfig &aSslConfig, QObject *parent) :
    QTcpServer(parent),
    m_ServerConfig(aSslConfig)
{
    UniqLogger *ul = UniqLogger::instance(m_ServerConfig.internalLoggerId);
    m_flogger = ul->createFileLogger("SSL", m_ServerConfig.logfile, m_ServerConfig.logWriterConfig);
    m_clogger = ul->createConsoleLogger("SSL");
}


bool
SslServer::listen(const QHostAddress &address, quint16 i_minPort, quint16 i_maxPort)
{
    bool res = false;

	for (quint16 i = i_minPort; i < i_maxPort; i++) {
        res = QTcpServer::listen(address, i);
		if (res)
			break;
	}

	return res;
}


bool
SslServer::listen()
{
    bool res = false;

    if (m_ServerConfig.portBindingPolicy == NrServerConfig::E_BindUsingPortRange) {
        for (quint16 i = m_ServerConfig.serverMinPort; i < m_ServerConfig.serverMaxPort; i++) {
            res = QTcpServer::listen(m_ServerConfig.serverAddress, i);
            if (res)
                break;
        }
    }
    else
        res = QTcpServer::listen(m_ServerConfig.serverAddress, m_ServerConfig.serverPort);

    return res;
}


bool SslServer::listen(const QHostAddress &address, quint16 port)
{
    return QTcpServer::listen(address, port);
}


/*!
  \brief we overload the virtual QTcpServer::incomingConnection(int) method in order to start the SSL Encryption
  */
void
#if QT_VERSION > 0x050000
SslServer::incomingConnection(qintptr socketDescriptor)
#else
SslServer::incomingConnection(int socketDescriptor)
#endif
{
    //MTSDBG << "############### server reports ssl socket on descriptor: " << socketDescriptor;
	QSslSocket *serverSocket = new QSslSocket;
    serverSocket->setProtocol(QSsl::AnyProtocol);
    
    MTSDBG << "using ssl socket at address " << serverSocket;

	if (serverSocket->setSocketDescriptor(socketDescriptor)) {
        MTSDBG << "Incoming connection from " << serverSocket->peerAddress().toString() << ":" << serverSocket->peerPort();
        MTSDBG << serverSocket;
        if (serverSocket->peerAddress().toString().isEmpty()) {

            *m_flogger << UNQL::LOG_CRITICAL << "Connecting socket seems to be disconnected: aborting!" << UNQL::eom;
            *m_clogger << UNQL::LOG_CRITICAL << "Connecting socket seems to be disconnected: aborting!" << UNQL::eom;
            qCritical() << "#########> Connecting socket seems to be disconnected: aborting!";
            serverSocket->abort();
            serverSocket->close();
            serverSocket->deleteLater();
            return;
        }
        //connect(serverSocket, SIGNAL(encrypted()), this, SLOT(ready()));

        qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
        connect(serverSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onSocketError(QAbstractSocket::SocketError)));

        if (!m_ServerConfig.disableEncryption)
        {
#if QT_VERSION < 0x050000
            qRegisterMetaType< QList<QSslError> >("QList<QSslError>");
#endif
            connect(serverSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(onSslErrors(QList<QSslError>)));

            serverSocket->setLocalCertificate(m_ServerConfig.certfile, QSsl::Pem);
            serverSocket->setPrivateKey(m_ServerConfig.keyfile, QSsl::Rsa, QSsl::Pem, m_ServerConfig.passphrase);

#if QT_VERSION >= 0x050000
            if(serverSocket->localCertificate().isBlacklisted()) {
                *m_flogger << UNQL::LOG_CRITICAL << "BlackListed certificate " << m_ServerConfig.certfile << UNQL::eom;
                *m_clogger << UNQL::LOG_CRITICAL << "BlackListed certificate " << m_ServerConfig.certfile << UNQL::eom;
            }
#else
            if(!serverSocket->localCertificate().isValid()) {
                *m_flogger << UNQL::LOG_CRITICAL << "Invalid certificate " << m_ServerConfig.certfile << UNQL::eom;
                *m_clogger << UNQL::LOG_CRITICAL << "Invalid certificate " << m_ServerConfig.certfile << UNQL::eom;
            }
#endif

            if(serverSocket->privateKey().isNull()) {
                *m_flogger << UNQL::LOG_CRITICAL << "Invalid private key (NULL)" << m_ServerConfig.keyfile << UNQL::eom;
                *m_clogger << UNQL::LOG_CRITICAL << "Invalid private key (NULL)" << m_ServerConfig.keyfile << UNQL::eom;
            }

            MTSDBG << "SSL Server key" << serverSocket->privateKey();
            MTSDBG << "SSL Server certificate"  << serverSocket->localCertificate();
            serverSocket->startServerEncryption();
        }

#if (QT_VERSION > 0x040700)
        this->addPendingConnection(serverSocket); //this does not work with qt < 4.7
#else
        m_sslSocketQ.enqueue(serverSocket);
#endif
        emit connectedClient();
	} else {

        *m_flogger << UNQL::LOG_CRITICAL << "Invalid set socket descriptor operation" << UNQL::eom;
        *m_clogger << UNQL::LOG_CRITICAL << "Invalid set socket descriptor operation" << UNQL::eom;
        qCritical() << "Could not set socket descriptor!!! ";
        delete serverSocket; //FIXME - this might be better replaced by deleteLater()
    }
}

#if (QT_VERSION <= 0x040700)
/*!
\brief this method mimics the nextPendingConnection from QTcpServer but returns instead only the SSL connections
It can be used to handle just ssl connection or to handle generic connenctions with qt < 4.7
*/
QSslSocket*
SslServer::nextSslPendingConnection()
{
	return m_sslSocketQ.dequeue();
}
#endif

/*
void SslServer::ready()
{
    QSslSocket *sslsock = (QSslSocket*) sender();

    MTSDBG << "socket is encrypted: " << sslsock->isEncrypted();

    *m_flogger << UNQL::LOG_INFO << "Server reports encription established" << UNQL::eom;
    *m_clogger << UNQL::LOG_INFO << "Server reports encription established" << UNQL::eom;
	
    emit connectedClient();
}
*/

void SslServer::onSocketError(QAbstractSocket::SocketError e)
{
    QTcpSocket *sock = (QTcpSocket*) sender();
    if (sock)
    {
        MTSDBG << "socket " << sock << " error " << e;

        QString err = "Server reports error: %1 / peer address: %2:%3";
        err = err.arg(sock->errorString()).arg(sock->peerAddress().toString()).arg(sock->peerPort());
        *m_flogger << UNQL::LOG_WARNING << err << UNQL::eom;
        *m_clogger << UNQL::LOG_WARNING << err << UNQL::eom;

        MTSDBG << "current socket state" << sock->state();
#if (QT_VERSION <= 0x040700)
        QSslSocket *ssock = dynamic_cast<QSslSocket*>(sock);
        if (ssock) {
            MTSDBG << "socket is still contained in the Q:" << m_sslSocketQ.contains(ssock);
            if (ssock->state() == QTcpSocket::UnconnectedState || ssock->state() == QTcpSocket::ClosingState) {
                MTSDBG << "socket is unconnected, the map now is:" << m_sslSocketQ.size() << m_sslSocketQ;
            }
        }
#endif
    }
    else
    {
        QString err = "Socket error event received but the socket is invalid (null)";
        *m_flogger << UNQL::LOG_CRITICAL << err << UNQL::eom;
        *m_clogger << UNQL::LOG_CRITICAL << err << UNQL::eom;
    }
}


void SslServer::onSslErrors(QList<QSslError> aErrorList)
{
    MTSDBG << "ssl error " << aErrorList;
    QList<QSslError> errorsToIgnore;

    foreach (QSslError se, aErrorList) {
		if (se != QSslError::NoError) {
            MTSDBG << se.errorString();
			*m_flogger << UNQL::LOG_CRITICAL << "Server reports SSL error: " << se.errorString() << UNQL::eom;
			*m_clogger << UNQL::LOG_CRITICAL << "Server reports SSL error: " << se.errorString() << UNQL::eom;
            if (se.error() == QSslError::SelfSignedCertificate || se.error() == QSslError::SelfSignedCertificateInChain)
			{
				if (m_ServerConfig.allowUntrustedCerts) {
                    MTSDBG << "Cert is SelfSigned... but we're ok with that...";
					*m_flogger << UNQL::LOG_INFO << "Client certificate is untrusted but we're ok with that" << UNQL::eom;
					*m_clogger << UNQL::LOG_INFO << "Client certificate is untrusted but we're ok with that"  << UNQL::eom;
					errorsToIgnore << se;
				}
			}
		}
    }

    QSslSocket *sslsock = (QSslSocket*) sender();
    if (sslsock) {
        if (m_ServerConfig.ignoreSslErrors) {
            *m_flogger << UNQL::LOG_WARNING << "There were SSL errors but server is configured to ignore them all" << UNQL::eom;
            *m_clogger << UNQL::LOG_WARNING << "There were SSL errors but server is configured to ignore them all" << UNQL::eom;
            sslsock->ignoreSslErrors();
        }
        else {
            *m_flogger << UNQL::LOG_WARNING << "Ignoring some SSL errors..." << UNQL::eom;
            *m_clogger << UNQL::LOG_WARNING << "Ignoring some SSL errors..." << UNQL::eom;
            if (errorsToIgnore.count() > 0)
                sslsock->ignoreSslErrors(errorsToIgnore);
        }
        MTSDBG << "socket is encrypted: " << sslsock->isEncrypted();
    }
}
 
void SslServer::setLogLevel(int logLevel)
{
    if (m_flogger) m_flogger->setVerbosityAcceptedLevel(logLevel);
}
