#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Definições de estruturas e variáveis globais
typedef struct
{
    char lexema[50];
    char tipo[20];
    int linha;
} Token;

typedef struct
{
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

void AdicionarSimbolo(const char *nome, const char *tipo)
{
    if (contadorSimbolos < MAX_SIMBOLOS)
    {
        strcpy(tabelaSimbolos[contadorSimbolos].nome, nome);
        strcpy(tabelaSimbolos[contadorSimbolos].tipo, tipo);
        contadorSimbolos++;
    }
    else
    {
        printf("Erro: limite da tabela de símbolos atingido.\n");
    }
}

const char *BuscarTipoSimbolo(const char *nome)
{
    for (int i = 0; i < contadorSimbolos; i++)
    {
        if (strcmp(tabelaSimbolos[i].nome, nome) == 0)
        {
            return tabelaSimbolos[i].tipo;
        }
    }
    return NULL; // Não encontrado
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

    if (c == '\'') { // Início de uma string
        int i = 0;
        c = fgetc(arquivoFonte); // Avança para o próximo caractere
        while (c != '\'' && c != EOF && i < 49) { // Lê até encontrar outro apóstrofo ou EOF
            token.lexema[i++] = c;
            c = fgetc(arquivoFonte);
        }
        if (c != '\'') { // Verifica se a string foi fechada corretamente
            Erro("String não fechada corretamente", NULL);
        }
        token.lexema[i] = '\0';
        strcpy(token.tipo, "string");
    } else if (isalpha(c)) {
        int i = 0;
        do {
            token.lexema[i++] = c;
            c = fgetc(arquivoFonte);
        } while ((isalnum(c) || c == '_') && i < 49); 
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
    }
    else if (isdigit(c)) {
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
    }
    else {
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
            case '(':
                strcpy(token.tipo, "(");
                break;
            case ')':
                strcpy(token.tipo, ")");
                break;
            case '>': case '<': case '=': case ';': case '.': case ',':
            case '+': case '-': case '*': case '/':
                strcpy(token.tipo, token.lexema);
                break;
            default:
                printf("Erro: Caracter inválido '%c' (código %d) na linha %d\n", c, c, linhaAtual);
                strcpy(token.tipo, "invalido");
        }
    }

    token.linha = linhaAtual;
    return token;
}

void Erro(const char *mensagem, const char *token)
{
    if (token != NULL)
    {
        printf("%d: %s [%s].\n", linhaAtual, mensagem, token);
    }
    else
    {
        printf("%d: %s.\n", linhaAtual, mensagem);
    }
    exit(1);
}

void CasaToken(const char *tipoEsperado)
{
    if (strcmp(tokenAtual.tipo, tipoEsperado) == 0)
    {
        printf("Consumindo token: %s [%s]\n", tokenAtual.tipo, tokenAtual.lexema);
        tokenAtual = ProximoToken();
    }
    else
    {
        Erro("token nao esperado", tokenAtual.lexema);
    }
}

void AnalisarPrograma()
{
    printf("S -> program id ; S\n");
    CasaToken("program");
    CasaToken("identificador");
    CasaToken(";");
    AnalisarBloco();
    CasaToken(".");
}

void AnalisarBloco()
{
    printf("S -> S ; S\n");
    AnalisarDeclaracaoVariaveis();
    AnalisarComandoComposto();
}

void AnalisarDeclaracaoVariaveis()
{
    printf("S -> var S ;\n");
    CasaToken("var");
    while (strcmp(tokenAtual.tipo, "identificador") == 0)
    {
        AnalisarListaIdentificadores();
        CasaToken(":");
        AnalisarTipo();
        CasaToken(";");
    }
}

void AnalisarListaIdentificadores()
{
    printf("S -> id\n");
    AdicionarSimbolo(tokenAtual.lexema, "");
    CasaToken("identificador");
    while (strcmp(tokenAtual.tipo, ",") == 0)
    {
        CasaToken(",");
        printf("S -> id\n");
        AdicionarSimbolo(tokenAtual.lexema, "");
        CasaToken("identificador");
    }
}

void AnalisarTipo()
{
    if (strcmp(tokenAtual.tipo, "integer") == 0 || strcmp(tokenAtual.tipo, "real") == 0)
    {
        printf("S -> tipo\n");
        const char *tipo = tokenAtual.tipo;
        for (int i = contadorSimbolos - 1; i >= 0; i--)
        {
            if (strcmp(tabelaSimbolos[i].tipo, "") == 0)
            {
                strcpy(tabelaSimbolos[i].tipo, tipo);
            }
        }
        CasaToken(tokenAtual.tipo);
    }
    else
    {
        Erro("Tipo inválido", NULL);
    }
}

