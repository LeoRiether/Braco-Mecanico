/*
 
  ____                        __  __           //\        _           
 |  _ \                      |  \/  |         |/ \|      (_)          
 | |_) |_ __ __ _  ___ ___   | \  / | ___  ___ __ _ _ __  _  ___ ___  
 |  _ <| '__/ _` |/ __/ _ \  | |\/| |/ _ \/ __/ _` | '_ \| |/ __/ _ \ 
 | |_) | | | (_| | (_| (_) | | |  | |  __/ (_| (_| | | | | | (_| (_) |
 |____/|_|  \__,_|\___\___/  |_|  |_|\___|\___\__,_|_| |_|_|\___\___/ 
                   )_)                                                
                                                                      
*/

#define besquerdo 9
#define bdireito 8
#define confirma 7

#define H1 10, 0
#define A1 10, 1
#define H2 3, 0
#define A2 3, 1

#define T 6
#define prec 5.68888888
#define mmax 69

#define ISDEBUG

#ifdef ISDEBUG
#define NBLINK(n) _nblink(n); 
#define DELAY(s) delay(s);
#else
#define NBLINK(n)
#define DELAY(s)
#endif

typedef unsigned long ui32;

int i = 0, j = 0, i1 = 0, i2 = 0;
int ii[2];
float mabs = 0, nabs = 0, mm, nn, tempo, t1, t2;
float m, n; //deve ler um ângulo de 4 bytes
int link1 = 0;
int link2 = 0;
int zerado = 0;
int fase, inverte, velocidade;

void setup() {
  Serial.begin(9600);

  pinMode(besquerdo, INPUT); //botão esquerdo
  pinMode(bdireito, INPUT);  //Botão direito
  pinMode(confirma, INPUT);  //Confirma

  pinMode(13, OUTPUT); // A04
  pinMode(12, OUTPUT); // A03
  pinMode(11, OUTPUT); // A02
  pinMode(10, OUTPUT); // A01

  pinMode(6, OUTPUT); // B04
  pinMode(5, OUTPUT); // B03
  pinMode(4, OUTPUT); // B02
  pinMode(3, OUTPUT); // B01

  ii[0] = 0;
  ii[1] = 0;
}

void loop() {
  //Zeramento Manual(possível automatização do processo com infravermelho
  //zeramento();

  recebeInt(&fase);

  switch (fase) {
  case 0:
    fase0();
    break;
  case 1:
    fase1();
    break;
  case 2:
    fase2();
    break;
  case 3:
    fase3();
    break;
  default:
    NBLINK(5)
    break;
  }
}

////////////////////////////////////// FASE 0 /////////////////////////////
void fase0() {
  i = 0;
  j = 0;

  //recebeAngulos(&m, &n);
  recebeFloat(&m);
  recebeFloat(&n);

  //atualiza a posição (Em angulo) absoluta do sistema
  mabs += m;
  nabs += n;

  //função que impede movimentação errada do robo
  // erros();

  // A = Movimento Anti-Horário,  H = Movimento Horário,  0 = Link 1, 1 = Link 2
  if (m >= 0) {
    rotatesimple(A1, 0, m);
  }

  if (n >= 0) {
    rotatesimple(A2, 1, n);
  }

  if (m < 0) {
    m = -m;
    rotatesimple(H1, 0, m);
  }

  if (n < 0) {
    n = -n;
    rotatesimple(H2, 1, n);
  }
}

/////////////////////////////////// FASE 1 /////////////////////////////
void fase1() {
  i = 0;
  // RETIRAR
  m = 0;
  n = 0;
  inverte = '0';

  recebeInt(&inverte);
  recebeAngulos(&m, &n);
  // Serial.print(inverte); Serial.print(' ');
  // Serial.print(m); Serial.print(' ');
  // Serial.print(n); Serial.print(' ');

  if (inverte == 0) {

    if (m >= 0) {
      rotatesimple(A1, 0, m);
    }

    if (m < 0) {
      m = -m;
      rotatesimple(H1, 0, m);
    }

    if (n >= 0) {
      rotatesimple(A2, 1, n);
    }

    if (n < 0) {
      n = -n;
      rotatesimple(H2, 1, n);
    }
  }

  if (inverte == 1) {
    //Separa o movimento em duas partes
    n = (n / 2);
    m = (m / 2);
    if (n >= 0) {
      rotatesimple(A2, 1, n);
    }

    if (n < 0) {
      n = -n;
      rotatesimple(H2, 1, n);
      n = -n;
    }

    if (m >= 0) {
      rotatesimple(A1, 0, m);
    }

    if (m < 0) {
      m = -m;
      rotatesimple(H1, 0, m);
      m = -m;
    }

    if (n >= 0) {
      rotatesimple(A2, 1, n);
    }

    if (n < 0) {
      n = -n;
      rotatesimple(H2, 1, n);
    }
    if (m >= 0) {
      rotatesimple(A1, 0, m);
    }

    if (m < 0) {
      m = -m;
      rotatesimple(H1, 0, m);
    }
  }
}

