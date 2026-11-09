#include <stdio.h>

void iniciar();
void simularFalha();
void valentao();
void anel();
void identificarNovoCoordenador();
void registrarMensagensTroca();
void contabilizarQuantidadeMensagens();
void informarEstadoAtualSistema();
void atualizarInformacoesCoordenador();
void tratarOperacoesInvalidas();

typedef struct processo Processo;

int main(){
    char* opcao;
    do{
        
        fgets(opcao, 5, stdin);

    } while(opcao[1] != 'f');

    return 0;
}

struct processo{
    int ID;
    int estado;
    Processo* coordenador;
};