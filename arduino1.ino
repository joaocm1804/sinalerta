# include <Wire.h>
# include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

int pinosLeds[6] = { 2, 4, 6, 8, 10, 12 };   
int pinosBotoes[6] = { 3, 5, 7, 9, 11, 13 };
bool state;

int array[100];
int idx = 0;
int rodada = 2;
bool teste = true;
bool jogo_ativo = false;
bool perdeu_o_jogo2 = false;
int botao_pressionado2 = 0;
int passo_atual_jogo_2 = 0;

bool oponente_perdeu = false;
bool nova_sequencia_recebida = false;
String mensagem_recebida = "";


// Nossa sequência de até 100 itens vai começar vazia.
int sequencia[100] = {};
// Indica a rodada atual que o jogo se encontra.
int rodada_atual = 0;
// Indica o passo atual dentro da sequência, é usado enquanto a sequência
// está sendo reproduzida.
int passo_atual_na_sequencia = 0;


int botao_pressionado = 0;
// Flag indicando se o jogo acabou.
int perdeu_o_jogo = false;


void setup() {
  lcd.init();
  lcd.clear();         
  lcd.backlight();      // Make sure backlight is on
  
  // Print a message on both lines of the LCD.
  //lcd.cursor(column,row) index starts at 0
  //lcd.clear();
  //mySerial.begin(9600);
  Serial.begin(9600);
  
  for (int i = 0; i < 6; i++) {
    pinMode(pinosLeds[i], OUTPUT);
  }

  for (int i = 0; i < 6; i++) {
    pinMode(pinosBotoes[i], INPUT_PULLUP);
  }
  
  randomSeed(analogRead(0));
}

void loop() {
  if (!jogo_ativo) { // Enquanto nenhum jogo estiver ativo
    rodada = 2;
    lcd.setCursor(0,0);   //Set cursor to character 2 on line 0
    lcd.print("Bem-vindo(a) ao");
  
    lcd.setCursor(3,1);   //Move cursor to character 2 on line 1
    lcd.print("sinAlerta");
    if (digitalRead(pinosBotoes[4]) == LOW) {
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("Jogo 1 ativo");
      digitalWrite(pinosLeds[4], HIGH);// Botão 11 pressionado
      delay(200); // Pequeno delay para debounce
      jogo_ativo = true; // Ativa o jogo
      jogo1(); // Chama o jogo 1
      digitalWrite(pinosLeds[4], LOW);
      lcd.clear();
    } 
    if (digitalRead(pinosBotoes[5]) == LOW) { // Botão 13 pressionado
      bool convite = convidar_jogo2();
      if (convite == true) {
        digitalWrite(pinosLeds[5], HIGH);
        delay(200);
        // Reset variables here
        idx = 0;
        rodada = 2;
        passo_atual_jogo_2 = 0;
        perdeu_o_jogo2 = false;
        teste = true;
        oponente_perdeu = false; // Reset oponente_perdeu
        nova_sequencia_recebida = false; // Reset nova_sequencia_recebida
        mensagem_recebida = ""; // Reset mensagem_recebida
        limpar_buffer(); // Clear serial buffer
        jogo_ativo = true;
        jogo2(); // Start jogo2
        digitalWrite(pinosLeds[5], LOW);
      } else {
        digitalWrite(pinosLeds[5], LOW);
        jogo_ativo = false;
      }
    }
  }
}

void printSequencia() {
  Serial.print("Sequência atual: ");
  for (int i = 0; i < idx; i++) {
    Serial.print(array[i]);
    Serial.print(" ");
  }
  Serial.println();
}
                         
String arrayParaString(int* arr, int length) {
  String result = "";
  for (int i = 0; i < length; i++) {
    result += String(arr[i]);  // Append each integer to the result string
  }
  return result;
}

void StringToArray(String input, int* arr) {
  for (int i = 0; i < input.length(); i++) {
    arr[i] = input[i] - '0';  // Convert character to integer
  }
}

