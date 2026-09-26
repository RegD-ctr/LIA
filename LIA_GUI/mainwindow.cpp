#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include "../LIA/LIA.h"

// mingw32-make linea para compilar el proyecto.
//.\release\LIA_GUI.exe linea de la terminal para ejecutar la interfaz

// se inicializa la ventana con titulo y el tamaño
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("--LIA--");
    setMinimumSize(900, 600);

    // widget central que contiene todo
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // area donde se muestra el codigo a analizar
    editorCodigo = new QTextEdit();
    editorCodigo->setPlaceholderText("Abre un archivo o escribe código LIA aquí...");
    editorCodigo->setFont(QFont("Courier New", 10));
    editorCodigo->setAutoFormatting(QTextEdit::AutoNone);

    // Paneles de salida de los tokens
    salidaTokens = new QTextEdit();
    salidaTokens->setReadOnly(true); // setReadOnly(true) evita que se editen en la ejecucion
    salidaTokens->setPlaceholderText("Tokens...");

    // panel de salida de la sintaxis
    salidaSintaxis = new QTextEdit();
    salidaSintaxis->setReadOnly(true);
    salidaSintaxis->setPlaceholderText("Sintaxis...");

    // panel de salida de los errores
    salidaErrores = new QTextEdit();
    salidaErrores->setReadOnly(true);
    salidaErrores->setPlaceholderText("Errores...");

    // Panel derecho: los tres QTextEdit apilados
    QWidget *panelDerecho = new QWidget();
    QVBoxLayout *layoutDerecho = new QVBoxLayout(panelDerecho);
    layoutDerecho->addWidget(salidaTokens);
    layoutDerecho->addWidget(salidaSintaxis);
    layoutDerecho->addWidget(salidaErrores);

    // Splitter horizontal: para el area del editor y los paneles de la derecha
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(editorCodigo);
    splitter->addWidget(panelDerecho);
    splitter->setStretchFactor(0, 2); // editor ocupa el doble
    splitter->setStretchFactor(1, 1);

    // Botones
    btnAbrir = new QPushButton("Abrir");
    btnGuardar = new QPushButton("Guardar");
    btnLimpiar = new QPushButton("Limpiar");
    btnAnalizar = new QPushButton("Analizar");
    btnSalir = new QPushButton("Salir");

    QHBoxLayout *layoutBotones = new QHBoxLayout();
    layoutBotones->addWidget(btnAbrir);
    layoutBotones->addWidget(btnGuardar);
    layoutBotones->addWidget(btnLimpiar);
    layoutBotones->addWidget(btnAnalizar);
    layoutBotones->addStretch(); // empuja el botón Salir a la derecha
    layoutBotones->addWidget(btnSalir);

    // Layout principal
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(central);
    layoutPrincipal->addWidget(splitter);
    layoutPrincipal->addLayout(layoutBotones);

    // Conectar botones a sus funciones
    /*connect es el sistema de señales y slots de Qt, le dice:
    "cuando se haga clic en este botón, llama a esta función".*/
    connect(btnAbrir, &QPushButton::clicked, this, &MainWindow::onAbrir);
    connect(btnGuardar, &QPushButton::clicked, this, &MainWindow::onGuardar);
    connect(btnLimpiar, &QPushButton::clicked, this, &MainWindow::onLimpiar);
    connect(btnAnalizar, &QPushButton::clicked, this, &MainWindow::onAnalizar);
    connect(btnSalir, &QPushButton::clicked, this, &QWidget::close);
}

void MainWindow::onAbrir()
{
    QString archivo = QFileDialog::getOpenFileName(
        this, "Abrir archivo", "", "Archivos de texto (*.txt *.lia);;Todos (*)");
    if (!archivo.isEmpty())
    {
        QFile file(archivo);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            editorCodigo->setText(in.readAll());
            file.close();
        }
    }
}

void MainWindow::onGuardar()
{
    QString archivo = QFileDialog::getSaveFileName(
        this, "Guardar archivo", "", "Archivos de texto (*.txt);;Todos (*)");
    if (!archivo.isEmpty())
    {
        QFile file(archivo);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out << editorCodigo->toPlainText();
            file.close();
        }
    }
}

void MainWindow::onLimpiar()
{
    editorCodigo->clear();
    salidaTokens->clear();
    salidaSintaxis->clear();
    salidaErrores->clear();
}

