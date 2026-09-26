/*
 * Lenguajes y Autómatas 6y
 * Lenguaje LIA — Analizador Léxico + Sintáctico
 * Reyes Gutiérrez Diego  23041065
 * Perez Macias Daniel    23041055
 */

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <cctype>
#include <unordered_map>
#include <fstream>
#include "LIA.h"

//  SECCIÓN 1 — ANALIZADOR LÉXICO


// Palabras reservadas
std::unordered_map<std::string, TokenType> palabras_reservadas = {
    {"class",   res_class},   {"endclass", res_endclass},
    {"int",     res_int},     {"float",    res_float},
    {"char",    res_char},    {"string",   res_string},
    {"bool",    res_bool},    {"if",       res_if},
    {"else",    res_else},    {"do",       res_do},
    {"dowhile", res_dowhile}, {"while",    res_while},
    {"input",   res_input},   {"output",   res_output},
    {"def",     res_def},     {"break",    res_break},
    {"loop",    res_loop}
};

// Convierte un carácter a su categoría de columna en la MATRIZ del autómata
int obtenerCategoria(char c)
{
    if (c == 'e' || c == 'E') return 5;
    if (std::islower(c))      return 0;
    if (std::isupper(c))      return 1;
    if (std::isdigit(c))      return 2;
    if (c == '_')  return 3;
    if (c == '.')  return 4;
    if (c == '+')  return 7;
    if (c == '-')  return 8;
    if (c == '*')  return 9;
    if (c == '/')  return 10;
    if (c == '%')  return 11;
    if (c == '=')  return 12;
    if (c == '!')  return 13;
    if (c == '<')  return 14;
    if (c == '>')  return 15;
    if (c == '&')  return 16;
    if (c == '|')  return 17;
    if (c == '\'') return 18;
    if (c == '"')  return 19;
    if (c == '$')  return 20;
    if (c == '\n') return 21;
    if (c == ' ' || c == '\t' || c == '\r') return 23;
    if (c == ')') return 24;
    if (c == '(') return 25;
    if (c == '[') return 26;
    if (c == ']') return 27;
    if (c == '{') return 28;
    if (c == '}') return 29;
    if (c == ';') return 30;
    if (c == ',') return 31;
    if (c == ':') return 32;
    return 33;  // carácter desconocido → error 506
}


const int MATRIZ[20][34] = {
    // Estado 0: inicio / entre tokens
    {1,1,3,506,506,1,506,105,106,107,108,128,9,12,10,11,13,14,15,17,19,0,506,0,120,119,121,122,129,130,123,124,131,506},
    // Estado 1: identificador o palabra reservada (sigue leyendo letras/dígitos)
    {1,1,1,1,101,1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101},
    // Estado 2: dígitos decimales del número real (después del punto)
    {103,103,2,103,103,6,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103},
    // Estado 3: primer dígito leído (puede ser entero o inicio de real)
    {102,102,4,102,5,6,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102},
    // Estado 4: entero con múltiples dígitos
    {102,102,4,102,5,6,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102},
    // Estado 5: se leyó el punto, esperando dígito decimal (error 500 si no llega)
    {500,500,2,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500,500},
    // Estado 6: se leyó E/e de notación científica (error 502 si no es dígito o signo)
    {502,502,8,502,502,502,502,7,7,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502,502},
    // Estado 7: signo +/- después de E (error 501 si no sigue dígito)
    {501,501,8,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501,501},
    // Estado 8: dígitos del exponente (acepta con retroceso como 104)
    {104,104,8,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104},
    // Estado 9: se leyó '=' (puede ser '=' o '==')
    {109,109,109,109,109,109,109,109,109,109,109,109,110,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109},
    // Estado 10: se leyó '<' (puede ser '<' o '<=')
    {111,111,111,111,111,111,111,111,111,111,111,111,112,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111},
    // Estado 11: se leyó '>' (puede ser '>' o '>=')
    {113,113,113,113,113,113,113,113,113,113,113,113,114,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113},
    // Estado 12: se leyó '!' (puede ser '!' o '!=')
    {116,116,116,116,116,116,116,116,116,116,116,116,115,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116},
    // Estado 13: se leyó '&' (espera otro '&' para '&&', error 503 si no)
    {503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,117,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503,503},
    // Estado 14: se leyó '|' (espera otro '|' para '||', error 504 si no)
    {504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,118,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504,504},
    // Estado 15: se leyó ' (inicio de constante carácter)
    {16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,505,16,16,505,16,16,16,16,16,16,16,16,16,16,16,16},
    // Estado 16: carácter dentro de '', espera cierre con ' (error 507 si no llega)
    {507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507,125,507,507,507,507,507,507,507,507,507,507,507,507,507,507,507},
    // Estado 17: se leyó " (inicio de constante string)
    {18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,126,18,507,18,18,18,18,18,18,18,18,18,18,18,18},
    // Estado 18: dentro de string, sigue hasta encontrar " o error con \n
    {18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,126,18,507,18,18,18,18,18,18,18,18,18,18,18,18},
    // Estado 19: comentario de línea ($), acepta todo hasta \n
    {19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,19,127,19,19,19,19,19,19,19,19,19,19,19,19}
};