void apagarTodosLeds() {
  // Desliga todos os LEDs
  for (int i = 0; i < 4; i++) {
    digitalWrite(pinosLeds[i], LOW);
  }
}

void montar_sequencia(){
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Monte com");
  lcd.setCursor(3,1);
  lcd.print(String(rodada) + " passos");
  idx = 0;

  while (idx < rodada) {
    processar_mensagem();
    if (oponente_perdeu) {
      jogo_ativo = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("O outro jogador");
      lcd.setCursor(0, 1);
      lcd.print("perdeu!");
      delay(2000);
      lcd.clear();
      return;
    }

    for (int i = 0; i < 4; i++) {
      if (digitalRead(pinosBotoes[i]) == LOW) {
        digitalWrite(pinosLeds[i], HIGH);
        array[idx] = i;
        idx++;
        delay(300);
        digitalWrite(pinosLeds[i], LOW);
        while (digitalRead(pinosBotoes[i]) == LOW) {
          delay(10);
          processar_mensagem();
          if (oponente_perdeu) {
            jogo_ativo = false;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("O outro jogador");
            lcd.setCursor(0, 1);
            lcd.print("perdeu!");
            delay(2000);
            lcd.clear();
            return;
          }
        }
      }
    }
    delay(10);
  }
  
  String mensagem = arrayParaString(array, idx);
  Serial.println(mensagem);
  idx = 0;
  apagarTodosLeds();
}



bool receber_mensagem(){
  while (!nova_sequencia_recebida) {
    processar_mensagem();
    if (oponente_perdeu) {
      jogo_ativo = false;
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Voce Ganhou!");
      //lcd.setCursor(0, 1);
      //lcd.print("perdeu!");
      delay(2000);
      lcd.clear();
      return false;
    }
    delay(10);
  }

  StringToArray(mensagem_recebida, array);
  mensagem_recebida = "";
  nova_sequencia_recebida = false;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Recebendo");
  lcd.setCursor(0,1);
  lcd.print("Sequencia");

  for (int i = 0; i <= rodada; i++){ 
    digitalWrite(pinosLeds[array[i]], HIGH);
    delay(700);
    digitalWrite(pinosLeds[array[i]], LOW);
    delay(500);
  }
  lcd.clear();
  lcd.setCursor(5,0);
  lcd.print("Tente");
  lcd.setCursor(4,1);
  lcd.print("Acertar");
  return true;
}






void aguardarJogador2() {
  for (int i = 0; i <= rodada; i++) {
    aguardarJogada2();

    if (array[passo_atual_jogo_2] != botao_pressionado2) {
      perdeu_o_jogo2 = true;
      return;
    }

    passo_atual_jogo_2++;
  }

  passo_atual_jogo_2 = 0;
}

void aguardarJogada2() {
  bool jogada_efetuada2 = false;
  while (!jogada_efetuada2 && jogo_ativo) {
    processar_mensagem();
    if (!jogo_ativo) {
      return;
    }
    for (int i = 0; i < 4; i++) { 
      if (digitalRead(pinosBotoes[i]) == LOW) {
        botao_pressionado2 = i;

        digitalWrite(pinosLeds[i], HIGH);
        delay(300);
        digitalWrite(pinosLeds[i], LOW);

        jogada_efetuada2 = true;

        while (digitalRead(pinosBotoes[i]) == LOW) {
          delay(10);
          processar_mensagem();
          if (!jogo_ativo) {
            return;
          }
        }
        break;
      }
    }
    delay(10);
  }
}