void AnalisarComandoComposto() {
    CasaToken("begin");
    while (strcmp(tokenAtual.tipo, "end") != 0) {
        AnalisarComando();
        if (strcmp(tokenAtual.tipo, ";") == 0) {
            CasaToken(";");
        } else if (strcmp(tokenAtual.tipo, "end") != 0) {
            Erro("Esperado ';' ou 'end'", tokenAtual.tipo);
        }
    }
    CasaToken("end");
}

void AnalisarComando() {
    if (strcmp(tokenAtual.tipo, "identificador") == 0) {
        const char *tipo = BuscarTipoSimbolo(tokenAtual.lexema);

        if (tipo == NULL || strcmp(tokenAtual.lexema, "writeln") == 0) {
            CasaToken("identificador");
            if (strcmp(tokenAtual.tipo, "(") == 0) {
                CasaToken("(");
                while (strcmp(tokenAtual.tipo, ")") != 0) {
                    AnalisarExpressao();
                    if (strcmp(tokenAtual.tipo, ",") == 0) {
                        CasaToken(",");
                    }
                }
                CasaToken(")");
            }
        } else {
            CasaToken("identificador");
            CasaToken(":=");
            AnalisarExpressao();
        }
    } else if (strcmp(tokenAtual.tipo, "if") == 0) {
        CasaToken("if");
        AnalisarExpressaoRelacional();
        CasaToken("then");
        AnalisarComando();

 if (strcmp(tokenAtual.tipo, "else") == 0) {
    CasaToken("else");

    if (strcmp(tokenAtual.tipo, ";") == 0) {
        Erro("Comando invalido apos 'else': vazio ou mal estruturado", tokenAtual.tipo);
    } else {
        AnalisarComando();
    }
}
    } else if (strcmp(tokenAtual.tipo, "while") == 0) {
        CasaToken("while");
        AnalisarExpressaoRelacional();
        CasaToken("do");
        AnalisarComando();
    } else if (strcmp(tokenAtual.tipo, "begin") == 0) {
        AnalisarComandoComposto();
    } else {
        Erro("Comando invalido", tokenAtual.tipo);
    }
}

void AnalisarExpressaoRelacional()
{
    AnalisarExpressao();
    if (strcmp(tokenAtual.tipo, "=") == 0 || strcmp(tokenAtual.tipo, "<") == 0 || strcmp(tokenAtual.tipo, ">") == 0)
    {
        CasaToken(tokenAtual.tipo);
        AnalisarExpressao();
    }
    else
    {
        Erro("Operador relacional esperado", tokenAtual.tipo);
    }
}

void AnalisarExpressao()
{
    AnalisarTermo();
    while (strcmp(tokenAtual.tipo, "+") == 0 || strcmp(tokenAtual.tipo, "-") == 0)
    {
        CasaToken(tokenAtual.tipo);
        AnalisarTermo();
    }
}

void AnalisarTermo()
{
    AnalisarFator();
    while (strcmp(tokenAtual.tipo, "*") == 0 || strcmp(tokenAtual.tipo, "/") == 0)
    {
        CasaToken(tokenAtual.tipo);
        AnalisarFator();
    }
}

void AnalisarFator()
{
    if (strcmp(tokenAtual.tipo, "identificador") == 0)
    {
        const char *tipo = BuscarTipoSimbolo(tokenAtual.lexema);
        if (tipo == NULL)
        {
            Erro("Símbolo não declarado", tokenAtual.lexema);
        }
        CasaToken("identificador");
    }
    else if (strcmp(tokenAtual.tipo, "numero") == 0 || strcmp(tokenAtual.tipo, "numero_real") == 0)
    {
        CasaToken(tokenAtual.tipo);
    }
    else if (strcmp(tokenAtual.tipo, "string") == 0) // Novo caso para strings
    {
        CasaToken("string");
    }
    else if (strcmp(tokenAtual.tipo, "(") == 0)
    {
        CasaToken("(");
        AnalisarExpressao();
        CasaToken(")");
    }
    else
    {
        Erro("Fator inválido", tokenAtual.tipo);
    }
}
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Erro: arquivo fonte não especificado.\n");
        return 1;
    }

    arquivoFonte = fopen(argv[1], "r");
    if (arquivoFonte == NULL)
    {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    tokenAtual = ProximoToken();
    AnalisarPrograma();

    if (strcmp(tokenAtual.tipo, "EOF") == 0)
    {
        printf("Analise concluida com sucesso.\n");
    }
    else
    {
        Erro("fim de arquivo não esperado", NULL);
    }

    fclose(arquivoFonte);
    return 0;
}