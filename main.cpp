#include <QtGui/QApplication>
#include "prmpixread.h"
//#include "matrixofpixels.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PrMpixRead w;
    w.show();

    return a.exec();
}
