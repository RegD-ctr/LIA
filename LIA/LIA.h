#ifndef LIA_H
#define LIA_H

#include <string>
#include <vector>

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

// Símbolo de fondo de pila
const int SIMBOLO_EOF    = 999;

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

    void analizar(const std::string& codigo);
};

// ─────────────────────────────────────────────────────────────────────────────
// FUNCIONES PÚBLICAS
// ─────────────────────────────────────────────────────────────────────────────
std::vector<token> AnalizarLexico(const std::string& codigo);

#endif // LIA_H