/////////////////////////////////// FASE 2 /////////////////////////////
void fase2() {
  i = 0;
  m = 0;
  n = 0;

  // while (true){if(Serial.available() > 0){break;}}
  //   velocidade = Serial.readString();
  //=> recebeInt(&velocidade); // Velocidade não é float?

  recebeAngulos(&m, &n);

  t1 = T;
  t2 = T;

  if (m >= 0 && n >= 0) {
    rotate2(A1, A2);
  }

  if (m >= 0 && n < 0) {
    n = -n;
    rotate2(A1, H2);
  }

  if (m < 0 && n >= 0) {
    m = -m;
    rotate2(H1, A2);
  }

  if (m < 0 && n < 0) {
    m = -m;
    n = -n;
    rotate2(H1, H2);
  }
}

/////////////////////////////////// FASE 3 ////////////////////////////
void fase3() {
  i = 0;
  j = 0;

  recebeAngulos(&m, &n);

  mm = abs(m) + 1;
  nn = abs(n) + 1;

  if (mm >= nn) {
    tempo = (mm / nn);
    t1 = T;
    t2 = t1 * tempo;
  }
  if (nn > mm) {
    tempo = (nn / mm);
    t2 = T;
    t1 = t2 * tempo;
  }

  if (m >= 0 && n >= 0) {
    rotateduplo(A1, A2);
  }

  if (m >= 0 && n < 0) {
    n = -n;
    rotateduplo(A1, H2);
  }

  if (m < 0 && n >= 0) {
    m = -m;
    rotateduplo(H1, A2);
  }

  if (m < 0 && n < 0) {
    m = -m;
    n = -n;
    rotateduplo(H1, H2);
  }
}
/////////////////////////////////////// ZERAMENTO ////////////////////////
void zeramento() {
  if (zerado == 0) {
    while (link2 == 0) {
      if (digitalRead(besquerdo) == HIGH) {
        rotatezeramento(A2, 1);
      }
      if (digitalRead(bdireito) == HIGH) {
        rotatezeramento(H2, 1);
      }

      if (digitalRead(confirma) == HIGH) {
        delay(300);
        if (digitalRead(confirma) == HIGH) {
          link2 = 1;
          i = 0;
          NBLINK(3)
          delay(500);
        }
      }
    }

    while (link1 == 0) {
      if (digitalRead(besquerdo) == HIGH) {
        rotatezeramento(A1, 0);
      }
      if (digitalRead(bdireito) == HIGH) {
        rotatezeramento(H1, 0);
      }

      if (digitalRead(confirma) == HIGH) {
        delay(300);
        if (digitalRead(confirma) == HIGH) {
          link1 = 1;
          i = 0;
          NBLINK(3)
          delay(500);
        }
      }
    }
    zerado = 1;
    Serial.write('k');
  }
}

////////////////////////////////// ROTACOES ///////////////////////////

//Rotação com mesma velocidade e ao mesmo tempo dos motores
void rotateduplo(int a1, int b1, int a2, int b2) {

  long tempoinicial1 = millis(); // Salva o Tempo atual em Milisegundos
  long tempoinicial2 = millis();
  while (i < m * prec || j < n * prec) {

    if (millis() - tempoinicial1 >= t1) {
      digitalWrite(10, LOW);
      digitalWrite(11, LOW);
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);
      digitalWrite(a1 + ii[0], HIGH);
      i++;
      tempoinicial1 = millis(); //atualiza o tempo 1
      if (b1 == 0) {
        ii[0]++;
        if (ii[0] > 3) {
          ii[0] = 0;
        }
      }
      if (b1 == 1) {
        ii[0]--;
        if (ii[0] < 0) {
          ii[0] = 3;
        }
      }
    }

    if (millis() - tempoinicial2 >= t2) {
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      digitalWrite(a2 + ii[1], HIGH);
      //delay(2);
      //digitalWrite(a2 + i2, LOW);
      j++;
      tempoinicial2 = millis(); //atualiza o tempo 2
      if (b2 == 0) {
        ii[1]++;
        if (ii[1] > 3) {
          ii[1] = 0;
        }
      }
      if (b2 == 1) {
        ii[1]--;
        if (ii[1] < 0) {
          ii[1] = 3;
        }
      }
    }
  }

  i = 0;
  j = 0;
}

