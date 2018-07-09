#include <QCoreApplication>

#include <clienttester.h>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ClientTester ct;

    return a.exec();
}
