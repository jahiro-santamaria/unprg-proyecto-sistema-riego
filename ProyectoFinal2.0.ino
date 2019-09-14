//Sistema de Riego Arduino utilizando GSM + 3 sensores
//Copyright (C) 2019 Jahiro Santamaría

//Este programa es software libre: puede redistribuirlo y/o modificarlo bajo
//los términos de la Licencia General Pública de GNU publicada por la Free
//Software Foundation, ya sea la versión 3 de la Licencia, o (a su elección)
//cualquier versión posterior.

//Este programa se distribuye con la esperanza de que sea útil pero SIN
//NINGUNA GARANTÍA; incluso sin la garantía implícita de MERCANTIBILIDAD o
//CALIFICADA PARA UN PROPÓSITO EN PARTICULAR. Vea la Licencia General Pública
//de GNU para más detalles.

//Usted ha debido de recibir una copia de la Licencia General Pública
//de GNU junto con este programa. Si no, vea <http://www.gnu.org/licenses/>.

//"Cuando comenzamos este proyecto solo Dios y mi equipo sabían lo que hacía, ahora solo Dios lo sabe"

#include <DHT.h>
#include <DHT_U.h>
#include <SoftwareSerial.h>
#define Pecho 5
#define Ptrig 6
SoftwareSerial SIM900(7,8);
//#define DEBUG(a) Serial.println(a);
String mensaje;
int TempyHum=2;
int relay=4;
int temp, humedad;
int lluviaA;
String lluvia;
int cont=0;
DHT dht(TempyHum,DHT11);
long duracion, distancia;
float pi=22/7;
int radio=5;
String numeroCel="923585445";
void setup() {
  SIM900.begin(19200);
  Serial.begin(9600);
  delay(10000);
  SIM900.print("AT+CMGF=1\r");
  delay(200);
  SIM900.print("AT+CNMI=2,2,0,0,0\r");
  delay(200);
  dht.begin();
  pinMode(Pecho,INPUT);
  pinMode(Ptrig,OUTPUT);
  pinMode(13,1);
  pinMode(relay,OUTPUT);
  Serial.println("SETUP LISTO");
  Serial.println("DATOS");
  datos();
  //Serial.println("SELECCIONE MODO DE RIEGO M(Manual)//A(Automatico)");
}

void loop() {
  while(cont==0){
    boolean saltoEncontrado=false;
    String ignore=" 20\",.\n\r";
    if(SIM900.available()>0){
    String mensajetemp=(String)SIM900.read();
    Serial.println(mensaje);
    select(mensajetemp);
      if((mensajetemp=="-") && !(saltoEncontrado)){
        saltoEncontrado=true;
        }else{
          if((saltoEncontrado)&&!(ignore.indexOf(mensajetemp)>=0)){
             mensaje+=mensajetemp;
             select(mensaje);
          }
        };
        if(mensaje.equals("0")){
          cont = 0;
          } else {
            cont = cont +1;
          }
    }
  }
  delay(500);
}

void select(String msn){
  if(msn.equals("M")){
     Serial.println("ENTRANDO AL MODO MANUAL");
     envioMensaje("ENTRANDO AL MODO MANUAL");
     Serial.println("SELECCIONE 0 PARA SALIR");
     envioMensaje("SELECCIONE 0 PARA SALIR");
     modoManual();
  }else if(msn.equals("A")){
     Serial.println("ENTRANDO AL MODO AUTOMATICO");
     envioMensaje("ENTRANDO AL MODO AUTOMATICO");
     modoAutomatico();
  }else if(msn.equals("0")){
     //Serial.println("SELECCIONE MODO DE RIEGO M(Manual)//A(Automatico)");  
     Serial.println("DATOS");
     envioMensaje("DATOS");
     datos();
  }else if(msn.equals("No")){
     Serial.println("SELECCIONE MODO DE RIEGO M(Manual)//A(Automatico)");
     envioMensaje("SELECCIONE MODO DE RIEGO M(Manual)//A(Automatico)");  
     Serial.println("DATOS");
     envioMensaje("DATOS");
     datos();
  }else if(msn.equals("Si")){
     Serial.println("Indique la cantidad de segundos que desea regar");
     envioMensaje("Indique la cantidad de segundos que desea regar");  
  }else if(msn.toInt()>0){
     encenderRelay(msn);
  }
}