void rotate2(int a1, int b1, int a2, int b2) {
  long tempoinicial1 = millis(); // Salva o Tempo atual em Milisegundos
  long tempoinicial2 = millis();
  while (i < m * prec || j < n * prec) {

    if (millis() - tempoinicial1 >= t1 && i < m * prec) {
      digitalWrite(10, LOW);
      digitalWrite(11, LOW);
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);
      digitalWrite(a1 + i1, HIGH);
      i++;
      tempoinicial1 = millis(); //atualiza o tempo 1
      if (b1 == 0) {
        i1++;
        if (i1 > 3) {
          i1 = 0;
        }
      }
      if (b1 == 1) {
        i1--;
        if (i1 < 0) {
          i1 = 3;
        }
      }
    }

    if (millis() - tempoinicial2 >= t2 && j < n * prec) {
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      digitalWrite(a2 + i2, HIGH);
      j++;
      tempoinicial2 = millis(); //atualiza o tempo 2
      if (b2 == 0) {
        i2++;
        if (i2 > 3) {
          i2 = 0;
        }
      }
      if (b2 == 1) {
        i2--;
        if (i2 < 0) {
          i2 = 3;
        }
      }
    }
  }

  i = 0;
  j = 0;
}

void rotatezeramento(int a, int b, int c) {
  if (b == 0) {
    digitalWrite(a + ii[c], HIGH);
    delay(T);
    digitalWrite(a + ii[c], LOW);
    ii[c]++;
    if (ii[c] > 3) {
      ii[c] = 0;
    }
  }

  if (b == 1) {
    digitalWrite(a + ii[c], HIGH);
    delay(T);
    digitalWrite(a + ii[c], LOW);
    ii[c]--;
    if (ii[c] < 0) {
      ii[c] = 3;
    }
  }
}

void rotatesimple(int a, int b, int c, float q) {
  int del = 2000;
  i = 0; //contador padrao
  while (i < (q * prec)) {
    digitalWrite(10, LOW);
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);    
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    digitalWrite(a + ii[c], HIGH);
    delay(T + (del / 1000));
    i++;
    

    if (b == 0) {
      /*ii[c]++;
            if (ii[c] > 3){
              ii[c] = 0; 
            }*/
      ii[c] = (ii[c] + 1) % 4;
    }
    if (b == 1) {
      /*ii[c]--;
            if (ii[c] < 0){
              ii[c] = 3;
            }*/
      // ((n % M) + M) % M            
      ii[c] = (((ii[c] - 1) % 4) + 4) % 4; // why tho?
    }

    if (del > 0 && i < q * prec * 0.15) {
      del = del - 50;
    }
    if (del < 2000 && i > q * prec * 0.85) {
      del = del + 50;
    }
  }
  i = 0;
}
/////////////////////////////////// ERROS ////////////////
void erros() {

  if (mabs > mmax || mabs < -mmax) {
    erro();
  }

  if (mabs >= 0 && (nabs > 170 - (2 * mabs) || nabs < -170)) {
    erro();
  }

  if (mabs <= 0 && (nabs < -170 - (2 * mabs) || nabs > 170)) {
    erro();
  }
}

void erro() {
  //Cancela a atualização de posição já que não teve movimento
  mabs -= m;
  nabs -= n;

  //Zera os angulos e impede o movimento
  m = 0;
  n = 0;

  //Aviso pelo led 13
  NBLINK(3)
}

/* 
       _____           _       __   ______    
      / ___/___  _____(_)___ _/ /  / ____/___ 
      \__ \/ _ \/ ___/ / __ `/ /  / /_  / __ \
     ___/ /  __/ /  / / /_/ / /  / __/ / / / /
    /____/\___/_/  /_/\__,_/_/  /_/   /_/ /_/ 
  
*/

int serialToInt() {
  return (Serial.read() << 8) | Serial.read();
}

void recebeInt(int *r) {
  while (!Serial.available()) {
    delay(50);
  }
  *r = serialToInt();
  Serial.write('\1');
}

float serialToFloat() {
  ui32 _ref = ((ui32)Serial.read() << 24UL |
               (ui32)Serial.read() << 16UL |
               (ui32)Serial.read() << 8UL |
               (ui32)Serial.read());
  return *((float *)&_ref);
}

void recebeFloat(float *r) {
  while (!Serial.available()) {
    delay(50);
  }
  *r = serialToFloat();
  Serial.write('\1');
}

void recebeAngulos(float* a0, float* a1) {
  while (!Serial.available()) {
    delay(50);
  }
  *a0 = serialToFloat();
  *a1 = serialToFloat();
  Serial.write('\1');
}

// DEBUG
void _nblink(int n) {
  while (n-- > 0) {
    digitalWrite(13, HIGH);
    delay(250);
    digitalWrite(13, LOW);
    delay(250);
  }
}
