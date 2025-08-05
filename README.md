# Sabiana VMC ESPHome Integration

Questo progetto permette l'integrazione avanzata di una VMC Sabiana ENY-SP-180 
con Home Assistant tramite ESPHome, sfruttando la comunicazione Modbus e 
l'automazione locale.

## Hardware supportato
- Scheda di sviluppo: ESP32
- VMC: Sabiana ENY-S/SP, ENY-SHP-170, ENY-P (Non testato)

### Hardware utilizzato e testato
- Scheda di sviluppo: Waveshare ESP32-S3-ETH-8DI-8RO
  (ESP32-S3, Ethernet W5500, 8 ingressi digitali, 8 relè)
- VMC: Sabiana ENY-SP-180

## Funzionalità principali
- Lettura e pubblicazione di tutti i parametri principali della VMC tramite Modbus
- Controllo remoto di accensione/spegnimento, modalità operative, velocità ventole, 
  soglie di temperatura e altre funzioni avanzate
- Integrazione con Home Assistant tramite API nativa ESPHome
- Automazioni locali per gestione stagionale, modalità rapide, allarmi e notifiche
- Visualizzazione di stato, allarmi, timer, parametri macchina e identificazione sistema

### Funzionalità specifiche della scheda utilizzata per lo sviluppo
- Utilizzo ethernet
- Supporto per RTC hardware
- LED RGB di stato
- Buzzer 
- Ingressi digitali 
- Relè

### Funzionalità secondarie
- RTC sincronizzato con HA o con chip interno per mantenere eventuali programmazioni
- LED indica lo stato di connessione a HA
- Uptime

## Struttura del progetto
- main.yaml: Configurazione principale hardware, networking, automazioni di base
- Blk0_SystemIdentification.yaml: Identificazione sistema e firmware VMC
- Blk1_MachineState.yaml: Stato macchina, sonde, allarmi, modalità
- Blk2_MachineParameters.yaml: Parametri macchina, limiti, offset, setpoint
- Blk3_Commands.yaml: Comandi e stato comandi VMC
- Blk4_UserTimerProgram.yaml: Programmi personalizzati dall'utente (da fare)
- Blk8_TimeAndDay.yaml: Lettura orario e giorno dalla VMC
- Climate.yaml: Integrazione clima e controlli avanzati (in sviluppo)
- modbus_helpers.h: Funzioni di supporto per parsing dati Modbus

### Scelte progettuali
A causa di problemi riscontrati durante lo sviluppo (caching errato), l'approccio scelto è stato quello di effettuare poche letture di molti registri contemporaneamente.

Questa scelta è stata motivata da diversi fattori:

- **Riduzione degli errori di caching:** Effettuando letture multiple in un'unica chiamata, si minimizza il rischio che dati obsoleti vengano restituiti a causa di un caching non aggiornato correttamente.
- **Miglioramento delle prestazioni:** Raggruppare le letture riduce il numero di chiamate al bus di comunicazione, diminuendo la latenza complessiva e migliorando l'efficienza del sistema.
- **Semplificazione della gestione degli stati:** Gestire i dati letti in blocco permette di avere una visione più coerente dello stato dei registri in un determinato istante, facilitando la logica applicativa e la diagnosi di eventuali problemi.
- **Scalabilità:** Questo approccio si adatta meglio a scenari in cui il numero di registri da monitorare cresce, evitando un aumento lineare delle chiamate e dei potenziali problemi di sincronizzazione.

Di contro la scrittura dei registri comporta che alcune entity sono duplicate.

## Come usare
1. Carica i file YAML e il file di helper C++ su ESPHome
2. Configura le credenziali di rete e Home Assistant nei file `secrets.yaml`
3. Collega ESP32 alla VMC Sabiana tramite interfaccia Modbus (RS485)
4. Carica il firmware sulla scheda tramite ESPHome
5. Integra il dispositivo in Home Assistant tramite ESPHome API
6. Personalizza automazioni e dashboard secondo le tue esigenze

## Personalizzazione
Per il mio utilizzo ho alimentato la scheda con cavo ethernet POE. Se si preferisce è possibile utilizzare il WiFi ed è sufficiente commentare la sezione `ethernet` e togliere il commento a `wifi`.

**N.B.**: Ricordarsi di creare il file `secrets.yaml` con i parametri:
- wifi_ssid
- wifi_password
- fallback_password
- ota_password
- api_encryption_key

### Il mio ambiente di sviluppo
- Visual Studio Code
- Docker lanciando da powershell (come amministratore) il seguente comando:
`docker run --rm -d --name esphome -v "${PWD}:/config" -v esphome_cache:/config/.esphome -p 6052:6052 ghcr.io/esphome/esphome`. Questo crea il sito [localhost](http://localhost:6052/) da cui è possibile modificare, compilare ed aggiornare l'ESP32. **N.B.**: eseguire il comando nella cartella in cui è presente il codice sorgente

## Licenza d'uso
Questo software è fornito "così com'è", senza alcuna garanzia espressa o implicita.
L'autore declina ogni responsabilità per eventuali danni diretti o indiretti derivanti dall'uso di questo progetto. L'uso è consentito esclusivamente per scopi personali, didattici o di test.
È vietata la distribuzione commerciale senza autorizzazione scritta dell'autore.

## Riferimenti esterni
- [Home Assistant](https://www.home-assistant.io/)
- [EspHome](https://esphome.io/)
- [Waveshare ESP32-S3-ETH-8DI-8RO](https://www.waveshare.com/wiki/ESP32-S3-ETH-8DI-8RO#Onboard_Resources)
- [Docker](https://www.docker.com/)
- [Visual Studio Code](https://code.visualstudio.com/)


### TODO
I prossimi sviluppi sono (non ordinati):
- Utilizzare il componente climate per avere una prima interfaccia grafica da utilizzare su HA
- Pubblicare l'integrazione su HACS
- Creare un'interfaccia grafica moderna e facile da utilizzare che permetta di modificare la maggior parte dei parametri
- Supporto MQTT
- Notifiche avanzate in caso di allarmi o anomalie
- Integrazione con sensori esterni
