#include <iostream>
#include <QApplication>
#include "janela.h"

using namespace std;


int main(int argc, char* argv[])
{
    QApplication app(argc,argv);

    janela jal;
    jal.show();

    return app.exec();
}
