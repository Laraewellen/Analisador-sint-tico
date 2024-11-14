#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct {
    char lexema[50];
    char tipo[20];
    int linha;
} Token;

typedef struct {
    char nome[50];
    char tipo[20];
} Simbolo;

#define MAX_SIMBOLOS 100
Simbolo tabelaSimbolos[MAX_SIMBOLOS];
int contadorSimbolos = 0;

FILE *arquivoFonte;
Token tokenAtual;
int linhaAtual = 1;

Token ProximoToken();
void Erro(const char *mensagem, const char *tipoEsperado);
void CasaToken(const char *tipoEsperado);
void AnalisarPrograma();
void AnalisarBloco();
void AnalisarDeclaracaoVariaveis();
void AnalisarListaIdentificadores();
void AnalisarTipo();
void AnalisarComandoComposto();
void AnalisarComando();
void AnalisarExpressaoRelacional();
void AnalisarExpressao();
void AnalisarTermo();
void AnalisarFator();

// Funções para gerenciamento da tabela de símbolos
void AdicionarSimbolo(const char *nome, const char *tipo) {
    strcpy(tabelaSimbolos[contadorSimbolos].nome, nome);
    strcpy(tabelaSimbolos[contadorSimbolos].tipo, tipo);
    contadorSimbolos++;
}

const char* BuscarTipoSimbolo(const char *nome) {
    for (int i = 0; i < contadorSimbolos; i++) {
        if (strcmp(tabelaSimbolos[i].nome, nome) == 0) {
            return tabelaSimbolos[i].tipo;
        }
    }
    return NULL; // Retorna NULL se o símbolo não for encontrado
}

// Função para obter o próximo token
Token ProximoToken() {
    Token token = {"", "", linhaAtual};
    int c;

    while ((c = fgetc(arquivoFonte)) != EOF) {
        if (c == ' ' || c == '\t') {
            continue;
        } else if (c == '\n') {
            linhaAtual++;
        } else if (c == '{') {
            while ((c = fgetc(arquivoFonte)) != '}' && c != EOF) {
                if (c == '\n') linhaAtual++;
            }
        } else if (c == '/' && (c = fgetc(arquivoFonte)) == '/') {
            while ((c = fgetc(arquivoFonte)) != '\n' && c != EOF);
            linhaAtual++;
        } else {
            ungetc(c, arquivoFonte);
            break;
        }
    }

    if (c == EOF) {
        strcpy(token.tipo, "EOF");
        return token;
    }

    c = fgetc(arquivoFonte);
    if (isalpha(c)) {
        int i = 0;
        do {
            token.lexema[i++] = c;
            c = fgetc(arquivoFonte);
        } while (isalnum(c) && i < 49);
        ungetc(c, arquivoFonte);
        token.lexema[i] = '\0';

        if (strcmp(token.lexema, "program") == 0 || strcmp(token.lexema, "var") == 0 ||
            strcmp(token.lexema, "begin") == 0 || strcmp(token.lexema, "end") == 0 ||
            strcmp(token.lexema, "integer") == 0 || strcmp(token.lexema, "real") == 0 ||
            strcmp(token.lexema, "if") == 0 || strcmp(token.lexema, "then") == 0 ||
            strcmp(token.lexema, "else") == 0 || strcmp(token.lexema, "while") == 0 || 
            strcmp(token.lexema, "do") == 0) {
            strcpy(token.tipo, token.lexema);
        } else {
            strcpy(token.tipo, "identificador");
        }
    } else if (isdigit(c)) {
        int i = 0;
        int pontoDecimal = 0;
        do {
            if (c == '.' && !pontoDecimal) {
                pontoDecimal = 1;
            } else if (c == '.' && pontoDecimal) {
                break;
            }
            token.lexema[i++] = c;
            c = fgetc(arquivoFonte);
        } while ((isdigit(c) || c == '.') && i < 49);
        ungetc(c, arquivoFonte);
        token.lexema[i] = '\0';
        strcpy(token.tipo, pontoDecimal ? "numero_real" : "numero");
    } else if (c == '\"') {
        int i = 0;
        token.lexema[i++] = c;
        while ((c = fgetc(arquivoFonte)) != '\"' && c != EOF && i < 49) {
            if (c == '\n') linhaAtual++;
            token.lexema[i++] = c;
        }
        token.lexema[i++] = c;
        token.lexema[i] = '\0';
        strcpy(token.tipo, "string");
    } else {
        token.lexema[0] = c;
        token.lexema[1] = '\0';
        switch (c) {
            case ':':
                c = fgetc(arquivoFonte);
                if (c == '=') {
                    strcpy(token.lexema, ":=");
                    strcpy(token.tipo, ":=");
                } else {
                    ungetc(c, arquivoFonte);
                    strcpy(token.tipo, ":");
                }
                break;
            case '>': case '<': case '=': case ';': case '.': case ',': 
            case '+': case '-': case '*': case '/':
                strcpy(token.tipo, token.lexema);
                break;
            default:
                strcpy(token.tipo, "invalido");
        }
    }

    token.linha = linhaAtual;
    return token;
}

