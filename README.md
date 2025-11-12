# Atividade prática do componente de Programação Para Sistemas Embarcados

## Como rodar localmente

* Instalar o Arduino IDE.
* Dentro do Arduino IDE instalar as depedências:

* Criar um arquivo "credencias.h" no diretório codigo-iot/atividade_praitca, como por exemplo:
```cpp
#ifndef CREDENCIAIS_H
#define CREDENCIAIS_H

// CREDENCIAIS DO THINGSPEAK
const char* MQTT_SERVER = "URL_DO_THINGSPEAK";
const char* MQTT_USER = "USUARIO_DO_THINGSPEAK";
const char* MQTT_PASSWORD = "SENHA_MQTT";
const char* TOPIC_PUBLISH = "TOPIC_PUBLISH";
const char* MQTT_CLIENT_ID = "MQTT_CLIENT_ID";

// CREDENCIAIS WIFI
const char* WIFI_SSID = "SEU_SSID_AQUI";
const char* WIFI_PASSWORD = "SUA_SENHA_AQUI";

#endif
```
colocar as credenciais do wifi e também do servidor thingspeak


## Integrantes:
* Amanda Naraoka
* Leonardo de Almeida Pereira
* Vinicius Lustosa Silva