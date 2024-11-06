#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definições para o analisador léxico e tokens (exemplo simplificado)
typedef struct {
    char lexema[100];
    char tipo[20];
    int linha;
} Token;

Token tokenAtual;

// Funções simuladas para o analisador léxico e de obtenção do próximo token
Token ProximoToken() {
    // Função simulada; substitua pelo código real do analisador léxico.
    Token token;
    token.linha = 1;
    strcpy(token.lexema, "x");
    strcpy(token.tipo, "identificador");
    return token;
}

// Função para consumir tokens do programa, em conformidade com o esperado
void CasaToken(const char *esperado) {
    printf("DEBUG: Token esperado: %s, Token atual: %s\n", esperado, tokenAtual.lexema);  // Debug
    if (strcmp(tokenAtual.tipo, esperado) == 0) {
        tokenAtual = ProximoToken();  // Obtém próximo token
    } else {
        printf("%d:token nao esperado [%s].\n", tokenAtual.linha, tokenAtual.lexema);
        exit(1);
    }
}

// Função para analisar o programa inicial
void AnalisarPrograma() {
    printf("DEBUG: Iniciando análise do programa\n");  // Debug
    CasaToken("program");
    CasaToken("identificador");
    CasaToken(";");
    AnalisarBloco();
    CasaToken(".");
}

// Analisadores sintáticos para cada produção da gramática
void AnalisarBloco() {
    printf("DEBUG: Analisando bloco\n");  // Debug
    AnalisarParteDeclaracoesVariaveis();
    AnalisarComandoComposto();
}

void AnalisarParteDeclaracoesVariaveis() {
    if (strcmp(tokenAtual.tipo, "var") == 0) {
        CasaToken("var");
        do {
            AnalisarDeclaracaoVariaveis();
            CasaToken(";");
        } while (strcmp(tokenAtual.tipo, "identificador") == 0);
    }
}

void AnalisarDeclaracaoVariaveis() {
    printf("DEBUG: Analisando declaração de variáveis\n");  // Debug
    AnalisarListaIdentificadores();
    CasaToken(":");
    AnalisarTipo();
}

void AnalisarListaIdentificadores() {
    CasaToken("identificador");
    while (strcmp(tokenAtual.lexema, ",") == 0) {
        CasaToken(",");
        CasaToken("identificador");
    }
}

void AnalisarTipo() {
    if (strcmp(tokenAtual.lexema, "integer") == 0 || strcmp(tokenAtual.lexema, "real") == 0) {
        CasaToken(tokenAtual.lexema);
    } else {
        printf("%d:token nao esperado [%s].\n", tokenAtual.linha, tokenAtual.lexema);
        exit(1);
    }
}

void AnalisarComandoComposto() {
    CasaToken("begin");
    AnalisarComando();
    while (strcmp(tokenAtual.lexema, ";") == 0) {
        CasaToken(";");
        AnalisarComando();
    }
    CasaToken("end");
}

void AnalisarComando() {
    printf("DEBUG: Analisando comando\n");  // Debug
    if (strcmp(tokenAtual.tipo, "identificador") == 0) {
        AnalisarAtribuicao();
    } else if (strcmp(tokenAtual.lexema, "if") == 0) {
        AnalisarComandoCondicional();
    } else if (strcmp(tokenAtual.lexema, "while") == 0) {
        AnalisarComandoRepetitivo();
    } else if (strcmp(tokenAtual.lexema, "begin") == 0) {
        AnalisarComandoComposto();
    } else {
        printf("%d:token nao esperado [%s].\n", tokenAtual.linha, tokenAtual.lexema);
        exit(1);
    }
}

void AnalisarAtribuicao() {
    CasaToken("identificador");
    CasaToken(":=");
    AnalisarExpressao();
}

void AnalisarComandoCondicional() {
    CasaToken("if");
    AnalisarExpressao();
    CasaToken("then");
    AnalisarComando();
    if (strcmp(tokenAtual.lexema, "else") == 0) {
        CasaToken("else");
        AnalisarComando();
    }
}

void AnalisarComandoRepetitivo() {
    CasaToken("while");
    AnalisarExpressao();
    CasaToken("do");
    AnalisarComando();
}

void AnalisarExpressao() {
    printf("DEBUG: Analisando expressão\n");  // Debug
    // Exemplo simplificado para fins de debug; implemente a análise completa de expressões conforme necessário.
    CasaToken("identificador");  // Exemplo de tratamento
}

int main() {
    tokenAtual = ProximoToken();
    AnalisarPrograma();

    if (strcmp(tokenAtual.tipo, "EOF") == 0) {
        printf("Compilação concluída com sucesso.\n");
    } else {
        printf("%d:fim de arquivo não esperado.\n", tokenAtual.linha);
    }

    return 0;
}
