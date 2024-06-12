#include <Keypad.h>
#include <Wire.h>  // Biblioteca para Comunicação I2C
#include <Adafruit_LiquidCrystal.h>

using namespace std;
// Construtor para comunicação I2C (endereço 0x20 no caso)
Adafruit_LiquidCrystal lcd(0);

enum Estado {
  q0,
  q1,
  q2,
  q3,
  q4,
  qaceita
};

Estado estadoAtual = q0;
Estado estadoAtualPrint;

const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns
char keys[ROWS][COLS] = {
  {'1','2', '3', 'A'},
  {'4','5', '6', 'B'},
  {'7','8', '9', 'C'},
  {'*','0', '#', 'D'}  
};
byte rowPins[ROWS] = {11, 10, 9, 8}; // connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6, 5, 4}; // connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

char palavra[16];
int index = 0;

void setup() {
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("   Maquina de");
  lcd.setCursor(0, 1);
  lcd.print("     Turing");
  delay(2000);
  lcd.clear();
  lcd.print("L=(1|2)* nmr de");
  lcd.setCursor(0, 1);
  lcd.print("1's e 2's iguais");
  delay(3000);
  lcd.clear();
  Serial.begin(9600);
  
  // Ativar pull-up internos para as colunas
  for (byte i = 0; i < COLS; i++) {
    pinMode(colPins[i], INPUT_PULLUP);
  }
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Digite a palavra");
  lcd.setCursor(0, 1);
  delay(10);
  lcd.print(palavra); // Display current input

  char key = keypad.getKey();
  if (key && (key =='1' || key == '2' || key == '5')) {
    if (key == '5') {
      palavra[index] = '\0';
      processarPalavra();
      index = 0; // Reset index for the next word
      memset(palavra, 0, sizeof(palavra)); // Clear the palavra array
      lcd.clear();
    } else {
      if (index < 16) { // Ensure we don't overflow the array
        palavra[index] = key;
        index++;
        lcd.setCursor(0, 1);
        lcd.print(palavra); // Update display with current input
      }
    }
  }
}

