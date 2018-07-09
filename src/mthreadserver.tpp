template <class T>
MultiThreadedServer<T>::MultiThreadedServer(const NrServerConfig &i_cfgRef,
                                            QObject* parent,
                                            quint16 i_numberOfThreads)
    :QMultiThreadedServer(i_cfgRef, i_numberOfThreads, parent)
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