void Erro(const char *mensagem, const char *token) {
    if (token != NULL) {
        printf("Erro: %s próximo a '%s'\n", mensagem, token);
    } else {
        printf("Erro: %s\n", mensagem);
    }
    exit(1);  // Interrompe a execução para que a análise não prossiga após um erro.
}

void CasaToken(const char *tipoEsperado) {
    if (strcmp(tokenAtual.tipo, tipoEsperado) == 0) {
        printf("Consumindo token: %s [%s]\n", tokenAtual.tipo, tokenAtual.lexema);
        tokenAtual = ProximoToken();
    } else {
        Erro("token nao esperado", tipoEsperado);
    }
}

// Função para inicializar o arquivo e o analisador
void AnalisarPrograma() {
    CasaToken("program");
    CasaToken("identificador");
    CasaToken(";");
    AnalisarBloco();
    CasaToken(".");
}

void AnalisarBloco() {
    AnalisarDeclaracaoVariaveis();
    AnalisarComandoComposto();
}

void AnalisarDeclaracaoVariaveis() {
    CasaToken("var");
    
    // Verifica se há identificadores e consome a lista de variáveis
    while (strcmp(tokenAtual.tipo, "identificador") == 0) {
        AnalisarListaIdentificadores();
        CasaToken(":");
        AnalisarTipo();
        CasaToken(";");  // Consome o ponto e vírgula após o tipo
    }
}


void AnalisarListaIdentificadores() {
    AdicionarSimbolo(tokenAtual.lexema, ""); // Adiciona o nome, tipo preenchido posteriormente
    CasaToken("identificador");
    while (strcmp(tokenAtual.tipo, ",") == 0) {
        CasaToken(",");
        AdicionarSimbolo(tokenAtual.lexema, "");
        CasaToken("identificador");
    }
}

void AnalisarTipo() {
    if (strcmp(tokenAtual.tipo, "integer") == 0 || strcmp(tokenAtual.tipo, "real") == 0) {
        const char* tipo = tokenAtual.tipo;
        for (int i = contadorSimbolos - 1; i >= 0; i--) {
            if (strcmp(tabelaSimbolos[i].tipo, "") == 0) {
                strcpy(tabelaSimbolos[i].tipo, tipo);
            }
        }
        CasaToken(tokenAtual.tipo);
    } else {
        Erro("Tipo inválido", NULL);
    }
}

