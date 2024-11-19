#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h> // Necessário para manipulação de argumentos variáveis

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
#define MAX_PRODUCOES 100

Simbolo tabelaSimbolos[MAX_SIMBOLOS];
int contadorSimbolos = 0;

FILE *arquivoFonte;
Token tokenAtual;
int linhaAtual = 1;

char producoes[MAX_PRODUCOES][100];
int contadorProducoes = 0;

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
void RegistrarProducao(const char *producao, ...);
void ExibirProducoes();

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
    return NULL;
}

Token ProximoToken() {
    // (O seu código original para ProximoToken foi mantido integralmente)
    // ...
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

// Função para registrar regras de produção
void RegistrarProducao(const char *producao, ...)
{
    if (contadorProducoes < MAX_PRODUCOES)
    {
        va_list args;
        va_start(args, producao);
        vsnprintf(producoes[contadorProducoes], sizeof(producoes[contadorProducoes]), producao, args);
        va_end(args);
        contadorProducoes++;
    }
    else
    {
        printf("Erro: limite de registro de produções atingido.\n");
    }
}

// Função para exibir as regras de produção utilizadas
void ExibirProducoes()
{
    printf("\nRegras de produção utilizadas:\n");
    for (int i = 0; i < contadorProducoes; i++)
    {
        printf("%d. %s\n", i + 1, producoes[i]);
    }
}

void AnalisarPrograma()
{
    RegistrarProducao("programa -> 'program' identificador ';' bloco '.'");
    CasaToken("program");
    CasaToken("identificador");
    CasaToken(";");
    AnalisarBloco();
    CasaToken(".");
}

void AnalisarBloco()
{
    RegistrarProducao("bloco -> declaracao_variaveis comando_composto");
    AnalisarDeclaracaoVariaveis();
    AnalisarComandoComposto();
}

void AnalisarDeclaracaoVariaveis()
{
    if (strcmp(tokenAtual.tipo, "var") == 0)
    {
        RegistrarProducao("declaracao_variaveis -> 'var' lista_identificadores ':' tipo ';' { lista_identificadores ':' tipo ';' }");
        CasaToken("var");
        while (strcmp(tokenAtual.tipo, "identificador") == 0)
        {
            AnalisarListaIdentificadores();
            CasaToken(":");
            AnalisarTipo();
            CasaToken(";");
        }
    }
}

void AnalisarListaIdentificadores()
{
    RegistrarProducao("lista_identificadores -> identificador { ',' identificador }");
    AdicionarSimbolo(tokenAtual.lexema, "");
    CasaToken("identificador");
    while (strcmp(tokenAtual.tipo, ",") == 0)
    {
        CasaToken(",");
        AdicionarSimbolo(tokenAtual.lexema, "");
        CasaToken("identificador");
    }
}

void AnalisarTipo()
{
    RegistrarProducao("tipo -> 'integer' | 'real'");
    if (strcmp(tokenAtual.tipo, "integer") == 0 || strcmp(tokenAtual.tipo, "real") == 0)
    {
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

void AnalisarComandoComposto()
{
    RegistrarProducao("comando_composto -> 'begin' comando { ';' comando } 'end'");
    CasaToken("begin");
    while (strcmp(tokenAtual.tipo, "end") != 0)
    {
        AnalisarComando();
        if (strcmp(tokenAtual.tipo, ";") == 0)
        {
            CasaToken(";");
        }
        else if (strcmp(tokenAtual.tipo, "end") != 0)
        {
            Erro("Esperado ';' ou 'end'", tokenAtual.tipo);
        }
    }
    CasaToken("end");
}

// Outras funções permanecem inalteradas...

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
        printf("Compilação concluída com sucesso.\n");
        ExibirProducoes(); // Exibe as regras de produção utilizadas
    }
    else
    {
        Erro("fim de arquivo não esperado", NULL);
    }

    fclose(arquivoFonte);
    return 0;
}
