#include <LedControl.h>

const int DIN_PIN = 10;
const int CS_PIN = 9;
const int CLK_PIN = 8;

const int matriz_boca = 0;
const int matriz_ojo_der = 1;
const int matriz_ojo_izq = 2;


//==================================================
// MATRIZ 0 : BOCA          (REVES)
// MATRIZ 1 : OJO DERECHO   (REVES)
// MATRIZ 2 : OJO IZQUIERDO (DERECHO)
//==================================================

const uint64_t TEST[] = {
  0xffffffffffffffff,    // LLENO
  0x0000000000000000      // VACIO
};  

// AL REVES
const uint64_t OJO_DERECHA[] = {
  0x3c4281818181423c, // ABIERTO
  0x00000000ff000000,  // CERRADO
  0x040a11214181423c  // ASUSTADO / TRISTE
};

// AL DERECHO
const uint64_t OJO_IZQUIERDA[] = {
  0x3c4281818181423c, // ABIERTO
  0x000000ff00000000,  // CERRADO
  0x3c42814121110a04 // ASUSTADO / TRISTE
};  

// AL REVES
const uint64_t BOCA[] = {
  0x000081ff7e000000,   // 0 - sonrisa 
  0x0000181800000000,   // 1 - dormir 1
  0x00183c3c18000000,   // 2 - dormir 2
  0x81ffffff7e3c0000,    // 3 - boca abierta
  0x000044aa11000000  // asustado
};

LedControl display = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 3);


unsigned int iCara=1;
unsigned int iPaso=0;

const int iCaraDuerme = 1;
const int iCaraSonrie = 2;
const int iCaraHabla = 3;
const int iCaraTest=4;
const int iCaraVacio=5;
const int iCaraAsustado = 6;
const int iCaraNormal=7;


bool bTemporizador=false;
int iTemporizador = 1000;
unsigned long TiempoInicioTemporizador = 0;

//============================================================================
// FUNCION GENERICA : ESPERA
//============================================================================
// Espera en Milisegundos, leyendo el serial por si recibimos cualquier orden
//============================================================================
//bool Espera(int iMiliSegundos)
//{
//  int iiCara=iCara;
//  unsigned long inicio, fin, transcurrido;
//  inicio = millis();
//  fin = inicio + iMiliSegundos;
//  transcurrido = 0;
//  while (transcurrido <= fin && iiCara==iCara)
//  {
//    transcurrido = millis();
//    Serial_Lee();
//  }
//  return true;
//}

//============================================================================
//============================================================================
void InicializaTemporizador(int iTiempo)
{
  iTemporizador=iTiempo;
  bTemporizador=true;
  TiempoInicioTemporizador=millis();
}

//============================================================================
// Espera no bliqueante del loop
//============================================================================
bool Esperar()
{
    if (millis() >= TiempoInicioTemporizador+iTemporizador)
    {
      // Fin periodo
      iTemporizador=0;
      bTemporizador=false;
    }

    return !bTemporizador;
}

//=================================================================
//=================================================================
void setup() {
  Serial.begin(9600);
  
  for (int i = 0; i < 3; i++) {
    display.clearDisplay(i);
    display.shutdown(i, false);
    display.setIntensity(i, 1);
  }

  iCara= iCaraDuerme; // Dormido
}

//=================================================================
//=================================================================
void PintaIcono(int iPanel, uint64_t image) {
  for (int i = 0; i < 8; i++) {
    byte row = (image >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      display.setLed( iPanel, i, j, bitRead(row, j));
    }
  }
}

//============================================================================
// BUCLE INFINITO
//============================================================================
void loop()
{
 Serial_Lee();
 PintaCara();
}

//============================================================================
//============================================================================
void PintaCara()
{
    switch (iCara) {
      case iCaraDuerme: duerme(); break; 
      case iCaraSonrie: sonrie(); break; 
      case iCaraHabla: habla(); break;
      case iCaraTest: test(); break;
      case iCaraVacio: vacio(); break;
      case iCaraAsustado: asustado(); break;
      case iCaraNormal: normal(); break;
      break;
    }   
}

//=======================================================================
// SI leemos algo entendible del serial, mandamos la orden determinada
//=======================================================================
void Serial_Lee()
{
  int iiCara=0;
  
    //si existe datos disponibles los leemos
  if (Serial.available()>0){
    //leemos la opcion enviada
    char TeclaApretada=Serial.read();
    switch (TeclaApretada) {
      case 'k': Serial.println("reset: ok");    iiCara=iCaraSonrie;   break; // reset: sonrie
      case 's': Serial.println("sonrie: ok");   iiCara=iCaraSonrie;   break; // sonrie
      case 'h': Serial.println("habla: ok");    iiCara=iCaraHabla;    break; // habla
      case 'd': Serial.println("duerme: ok");   iiCara=iCaraDuerme;   break; // duerme
      case 't': Serial.println("test: ok");     iiCara=iCaraTest;     break; // test
      case 'v': Serial.println("vacio: ok");    iiCara=iCaraVacio;    break; // vacio
      case 'a': Serial.println("asustado: ok"); iiCara=iCaraAsustado; break; // asustado
      case 'n': Serial.println("normal: ok");   iiCara=iCaraNormal;   break; // normal
    }
    if (iiCara!=0 && iiCara!=iCara)
      {
        iCara=iiCara;
        iPaso=0;
        bTemporizador=false;
      }
  }
}

//=======================================================================
void test()
{
   PintaIcono(matriz_ojo_der, TEST[0]);
   PintaIcono(matriz_ojo_izq, TEST[0]);
   PintaIcono(matriz_boca, TEST[0]);
}