void AnalisarComandoComposto() {
    CasaToken("begin");
    
    // Se houver comandos para analisar, processa-os
    while (strcmp(tokenAtual.tipo, "end") != 0) {
        AnalisarComando();
        
        // Se após o comando encontrado houver um ponto e vírgula (;), consome-o
        if (strcmp(tokenAtual.tipo, ";") == 0) {
            CasaToken(";");
        } else if (strcmp(tokenAtual.tipo, "end") != 0) {
            // Se não encontrar ";" ou "end", significa que houve um erro
            Erro("Esperado ';' ou 'end' após o comando", NULL);
        }
    }
    
    // Consome o token "end" para fechar o bloco composto
    CasaToken("end");
}

void AnalisarComando() {
    if (strcmp(tokenAtual.tipo, "identificador") == 0) {
        const char* tipoVar = BuscarTipoSimbolo(tokenAtual.lexema);
        if (!tipoVar) {
            Erro("Variável não declarada", tokenAtual.lexema);
        }
        CasaToken("identificador");
        CasaToken(":=");
        AnalisarExpressao();
    } else if (strcmp(tokenAtual.tipo, "begin") == 0) {
        AnalisarComandoComposto();
    } else {
        Erro("Comando inválido", NULL);
    }
}

void AnalisarExpressao() {
    const char *tipoOperando = NULL;

    // Verifica o primeiro operando
    if (strcmp(tokenAtual.tipo, "identificador") == 0) {
        tipoOperando = BuscarTipoSimbolo(tokenAtual.lexema);
        if (tipoOperando == NULL) {
            Erro("Variável não declarada", tokenAtual.lexema);
        }
    } else if (strcmp(tokenAtual.tipo, "numero") == 0 || strcmp(tokenAtual.tipo, "numero_real") == 0) {
        tipoOperando = tokenAtual.tipo;
    } else if (strcmp(tokenAtual.tipo, "string") == 0) {
        tipoOperando = "string";  // Se o operando for string
    } else {
        Erro("Operando inválido", NULL);
    }

    CasaToken(tokenAtual.tipo);  // Consome o primeiro operando

    // Trata os operadores aritméticos (+ e -)
    while (strcmp(tokenAtual.tipo, "+") == 0 || strcmp(tokenAtual.tipo, "-") == 0) {
        CasaToken(tokenAtual.tipo);  // Consome o operador
        const char *tipoProximoOperando = NULL;

        // Verifica o tipo do próximo operando
        if (strcmp(tokenAtual.tipo, "identificador") == 0) {
            tipoProximoOperando = BuscarTipoSimbolo(tokenAtual.lexema);
            if (tipoProximoOperando == NULL) {
                Erro("Variável não declarada", tokenAtual.lexema);
            }
        } else if (strcmp(tokenAtual.tipo, "numero") == 0 || strcmp(tokenAtual.tipo, "numero_real") == 0) {
            tipoProximoOperando = tokenAtual.tipo;
        } else if (strcmp(tokenAtual.tipo, "string") == 0) {
            tipoProximoOperando = "string";
        }

        // Verifica se os tipos são compatíveis (exclui combinação de tipos inválidos, como string com número)
        if ((tipoOperando && tipoProximoOperando) && 
            ((strcmp(tipoOperando, "string") == 0 && strcmp(tipoProximoOperando, "string") != 0) ||
             (strcmp(tipoOperando, "numero") == 0 && strcmp(tipoProximoOperando, "string") == 0) ||
             (strcmp(tipoOperando, "numero_real") == 0 && strcmp(tipoProximoOperando, "string") == 0))) {
            Erro("Erro de tipo: operação inválida entre tipos incompatíveis", tokenAtual.lexema);
        }

        CasaToken(tokenAtual.tipo);  // Consome o próximo operando
    }
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo fonte>\n", argv[0]);
        return 1;
    }

    arquivoFonte = fopen(argv[1], "r");
    if (arquivoFonte == NULL) {
        printf("Erro ao abrir o arquivo %s\n", argv[1]);
        return 1;
    }

    tokenAtual = ProximoToken();
    AnalisarPrograma();
    printf("Análise sintática concluída com sucesso.\n");

    fclose(arquivoFonte);
    return 0;
}