bool requiereRetroceso(int tokenType)
{
    switch (tokenType) {
        case 101: // identificador
        case 102: // enteros
        case 103: // reales
        case 104: // notacion_cientifica
        case 109: // asigna (=)
        case 111: // menor (<)
        case 113: // mayor (>)
        case 116: // op_not (!)
        case 500: // err_numreal_incompleto
        case 501: // err_numreal_exponencial_incompleto
        case 502: // err_numreal_exponencial_invalido
        case 503: // err_and_incompleto
        case 504: // err_or_incompleto
        case 507: // err_cte_caracter_incompleto
            return true;
        default:
            return false;
    }
}

// Motor compartido del autómata: procesa un paso del autómata y devuelve
// el token aceptado (si lo hay) o un token vacío (tipo=reservada) si sigue leyendo.
// Esta lógica es usada por AnalizarLexico() y por AnalizadorLexicoIncremental.
static void procesarCaracter(
    char c, int& estadoActual, std::string& lexemaActual,
    int& fila, int& col, int& inicioColumna, size_t& pos,
    std::vector<token>* salida,   
    token* tokenSalida,           
    bool& tokenListo)
{
    int categoria      = obtenerCategoria(c);
    int siguienteEstado = MATRIZ[estadoActual][categoria];

    tokenListo = false;

    if (siguienteEstado >= 100) {
        // ── Aceptación ──
        bool retroceso = requiereRetroceso(siguienteEstado);
        bool esError   = (siguienteEstado >= 500);

        if (!retroceso) {
            if (c != '\0' && c != '\n' && c != '\t' && c != '\r')
                lexemaActual += c;
        }

        if (esError) {
            std::cout << "Error Léxico (" << siguienteEstado << ") Fila:"
                      << fila << " Col:" << col << " Char:'"
                      << (c == '\n' ? "\\n" : c == '\0' ? "\\0" : std::string(1, c))
                      << "'\n";
        }

        TokenType tipo = static_cast<TokenType>(siguienteEstado);
        if (tipo == identificador) {
            auto it = palabras_reservadas.find(lexemaActual);
            if (it != palabras_reservadas.end())
                tipo = it->second;
        }

        token t;
        t.tipo    = tipo;
        t.lexema  = lexemaActual;
        t.fila    = fila;
        t.columna = inicioColumna;

        if (salida)      salida->push_back(t);
        if (tokenSalida) *tokenSalida = t;
        tokenListo = true;

        estadoActual = 0;
        lexemaActual = "";

        if (!retroceso) {
            pos++;
            if (c == '\n') { fila++; col = 1; } else { col++; }
            inicioColumna = col;
        } else {
            inicioColumna = col;
        }

    } else if (siguienteEstado == 0) {
        // ── Espacio/salto entre tokens: descartar ──
        pos++;
        if (c == '\n') { fila++; col = 1; } else { col++; }
        inicioColumna = col;

    } else {
        // ── Seguir construyendo el lexema ──
        estadoActual = siguienteEstado;
        if (c != '\0') lexemaActual += c;
        pos++;
        if (c == '\n') { fila++; col = 1; } else { col++; }
    }
}

