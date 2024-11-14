#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct {
    char lexema[50];
    char tipo[20];
    int linha;
} Token;

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

void AbrirArquivo(const char *nomeArquivo) {
    arquivoFonte = fopen(nomeArquivo, "r");
    if (arquivoFonte == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(1);
    }
}

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

void Erro(const char *mensagem, const char *tipoEsperado) {
    if (tipoEsperado) {
        printf("%d:%s, esperado [%s] mas encontrado [%s].\n", tokenAtual.linha, mensagem, tipoEsperado, tokenAtual.lexema);
    } else {
        printf("%d:%s [%s].\n", tokenAtual.linha, mensagem, tokenAtual.lexema);
    }
    exit(1);
}

void CasaToken(const char *tipoEsperado) {
    if (strcmp(tokenAtual.tipo, tipoEsperado) == 0) {
        printf("Consumindo token: %s [%s]\n", tokenAtual.tipo, tokenAtual.lexema);
        tokenAtual = ProximoToken();
    } else {
        Erro("token nao esperado", tipoEsperado);
    }
}

void AnalisarPrograma() {
    CasaToken("program");
    CasaToken("identificador");
    CasaToken(";");
    AnalisarBloco();
    CasaToken(".");
}

void AnalisarBloco() {
    if (strcmp(tokenAtual.tipo, "var") == 0) {
        CasaToken("var");
        while (strcmp(tokenAtual.tipo, "identificador") == 0) {
            AnalisarDeclaracaoVariaveis();
            CasaToken(";");
        }
    }
    AnalisarComandoComposto();
}

void AnalisarDeclaracaoVariaveis() {
    AnalisarListaIdentificadores();
    CasaToken(":");
    AnalisarTipo();
}

void AnalisarListaIdentificadores() {
    CasaToken("identificador");
    while (strcmp(tokenAtual.tipo, ",") == 0) {
        CasaToken(",");
        CasaToken("identificador");
    }
}

void AnalisarTipo() {
    if (strcmp(tokenAtual.tipo, "integer") == 0 || strcmp(tokenAtual.tipo, "real") == 0) {
        CasaToken(tokenAtual.tipo);
    } else {
        Erro("Tipo inválido", NULL);
    }
}

void AnalisarComandoComposto() {
    CasaToken("begin");
    do {
        AnalisarComando();
        if (strcmp(tokenAtual.tipo, ";") == 0) CasaToken(";");
    } while (strcmp(tokenAtual.tipo, "end") != 0);
    CasaToken("end");
}

void AnalisarComando() {
    if (strcmp(tokenAtual.tipo, "identificador") == 0) {
        CasaToken("identificador");
        CasaToken(":=");
        AnalisarExpressao();
    } else if (strcmp(tokenAtual.tipo, "if") == 0) {
        CasaToken("if");
        AnalisarExpressaoRelacional();
        CasaToken("then");
        AnalisarComando();
        if (strcmp(tokenAtual.tipo, "else") == 0) {
            CasaToken("else");
            AnalisarComando();
        }
    } else if (strcmp(tokenAtual.tipo, "while") == 0) {
        CasaToken("while");
        AnalisarExpressaoRelacional();
        CasaToken("do");
        AnalisarComando();
    } else {
        Erro("Comando inválido", NULL);
    }
}

void AnalisarExpressaoRelacional() {
    AnalisarExpressao();
    if (strcmp(tokenAtual.tipo, ">") == 0 || strcmp(tokenAtual.tipo, "<") == 0 || strcmp(tokenAtual.tipo, "=") == 0 ||
        strcmp(tokenAtual.tipo, "<=") == 0 || strcmp(tokenAtual.tipo, ">=") == 0 || strcmp(tokenAtual.tipo, "<>") == 0) {
        CasaToken(tokenAtual.tipo);
        AnalisarExpressao();
    } else {
        Erro("Operador relacional esperado", NULL);
    }
}

void AnalisarExpressao() {
    AnalisarTermo();
    while (strcmp(tokenAtual.tipo, "+") == 0 || strcmp(tokenAtual.tipo, "-") == 0) {
        CasaToken(tokenAtual.tipo);
        AnalisarTermo();
    }
}

void AnalisarTermo() {
    AnalisarFator();
    while (strcmp(tokenAtual.tipo, "*") == 0 || strcmp(tokenAtual.tipo, "/") == 0) {
        CasaToken(tokenAtual.tipo);
        AnalisarFator();
    }
}

void AnalisarFator() {
    if (strcmp(tokenAtual.tipo, "identificador") == 0 || strcmp(tokenAtual.tipo, "numero") == 0 || strcmp(tokenAtual.tipo, "numero_real") == 0) {
        CasaToken(tokenAtual.tipo);
    } else if (strcmp(tokenAtual.tipo, "string") == 0) {
        CasaToken("string");
    } else if (strcmp(tokenAtual.tipo, "(") == 0) {
        CasaToken("(");
        AnalisarExpressao();
        CasaToken(")");
    } else {
        Erro("Fator inválido", NULL);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_fonte>\n", argv[0]);
        return 1;
    }
    AbrirArquivo(argv[1]);
    tokenAtual = ProximoToken();
    AnalisarPrograma();
    if (strcmp(tokenAtual.tipo, "EOF") == 0) {
        printf("Análise concluída com sucesso!\n");
    } else {
        Erro("Código após o final do programa", NULL);
    }
    fclose(arquivoFonte);
    return 0;
}
