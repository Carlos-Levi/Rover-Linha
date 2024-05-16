#include <IRremote.h> // Biblioteca para controle remoto IR

#define SENSOR_ESQUERDA A0 // Pino analógico para o sensor esquerdo
#define SENSOR_DIREITA A1 // Pino analógico para o sensor direito
#define SENSOR_OBSTACULO A2 // Pino analógico para o sensor de obstáculo
#define MOTOR_ESQUERDO 10 // Pino PWM para o motor esquerdo
#define MOTOR_DIREITO 11 // Pino PWM para o motor direito
#define LED_VERMELHO 12 // Pino para o LED vermelho
#define LED_VERDE 13 // Pino para o LED verde

IRrecv receiver(2); // Pino para o receptor IR
decode_results results; // Variável para armazenar os resultados do receptor IR

int leituraMinimaEsquerda = 1023; // Valor mínimo lido pelo sensor esquerdo durante a calibração
int leituraMinimaDireita = 1023; // Valor mínimo lido pelo sensor direito durante a calibração
int leituraMaximaEsquerda = 0; // Valor máximo lido pelo sensor esquerdo durante a calibração
int leituraMaximaDireita = 0; // Valor máximo lido pelo sensor direito durante a calibração
int thresholdEsquerda; // Limiar de leitura do sensor esquerdo
int thresholdDireita; // Limiar de leitura do sensor direito
int thresholdObstaculo = 500; // Limiar de leitura do sensor de obstáculo
int velocidadeMaxima = 255; // Velocidade máxima dos motores
int velocidadeCurva = 150; // Velocidade dos motores ao fazer curvas
int pausa = 500; // Tempo de pausa ao encontrar uma interrupção na linha

void setup() {
  pinMode(SENSOR_ESQUERDA, INPUT);
  pinMode(SENSOR_DIREITA, INPUT);
  pinMode(SENSOR_OBSTACULO, INPUT);
  pinMode(MOTOR_ESQUERDO, OUTPUT);
  pinMode(MOTOR_DIREITO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);

  receiver.enableIRIn(); // Inicializar o receptor IR

  calibrarSensores(); // Realizar calibração automática dos sensores
}

void loop() {
  int leituraEsquerda = analogRead(SENSOR_ESQUERDA);
  int leituraDireita = analogRead(SENSOR_DIREITA);
  int leituraObstaculo = analogRead(SENSOR_OBSTACULO);

  // Detecção de obstáculos
  if (leituraObstaculo < thresholdObstaculo) {
    evitarObstaculo();
  } else {
    // Controle remoto
    if (receiver.decode(&results)) {
      interpretarComandoRemoto();
      receiver.resume(); // Reiniciar o receptor IR para o próximo comando
    } else {
      // Seguindo a linha
      if (leituraEsquerda < thresholdEsquerda && leituraDireita < thresholdDireita) { // Ambos os sensores na linha
        moverParaFrente(velocidadeMaxima);
      } else if (leituraEsquerda < thresholdEsquerda) { // Somente o sensor esquerdo na linha
        fazerCurvaDireita();
      } else if (leituraDireita < thresholdDireita) { // Somente o sensor direito na linha
        fazerCurvaEsquerda();
      } else { // Ambos os sensores fora da linha
        pararPorPausa();
      }
    }
  }
}

void calibrarSensores() {
  // Realiza várias leituras dos sensores para determinar os valores mínimos e máximos
  for (int i = 0; i < 100; i++) {
    int leituraEsquerda = analogRead(SENSOR_ESQUERDA);
    int leituraDireita = analogRead(SENSOR_DIREITA);

    if (leituraEsquerda < leituraMinimaEsquerda) {
      leituraMinimaEsquerda = leituraEsquerda;
    }
    if (leituraEsquerda > leituraMaximaEsquerda) {
      leituraMaximaEsquerda = leituraEsquerda;
    }

    if (leituraDireita < leituraMinimaDireita) {
      leituraMinimaDireita = leituraDireita;
    }
    if (leituraDireita > leituraMaximaDireita) {
      leituraMaximaDireita = leituraDireita;
    }

    delay(10);
  }

  // Define os limiares de leitura baseados nos valores mínimos e máximos
  thresholdEsquerda = (leituraMaximaEsquerda + leituraMinimaEsquerda) / 2;
  thresholdDireita = (leituraMaximaDireita + leituraMinimaDireita) / 2;
}

void moverParaFrente(int velocidade) {
  digitalWrite(MOTOR_ESQUERDO, HIGH);
  digitalWrite(MOTOR_DIREITO, HIGH);
  analogWrite(MOTOR_ESQUERDO, velocidade);
  analogWrite(MOTOR_DIREITO, velocidade);
}

void fazerCurvaEsquerda() {
  digitalWrite(MOTOR_ESQUERDO, HIGH);
  digitalWrite(MOTOR_DIREITO, HIGH);
  analogWrite(MOTOR_ESQUERDO, velocidadeCurva);
  analogWrite(MOTOR_DIREITO, velocidadeMaxima);
}

void fazerCurvaDireita() {
  digitalWrite(MOTOR_ESQUERDO, HIGH);
  digitalWrite(MOTOR_DIREITO, HIGH);
  analogWrite(MOTOR_ESQUERDO, velocidadeMaxima);
  analogWrite(MOTOR_DIREITO, velocidadeCurva);
}

void pararPorPausa() {
  digitalWrite(MOTOR_ESQUERDO, LOW);
  digitalWrite(MOTOR_DIREITO, LOW);
  delay(pausa);
}

void evitarObstaculo() {
  digitalWrite(MOTOR_ESQUERDO, HIGH);
  digitalWrite(MOTOR_DIREITO, HIGH);
  analogWrite(MOTOR_ESQUERDO, velocidadeCurva);
  analogWrite(MOTOR_DIREITO, velocidadeCurva);
  delay(1000); // Girar por 1 segundo
  digitalWrite(MOTOR_ESQUERDO, HIGH);
  digitalWrite(MOTOR_DIREITO, LOW);
  delay(1000); // Girar para a direita por 1 segundo
}

void interpretarComandoRemoto() {
  switch(results.value) {
    case 0xFFA25D: // Botão 1 pressionado
      // Implementar ação desejada para o botão 1
      break;
    case 0xFF629D: // Botão 2 pressionado
      // Implementar ação desejada para o botão 2
      break;
    // Adicionar outros casos para os botões do controle remoto
  }
}

