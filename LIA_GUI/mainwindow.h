#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void onAbrir();
    void onGuardar();
    void onLimpiar();
    void onAnalizar();

private:
    QTextEdit *editorCodigo;
    QTextEdit *salidaTokens;
    QTextEdit *salidaSintaxis;
    QTextEdit *salidaErrores;

    QPushButton *btnAbrir;
    QPushButton *btnGuardar;
    QPushButton *btnLimpiar;
    QPushButton *btnAnalizar;
    QPushButton *btnSalir;
};

#endif