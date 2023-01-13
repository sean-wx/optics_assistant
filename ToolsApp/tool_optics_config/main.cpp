#include <QApplication>
#include "ComparisonToolWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ComparisonToolWindow w;
    w.show();
    return a.exec();
}