void gameOver2() {
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("PERDEU!");
  for (int i = 0; i <= 3; i++) {
    digitalWrite(pinosLeds[i], HIGH);
    delay(200);
    digitalWrite(pinosLeds[i], LOW);
  }

  for (int i = 0; i <= 3; i++) {
    digitalWrite(pinosLeds[0], HIGH);
    digitalWrite(pinosLeds[1], HIGH);
    digitalWrite(pinosLeds[2], HIGH);
    digitalWrite(pinosLeds[3], HIGH);
    delay(100);
    digitalWrite(pinosLeds[0], LOW);
    digitalWrite(pinosLeds[1], LOW);
    digitalWrite(pinosLeds[2], LOW);
    digitalWrite(pinosLeds[3], LOW);
    delay(100);
  }

  perdeu_o_jogo2 = true;
}

void leds_acertou(){
  for (int z = 0; z <4; z++){
    digitalWrite(pinosLeds[z], HIGH);
  }
  delay(1000);
  for (int z = 0; z <4; z++){
    digitalWrite(pinosLeds[z], LOW);
  }
}

void jogo2(){
  limpar_buffer();
  passo_atual_jogo_2 = 0;
  while (jogo_ativo){
    processar_mensagem();
    if (oponente_perdeu) {
      jogo_ativo = false;
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("Voce ganhou!");
      //lcd.setCursor(0, 1);
      //lcd.print("perdeu!");
      delay(2000);
      lcd.clear();
      resetarVariaveis();
      break;
    }

    if (teste){
      montar_sequencia();
      if (!jogo_ativo) {
        // O jogo foi encerrado durante montar_sequencia()
        break;
      }
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("Vez do");
      lcd.setCursor(3,1);
      lcd.print("oponente");
      teste = false;
    } else {
      bool mensagem_recebida = receber_mensagem();
      if (!jogo_ativo) {
        // O jogo foi encerrado durante receber_mensagem()
        break;
      }
      if (mensagem_recebida) {
        aguardarJogador2();
        if(!perdeu_o_jogo2){
          leds_acertou();
          rodada++;
          teste = true;
        } else {
          gameOver2();
          Serial.println("PERDEU");
          resetarVariaveis();
        }
      } else {
        gameOver2();
        jogo_ativo = false;
        teste = true;
      }
    }
  }
}




void proximaRodada() {
  int numero_sorteado = random(0, 4); // Sorteia entre 0 e 3
  sequencia[rodada_atual++] = numero_sorteado;
}

void reproduzirSequencia() {
  //Serial.print("Sequência atual: ");
  for (int i = 0; i < rodada_atual; i++) {
    //Serial.print(sequencia[i]);
    //Serial.print(" ");
    digitalWrite(pinosLeds[sequencia[i]], HIGH);
    delay(500);
    digitalWrite(pinosLeds[sequencia[i]], LOW);
    delay(100);
  }
  //Serial.println();
}

void aguardarJogador() {
  for (int i = 0; i < rodada_atual; i++) {
    aguardarJogada();

    if (sequencia[passo_atual_na_sequencia] != botao_pressionado) {
      gameOver(); 
      return;
    }

    passo_atual_na_sequencia++;
  }

  passo_atual_na_sequencia = 0;
}

void aguardarJogada() {
  bool jogada_efetuada = false;
  while (!jogada_efetuada) {
    for (int i = 0; i <= 3; i++) { 
      if (digitalRead(pinosBotoes[i]) == LOW) {
        botao_pressionado = i;

        digitalWrite(pinosLeds[i], HIGH);
        delay(300);
        digitalWrite(pinosLeds[i], LOW);

        jogada_efetuada = true;
      }
    }

    if (digitalRead(pinosBotoes[4]) == LOW) {
      perdeu_o_jogo = true;
      return;
    }

    delay(10);
  }
}

void gameOver() {
  for (int i = 0; i <= 3; i++) {
    digitalWrite(pinosLeds[i], HIGH);
    delay(200);
    digitalWrite(pinosLeds[i], LOW);
  }

  for (int i = 0; i <= 3; i++) {
    digitalWrite(pinosLeds[0], HIGH);
    digitalWrite(pinosLeds[1], HIGH);
    digitalWrite(pinosLeds[2], HIGH);
    digitalWrite(pinosLeds[3], HIGH);
    delay(100);
    digitalWrite(pinosLeds[0], LOW);
    digitalWrite(pinosLeds[1], LOW);
    digitalWrite(pinosLeds[2], LOW);
    digitalWrite(pinosLeds[3], LOW);
    delay(100);
  }

  perdeu_o_jogo = true;
}

