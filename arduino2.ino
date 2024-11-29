// LCD
# include <Wire.h>
# include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);
////////////////////////////////////////////////////////////

int pinosLeds[6] = { 2, 4, 6, 8, 10, 12 };   
int pinosBotoes[6] = { 3, 5, 7, 9, 11, 13 };
bool state;

int idx = 0;
int rodada = 2;
bool teste = true;
int botao_pressionado2 = 0;
int passo_atual_jogo_2 = 0;
bool acertou =false;
bool jogo_ativo = false;
bool negou_w = false;
bool teste2 = true;
int array[100];
bool oponente_perdeu = false;
bool nova_sequencia_recebida = false;
String mensagem_recebida = "";



bool perdeu_o_jogo2 = false;


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
  lcd.backlight();     
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
  // LCD
  lcd.setCursor(0,0);   
  lcd.print("Bem-vindo(a) ao");
  lcd.setCursor(3,1);   
  lcd.print("sinAlerta"); 
  /////////////////////////////////////////////////////////////////////////////// 
    if (digitalRead(pinosBotoes[4]) == LOW && negou_w == false) {
      digitalWrite(pinosLeds[4], HIGH);// Botão 11 pressionado
      delay(200); // Pequeno delay para debounce
      // LCD
      lcd.clear();         
      lcd.setCursor(2,0);  
      lcd.print("Inicia jogo!");
      lcd.setCursor(4,1);  
      lcd.print("1 Player");
      //////////////////////////////////////////////////////
      jogo_ativo = true; // Ativa o jogo
      jogo1(); // Chama o jogo 1
      //digitalWrite(pinosLeds[5], LOW);
    } else{
      bool resposta = aceitar_recusar_jogo2();
      if (resposta  == true){
        rodada = 2;
        jogo_ativo = true;
        digitalWrite(pinosLeds[5], HIGH);
        // LCD
        lcd.clear();         
        lcd.setCursor(2,0);  
        lcd.print("Inicia jogo!");
        lcd.setCursor(3,1);  
        lcd.print("2 Players");
        ///////////////////////////////////////////////////////////////
        jogo2();
      } else{
        digitalWrite(pinosLeds[5], LOW);
        delay(1000);
        negou_w = false;
      }
    }
    //if (digitalRead(pinosBotoes[5]) == LOW) {// Botão 13 pressionado
     // digitalWrite(pinosLeds[5], HIGH);
     // delay(200); // Pequeno delay para debounce
     // jogo_ativo = true; // Ativa o jogo
     // jogo2(); // Chama o jogo 2
   // }
  }
}

void printSequencia() {
  Serial.print("Sequência atual: ");
  for (int j = 0; j < idx; j++) {
    Serial.print(array[j]);
    Serial.print(" ");
  }
  Serial.println();
}

String arrayParaString(int* arr, int length) {
  String result = "";
  for (int i = 0; i < length; i++) {
    result += String(arr[i]);
  }
  return result;
}

void StringToArray(String input, int* arr) {
  for (int i = 0; i < input.length(); i++) {
    arr[i] = input[i] - '0';
  }
}

void aguardarJogador2() {
  for (int i = 0; i < rodada; i++) {
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
  while (!jogada_efetuada2) {
    processar_mensagem();
    if (oponente_perdeu) {
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
        }
        break;
      }
    }
    delay(10);
  }
}



