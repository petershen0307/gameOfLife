#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GameOfLifeWidget w;
    w.setWindowTitle("Game of life");
    w.show();

    return a.exec();
}