// ── Análisis léxico completo (devuelve todos los tokens de golpe) ──
std::vector<token> AnalizarLexico(const std::string& codigo)
{
    int estadoActual = 0;
    int fila = 1, col = 1, inicioColumna = 1;
    std::string lexemaActual;
    size_t pos = 0;
    std::vector<token> tokens;

    while (pos <= codigo.length()) {
        char c = (pos < codigo.length()) ? codigo[pos] : '\0';
        if (pos == codigo.length() && estadoActual == 0) break;

        bool listo = false;
        procesarCaracter(c, estadoActual, lexemaActual,
                         fila, col, inicioColumna, pos,
                         &tokens, nullptr, listo);
    }
    return tokens;
}

// ── Léxico incremental: entrega un token por llamada ──
token AnalizadorLexicoIncremental::siguienteToken()
{
    while (pos <= codigo.length()) {
        char c = (pos < codigo.length()) ? codigo[pos] : '\0';

        // Fin de archivo en estado inicial → token EOF
        if (pos == codigo.length() && estadoActual == 0) {
            token eof;
            eof.tipo    = token_eof;
            eof.lexema  = "$";
            eof.fila    = fila;
            eof.columna = col;
            return eof;
        }

        bool listo = false;
        token t;
        procesarCaracter(c, estadoActual, lexemaActual,
                         fila, col, inicioColumna, pos,
                         nullptr, &t, listo);

        if (listo) {
            // Ignorar errores léxicos (ya detectados en el análisis léxico)
            // y también ignorar comentarios (el sintáctico no los necesita)
            if (static_cast<int>(t.tipo) >= 500) continue;
            if (t.tipo == comentario_linea)       continue;
            return t;  // ← aquí se "pausa" el autómata
        }
    }

    // Seguro de fin de archivo
    token eof;
    eof.tipo    = token_eof;
    eof.lexema  = "$";
    eof.fila    = fila;
    eof.columna = col;
    return eof;
}


//  SECCIÓN 2 — ANALIZADOR SINTÁCTICO

// ── Tabla de producciones ──
// Cada entrada es la lista de símbolos que reemplazan al no terminal.
// Índice = número de producción (1 a 67). El índice 0 no se usa.
// Terminales:   usan los valores de TokenType (ej. res_class=132)
// No terminales: usan las constantes NT_x (ej. NT_DECLARA=202)
// Épsilon (ε):  vector vacío {}

