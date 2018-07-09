#ifndef MTCOMMON_H
#define MTCOMMON_H

#ifdef ENABLE_MTSERVER_DBG
    #define MTSDBG qDebug()
#else
    #define MTSDBG if (true) {} else qDebug()
#endif

#endif // MTCOMMON_H
