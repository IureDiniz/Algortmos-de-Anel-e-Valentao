class Processo:
    def __init__(self, id, estado):
        self.id = id
        self.estado = estado
        self.coordenador = self


def conferir(processo):
    print(f"processo: P{processo.id}")
    print(f"coordenador: P{processo.coordenador.id}")
    print(f"estado: {processo.estado}\n")

def inicializarProcessos(n, processos):
    if n <= 0:
        print("Erro: a quantidade de processos deve ser positiva.")
        return

    for i in range(n):
        estado = "ativo"
        processo = Processo(i, estado)
        processos.append(processo)

    for i in range(n):
        processos[i].coordenador = processos[n - 1]

    print(f"Sistema iniciado com {n} processos.")

def simularFalha(id, processos):
    id -= 1

    for processo in processos:
        if processo.id == id:

            if processo.estado == "inativo":
                print(f"Falha ja registrada: P{processo.id+1}.")
                return

            processo.estado = "inativo"
            print(f"Falha registrada: P{processo.id+1}.")
            return

    print(f"Falha: processo P{id+1} inexistente.")

def algoritmoValentao(id, processos, mensagens, idOriginal):
    id -= 1
    idOriginal -= 1

    if idOriginal >= len(processos):
        print(f"Erro: processo P{idOriginal+1} inexistente.")
        return

    if id >= len(processos):
        return

    if processos[idOriginal].estado == "inativo":
        print(f"Erro: processo P{processos[idOriginal].id+1} inativo.")
        return

    if processos[id].estado == "inativo":
        algoritmoValentao(id+2, processos, mensagens, idOriginal+1)
        return    

    processos[id].coordenador = processos[id]

    i = id + 1
    coord = processos[id].coordenador
    while True:
        if i >= len(processos):
            break

        if processos[i].estado == "ativo":
            mensagens.append(f"ELECTION P{processos[id].id+1} P{processos[i].id+1}")
            mensagens.append(f"OK P{processos[i].id+1} P{processos[id].id+1}")
            coord = processos[i]
        i += 1

    algoritmoValentao(id+2, processos, mensagens, idOriginal+1)


    i = 0
    processos[coord.id].coordenador = coord
    if processos[id].id == processos[id].coordenador.id:
        while True:
            if i >= coord.id:
                break

            if processos[i].estado == "ativo":
                processos[i].coordenador = coord
                mensagens.append(f"COORDINATOR P{coord.id+1} P{processos[i].id+1}")

            i += 1

        print(f"Valentao: coordenador P{processos[id].coordenador.id+1}; mensagens: {len(mensagens)}")



def algoritmoAnel(id, processos, mensagens):
    inicio = id - 1
    ativos = []

    if inicio >= len(processos):
        print(f"Erro: processo P{inicio+1} inexistente.")
        return
    
    if processos[inicio].estado == "inativo":
        print(f"Erro: processo P{processos[inicio].id+1} inativo.")
        return

    for i in range(len(processos)):
        indice = (inicio + i) % len(processos)

        if processos[indice].estado == "ativo":
            ativos.append(indice)

    coord = max(ativos)

    if len(ativos) == 1:
        processos[coord].coordenador = processos[coord]
        print(f"Anel: coordenador P{coord+1}; mensagens: 0")
        return

    for i in range(len(ativos)):
        atual = ativos[i]
        proximo = ativos[(i + 1) % len(ativos)]

        mensagens.append(f"ELECTION P{processos[atual].id+1} P{processos[proximo].id+1}")

    for i in ativos:
        processos[i].coordenador = processos[coord]

    for i in range(len(ativos)):
        atual = ativos[i]
        proximo = ativos[(i + 1) % len(ativos)]

        mensagens.append(f"COORDINATOR P{processos[atual].id+1} P{processos[proximo].id+1}")

    print(f"Anel: coordenador P{coord+1}; mensagens {len(mensagens)}")

def exibirProcessos(processos):
    ativos = []
    inativos = []
    coordenador = None

    if not processos:
        print("Erro: inicialize o sistema com -n N.")
        return

    for p in processos:
        if p.estado == "ativo":
            ativos.append(p)
            coordenador = p.coordenador
        elif p.estado == "inativo":
            inativos.append(p)

    print("Ativos:", end='')
    if not ativos:
        print(" nenhum", end='')
    else:
        for p in ativos:
            print(f' P{p.id+1}', end='')

    print("\nInativos:", end='')
    if not inativos:
        print(" nenhum", end='')
    else:
        for p in inativos:
            print(f' P{p.id+1}', end='')

    if coordenador.estado == "ativo":
        print(f"\nCoordenador: P{coordenador.id+1}")
    else:
        print("\nCoordenador: indefinido")

def exibirMensagens(mensagens):
    if not mensagens:
        print("Nenhuma mensagem registrada.")
        return

    for m in mensagens:
        print(m)


mensagens = []
processos = []
while True:
    escolha = input()

    if escolha == "-f":
        break
    elif escolha[1] == "n":
        processos = []
        inicializarProcessos(int(escolha[3:]), processos)
    elif escolha[1] == "x":
        simularFalha(int(escolha[3:]), processos)
    elif escolha[1] == "v":
        mensagens = []
        algoritmoValentao(int(escolha[3:]), processos, mensagens, int(escolha[3:]))
    elif escolha[1] == "a":
        mensagens = []
        algoritmoAnel(int(escolha[3:]), processos, mensagens)
    elif escolha[1] == "s":
        exibirProcessos(processos)
    elif escolha[1] == "m":
        exibirMensagens(mensagens)