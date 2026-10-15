#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>

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
    // Editor principal de código
    QTextEdit *editorCodigo;

    // Componente de Pestañas (Tabbed Pane)
    QTabWidget *tabPanel;

    // Pestaña 1: Consolas de Análisis
    QTextEdit *salidaTokens;
    QTextEdit *salidaSintaxis;
    QTextEdit *salidaErrores;

    // Pestaña 2: Tablas de Símbolos y Constantes
    QTableWidget *tablaSimbolos;
    QTableWidget *tablaConstantes;

    // Pestaña 3: Cuádruplos
    QTableWidget *tablaCuadruplos;

    // Pestaña 4: Pilas del Analizador Semántico
    QTableWidget *tablaPilas;

    // Botones de la interfaz
    QPushButton *btnAbrir;
    QPushButton *btnGuardar;
    QPushButton *btnLimpiar;
    QPushButton *btnAnalizar;
    QPushButton *btnSalir;
};

#endif // MAINWINDOW_H