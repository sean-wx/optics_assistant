#include <QApplication>
#include "BatchComparison.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BatchComparison w;
    w.show();
    return a.exec();
}
