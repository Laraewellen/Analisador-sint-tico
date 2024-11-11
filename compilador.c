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

// Declaração das funções
Token ProximoToken();
void Erro(char *mensagem);
void Consumir(char *tipoEsperado);
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
    char lexema[50];
    int c;

    // Ignorar espaços, quebras de linha e comentários
    while ((c = fgetc(arquivoFonte)) != EOF) {
        if (c == ' ' || c == '\t') {
            continue;
        } else if (c == '\n') {
            linhaAtual++;
        } else if (c == '/' && (c = fgetc(arquivoFonte)) == '/') {
            // Ignora comentários de linha
            while ((c = fgetc(arquivoFonte)) != '\n' && c != EOF);
            linhaAtual++;
        } else {
            ungetc(c, arquivoFonte);  // Retorna o caractere não-processado ao buffer
            break;
        }
    }

    // Fim do arquivo
    if (c == EOF) {
        strcpy(token.tipo, "EOF");
        return token;
    }

    // Lê um lexema do arquivo
    fscanf(arquivoFonte, "%49s", lexema);

    // Determinação do tipo de token (simplificado para exemplo)
    if (strcmp(lexema, "program") == 0 || strcmp(lexema, "var") == 0 ||
        strcmp(lexema, "begin") == 0 || strcmp(lexema, "end") == 0 ||
        strcmp(lexema, "integer") == 0 || strcmp(lexema, "real") == 0 ||
        strcmp(lexema, "if") == 0 || strcmp(lexema, "then") == 0 ||
        strcmp(lexema, "else") == 0 || strcmp(lexema, "while") == 0 || 
        strcmp(lexema, "do") == 0) {
        strcpy(token.tipo, lexema);
    } else if (strcmp(lexema, ":=") == 0 || strcmp(lexema, ">") == 0 ||
               strcmp(lexema, "<") == 0 || strcmp(lexema, "=") == 0 ||
               strcmp(lexema, ";") == 0 || strcmp(lexema, ":") == 0 ||
               strcmp(lexema, ".") == 0 || strcmp(lexema, ",") == 0 ||
               strcmp(lexema, "+") == 0 || strcmp(lexema, "-") == 0 ||
               strcmp(lexema, "*") == 0 || strcmp(lexema, "/") == 0) {
        strcpy(token.tipo, lexema);
    } else if (isalpha(lexema[0])) {
        strcpy(token.tipo, "identificador");
    } else if (isdigit(lexema[0])) {
        strcpy(token.tipo, "numero");
    } else {
        strcpy(token.tipo, "invalido");
    }

    strcpy(token.lexema, lexema);
    token.linha = linhaAtual;
    return token;
}

void Erro(char *mensagem) {
    printf("%d: %s [%s].\n", tokenAtual.linha, mensagem, tokenAtual.lexema);
    tokenAtual = ProximoToken();
}

void Consumir(char *tipoEsperado) {
    if (strcmp(tokenAtual.tipo, tipoEsperado) == 0) {
        tokenAtual = ProximoToken();
    } else {
        Erro("token nao esperado");
    }
}

void AnalisarPrograma() {
    printf("DEBUG: Iniciando análise do programa\n");
    Consumir("program");
    Consumir("identificador");
    Consumir(";");
    AnalisarBloco();
    Consumir(".");
}

void AnalisarBloco() {
    if (strcmp(tokenAtual.tipo, "var") == 0) {
        Consumir("var");
        while (strcmp(tokenAtual.tipo, "identificador") == 0) {
            AnalisarDeclaracaoVariaveis();
            Consumir(";");
        }
    }
    AnalisarComandoComposto();
}

void AnalisarDeclaracaoVariaveis() {
    AnalisarListaIdentificadores();
    Consumir(":");
    AnalisarTipo();
}

void AnalisarListaIdentificadores() {
    do {
        Consumir("identificador");
    } while (strcmp(tokenAtual.tipo, ",") == 0 && (Consumir(","), 1));
}

void AnalisarTipo() {
    if (strcmp(tokenAtual.tipo, "integer") == 0 || strcmp(tokenAtual.tipo, "real") == 0) {
        Consumir(tokenAtual.tipo);
    } else {
        Erro("Tipo inválido");
    }
}

void AnalisarComandoComposto() {
    Consumir("begin");
    do {
        AnalisarComando();
        if (strcmp(tokenAtual.tipo, ";") == 0) Consumir(";");
    } while (strcmp(tokenAtual.tipo, "end") != 0);
    Consumir("end");
}

void AnalisarComando() {
    if (strcmp(tokenAtual.tipo, "identificador") == 0) {
        Consumir("identificador");
        Consumir(":=");
        AnalisarExpressao();
    } else if (strcmp(tokenAtual.tipo, "if") == 0) {
        Consumir("if");
        AnalisarExpressaoRelacional();
        Consumir("then");
        AnalisarComando();
        if (strcmp(tokenAtual.tipo, "else") == 0) {
            Consumir("else");
            AnalisarComando();
        }
    } else if (strcmp(tokenAtual.tipo, "while") == 0) {
        Consumir("while");
        AnalisarExpressaoRelacional();
        Consumir("do");
        AnalisarComando();
    } else {
        Erro("Comando inválido");
    }
}

void AnalisarExpressaoRelacional() {
    AnalisarExpressao();
    if (strcmp(tokenAtual.tipo, ">") == 0 || strcmp(tokenAtual.tipo, "<") == 0 || strcmp(tokenAtual.tipo, "=") == 0 ||
        strcmp(tokenAtual.tipo, "<=") == 0 || strcmp(tokenAtual.tipo, ">=") == 0 || strcmp(tokenAtual.tipo, "<>") == 0) {
        Consumir(tokenAtual.tipo);
        AnalisarExpressao();
    } else {
        Erro("Operador relacional esperado");
    }
}

void AnalisarExpressao() {
    AnalisarTermo();
    while (strcmp(tokenAtual.tipo, "+") == 0 || strcmp(tokenAtual.tipo, "-") == 0) {
        Consumir(tokenAtual.tipo);
        AnalisarTermo();
    }
}

void AnalisarTermo() {
    AnalisarFator();
    while (strcmp(tokenAtual.tipo, "*") == 0 || strcmp(tokenAtual.tipo, "/") == 0) {
        Consumir(tokenAtual.tipo);
        AnalisarFator();
    }
}

void AnalisarFator() {
    if (strcmp(tokenAtual.tipo, "identificador") == 0 || strcmp(tokenAtual.tipo, "numero") == 0) {
        Consumir(tokenAtual.tipo);
    } else {
        Erro("Fator inválido");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <arquivo.pas>\n", argv[0]);
        return 1;
    }

    AbrirArquivo(argv[1]);
    tokenAtual = ProximoToken();
    AnalisarPrograma();
    if (strcmp(tokenAtual.tipo, "EOF") == 0) {
        printf("Análise concluída com sucesso.\n");
    } else {
        Erro("Código após o final do programa");
    }

    fclose(arquivoFonte);
    return 0;
}