const std::vector<std::vector<int>> PRODUCCIONES = {
    {},  // 0 — no se usa

    // 1.  PROGRAM → class ( id ) { DECLARA ESTATUTOS }
    {132, 119, 101, 120, 129, NT_DECLARA, NT_ESTATUTOS, 130},

    // 2.  DECLARA → DECLARA' DECLARA
    {NT_DECLARA_P, NT_DECLARA},

    // 3.  DECLARA → ε
    {},

    // 4.  DECLARA' → def id ID' : TIPO ;
    {145, 101, NT_ID_P, 131, NT_TIPO, 123},

    // 5.  ID' → , id ID'
    {124, 101, NT_ID_P},

    // 6.  ID' → ε
    {},

    // 7.  TIPO → int
    {134},

    // 8.  TIPO → float
    {135},

    // 9.  TIPO → char
    {136},

    // 10. TIPO → bool
    {138},

    // 11. TIPO → string
    {137},

    // 12. ESTATUTOS → ESTATUTOS' ESTATUTOS
    {NT_ESTATUTOS_P, NT_ESTATUTOS},

    // 13. ESTATUTOS → ε
    {},

    // 14. ESTATUTOS' → EST_ASIG
    {NT_EST_ASIG},

    // 15. ESTATUTOS' → EST_IF
    {NT_EST_IF},

    // 16. ESTATUTOS' → EST_WHILE
    {NT_EST_WHILE},

    // 17. ESTATUTOS' → EST_DO
    {NT_EST_DO},

    // 18. ESTATUTOS' → EST_READ  (input)
    {NT_EST_READ},

    // 19. ESTATUTOS' → EST_WRITE (output)
    {NT_EST_WRITE},

    // 20. ESTATUTOS' → EST_LOOP
    {NT_EST_LOOP},

    // 21. ESTATUTOS' → EST_BREAK
    {NT_EST_BREAK},

    // 22. EST_ASIG → id = EXPR ;
    {101, 109, NT_EXPR, 123},

    // 23. EST_WRITE (input) → input ( EXPR EXPR' ) ;
    //     Nota: input es la instrucción de escritura/salida en LIA
    {143, 119, NT_EXPR, NT_EXPR_P, 120, 123},

    // 24. EXPR' → , EXPR EXPR'
    {124, NT_EXPR, NT_EXPR_P},

    // 25. EXPR' → ε
    {},

    // 26. EST_READ (output) → output ( id ID' ) ;
    //     Nota: output es la instrucción de lectura/entrada en LIA
    {144, 119, 101, NT_ID_P, 120, 123},

    // 27. EST_BREAK → break ;
    {147, 123},

    // 28. EST_DO → do ESTATUTOS dowhile ( EXPR ) ;
    {141, NT_ESTATUTOS, 146, 119, NT_EXPR, 120, 123},

    // 29. EST_IF → if ( EXPR ) { ESTATUTOS EST_IF' }
    {139, 119, NT_EXPR, 120, 129, NT_ESTATUTOS, NT_EST_IF_P, 130},

    // 30. EST_IF' → else ESTATUTOS 
    {140, NT_ESTATUTOS},

    // 31. EST_IF' → ε
    {},

    // 32. EST_WHILE → while ( EXPR ) { ESTATUTOS }
    {142, 119, NT_EXPR, 120, 129, NT_ESTATUTOS, 130},

    // 33. EST_LOOP → loop { ESTATUTOS }
    {148, 129, NT_ESTATUTOS, 130},

    // 34. EXPR → EXPR2 EXPR''
    {NT_EXPR2, NT_EXPR_PP},

    // 35. EXPR'' → || EXPR2 EXPR''
    {118, NT_EXPR2, NT_EXPR_PP},

    // 36. EXPR'' → ε
    {},

    // 37. EXPR2 → EXPR3 EXPR2'
    {NT_EXPR3, NT_EXPR2_P},

    // 38. EXPR2' → && EXPR3 EXPR2'
    {117, NT_EXPR3, NT_EXPR2_P},

    // 39. EXPR2' → ε
    {},

    // 40. EXPR3 → EXPR3' EXPR4
    {NT_EXPR3_P, NT_EXPR4},

    // 41. EXPR3' → !
    {116},

    // 42. EXPR3' → ε
    {},

    // 43. EXPR4 → EXPR5 EXPR4'
    {NT_EXPR5, NT_EXPR4_P},

    // 44. EXPR4' → OPREL EXPR5
    {NT_OPREL, NT_EXPR5},

    // 45. EXPR4' → ε
    {},

    // 46. EXPR5 → TERM EXPR5'
    {NT_TERM, NT_EXPR5_P},

    // 47. EXPR5' → + TERM EXPR5'
    {105, NT_TERM, NT_EXPR5_P},

    // 48. EXPR5' → - TERM EXPR5'
    {106, NT_TERM, NT_EXPR5_P},

    // 49. EXPR5' → ε
    {},

    // 50. TERM → FACT TERM'
    {NT_FACT, NT_TERM_P},

    // 51. TERM' → * FACT TERM'
    {107, NT_FACT, NT_TERM_P},

    // 52. TERM' → / FACT TERM'
    {108, NT_FACT, NT_TERM_P},

    // 53. TERM' → % FACT TERM'
    {128, NT_FACT, NT_TERM_P},

    // 54. TERM' → ε
    {},

    // 55. FACT → id
    {101},

    // 56. FACT → cteentera
    {102},

    // 57. FACT → ctereal
    {103},

    // 58. FACT → ctenotacion
    {104},

    // 59. FACT → ctecaracter
    {125},

    // 60. FACT → ctestring
    {126},

    // 61. FACT → ( EXPR )
    {119, NT_EXPR, 120},

    // 62. OPREL → ==
    {110},

    // 63. OPREL → !=
    {115},

    // 64. OPREL → <
    {111},

    // 65. OPREL → <=
    {112},

    // 66. OPREL → >
    {113},

    // 67. OPREL → >=
    {114},
};

