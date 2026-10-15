#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]){
    QApplication app(argc, argv); //inicializa QT y todo el sistema grafico

    MainWindow ventana; // crea la ventana principa
    ventana.show(); //se muestra la ventana

    return app.exec(); //se arranca el loop de eventos de los clics y teclas
}



