#include <QCoreApplication>

#include <echoservertest.h>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    EchoServerTest est;

    return a.exec();
}
