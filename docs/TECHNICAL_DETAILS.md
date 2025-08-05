# Dettagli tecnici per sviluppatori

## Scelte progettuali
A causa di problemi riscontrati durante lo sviluppo (caching errato), l'approccio scelto è stato quello di effettuare poche letture di molti registri contemporaneamente.

Questa scelta è stata motivata da diversi fattori:

- **Riduzione degli errori di caching:** Effettuando letture multiple in un'unica chiamata, si minimizza il rischio che dati obsoleti vengano restituiti a causa di un caching non aggiornato correttamente.
- **Miglioramento delle prestazioni:** Raggruppare le letture riduce il numero di chiamate al bus di comunicazione, diminuendo la latenza complessiva e migliorando l'efficienza del sistema.
- **Semplificazione della gestione degli stati:** Gestire i dati letti in blocco permette di avere una visione più coerente dello stato dei registri in un determinato istante, facilitando la logica applicativa e la diagnosi di eventuali problemi.
- **Scalabilità:** Questo approccio si adatta meglio a scenari in cui il numero di registri da monitorare cresce, evitando un aumento lineare delle chiamate e dei potenziali problemi di sincronizzazione.

Di contro la scrittura dei registri comporta che alcune entity sono duplicate.

### Funzionalità specifiche della scheda utilizzata per lo sviluppo
- Utilizzo ethernet
- Supporto per RTC hardware
- LED RGB di stato (disabilitato di default)
- Buzzer (disabilitato di default)
- Ingressi digitali (disabilitati di default)
- Relè (disabilitato di default)

### Funzionalità secondarie
- RTC sincronizzato con HA o con chip interno per mantenere eventuali programmazioni
- LED indica lo stato di connessione a HA
- Uptime

## Struttura del progetto
- **substitutions.yaml**: Variabili modificabili alla bisogna
- **main.yaml**: Configurazione principale hardware, networking, automazioni di base
- modbus.yaml: Configurazione del protocollo ModBus
- config/Blk0_SystemIdentification.yaml: Identificazione sistema e firmware VMC
- config/Blk1_MachineState.yaml: Stato macchina, sonde, allarmi, modalità
- config/Blk2_MachineParameters.yaml: Parametri macchina, limiti, offset, setpoint
- config/Blk3_Commands.yaml: Comandi e stato comandi VMC
- config/Blk4_UserTimerProgram.yaml: Programmi personalizzati dall'utente (da fare)
- config/Blk8_TimeAndDay.yaml: Lettura orario e giorno dalla VMC
- climate.yaml: Integrazione clima e controlli avanzati (in sviluppo)
- modbus_helpers.h: Funzioni di supporto per parsing dati Modbus
- ethernet.yaml/wifi.yaml: Configurazione metodo di connessione alla rete
- buzzer.yaml: Modulo per gestire un piccolo altoparlante (disabilitato di default)
- led.yaml: Modulo per gestire il led di stato presente sulla scheda
- logger.yaml: Configurazione dei log (disabilitare se non necessario)
- relais.yaml: Modulo per gestire i relè (disabilitato di default)
- digital_input.yaml: Modulo per gestire gli input digitali (disabilitato di default)
- rtc.yaml: Modulo per sincronizzare l'ora con HA

Per personalizzare il progetto è sufficiente modificare i file indicati in grassetto.

## Il mio ambiente di sviluppo
- Visual Studio Code
- Docker lanciando da powershell (come amministratore) il seguente comando:
`docker run --rm -d --name esphome -v "${PWD}:/config" -v esphome_cache:/config/.esphome -p 6052:6052 ghcr.io/esphome/esphome`. Questo crea il sito [localhost](http://localhost:6052/) da cui è possibile modificare, compilare ed aggiornare l'ESP32. **N.B.**: eseguire il comando nella cartella in cui è presente il codice sorgente