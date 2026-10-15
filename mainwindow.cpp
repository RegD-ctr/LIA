#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include "../LIA/LIA.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("-- LIA - Entorno de Compilación y Traducción --");
    setMinimumSize(1024, 700);
    
    // 1. Agrandar la ventana a pantalla completa al iniciar
    showMaximized();

    // Widget central
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    // Editor de código fuente
    editorCodigo = new QTextEdit();
    editorCodigo->setPlaceholderText("Escribe o abre un archivo de código LIA aquí...");
    editorCodigo->setFont(QFont("Courier New", 11));

    // -------------------------------------------------------------------------
    // CREACIÓN DEL PANEL DE PESTAÑAS (QTabWidget)
    // -------------------------------------------------------------------------
    tabPanel = new QTabWidget();

    // --- PESTAÑA 1: CONSOLAS DE ANÁLISIS ---
    QWidget *tabAnalisis = new QWidget();
    QVBoxLayout *layoutAnalisis = new QVBoxLayout(tabAnalisis);

    salidaTokens = new QTextEdit();
    salidaTokens->setReadOnly(true);
    salidaTokens->setPlaceholderText("Tokens identificados...");

    salidaSintaxis = new QTextEdit();
    salidaSintaxis->setReadOnly(true);
    salidaSintaxis->setPlaceholderText("Resultado del análisis sintáctico...");

    salidaErrores = new QTextEdit();
    salidaErrores->setReadOnly(true);
    salidaErrores->setPlaceholderText("Errores encontrados...");

    layoutAnalisis->addWidget(salidaTokens);
    layoutAnalisis->addWidget(salidaSintaxis);
    layoutAnalisis->addWidget(salidaErrores);

    // --- PESTAÑA 2: TABLA DE SÍMBOLOS Y CONSTANTES ---
    QWidget *tabSimbolosConstantes = new QWidget();
    QVBoxLayout *layoutSimbolos = new QVBoxLayout(tabSimbolosConstantes);

    tablaSimbolos = new QTableWidget();
    tablaSimbolos->setColumnCount(5);
    tablaSimbolos->setHorizontalHeaderLabels({"NID", "VARIABLE", "TIPO", "RAM", "AP"});
    tablaSimbolos->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    tablaConstantes = new QTableWidget();
    tablaConstantes->setColumnCount(3);
    tablaConstantes->setHorizontalHeaderLabels({"NCTE", "NOMBRE", "TIPO"});
    tablaConstantes->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layoutSimbolos->addWidget(tablaSimbolos);
    layoutSimbolos->addWidget(tablaConstantes);

    // --- PESTAÑA 3: CUÁDRUPLOS ---
    QWidget *tabCuadruplos = new QWidget();
    QVBoxLayout *layoutCuadruplos = new QVBoxLayout(tabCuadruplos);

    tablaCuadruplos = new QTableWidget();
    tablaCuadruplos->setColumnCount(5);
    tablaCuadruplos->setHorizontalHeaderLabels({"Num", "COPER", "OP1", "OP2", "RES"});
    tablaCuadruplos->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layoutCuadruplos->addWidget(tablaCuadruplos);

    // --- PESTAÑA 4: ESTADO DE PILAS ---
    QWidget *tabPilas = new QWidget();
    QVBoxLayout *layoutPilas = new QVBoxLayout(tabPilas);

    tablaPilas = new QTableWidget();
    tablaPilas->setColumnCount(5);
    tablaPilas->setHorizontalHeaderLabels({"OPERADORES", "OPERANDOS", "TIPOS", "SALTOS", "AVAILS"});
    tablaPilas->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layoutPilas->addWidget(tablaPilas);

    // AGREGAR TODAS LAS PESTAÑAS AL TAB PANEL
    tabPanel->addTab(tabAnalisis, "Análisis Léxico/Sintáctico");
    tabPanel->addTab(tabSimbolosConstantes, "Tabla Símbolos/Constantes");
    tabPanel->addTab(tabCuadruplos, "Cuádruplos");
    tabPanel->addTab(tabPilas, "Pilas Semánticas");

    // -------------------------------------------------------------------------
    // SPLITTER Y LAYOUT GENERAL
    // -------------------------------------------------------------------------
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(editorCodigo);
    splitter->addWidget(tabPanel);
    splitter->setStretchFactor(0, 1); // Distribución proporcional 50/50
    splitter->setStretchFactor(1, 1);

    // Botones de acción
    btnAbrir = new QPushButton("Abrir");
    btnGuardar = new QPushButton("Guardar");
    btnLimpiar = new QPushButton("Limpiar");
    btnAnalizar = new QPushButton("Analizar y Traducir");
    btnSalir = new QPushButton("Salir");

    QHBoxLayout *layoutBotones = new QHBoxLayout();
    layoutBotones->addWidget(btnAbrir);
    layoutBotones->addWidget(btnGuardar);
    layoutBotones->addWidget(btnLimpiar);
    layoutBotones->addWidget(btnAnalizar);
    layoutBotones->addStretch();
    layoutBotones->addWidget(btnSalir);

    QVBoxLayout *layoutPrincipal = new QVBoxLayout(central);
    layoutPrincipal->addWidget(splitter);
    layoutPrincipal->addLayout(layoutBotones);

    // Conexión de eventos
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