// funcion que convierte el numero del tipo a su nombre real
QString nombreTipo(TokenType tipo)
{
    switch (tipo)
    {
    case reservada:
        return "reservada";
    case identificador:
        return "identificador";
    case enteros:
        return "entero";
    case reales:
        return "real";
    case notacion_cientifica:
        return "notacion_cientifica";
    case suma:
        return "suma";
    case resta:
        return "resta";
    case multiplicacion:
        return "multiplicacion";
    case division:
        return "division";
    case asigna:
        return "asignacion";
    case igual:
        return "igual";
    case menor:
        return "menor";
    case menorigual:
        return "menor_igual";
    case mayor:
        return "mayor";
    case mayorigual:
        return "mayor_igual";
    case diferente:
        return "diferente";
    case op_not:
        return "op_not";
    case op_and:
        return "op_and";
    case op_or:
        return "op_or";
    case parentesis_abre:
        return "parentesis_abre";
    case parentesis_cierra:
        return "parentesis_cierra";
    case corchete_abre:
        return "corchete_abre";
    case corchete_cierra:
        return "corchete_cierra";
    case punto_coma:
        return "punto_coma";
    case coma:
        return "coma";
    case cte_caracter:
        return "cte_caracter";
    case cte_string:
        return "cte_string";
    case comentario_linea:
        return "comentario";
    case modulus:
        return "modulo";
    case llave_abre:
        return "llave_abre";
    case llave_cierra:
        return "llave_cierra";
    case dos_puntos:
        return "dos_puntos";
    case res_class:
        return "reservada: class";
    case res_endclass:
        return "reservada: endclass";
    case res_int:
        return "reservada: int";
    case res_float:
        return "reservada: float";
    case res_char:
        return "reservada: char";
    case res_string:
        return "reservada: string";
    case res_bool:
        return "reservada: bool";
    case res_if:
        return "reservada: if";
    case res_else:
        return "reservada: else";
    case res_do:
        return "reservada: do";
    case res_while:
        return "reservada: while";
    case res_input:
        return "reservada: input";
    case res_output:
        return "reservada: output";
    case res_def:
        return "reservada: def";
    case res_dowhile:
        return "reservada: dowhile";
    case res_break:
        return "reservada: break";
    case res_loop:
        return "reservada: loop";
    case err_numreal_incompleto:
        return "ERROR: numero real incompleto";
    case err_numreal_exponencial_incompleto:
        return "ERROR: exponencial incompleto";
    case err_numreal_exponencial_invalido:
        return "ERROR: exponencial invalido";
    case err_and_incompleto:
        return "ERROR: && incompleto";
    case err_or_incompleto:
        return "ERROR: || incompleto";
    case err_cte_caracter_vacio:
        return "ERROR: caracter vacio";
    case err_caracter_no_reconocido:
        return "ERROR: caracter no reconocido";
    case err_cte_caracter_incompleto:
        return "ERROR: caracter incompleto";
    default:
        return "desconocido";
    }
}

void MainWindow::onAnalizar()
{
    std::string codigo = editorCodigo->toPlainText().toStdString();

    if (codigo.empty())
    {
        salidaErrores->setText("No hay código para analizar.");
        return;
    }

    std::vector<token> tokens = AnalizarLexico(codigo);

    salidaTokens->clear();
    salidaSintaxis->clear();
    salidaErrores->clear();

    QString textoTokens = "";
    QString textoErrores = "";

    for (const auto &t : tokens)
    {
        QString linea = QString("TIPO: [%1] |LEXEMA: '%2' | Fila %3, Col %4\n")
                            .arg(nombreTipo(t.tipo))
                            .arg(QString::fromStdString(t.lexema))
                            .arg(t.fila)
                            .arg(t.columna);

        if (t.tipo >= 500)
            textoErrores += linea;
        else
            textoTokens += linea;
    }

    // Análisis sintáctico
    AnalizadorSintactico sintactico;
    sintactico.analizar(codigo);

    QString textoSintaxis = "";
    if (sintactico.exitoso)
    {
        textoSintaxis = "✓ Análisis sintáctico correcto.";
    }
    else
    {
        textoSintaxis = "✗ Errores sintácticos:\n\n";
        for (const auto &err : sintactico.errores)
        {
            QString lineaErr = QString("Fila %1, Col %2: %3\n")
                                   .arg(err.fila)
                                   .arg(err.columna)
                                   .arg(QString::fromStdString(err.mensaje));
            textoSintaxis += lineaErr;
            textoErrores += lineaErr; // también al panel de errores
        }
    }

    salidaTokens->setText(textoTokens.isEmpty() ? "Sin tokens." : textoTokens);
    salidaSintaxis->setText(textoSintaxis);
    salidaErrores->setText(textoErrores.isEmpty() ? "Sin errores." : textoErrores);
}
