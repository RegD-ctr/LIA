#ifndef LIA_H
#define LIA_H

#include <string>
#include <vector>
#include <stack>
#include <unordered_map>

// ─────────────────────────────────────────────────────────────────────────────
// TOKENS DEL ANALIZADOR LÉXICO
// ─────────────────────────────────────────────────────────────────────────────
enum TokenType {
    // Tokens generales
    reservada           = 100,
    identificador       = 101,
    enteros             = 102,
    reales              = 103,
    notacion_cientifica = 104,
    suma                = 105,
    resta               = 106,
    multiplicacion      = 107,
    division            = 108,
    asigna              = 109,
    igual               = 110,
    menor               = 111,
    menorigual          = 112,
    mayor               = 113,
    mayorigual          = 114,
    diferente           = 115,
    op_not              = 116,
    op_and              = 117,
    op_or               = 118,
    parentesis_abre     = 119,
    parentesis_cierra   = 120,
    corchete_abre       = 121,
    corchete_cierra     = 122,
    punto_coma          = 123,
    coma                = 124,
    cte_caracter        = 125,
    cte_string          = 126,
    comentario_linea    = 127,
    modulus             = 128,
    llave_abre          = 129,
    llave_cierra        = 130,
    dos_puntos          = 131,

    // Palabras reservadas de LIA
    res_class           = 132,
    res_endclass        = 133,
    res_int             = 134,
    res_float           = 135,
    res_char            = 136,
    res_string          = 137,
    res_bool            = 138,
    res_if              = 139,
    res_else            = 140,
    res_do              = 141,
    res_while           = 142,
    res_input           = 143,
    res_output          = 144,
    res_def             = 145,
    res_dowhile         = 146,
    res_break           = 147,
    res_loop            = 148,

    // Token especial de fin de archivo (usado por el sintáctico)
    token_eof           = 149,

    // Errores léxicos
    err_numreal_incompleto              = 500,
    err_numreal_exponencial_incompleto  = 501,
    err_numreal_exponencial_invalido    = 502,
    err_and_incompleto                  = 503,
    err_or_incompleto                   = 504,
    err_cte_caracter_vacio              = 505,
    err_caracter_no_reconocido          = 506,
    err_cte_caracter_incompleto         = 507
};

// ─────────────────────────────────────────────────────────────────────────────
// ESTRUCTURA DE TOKEN
// ─────────────────────────────────────────────────────────────────────────────
struct token {
    TokenType   tipo;
    std::string lexema;
    std::string gramema;
    int         fila;
    int         columna;
};

// ─────────────────────────────────────────────────────────────────────────────
// CONSTANTES DE NO TERMINALES (solo para el sintáctico, internos)
// Van del 201 al 231 para no chocar con los TokenType del léxico
// ─────────────────────────────────────────────────────────────────────────────
const int NT_PROGRAM     = 201;
const int NT_DECLARA     = 202;
const int NT_DECLARA_P   = 203;   // DECLARA'
const int NT_ID_P        = 204;   // ID'
const int NT_TIPO        = 205;
const int NT_ESTATUTOS   = 206;
const int NT_ESTATUTOS_P = 207;   // ESTATUTOS'
const int NT_EST_ASIG    = 208;
const int NT_EST_WRITE   = 209;
const int NT_EXPR_P      = 210;   // EXPR'
const int NT_EST_READ    = 211;
const int NT_EST_BREAK   = 212;
const int NT_EST_DO      = 213;
const int NT_EST_IF      = 214;
const int NT_EST_IF_P    = 215;   // EST_IF'
const int NT_EST_WHILE   = 216;
const int NT_EST_LOOP    = 217;
const int NT_EXPR        = 218;
const int NT_EXPR_PP     = 219;   // EXPR''
const int NT_EXPR2       = 220;
const int NT_EXPR2_P     = 221;   // EXPR2'
const int NT_EXPR3       = 222;
const int NT_EXPR3_P     = 223;   // EXPR3'
const int NT_EXPR4       = 224;
const int NT_EXPR4_P     = 225;   // EXPR4'
const int NT_EXPR5       = 226;
const int NT_EXPR5_P     = 227;   // EXPR5'
const int NT_TERM        = 228;
const int NT_TERM_P      = 229;   // TERM'
const int NT_FACT        = 230;
const int NT_OPREL       = 231;