void gameOver2() {
  lcd.clear();
  lcd.setCursor(5,0);
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

void montar_sequencia(){
  idx = 0;
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Monte com");
  lcd.setCursor(3,1);
  lcd.print(String(rodada) + " passos");
  while (idx < rodada) {
    for (int i = 0; i < 4; i++) {
      if (digitalRead(pinosBotoes[i]) == LOW) {
        digitalWrite(pinosLeds[i], HIGH);

        array[idx] = i;
        idx++;
        delay(300);
        digitalWrite(pinosLeds[i], LOW);

        // Aguarda o botão ser liberado
        while (digitalRead(pinosBotoes[i]) == LOW) {
          delay(10);
        }
      }
    }
    delay(10);
  }

  // Após a sequência ser montada, envia pelo Serial
  String mensagem = arrayParaString(array, idx);
  Serial.println(mensagem);
  idx = 0;
  apagarTodosLeds();
}

void apagarTodosLeds() {
  // Desliga todos os LEDs
  for (int i = 0; i < 4; i++) {
    digitalWrite(pinosLeds[i], LOW);
  }
}

void receber_mensagem() {
  if (nova_sequencia_recebida) {
    StringToArray(mensagem_recebida, array);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Recebendo");
    lcd.setCursor(0, 1);
    lcd.print("Sequencia");
    for (int i = 0; i < rodada; i++) {
      digitalWrite(pinosLeds[array[i]], HIGH);
      delay(700);
      digitalWrite(pinosLeds[array[i]], LOW);
      delay(500);
    }
    teste = false;
    nova_sequencia_recebida = false;
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("Tente");
    lcd.setCursor(4, 1);
    lcd.print("Acertar");
  }
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

void jogo2() {
  teste2 = true;
  while (teste2) {
    processar_mensagem();
    if (oponente_perdeu) {
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print("Voce ganhou!");
      delay(2000);
      resetarVariaveis();
      break;
    }
    if (teste) {
      receber_mensagem();
      processar_mensagem(); // Verifica novas mensagens
    } else {
      aguardarJogador2();
      processar_mensagem();
      if (oponente_perdeu) {
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Voce ganhou!");
        delay(2000);
        resetarVariaveis();
        break;
      }
      if (!perdeu_o_jogo2) {
        leds_acertou();
        rodada++;
        montar_sequencia();
        lcd.clear();
        lcd.setCursor(4, 0);
        lcd.print("Vez do");
        lcd.setCursor(3, 1);
        lcd.print("oponente");
        perdeu_o_jogo2 = false;
        teste = true;
      } else {
        gameOver2();
        Serial.println("PERDEU");
        resetarVariaveis();
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
  lcd.clear();
  lcd.setCursor(5,0);
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
  digitalWrite(pinosLeds[4], LOW);
  negou_w = false;
  teste = true;
  teste2 = true;
  rodada = 2;
  perdeu_o_jogo2 = false;
  idx = 0;
  passo_atual_jogo_2 = 0;
  
  limpar_buffer();
  lcd.clear();
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

bool aceitar_recusar_jogo2(){
  //limpar_buffer();
  if (Serial.available() > 0){
    char convite = Serial.read();
    if (convite == 'p'){
      lcd.clear();
      lcd.setCursor(3,0);
      lcd.print("Convite");
      lcd.setCursor(3,1);
      lcd.print("Recebido");
      negou_w = true;
      bool aceitou = false;
      bool negou = false;
      // ... inside your function where Arduino 2 accepts the game
      oponente_perdeu = false;
      nova_sequencia_recebida = false;
      mensagem_recebida = "";
      limpar_buffer();

      unsigned long startTime = millis();
      while (aceitou == false && negou == false && millis() - startTime <= 5000){
        piscarLed(5);
        if (digitalRead(pinosBotoes[5]) == LOW){
          aceitou = true;
        }
        if (digitalRead(pinosBotoes[4]) == LOW){
          negou = true;
        }
      }
      digitalWrite(pinosLeds[5], LOW);
      if (aceitou == true){
        Serial.write('a');
        return true;
      } else{
        Serial.write('n');
        return false;
      }
    }
  }
  return false;
}

void limpar_buffer(){
  while (Serial.available() > 0){
    Serial.read();
  }
}

void processar_mensagem() {
  if (Serial.available() > 0) {
    String mensagem = Serial.readStringUntil('\n');
    mensagem.trim();
    if (mensagem == "PERDEU") {
      oponente_perdeu = true;
    } else {
      // Assume que é uma sequência
      mensagem_recebida = mensagem;
      nova_sequencia_recebida = true;
    }
  }
}

void resetarVariaveis() {
  jogo_ativo = false;
  teste2 = false;
  oponente_perdeu = false;
  perdeu_o_jogo2 = false;
  teste = true;
  rodada = 2;
  idx = 0;
  passo_atual_jogo_2 = 0;
  nova_sequencia_recebida = false;
  mensagem_recebida = "";
  limpar_buffer();
  lcd.clear();
}




