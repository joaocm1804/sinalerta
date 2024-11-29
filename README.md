# 🟦🟧sinAlerta🟨🟪:
## Descrição

**sinAlerta** é um jogo de memória, desenvolvido por estudantes da CESAR School como parte de um projeto acadêmico. Utilizando a plataforma **Arduino**, o sinAlerta apresenta uma solução **criativa** e **interativa** voltada para a conscientização no **trânsito**, alinhada ao conceito de **Playable Cities**, tema principal do desenvolvimento da Solução. O projeto busca abordar um problema recorrente nas cidades: a falta de atenção de pedestres que atravessam fora da faixa ou no momento errado, contribuindo para a ocorrência de graves acidentes. Com isso, o sinAlerta oferece uma abordagem lúdica para sensibilizar e engajar as pessoas em práticas mais seguras no trânsito.

---

## 🛠 Conceito

O conceito de **Playable Cities** busca transformar ambientes urbanos em espaços interativos, conectados e acessíveis. A proposta é criar uma relação mais divertida e engajada entre as pessoas e a cidade, incentivando o uso seguro e consciente dos espaços públicos.

Com o **sinAlerta**, o objetivo é:
- **Conscientizar pedestres** sobre a importância de atravessar na faixa.
- **Motivar o respeito ao tempo do semáforo**, transformando o tempo de espera em uma experiência lúdica.
- **Promover mobilidade urbana segura** por meio de uma abordagem interativa.

---

## 🎮 Como Funciona

1. O **sinAlerta** é projetado para ser integrado a postes de semáforo, oferecendo entretenimento e conscientização enquanto os pedestres aguardam o sinal verde.
2. Existem dois modos de jogo disponíveis:
   - **Modo Individual**:
     - Uma sequência de LEDs é gerada aleatoriamente.
     - O jogador deve reproduzir a sequência correta pressionando os botões correspondentes.
     - A cada acerto, uma nova sequência é gerada, aumentando a dificuldade.
     - O jogo continua até que o jogador erre.
   - **Modo Versus**:
     - Um jogador inicia o jogo e convida outro através de um segundo Arduino.
     - O convite pode ser aceito (pressionando o botão de 2 jogadores) ou recusado (pressionando o botão de 1 jogador).
     - No jogo:
       - O jogador que iniciou cria uma sequência de 2 passos.
       - O segundo jogador tenta reproduzir a sequência corretamente.
       - Se acertar, ele cria uma nova sequência com um passo a mais e envia para o jogador 1.
       - O jogo alterna entre os jogadores, aumentando a dificuldade, até que alguém erre.

---

## 🔧 Componentes Utilizados

- 2x Placa Arduino Mega
- 2x LCDs 16x2 I2C
- 12x LEDs (versão básica) / 52x LEDs (versão estética)
- 12x Botões
- 2x Protoboard
- Resistores de 220Ω
- Jumpers

---

##Imagens do protótipo (TinkerCad)


##Imagens do protótipo (Real)

---

## 🚀 Como Configurar

### Pré-requisitos
- Arduino IDE instalado no seu computador.
- Biblioteca <wire.h> e <LiquidCrystal_I2C.h>
- Drivers USB para comunicação com o Arduino.

### Passo a Passo
1. Clone este repositório:
   ```bash
   git clone https://github.com/joaocm1804/sinalerta.git