// Símbolo de fondo de pila sintáctica
const int SIMBOLO_EOF    = 999;

// ─────────────────────────────────────────────────────────────────────────────
// CÓDIGOS DE CLASES DE OPERANDOS
// ─────────────────────────────────────────────────────────────────────────────
const int CLASE_VARIABLE  = 1;
const int CLASE_CONSTANTE = 2;
const int CLASE_TEMPORAL  = 3;

// ─────────────────────────────────────────────────────────────────────────────
// CÓDIGOS DE TIPOS DE DATOS SEMÁNTICOS
// ─────────────────────────────────────────────────────────────────────────────
const int TIPO_ERROR    = 0;
const int TIPO_INT      = 1;
const int TIPO_FLOAT    = 2;
const int TIPO_NOTACION = 3;
const int TIPO_CHAR     = 4;
const int TIPO_STRING   = 5;
const int TIPO_BOOL     = 6;

// ─────────────────────────────────────────────────────────────────────────────
// CÓDIGOS DE OPERADORES ESPECIALES
// Los operadores estándar usan su valor de TokenType (suma=105, resta=106, etc.)
// ─────────────────────────────────────────────────────────────────────────────
const int OP_NEG         = 900;   // Negación aritmética unaria
const int OP_FONDO_FALSO = 901;   // Marca de paréntesis '('

// Base de numeración de cuádruplos (1-based para saltos)
const int BASE_CUADRUPLOS = 1;

// ─────────────────────────────────────────────────────────────────────────────
// ESTRUCTURAS DEL ANALIZADOR SEMÁNTICO
// ─────────────────────────────────────────────────────────────────────────────

// Representa un tipo del sistema de tipos de LIA
struct TipoDato {
    int         codigo;
    std::string nombre;
    int         tamanoBytes;
};

// Elemento de la pila de operandos
struct Operando {
    std::string nombre;
    int         direccion;   // Índice en tabla_simbolos o tabla_constantes; -1 para temporales
    int         clase;       // CLASE_VARIABLE, CLASE_CONSTANTE o CLASE_TEMPORAL
};

// Variable declarada en la tabla de símbolos
struct Simbolo {
    std::string nombre;
    int         tipo;
    int         direccion;   // Índice asignado en tabla_simbolos
};

// Constante registrada en la tabla de constantes
struct Constante {
    std::string valor;
    int         tipo;
    int         direccion;   // Índice asignado en tabla_constantes
};

// Cuádruplo de código intermedio (operación de 3 direcciones)
struct Cuadruplo {
    std::string oper;
    std::string op1;
    std::string op2;
    std::string resultado;
};

// Error semántico detectado durante el análisis
struct ErrorSemantico {
    int         fila;
    int         columna;
    std::string mensaje;
};

// Estado completo del análisis semántico
struct EstadoSemantico {
    // ── Las 5 Pilas (implementadas con vector para inspección visual en GUI) ──
    std::vector<Operando> pila_operandos;
    std::vector<int>      pila_operadores;   // Códigos de operador (TokenType, OP_NEG, OP_FONDO_FALSO)
    std::vector<int>      pila_tipos;        // Un código de tipo por cada operando
    std::vector<int>      pila_saltos;       // Índices de cuádruplos pendientes de rellenar
    std::vector<int>      pila_avails;       // Números de temporales libres
    int contadorTemporales;

    // ── Tablas ──
    std::vector<TipoDato>   tabla_tipos;
    std::vector<Cuadruplo>  tabla_cuadruplos;
    std::vector<Constante>  tabla_constantes;
    std::vector<Simbolo>    tabla_simbolos;  // Variables declaradas
    std::unordered_map<std::string, int> indice_simbolos;
    std::unordered_map<std::string, int> indice_constantes;

    // ── Errores ──
    std::vector<ErrorSemantico> errores;
};

