#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definição dos tipos de tokens
typedef enum {
    TOKEN_PROGRAM, TOKEN_VAR, TOKEN_BEGIN, TOKEN_END, TOKEN_IF, TOKEN_THEN, TOKEN_ELSE,
    TOKEN_WHILE, TOKEN_DO, TOKEN_IDENTIFIER, TOKEN_INTEGER, TOKEN_REAL, TOKEN_SEMICOLON,
    TOKEN_COLON, TOKEN_COMMA, TOKEN_DOT, TOKEN_ASSIGN, TOKEN_EOF, TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[100];
} Token;

// Variáveis globais
Token tokenAtual;           // Token atual
int linhaAtual = 1;         // Linha atual do código fonte
FILE *file;                 // Arquivo de entrada com o código MicroPascal

// Função protótipo para o analisador léxico
Token getNextToken(FILE *file);

// Função que lê o próximo token do arquivo e atualiza o token atual
void proximoToken() {
    tokenAtual = getNextToken(file);
    // Incrementa a linha se encontrar uma nova linha no código
    if (strcmp(tokenAtual.lexeme, "\n") == 0) {
        linhaAtual++;
    }
}

// Função CasaToken: verifica se o token atual é o esperado, caso contrário, exibe um erro
void CasaToken(TokenType esperado) {
    if (tokenAtual.type == esperado) {
        proximoToken();  // Lê o próximo token se o atual for o esperado
    } else if (tokenAtual.type == TOKEN_EOF) {
        printf("%d:fim de arquivo não esperado.\n", linhaAtual);
        exit(1);
    } else {
        printf("%d:token nao esperado [%s].\n", linhaAtual, tokenAtual.lexeme);
        exit(1);
    }
}

// Funções para cada símbolo não-terminal da gramática

void AnalisarPrograma() {
    CasaToken(TOKEN_PROGRAM);      // Espera "program"
    CasaToken(TOKEN_IDENTIFIER);   // Espera um identificador (nome do programa)
    CasaToken(TOKEN_SEMICOLON);    // Espera ponto e vírgula
    AnalisarBloco();               // Chama a função para o bloco
    CasaToken(TOKEN_DOT);          // Espera ponto final
}

void AnalisarBloco() {
    AnalisarParteDeclaracoesVariaveis(); // Processa a parte de declarações
    AnalisarComandoComposto();           // Processa o comando composto
}

void AnalisarParteDeclaracoesVariaveis() {
    if (tokenAtual.type == TOKEN_VAR) {
        CasaToken(TOKEN_VAR);
        AnalisarDeclaracaoVariaveis();
        while (tokenAtual.type == TOKEN_SEMICOLON) {
            CasaToken(TOKEN_SEMICOLON);
            AnalisarDeclaracaoVariaveis();
        }
        CasaToken(TOKEN_SEMICOLON);
    }
}

void AnalisarDeclaracaoVariaveis() {
    AnalisarListaIdentificadores();
    CasaToken(TOKEN_COLON);
    AnalisarTipo();
}

void AnalisarListaIdentificadores() {
    CasaToken(TOKEN_IDENTIFIER);
    while (tokenAtual.type == TOKEN_COMMA) {
        CasaToken(TOKEN_COMMA);
        CasaToken(TOKEN_IDENTIFIER);
    }
}

void AnalisarTipo() {
    if (tokenAtual.type == TOKEN_INTEGER) {
        CasaToken(TOKEN_INTEGER);
    } else if (tokenAtual.type == TOKEN_REAL) {
        CasaToken(TOKEN_REAL);
    } else {
        printf("%d:token nao esperado [%s].\n", linhaAtual, tokenAtual.lexeme);
        exit(1);
    }
}

void AnalisarComandoComposto() {
    CasaToken(TOKEN_BEGIN);
    AnalisarComando();
    CasaToken(TOKEN_SEMICOLON);
    while (tokenAtual.type != TOKEN_END) {
        AnalisarComando();
        CasaToken(TOKEN_SEMICOLON);
    }
    CasaToken(TOKEN_END);
}

void AnalisarComando() {
    if (tokenAtual.type == TOKEN_IDENTIFIER) {
        AnalisarAtribuicao();
    } else if (tokenAtual.type == TOKEN_BEGIN) {
        AnalisarComandoComposto();
    } else if (tokenAtual.type == TOKEN_IF) {
        AnalisarComandoCondicional();
    } else if (tokenAtual.type == TOKEN_WHILE) {
        AnalisarComandoRepetitivo();
    } else {
        printf("%d:token nao esperado [%s].\n", linhaAtual, tokenAtual.lexeme);
        exit(1);
    }
}

// Exemplo de implementação das funções de atribuição, comando condicional e repetitivo

void AnalisarAtribuicao() {
    CasaToken(TOKEN_IDENTIFIER);  // Variável
    CasaToken(TOKEN_ASSIGN);      // Símbolo de atribuição :=
    AnalisarExpressao();
}

void AnalisarComandoCondicional() {
    CasaToken(TOKEN_IF);          // "if"
    AnalisarExpressao();
    CasaToken(TOKEN_THEN);        // "then"
    AnalisarComando();
    if (tokenAtual.type == TOKEN_ELSE) {
        CasaToken(TOKEN_ELSE);    // "else"
        AnalisarComando();
    }
}

void AnalisarComandoRepetitivo() {
    CasaToken(TOKEN_WHILE);       // "while"
    AnalisarExpressao();
    CasaToken(TOKEN_DO);          // "do"
    AnalisarComando();
}

// Função de exemplo para análise de expressão (simplificada)
void AnalisarExpressao() {
    // Aqui, você implementaria a lógica de expressão completa
    // Vamos supor que seja apenas um número ou um identificador
    if (tokenAtual.type == TOKEN_IDENTIFIER || tokenAtual.type == TOKEN_INTEGER) {
        CasaToken(tokenAtual.type);
    } else {
        printf("%d:token nao esperado [%s].\n", linhaAtual, tokenAtual.lexeme);
        exit(1);
    }
}

// Função principal do compilador
int main() {
    file = fopen("programa.mpas", "r");
    if (!file) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    linhaAtual = 1;       // Inicializa a linha
    proximoToken();       // Lê o primeiro token
    AnalisarPrograma();   // Inicia a análise sintática

    printf("Compilação concluída sem erros.\n");
    fclose(file);
    return 0;
}

// Função exemplo para obter o próximo token (getNextToken)
Token getNextToken(FILE *file) {
    Token token;
    // Implementação da análise léxica (simplificada, exemplo):
    // Aqui você leria do arquivo e atribuía o tipo e lexema do token
    // ...
    return token;
}