void tocarInicio() {
  for (int i = 0; i <= 3; i++) {
    digitalWrite(pinosLeds[i], HIGH);
  }
  delay(500);
  for (int i = 0; i <= 3; i++) {
    digitalWrite(pinosLeds[i], LOW);
  }
  delay(500);
}

void jogo1() {
  resetarJogo();
  tocarInicio();
  delay(500);

  while (!perdeu_o_jogo) { // Continua enquanto o jogador não perder
    proximaRodada();       // Adiciona um novo passo à sequência
    reproduzirSequencia(); // Reproduz a sequência até o momento
    aguardarJogador();     // Aguarda o jogador reproduzir a sequência

    if (!perdeu_o_jogo) {  // Se o jogador acertar, continua para a próxima rodada
      delay(1000);         // Pequena pausa antes da próxima rodada
    }
  }

  gameOver(); // Executa a sequência de "fim de jogo" caso o jogador erre
  jogo_ativo = false;
}

void resetarJogo() {
  for (int i = 0; i < 100; i++) {
    sequencia[i] = 0;
  }
  rodada_atual = 0;
  passo_atual_na_sequencia = 0;
  perdeu_o_jogo = false;
}

void piscarLed(int idx_led){
  digitalWrite(pinosLeds[idx_led], HIGH);
  delay(500);
  digitalWrite(pinosLeds[idx_led], LOW);// Para parar pelo botão, deve pressionar.
  delay(450);
}

bool convidar_jogo2(){
  limpar_buffer();
  Serial.write('p'); // p = PERGUNTA
  char resposta = 'm';
  lcd.clear();
  unsigned long startTime = millis();
  while (millis() - startTime <= 5000 and resposta != 'a' and resposta != 'n'){
    piscarLed(5);
    lcd.setCursor(4,0);
    lcd.print("Convite");
    lcd.setCursor(4,1);
    lcd.print("Enviado");
    
    if (Serial.available() > 0) {
      resposta = Serial.read(); // Lê a resposta se houver algo disponível
    }
    
  }
  
  digitalWrite(pinosLeds[5], LOW);
  if (resposta == 'a'){
    return true;
  } else{
    return false;
  }
  lcd.clear();
      
}

void limpar_buffer(){
  while (Serial.available() > 0){
    Serial.read();
  }
}

bool checkForPerdeu() {
  if (Serial.available() > 0) {
    String mensagem = Serial.readStringUntil('\n');
    if (mensagem == "PERDEU") {
      // O outro jogador perdeu, fim do jogo
      jogo_ativo = false;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("O outro jogador");
      lcd.setCursor(0, 1);
      lcd.print("perdeu!");
      delay(1000); // Aguarda para o usuário ler a mensagem
      return true;
    }
  }
  return false;
}

void processar_mensagem() {
  if (Serial.available() > 0) {
    String mensagem = Serial.readStringUntil('\n');
    mensagem.trim();
    if (!jogo_ativo) {
      // If the game is not active, ignore any incoming messages
      return;
    }
    if (mensagem == "PERDEU") {
      oponente_perdeu = true;
    } else {
      mensagem_recebida = mensagem;
      nova_sequencia_recebida = true;
    }
  }
}


void resetarVariaveis() {
  jogo_ativo = false;
  perdeu_o_jogo2 = false;
  teste = true;
  rodada = 2;
  idx = 0;
  passo_atual_jogo_2 = 0;
  nova_sequencia_recebida = false;
  mensagem_recebida = "";
  oponente_perdeu = false;
  limpar_buffer();
  lcd.clear();
}