void vacio()
{
   PintaIcono(matriz_ojo_der, TEST[1]);
   PintaIcono(matriz_ojo_izq, TEST[1]);
   PintaIcono(matriz_boca, TEST[1]);
}

//=======================================================================
//=======================================================================
void habla()
{
    if (bTemporizador)
  {
    bool bFin=Esperar();  
    if (bFin) {iPaso++; if (iPaso>11) iPaso=1;}
  }
  else
  { 
    if (iPaso==0) habla_start(); 
    if (iPaso==11) habla_4();
    if (iPaso==10) habla_2();
    if (iPaso==9) habla_3();
    if (iPaso==8) habla_2();
    if (iPaso==7) habla_3();
    if (iPaso==6) habla_2();
    if (iPaso==5) habla_3();
    if (iPaso==4) habla_2();
    if (iPaso==3) habla_3();
    if (iPaso==2) habla_2();
    if (iPaso==1) habla_1();
  }

}

void habla_start()
{
  iPaso=1;
}

void habla_1()
{
  PintaIcono(matriz_boca, BOCA[3]);
  // PARPADEO
  PintaIcono(matriz_ojo_der, OJO_DERECHA[0]);
  PintaIcono(matriz_ojo_izq, OJO_IZQUIERDA[0]);
  InicializaTemporizador(250);  
}

void habla_2()
{
  PintaIcono(matriz_boca, BOCA[0]);
  InicializaTemporizador(250);  
}

void habla_3()
{
  PintaIcono(matriz_boca, BOCA[3]);
  InicializaTemporizador(250);  
}
  
void habla_4()
{
  PintaIcono(matriz_ojo_der, OJO_DERECHA[1]);
  PintaIcono(matriz_ojo_izq, OJO_IZQUIERDA[1]);
  InicializaTemporizador(100);
}

//=======================================================================
//=======================================================================
void duerme()
{
  if (bTemporizador)
  {
    bool bFin=Esperar();  
    if (bFin) {iPaso++; if (iPaso>2) iPaso=1;}
  }
  else
  { 
    if (iPaso==0) duerme_start();
    if (iPaso==2) duerme_2();
    if (iPaso==1) duerme_1();
  }
}

void duerme_start()
{
  PintaIcono(matriz_ojo_der, OJO_DERECHA[1]);
  PintaIcono(matriz_ojo_izq, OJO_IZQUIERDA[1]);
 iPaso=1;
}

void duerme_1()
{
  PintaIcono(matriz_boca, BOCA[1]);
  InicializaTemporizador(1000); 
}

void duerme_2()
{
  PintaIcono(matriz_boca, BOCA[2]);
  InicializaTemporizador(1000);
}

//=======================================================================
//=======================================================================
void sonrie() {
  if (bTemporizador)
  {
    bool bFin=Esperar();  
    if (bFin) {iPaso++; if (iPaso>2) iPaso=1;}
  }
  else
  {
    if (iPaso==0) sonrie_start();
    if (iPaso==2) sonrie_2();
    if (iPaso==1) sonrie_1();
  }
}

void sonrie_start()
{
  PintaIcono(matriz_boca, BOCA[0]);
  iPaso=1;
}

void sonrie_1()
{
  // PARPADEO
  PintaIcono(matriz_ojo_der, OJO_DERECHA[0]);
  PintaIcono(matriz_ojo_izq, OJO_IZQUIERDA[0]);
  InicializaTemporizador(2000);
}

void sonrie_2()
{
  PintaIcono(matriz_ojo_der, OJO_DERECHA[1]);
  PintaIcono(matriz_ojo_izq, OJO_IZQUIERDA[1]);
  InicializaTemporizador(100);
}


//=======================================================================
//=======================================================================
void normal() {
  if (bTemporizador)
  {
    bool bFin=Esperar();  
    if (bFin) {iPaso++; if (iPaso>2) iPaso=1;}
  }
  else
  {
    if (iPaso==0) normal_start();
    if (iPaso==2) normal_2();
    if (iPaso==1) normal_1();
  }
}

void normal_start()
{
  PintaIcono(matriz_boca, TEST[1]); // sin boca
  iPaso=1;
}

void normal_1()
{
  // PARPADEO
  PintaIcono(matriz_ojo_der, OJO_DERECHA[0]);
  PintaIcono(matriz_ojo_izq, OJO_IZQUIERDA[0]);
  InicializaTemporizador(2000);
}

void normal_2()
{
  PintaIcono(matriz_ojo_der, OJO_DERECHA[1]);
  PintaIcono(matriz_ojo_izq, OJO_IZQUIERDA[1]);
  InicializaTemporizador(100);
}

//=======================================================================
//=======================================================================
void asustado() {

if (bTemporizador)
{
  bool bFin=Esperar();  
  if (bFin) {iPaso++; if (iPaso>2) iPaso=1;}
}
else
{
  
 if (iPaso==0) asustado_start();
 if (iPaso==2) asustado_2();
 if (iPaso==1) asustado_1();
}
}

void asustado_start()
{
  PintaIcono(matriz_boca, BOCA[4]);
  iPaso=1;
}

void asustado_1()
{
  // PARPADEO
  PintaIcono(matriz_ojo_der, OJO_DERECHA[2]);
  PintaIcono(matriz_ojo_izq, OJO_IZQUIERDA[2]); 
  InicializaTemporizador(2000);
}



void asustado_2()
{
  PintaIcono(matriz_ojo_der, OJO_DERECHA[1]);
  PintaIcono(matriz_ojo_izq, OJO_IZQUIERDA[1]);
  InicializaTemporizador(100);
}
