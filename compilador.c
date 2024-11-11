#include <stdio.h>
#include <string.h>

// Estrutura para representar um token
typedef struct {
    char lexema[50];
    char tipo[20];
    int linha;
} Token;

int pos = 0;  // Posição no fluxo de tokens de teste
Token tokenAtual;

// Funções do analisador sintático
Token ProximoToken();
void Erro(char *mensagem);
void Consumir(char *tipoEsperado);
void AnalisarPrograma();
void AnalisarBloco();
void AnalisarParteDeclaracoesVariaveis();
void AnalisarDeclaracaoVariaveis();
void AnalisarListaIdentificadores();
void AnalisarTipo();
void AnalisarComandoComposto();
void AnalisarComando();
void AnalisarAtribuicao();
void AnalisarComandoCondicional();
void AnalisarComandoRepetitivo();
void AnalisarExpressao();
void AnalisarTermo();
void AnalisarFator();
void AnalisarExpressaoRelacional();

// Simulação do fluxo de tokens para teste
Token ProximoToken() {
    Token token;
    
    const char *fluxo_tokens[] = {"program", "identificador", ";", "var", "identificador", ",", "identificador", ":", "integer", ";", 
                                  "begin", "identificador", ":=", "numero", ";", "identificador", ":=", "identificador", "+", "numero", ";", 
                                  "if", "identificador", ">", "identificador", "then", "identificador", ":=", "numero", "else", 
                                  "identificador", ":=", "numero", ";", "while", "identificador", "<", "numero", "do", 
                                  "identificador", ":=", "identificador", "+", "numero", ";", "end", ".", "EOF"};

    const char *fluxo_tipos[] = {"program", "identificador", ";", "var", "identificador", ",", "identificador", ":", "integer", ";", 
                                 "begin", "identificador", ":=", "numero", ";", "identificador", ":=", "identificador", "+", "numero", ";", 
                                 "if", "identificador", ">", "identificador", "then", "identificador", ":=", "numero", "else", 
                                 "identificador", ":=", "numero", ";", "while", "identificador", "<", "numero", "do", 
                                 "identificador", ":=", "identificador", "+", "numero", ";", "end", ".", "EOF"};
    
    if (pos < sizeof(fluxo_tokens) / sizeof(fluxo_tokens[0])) {
        strcpy(token.lexema, fluxo_tokens[pos]);
        strcpy(token.tipo, fluxo_tipos[pos]);
        token.linha = 1;  // Linha de exemplo
        pos++;
    } else {
        strcpy(token.tipo, "EOF");
    }

    return token;
}

void Erro(char *mensagem) {
    printf("%d:%s [%s].\n", tokenAtual.linha, mensagem, tokenAtual.lexema);
    // Consumir o próximo token para evitar loops infinitos
    tokenAtual = ProximoToken();
}

void Consumir(char *tipoEsperado) {
    if (strcmp(tokenAtual.tipo, tipoEsperado) == 0) {
        tokenAtual = ProximoToken();
    } else {
        char mensagem[100];
        sprintf(mensagem, "token nao esperado");
        Erro(mensagem);
    }
}

void AnalisarPrograma() {
    printf("DEBUG: Iniciando análise do programa\n");
    if (strcmp(tokenAtual.tipo, "program") == 0) {
        Consumir("program");
        Consumir("identificador");
        Consumir(";");
        AnalisarBloco();
        Consumir(".");
    } else {
        Erro("Esperado 'program' no início do programa");
    }
}

void AnalisarBloco() {
    AnalisarParteDeclaracoesVariaveis();
    AnalisarComandoComposto();
}

void AnalisarParteDeclaracoesVariaveis() {
    if (strcmp(tokenAtual.tipo, "var") == 0) {
        Consumir("var");
        while (strcmp(tokenAtual.tipo, "identificador") == 0) {
            AnalisarDeclaracaoVariaveis();
            Consumir(";");
        }
    }
}

void AnalisarDeclaracaoVariaveis() {
    AnalisarListaIdentificadores();
    Consumir(":");
    AnalisarTipo();
}

void AnalisarListaIdentificadores() {
    Consumir("identificador");
    while (strcmp(tokenAtual.tipo, ",") == 0) {
        Consumir(",");
        Consumir("identificador");
    }
}

void AnalisarTipo() {
    if (strcmp(tokenAtual.tipo, "integer") == 0) {
        Consumir("integer");
    } else if (strcmp(tokenAtual.tipo, "real") == 0) {
        Consumir("real");
    } else {
        Erro("Tipo inválido");
    }
}

void AnalisarComandoComposto() {
    Consumir("begin");
    while (strcmp(tokenAtual.tipo, "end") != 0) {
        AnalisarComando();
        Consumir(";");
    }
    Consumir("end");
}

void AnalisarComando() {
    if (strcmp(tokenAtual.tipo, "identificador") == 0) {
        AnalisarAtribuicao();
    } else if (strcmp(tokenAtual.tipo, "if") == 0) {
        AnalisarComandoCondicional();
    } else if (strcmp(tokenAtual.tipo, "while") == 0) {
        AnalisarComandoRepetitivo();
    } else {
        Erro("Comando inválido");
    }
}

void AnalisarAtribuicao() {
    Consumir("identificador");
    Consumir(":=");
    AnalisarExpressao();
}

void AnalisarComandoCondicional() {
    Consumir("if");
    AnalisarExpressaoRelacional();
    Consumir("then");
    AnalisarComando();
    if (strcmp(tokenAtual.tipo, "else") == 0) {
        Consumir("else");
        AnalisarComando();
    }
}

void AnalisarComandoRepetitivo() {
    Consumir("while");
    AnalisarExpressaoRelacional();
    Consumir("do");
    AnalisarComando();
}

void AnalisarExpressaoRelacional() {
    AnalisarExpressao();
    if (strcmp(tokenAtual.tipo, ">") == 0 || strcmp(tokenAtual.tipo, "<") == 0 || 
        strcmp(tokenAtual.tipo, "=") == 0 || strcmp(tokenAtual.tipo, "<=") == 0 || 
        strcmp(tokenAtual.tipo, ">=") == 0 || strcmp(tokenAtual.tipo, "<>") == 0) {
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

int main() {
    tokenAtual = ProximoToken();
    AnalisarPrograma();

    if (strcmp(tokenAtual.tipo, "EOF") == 0) {
        printf("Análise concluída com sucesso.\n");
    } else {
        Erro("Código após o final do programa");
    }

    return 0;
}
