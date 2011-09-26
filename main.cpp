#include <QtGui/QApplication>
#include "qnotepad.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Notepad");
    a.setApplicationVersion("1.0");
    a.setOrganizationName("DA-IICT");
    a.setOrganizationDomain("www.daiict.ac.in");

    QNotePad w;
    w.show();

    return a.exec();
}
