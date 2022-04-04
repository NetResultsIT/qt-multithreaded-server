template <class T>
MultiThreadedServer<T>::MultiThreadedServer(const NrServerConfig &i_cfgRef,
                                            Logger* i_pLogger,
                                            QObject* parent,
                                            quint16 i_numberOfThreads)
    :QMultiThreadedServer(i_cfgRef, i_numberOfThreads, i_pLogger, parent)
{
    /* This CTOR has been left intentionally blank */
}


template <class T>
MultiThreadedServer<T>::~MultiThreadedServer()
{
    /* Nothing to see here, move along */
}


template <class T>
NrServerWorker*
MultiThreadedServer<T>::getNewWorkerPointer(QTcpSocket*sock)
{
    T *wo = new T(sock);
    return wo;
}