// ── Mapeo de TokenType → columna de la tabla predictiva ──
// Las columnas siguen el orden de tu hoja "Matriz predictiva" de izquierda a derecha:
// 0:class  1:def  2:int  3:float  4:char  5:bool  6:string  7:if  8:else
// 9:do  10:dowhile  11:while  12:input  13:output  14:loop  15:break
// 16:id  17:cteentera  18:ctereal  19:ctenotacion  20:ctecaracter  21:ctestring
// 22:+  23:-  24:*  25:/  26:%  27:==  28:!=  29:<  30:<=  31:>  32:>=
// 33:&&  34:||  35:!  36:=  37:(  38:)  39:{  40:}  41:,  42:;  43::  44:$
int tokenAColumna(int tipo) {
    switch (tipo) {
        case 132: return 0;   case 145: return 1;   case 134: return 2;
        case 135: return 3;   case 136: return 4;   case 138: return 5;
        case 137: return 6;   case 139: return 7;   case 140: return 8;
        case 141: return 9;   case 146: return 10;  case 142: return 11;
        case 143: return 12;  case 144: return 13;  case 148: return 14;
        case 147: return 15;  case 101: return 16;  case 102: return 17;
        case 103: return 18;  case 104: return 19;  case 125: return 20;
        case 126: return 21;  case 105: return 22;  case 106: return 23;
        case 107: return 24;  case 108: return 25;  case 128: return 26;
        case 110: return 27;  case 115: return 28;  case 111: return 29;
        case 112: return 30;  case 113: return 31;  case 114: return 32;
        case 117: return 33;  case 118: return 34;  case 116: return 35;
        case 109: return 36;  case 119: return 37;  case 120: return 38;
        case 129: return 39;  case 130: return 40;  case 124: return 41;
        case 123: return 42;  case 131: return 43;  case 149: return 44;
        default:  return -1;
    }
}

// NT_x (201-231) → índice de fila (0-30)
int ntAFila(int nt) { return nt - 201; }