// Limpiar todas las vistas y tablas
void MainWindow::onLimpiar() {
    editorCodigo->clear();
    salidaTokens->clear();
    salidaSintaxis->clear();
    salidaErrores->clear();

    tablaSimbolos->setRowCount(0);
    tablaConstantes->setRowCount(0);
    tablaCuadruplos->setRowCount(0);
    tablaPilas->setRowCount(0);
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

    // 1. Análisis Léxico
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

    // 2. Análisis Sintáctico y Semántico
    AnalizadorSintactico sintactico;
    sintactico.analizar(codigo);

    QString textoSintaxis = "";
    if (sintactico.exitoso)
    {
        textoSintaxis = "Análisis sintáctico y semántico correcto.";
    }
    else
    {
        textoSintaxis = "Errores sintácticos / semánticos:\n\n";
        for (const auto &err : sintactico.errores)
        {
            QString lineaErr = QString("Fila %1, Col %2: %3\n")
                                   .arg(err.fila)
                                   .arg(err.columna)
                                   .arg(QString::fromStdString(err.mensaje));
            textoSintaxis += lineaErr;
            textoErrores += lineaErr; // También al panel de errores
        }
    }

    salidaTokens->setText(textoTokens.isEmpty() ? "Sin tokens." : textoTokens);
    salidaSintaxis->setText(textoSintaxis);
    salidaErrores->setText(textoErrores.isEmpty() ? "Sin errores." : textoErrores);


    // --- A) TABLA DE SÍMBOLOS (Direcciones e Índices desde 1) ---
    tablaSimbolos->setRowCount(0);
    for (size_t i = 0; i < sintactico.semantica.tabla_simbolos.size(); ++i)
    {
        const auto &s = sintactico.semantica.tabla_simbolos[i];
        int row = tablaSimbolos->rowCount();
        tablaSimbolos->insertRow(row);
        tablaSimbolos->setItem(row, 0, new QTableWidgetItem(QString::number(s.direccion + 1))); // NID / Dir desde 1
        tablaSimbolos->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(s.nombre)));
        tablaSimbolos->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(nombreTipoDato(s.tipo))));
        tablaSimbolos->setItem(row, 3, new QTableWidgetItem("RAM"));
        tablaSimbolos->setItem(row, 4, new QTableWidgetItem(QString::number(i + 1)));           // AP desde 1
    }

    // --- B) TABLA DE CONSTANTES (Direcciones desde 1) ---
    tablaConstantes->setRowCount(0);
    for (size_t i = 0; i < sintactico.semantica.tabla_constantes.size(); ++i)
    {
        const auto &c = sintactico.semantica.tabla_constantes[i];
        int row = tablaConstantes->rowCount();
        tablaConstantes->insertRow(row);
        tablaConstantes->setItem(row, 0, new QTableWidgetItem(QString::number(c.direccion + 1))); // NCTE desde 1
        tablaConstantes->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(c.valor)));
        tablaConstantes->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(nombreTipoDato(c.tipo))));
    }

    // --- C) TABLA DE CUÁDRUPLOS (Numeración desde 1) ---
    tablaCuadruplos->setRowCount(0);
    for (size_t i = 0; i < sintactico.semantica.tabla_cuadruplos.size(); ++i)
    {
        const auto &q = sintactico.semantica.tabla_cuadruplos[i];
        int row = tablaCuadruplos->rowCount();
        tablaCuadruplos->insertRow(row);
        tablaCuadruplos->setItem(row, 0, new QTableWidgetItem(QString::number(i + 1)));          // # Cuádruplo desde 1
        tablaCuadruplos->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(q.oper)));
        tablaCuadruplos->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(q.op1)));
        tablaCuadruplos->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(q.op2)));
        tablaCuadruplos->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(q.resultado)));
    }

    // --- D) ESTADO DE LAS PILAS SEMÁNTICAS ---
    tablaPilas->setRowCount(0);
    size_t maxFilas = std::max({
        sintactico.semantica.pila_operadores.size(),
        sintactico.semantica.pila_operandos.size(),
        sintactico.semantica.pila_tipos.size(),
        sintactico.semantica.pila_saltos.size(),
        sintactico.semantica.pila_avails.size()
    });

    for (size_t i = 0; i < maxFilas; ++i)
    {
        int row = tablaPilas->rowCount();
        tablaPilas->insertRow(row);

        // Se usa nombreOperador() para convertir el código numérico a símbolo (+)
        QString oper  = (i < sintactico.semantica.pila_operadores.size()) 
                          ? QString::fromStdString(nombreOperador(sintactico.semantica.pila_operadores[i])) 
                          : "";
        QString opnd  = (i < sintactico.semantica.pila_operandos.size())  
                          ? QString::fromStdString(sintactico.semantica.pila_operandos[i].nombre) 
                          : "";
        QString tipo  = (i < sintactico.semantica.pila_tipos.size())      
                          ? QString::fromStdString(nombreTipoDato(sintactico.semantica.pila_tipos[i])) 
                          : "";
        QString salto = (i < sintactico.semantica.pila_saltos.size())     
                          ? QString::number(sintactico.semantica.pila_saltos[i]) 
                          : "";
        QString avail = (i < sintactico.semantica.pila_avails.size())     
                          ? QString::fromStdString(nombreTemporal(sintactico.semantica.pila_avails[i])) 
                          : "";

        tablaPilas->setItem(row, 0, new QTableWidgetItem(oper));
        tablaPilas->setItem(row, 1, new QTableWidgetItem(opnd));
        tablaPilas->setItem(row, 2, new QTableWidgetItem(tipo));
        tablaPilas->setItem(row, 3, new QTableWidgetItem(salto));
        tablaPilas->setItem(row, 4, new QTableWidgetItem(avail));
    }
}
