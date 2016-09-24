#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    qApp->setStyleSheet("QToolTip { color: #ffffff; border: 1px solid white; }");
    w.setWindowIcon(QIcon(QString(":/Resource/Ico.ico")));
    w.setWindowIconText(QString("GreenNature's LAB"));
    w.setWindowTitle(QString("FIshoT - " APP_VERSION));

    w.show();

    return a.exec();
}