// ── Tabla predictiva [no_terminal][token] = número de producción ──
// -1 = error (celda vacía en el Excel)
// Filas: 0=PROGRAM … 30=OPREL  (misma posición que NT_x - 201)
// Columnas: 0 a 44 según tokenAColumna()
const int TABLA[31][45] = {
    // PROGRAM (0)
    { 1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    // DECLARA (1)
    {-1, 2,-1,-1,-1,-1,-1, 3,-1, 3,-1, 3, 3, 3, 3, 3, 3,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 3,-1,-1,-1,-1},
    // DECLARA' (2)
    {-1, 4,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    // ID' (3)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 6,-1,-1, 5,-1, 6,-1},
    // TIPO (4)
    {-1,-1, 7, 8, 9,10,11,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    // ESTATUTOS (5)
    {-1,-1,-1,-1,-1,-1,-1,12,13,12,13,12,12,12,12,12,12,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,13,-1,-1,-1,-1},
    // ESTATUTOS' (6)
    {-1,-1,-1,-1,-1,-1,-1,15,-1,17,-1,16,19,18,20,21,14,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    // EST_ASIG (7)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,22,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    // EST_WRITE (8) — se activa con input (col 12)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,23,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    // EXPR' (9)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,25,-1,-1,24,-1,-1,-1},
    // EST_READ (10) — se activa con output (col 13)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,26,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    // EST_BREAK (11)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,27,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    // EST_DO (12)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,28,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    // EST_IF (13)
    {-1,-1,-1,-1,-1,-1,-1,29,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    // EST_IF' (14)
    {-1,-1,-1,-1,-1,-1,-1,-1,30,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,31,-1,-1,-1,-1},
    // EST_WHILE (15)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,32,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    // EST_LOOP (16)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,33,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
    // EXPR (17)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,34,34,34,34,34,34,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,34,-1,34,-1,-1,-1,-1,-1,-1,-1},
    // EXPR'' (18)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,35,-1,-1,-1,36,-1,-1,36,36,-1,-1},
    // EXPR2 (19)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,37,37,37,37,37,37,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,37,-1,37,-1,-1,-1,-1,-1,-1,-1},
    // EXPR2' (20)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,38,39,-1,-1,-1,39,-1,-1,39,39,-1,-1},
    // EXPR3 (21)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,40,40,40,40,40,40,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,40,-1,40,-1,-1,-1,-1,-1,-1,-1},
    // EXPR3' (22)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,42,42,42,42,42,42,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,41,-1,42,-1,-1,-1,-1,-1,-1,-1},
    // EXPR4 (23)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,43,43,43,43,43,43,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,43,-1,-1,-1,-1,-1,-1,-1},
    // EXPR4' (24)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,44,44,44,44,44,44,45,45,-1,-1,-1,45,-1,-1,45,45,-1,-1},
    // EXPR5 (25)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,46,46,46,46,46,46,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,46,-1,-1,-1,-1,-1,-1,-1},
    // EXPR5' (26)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,47,48,-1,-1,-1,49,49,49,49,49,49,49,49,-1,-1,-1,49,-1,-1,49,49,-1,-1},
    // TERM (27)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,50,50,50,50,50,50,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,50,-1,-1,-1,-1,-1,-1,-1},
    // TERM' (28)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,54,54,51,52,53,54,54,54,54,54,54,54,54,-1,-1,-1,54,-1,-1,54,54,-1,-1},
    // FACT (29)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,55,56,57,58,59,60,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,61,-1,-1,-1,-1,-1,-1,-1},
    // OPREL (30)
    {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,63,64,65,66,67,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
};

// ── Nombre legible de un símbolo (para mensajes de error útiles) ──
std::string nombreSimbolo(int s) {
    switch (s) {
        case 101: return "id";          case 102: return "cteentera";
        case 103: return "ctereal";     case 104: return "ctenotacion";
        case 105: return "+";           case 106: return "-";
        case 107: return "*";           case 108: return "/";
        case 109: return "=";           case 110: return "==";
        case 111: return "<";           case 112: return "<=";
        case 113: return ">";           case 114: return ">=";
        case 115: return "!=";          case 116: return "!";
        case 117: return "&&";          case 118: return "||";
        case 119: return "(";           case 120: return ")";
        case 123: return ";";           case 124: return ",";
        case 125: return "ctecaracter"; case 126: return "ctestring";
        case 128: return "%";           case 129: return "{";
        case 130: return "}";           case 131: return ":";
        case 132: return "class";       case 133: return "endclass";
        case 134: return "int";         case 135: return "float";
        case 136: return "char";        case 137: return "string";
        case 138: return "bool";        case 139: return "if";
        case 140: return "else";        case 141: return "do";
        case 142: return "while";       case 143: return "input";
        case 144: return "output";      case 145: return "def";
        case 146: return "dowhile";     case 147: return "break";
        case 148: return "loop";        case 149: return "$";
        case NT_PROGRAM:     return "PROGRAM";
        case NT_DECLARA:     return "DECLARA";
        case NT_DECLARA_P:   return "DECLARA'";
        case NT_ID_P:        return "ID'";
        case NT_TIPO:        return "TIPO";
        case NT_ESTATUTOS:   return "ESTATUTOS";
        case NT_ESTATUTOS_P: return "ESTATUTOS'";
        case NT_EST_ASIG:    return "EST_ASIG";
        case NT_EST_WRITE:   return "EST_WRITE (input)";
        case NT_EXPR_P:      return "EXPR'";
        case NT_EST_READ:    return "EST_READ (output)";
        case NT_EST_BREAK:   return "EST_BREAK";
        case NT_EST_DO:      return "EST_DO";
        case NT_EST_IF:      return "EST_IF";
        case NT_EST_IF_P:    return "EST_IF'";
        case NT_EST_WHILE:   return "EST_WHILE";
        case NT_EST_LOOP:    return "EST_LOOP";
        case NT_EXPR:        return "EXPR";
        case NT_EXPR_PP:     return "EXPR''";
        case NT_EXPR2:       return "EXPR2";
        case NT_EXPR2_P:     return "EXPR2'";
        case NT_EXPR3:       return "EXPR3";
        case NT_EXPR3_P:     return "EXPR3'";
        case NT_EXPR4:       return "EXPR4";
        case NT_EXPR4_P:     return "EXPR4'";
        case NT_EXPR5:       return "EXPR5";
        case NT_EXPR5_P:     return "EXPR5'";
        case NT_TERM:        return "TERM";
        case NT_TERM_P:      return "TERM'";
        case NT_FACT:        return "FACT";
        case NT_OPREL:       return "OPREL";
        case SIMBOLO_EOF:    return "$";
        default: return "?(" + std::to_string(s) + ")";
    }
}

// ── Algoritmo principal: pila + tabla predictiva ──
void AnalizadorSintactico::analizar(const std::string& codigo)
{
    errores.clear();
    exitoso = true;

    AnalizadorLexicoIncremental lexico(codigo);

    // Pila inicial: fondo=EOF, tope=símbolo inicial de la gramática
    std::stack<int> pila;
    pila.push(SIMBOLO_EOF);
    pila.push(NT_PROGRAM);

    token actual = lexico.siguienteToken();

    while (!pila.empty()) {
        int tope = pila.top();

        // ── Caso 1: ambos EOF → éxito ──
        if (tope == SIMBOLO_EOF && actual.tipo == token_eof)
            break;

        bool esTerminal = (tope < 201 || tope > 231) && (tope != SIMBOLO_EOF);

        if (esTerminal) {
            // ── Caso 2: terminal ──
            if (tope == static_cast<int>(actual.tipo)) {
                // Coinciden → consumir
                pila.pop();
                actual = lexico.siguienteToken();
            } else {
                // No coinciden → error
                ErrorSintactico err;
                err.fila    = actual.fila;
                err.columna = actual.columna;
                err.mensaje = "Se esperaba '" + nombreSimbolo(tope) +
                              "' pero se encontró '" + actual.lexema +
                              "' (" + nombreSimbolo(static_cast<int>(actual.tipo)) + ")";
                errores.push_back(err);
                exitoso = false;
                // Recuperación modo pánico: descartar símbolo de la pila
                pila.pop();
            }

        } else if (tope >= 201 && tope <= 231) {
        // ── Caso 3: no terminal → consultar tabla ──
        int fil = ntAFila(tope);
        int col = tokenAColumna(static_cast<int>(actual.tipo));
        int prod = (col != -1) ? TABLA[fil][col] : -1;

        if (prod != -1) {
            pila.pop();
            const std::vector<int>& p = PRODUCCIONES[prod];
            for (int i = (int)p.size() - 1; i >= 0; i--)
                pila.push(p[i]);
        } else {
            // Celda vacía → error
            ErrorSintactico err;
            err.fila    = actual.fila;
            err.columna = actual.columna;
            err.mensaje = "Token inesperado '" + actual.lexema +
                        "' (" + nombreSimbolo(static_cast<int>(actual.tipo)) +
                        ") al analizar " + nombreSimbolo(tope);
            errores.push_back(err);
            exitoso = false;

            if (actual.tipo == token_eof)
                pila.pop();
            else
                actual = lexico.siguienteToken();
            // ────────────────────────────────────────────────────────────────
        }
    }
}}

/*
std::string leerArchivo(const std::string& nombreArchivo)
{
    std::ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir '" << nombreArchivo << "'\n";
        return "";
    }
    std::string contenido, linea;
    while (std::getline(archivo, linea))
        contenido += linea + "\n";
    archivo.close();
    return contenido;
}

int main()
{
    std::string nombreArchivo = "C:/Users/diego/OneDrive/Desktop/LIA/Lenguaje.txt";
    std::string codigo = leerArchivo(nombreArchivo);

    if (codigo.empty()) {
        std::cout << "No se pudo leer el archivo o está vacío.\n";
        return 1;
    }

    // ── Análisis léxico ──
    std::vector<token> tokens = AnalizarLexico(codigo);
    std::cout << "\n--- Análisis Léxico ---\n";
    for (const auto& t : tokens) {
        if (static_cast<int>(t.tipo) >= 500)
            std::cout << "[ERROR] " << t.tipo << " | '" << t.lexema
                      << "' | Fila:" << t.fila << " Col:" << t.columna << "\n";
        else
            std::cout << "[TOKEN] " << t.tipo << " | '" << t.lexema
                      << "' | Fila:" << t.fila << " Col:" << t.columna << "\n";
    }

    // ── Análisis sintáctico ──
    AnalizadorSintactico sintactico;
    sintactico.analizar(codigo);

    std::cout << "\n--- Análisis Sintáctico ---\n";
    if (sintactico.exitoso) {
        std::cout << "Análisis sintáctico correcto. Sin errores.\n";
    } else {
        for (const auto& err : sintactico.errores)
            std::cout << "Error Fila:" << err.fila << " Col:" << err.columna
                      << " — " << err.mensaje << "\n";
    }

    return 0;
}
*/
