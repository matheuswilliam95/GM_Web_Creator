#include <OneWire.h> //INCLUSÃO DE BIBLIOTECA
#include <DallasTemperature.h> //INCLUSÃO DE BIBLIOTECA

#define DS18B20 7 //DEFINE O PINO DIGITAL UTILIZADO PELO SENSOR

OneWire ourWire(DS18B20); //CONFIGURA UMA INSTÂNCIA ONEWIRE PARA SE COMUNICAR COM O SENSOR
DallasTemperature sensors(&ourWire); //BIBLIOTECA DallasTemperature UTILIZA A OneWire

void setup() {
  Serial.begin(9600); //INICIALIZA A SERIAL
  sensors.begin(); //INICIA O SENSOR
  delay(1000); //INTERVALO DE 1 SEGUNDO
}

void loop() {
  sensors.requestTemperatures();//SOLICITA QUE A FUNÇÃO INFORME A TEMPERATURA DO SENSOR
  Serial.print("Temperatura: "); //IMPRIME O TEXTO NA SERIAL
  Serial.print(sensors.getTempCByIndex(0)); //IMPRIME NA SERIAL O VALOR DE TEMPERATURA MEDIDO
  Serial.println("*C"); //IMPRIME O TEXTO NA SERIAL
  delay(250);//INTERVALO DE 250 MILISSEGUNDOS
}