void datos(){
  humedad=dht.readHumidity();
  temp= dht.readTemperature();
  Serial.print("TEMPERATURA: ");
  Serial.print(temp);
  Serial.println("°C");
  envioMensaje("TEMPERATURA: "+(String)temp+"ºC");
  Serial.print("HUMEDAD: ");
  Serial.print(humedad);
  Serial.println("%");
  envioMensaje("HUMEDAD: "+(String)humedad+"%");
  digitalWrite(Ptrig,LOW);
  delayMicroseconds(2);
  digitalWrite(Ptrig,HIGH);
  delayMicroseconds(10);
  digitalWrite(Ptrig,LOW);
  duracion =pulseIn(Pecho,HIGH);
  distancia=(duracion/2)/29;
  Serial.print("CANTIDAD DE AGUA DISPONIBLE: ");
  float cantidad = distancia*2*pi*radio;
  Serial.print(cantidad);
  Serial.println("ml.");
  envioMensaje("CANTIDAD DE AGUA DISPONIBLE: "+(String)cantidad+" ml.");
  //Serial.println(lluvia);
  lluviaA=analogRead(0);
  if(lluviaA<300){
    Serial.println("LLUVIA INTENSA, PROHIBIDO REGAR, ESPERE QUE SEQUE EL SENSOR");
    envioMensaje("LLUVIA INTENSA, PROHIBIDO REGAR, ESPERE QUE SEQUE EL SENSOR");
  }else if(lluviaA <500){
    Serial.println("LLUVIA MODERADA, PROHIBIDO REGAR, ESPERE QUE SEQUE EL SENSOR");
    envioMensaje("LLUVIA MODERADA, PROHIBIDO REGAR, ESPERE QUE SEQUE EL SENSOR");
  }else{
    Serial.println("LLUVIA NO DETECTADA"); 
    envioMensaje("LLUVIA NO DETECTADA");
    Serial.println("SELECCIONE MODO DE RIEGO M(Manual)//A(Automatico)");
    envioMensaje("SELECCIONE MODO DE RIEGO M(Manual)//A(Automatico)");
  }
}

void encenderRelay(String msn){
  digitalWrite(relay,HIGH);
  Serial.println("REGANDO");
  envioMensaje("REGANDO");
  delay(msn.toInt()*1000);
  digitalWrite(relay,LOW);
  Serial.println("SE TERMINO DE REGAR");
  envioMensaje("SE TERMINO DE REGAR");
  loop();
}

void modoManual(){
  Serial.println("¿Desea Regar?");
  envioMensaje("¿DESEA REGAR?");
  boolean saltoEncontrado=false;
  String ignore=" 20\",.\n\r";
  if(SIM900.available()>0){
    String mensajetemp=(String)SIM900.read();
    Serial.println(mensaje);
    select(mensajetemp);
    if((mensajetemp=="-") && !(saltoEncontrado)){
      saltoEncontrado=true;
      }else{
        if((saltoEncontrado)&&!(ignore.indexOf(mensajetemp)>=0)){
           mensaje+=mensajetemp;
           select(mensaje);
        }
      }
  }
}

void modoAutomatico(){
  if(humedad<75){
      if(lluviaA>=500){
          digitalWrite(relay,HIGH);
          Serial.println("REGANDO");
          envioMensaje("REGANDO");
          delay(10000);
          digitalWrite(relay,LOW);
          Serial.println("SE TERMINO DE REGAR");
          envioMensaje("SE TERMINO DE REGAR");
          loop();
      };
  }else{
    Serial.println("NO HAY CONDICIONES PARA REGAR");
    envioMensaje("NO HAY CONDICIONES PARA REGAR");
    loop();
    }
}

void envioMensaje(String mensaje){
  delay(200);
  SIM900.println("AT+CMGS=\""+numeroCel+"\"");
  delay(500);
  SIM900.println(mensaje);
  delay(500);
  SIM900.println((char)26);
  delay(500);
  SIM900.println();
  delay(200);
  Serial.println("MENSAJE ENVIADO");
  Serial.println("AT+CMGS=\""+numeroCel+"\"");
  Serial.println("SMS: "+mensaje);
  limpiarSerial(false);
  delay(200);
  limpiarSerial(true);
}

void limpiarSerial(boolean temp){
  while(!(SIM900.available()>0)&&temp){
    delay(1000);
    Serial.println("Enviando");
    }
  if(temp){
    Serial.println("Enviado, limpiando serial");
    }
  while(SIM900.available()>0){
    char temp=(char)SIM900.read();
    }
}