// ─────────────────────────────────────────────────────────────────────────────
// ERRORES SINTÁCTICOS
// ─────────────────────────────────────────────────────────────────────────────
struct ErrorSintactico {
    int         fila;
    int         columna;
    std::string mensaje;
};

// ─────────────────────────────────────────────────────────────────────────────
// ANALIZADOR LÉXICO INCREMENTAL
// Entrega un token por llamada en vez de todos de golpe.
// El estado interno (pos, fila, col, etc.) queda guardado entre llamadas.
// ─────────────────────────────────────────────────────────────────────────────
struct AnalizadorLexicoIncremental {
    const std::string& codigo;
    size_t      pos;
    int         fila;
    int         col;
    int         inicioColumna;
    int         estadoActual;
    std::string lexemaActual;

    AnalizadorLexicoIncremental(const std::string& codigo)
        : codigo(codigo), pos(0), fila(1), col(1),
          inicioColumna(1), estadoActual(0), lexemaActual("") {}

    token siguienteToken();
};

// ─────────────────────────────────────────────────────────────────────────────
// ANALIZADOR SINTÁCTICO
// ─────────────────────────────────────────────────────────────────────────────
struct AnalizadorSintactico {
    std::vector<ErrorSintactico> errores;
    bool exitoso;

    // Estado semántico accesible para la GUI
    EstadoSemantico semantica;

    void analizar(const std::string& codigo);
};

// ─────────────────────────────────────────────────────────────────────────────
// FUNCIONES PÚBLICAS Y AUXILIARES
// ─────────────────────────────────────────────────────────────────────────────

// Léxico
std::vector<token> AnalizarLexico(const std::string& codigo);

// Control de Estado Semántico
void reiniciarEstadoSemantico(EstadoSemantico& e);
void marcarErrorSemantico(EstadoSemantico& e, int fila, int columna, const std::string& mensaje);

// Manejo de Pilas con protección contra subdesbordamiento (empty stack)
void empujarOperando(EstadoSemantico& e, const Operando& op);
bool sacarOperando(EstadoSemantico& e, Operando& destino);
bool topeOperando(const EstadoSemantico& e, Operando& destino);

void empujarOperador(EstadoSemantico& e, int op);
bool sacarOperador(EstadoSemantico& e, int& destino);
bool topeOperador(const EstadoSemantico& e, int& destino);

void empujarTipo(EstadoSemantico& e, int tipo);
bool sacarTipo(EstadoSemantico& e, int& destino);
bool topeTipo(const EstadoSemantico& e, int& destino);

void empujarSalto(EstadoSemantico& e, int salto);
bool sacarSalto(EstadoSemantico& e, int& destino);
bool topeSalto(const EstadoSemantico& e, int& destino);

// Avail y Temporales
int obtenerTemporal(EstadoSemantico& e);
std::string nombreTemporal(int n);
void regresarTemporal(EstadoSemantico& e, int n);

// Nombres y Conversiones
std::string nombreOperador(int codigo);
std::string nombreTipoDato(int codigo);
int tipoDeToken(int tipoToken);

// Cubo Semántico
int AccionSemantica(int tipo1, int tipo2, int operador);
int AccionSemantica(int tipo1, int tipo2, const std::string& oper);
int AccionSemanticaUnica(int tipo, int operador);
int AccionSemanticaUnica(int tipo, const std::string& oper);

// Tablas de Símbolos y Constantes
int insertarConstante(EstadoSemantico& e, const std::string& lexema, int tipo);
int insertarSimbolo(EstadoSemantico& e, const std::string& nombre, int tipo);
int buscarSimbolo(const EstadoSemantico& e, const std::string& nombre);

// Generación y Relleno de Cuádruplos (Backpatching)
int generarCuadruplo(EstadoSemantico& e, const std::string& oper, const std::string& op1, const std::string& op2, const std::string& resultado);
int siguienteIndiceCuadruplo(const EstadoSemantico& e);
bool rellenarCuadruplo(EstadoSemantico& e, int indiceCuadruplo, const std::string& destino);

#endif // LIA_H