void processarPalavra() {
  if (strlen(palavra) == 0) { // Verifica se a palavra está vazia
    lcd.clear();
    lcd.print("Fita: vazia");
    delay(2000);
  }else{
  	lcd.clear();
    lcd.print("Fita: ");
    lcd.print(palavra);
    delay(2000);  // Delay to show the word on the LCD
  }
  estadoAtual = q0; // Reiniciar o estado
  int cabecote = 0;
  lcd.clear();
  char lado;
  while (estadoAtual != qaceita && cabecote >= 0 && cabecote < strlen(palavra)) {
    char simboloLido = palavra[cabecote];
    delay(1000);
    
    lcd.setCursor(0, 0);
    lcd.print("Estado: q");
    lcd.print((int)estadoAtual); // Exibe o estado como número (q0, q1, etc.)
    lcd.setCursor(0, 1);
    lcd.print("Fita: ");
    for (int i = 0; i < strlen(palavra); i++) {
      if (i == cabecote) {
        lcd.print("[");  // Delimita o cabeçote
        lcd.print(palavra[i]);
        lcd.print("]");
      } else {
        lcd.print(palavra[i]);
      }
    }
    
    switch (estadoAtual) {
      case q0:
        estadoAtualPrint = q0;
        if (simboloLido == '1') {
          estadoAtualPrint = q0;
          estadoAtual = q1;
          palavra[cabecote] = 'X';
          cabecote++;
          lado = 'R';
        } else if (simboloLido == '2') {
          estadoAtualPrint = q0;
          estadoAtual = q4;
          palavra[cabecote] = 'Y';
          cabecote++;
          lado = 'R';
        } else if (simboloLido == 'X' || simboloLido == 'Y') {
          estadoAtualPrint = q0;
          cabecote++;
          lado = 'R';
        } else {
          lcd.clear();
          lcd.print("Palavra rejeitada:");
          lcd.setCursor(0, 1);
          lcd.print(palavra);
          delay(3000);  // Delay to show the rejection message
          return;
        }
        break;
      case q1:
        estadoAtualPrint = q1;
        if (simboloLido == '1' || simboloLido == 'Y') {
          cabecote++;
          lado = 'R';
        } else if (simboloLido == '2') {
          estadoAtualPrint = q1;  
          estadoAtual = q2;
          palavra[cabecote] = 'Y';
          cabecote--;
          lado = 'L';
        } else {
          lcd.clear();
          lcd.print("Palavra rejeitada:");
          lcd.setCursor(0, 1);
          lcd.print(palavra);
          delay(3000);  // Delay to show the rejection message
          return;
        }
        break;
      case q2:
        estadoAtualPrint = q2;
        if (simboloLido == '1' || simboloLido == 'Y') {
          cabecote--;
          lado = 'L';
        } else if (simboloLido == 'X') {
          estadoAtualPrint = q2;  
          estadoAtual = q0;
          cabecote++;
          lado = 'R';
        } else {
          lcd.clear();
          lcd.print("Palavra rejeitada:");
          lcd.setCursor(0, 1);
          lcd.print(palavra);
          delay(3000);  // Delay to show the rejection message
          return;
        }
        break;
      case q3:
        estadoAtualPrint = q3;
        if (simboloLido == '2' || simboloLido == 'X') {
          cabecote--;
          lado = 'L';
        } else if (simboloLido == 'Y') {
          estadoAtualPrint = q3;
          estadoAtual = q0;
          cabecote++;
          lado = 'R';
        } else {
          lcd.clear();
          lcd.print("Palavra rejeitada:");
          lcd.setCursor(0, 1);
          lcd.print(palavra);
          delay(3000);  // Delay to show the rejection message
          return;
        }
        break;
      case q4:
        estadoAtualPrint = q4;
        if (simboloLido == '1') {
          estadoAtualPrint = q4;
          estadoAtual = q3;
          palavra[cabecote] = 'X';
          cabecote--;
          lado = 'L';
        } else if (simboloLido == '2' || simboloLido == 'X') {
          cabecote++;
          lado = 'R';
        } else {
          lcd.clear();
          lcd.print("Palavra rejeitada:");
          lcd.setCursor(0, 1);
          lcd.print(palavra);
          delay(3000);  // Delay to show the rejection message
          return;
        }
        break;
        default:
        estadoAtual = q0;
        break;
        
    }//fim switch

    Serial.print("\n\n(e,i,i',e',s)");
    Serial.print("\n\n onde:");
    Serial.print("\n\n e - Estado atual: ");
    Serial.print(estadoAtualPrint);
    Serial.print("\n i - Simbolo lido: ");
    Serial.print(simboloLido);
    Serial.print("\n i' - Simbolo a imprimir: ");
    Serial.print(palavra[cabecote]);
    Serial.print("\n e' - Proximo estado: ");
    Serial.print(estadoAtual);
    Serial.print("\n s - Sentido Movimento: ");
    Serial.print(lado);

  }//fim while

  if (estadoAtual == q0 && cabecote == strlen(palavra)) {
    lcd.clear();
    if(strlen(palavra) == 0){
      lcd.print("Palavra aceita:");
      lcd.setCursor(0, 1);
      lcd.print("Vazia");
      delay(3000);
    }else{
      lcd.print("Palavra aceita:");
      lcd.setCursor(0, 1);
      lcd.print(palavra);
      delay(3000);  // Delay to show the acceptance message
    }    
  }else{
    lcd.clear();
    lcd.print("Palavra rejeitada:");
    lcd.setCursor(0, 1);
    lcd.print(palavra);
    delay(3000);  // Delay to show the rejection message
  }
  lcd.clear();
  lcd.print("Outra palavra?");
  lcd.setCursor(0, 1);
  lcd.print("1-Sim  2-Nao");

  char opcao;
  do {
    opcao = keypad.getKey();
  } while (opcao != '1' && opcao != '2'); // Aguarda até que 1 ou 2 seja pressionado

  if (opcao == '1') {
    // Reinicia para uma nova palavra
    index = 0;
    memset(palavra, 0, sizeof(palavra));
    lcd.clear();
  } else {
    lcd.clear();
    lcd.print("Fim do programa!");
    while (true) {
    }
  }
}
