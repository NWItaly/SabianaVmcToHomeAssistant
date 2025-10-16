# Dettagli tecnici per sviluppatori

## 1. Scelte progettuali
A causa di problemi riscontrati durante lo sviluppo (caching errato), l'approccio scelto è stato quello di effettuare poche letture di molti registri contemporaneamente.

Questa scelta è stata motivata da diversi fattori:

- **Riduzione degli errori di caching:** Effettuando letture multiple in un'unica chiamata, si minimizza il rischio che dati obsoleti vengano restituiti a causa di un caching non aggiornato correttamente.
- **Miglioramento delle prestazioni:** Raggruppare le letture riduce il numero di chiamate al bus di comunicazione, diminuendo la latenza complessiva e migliorando l'efficienza del sistema.
- **Semplificazione della gestione degli stati:** Gestire i dati letti in blocco permette di avere una visione più coerente dello stato dei registri in un determinato istante, facilitando la logica applicativa e la diagnosi di eventuali problemi.
- **Scalabilità:** Questo approccio si adatta meglio a scenari in cui il numero di registri da monitorare cresce, evitando un aumento lineare delle chiamate e dei potenziali problemi di sincronizzazione.

Di contro la scrittura dei registri comporta che alcune entity siano duplicate.

### 1.1 Funzionalità specifiche della scheda utilizzata per lo sviluppo
- Utilizzo ethernet
- Supporto per RTC hardware
- LED RGB di stato (disabilitato di default)
- Buzzer (disabilitato di default)
- Ingressi digitali (disabilitati di default)
- Relè (disabilitato di default)

### 1.2 Funzionalità secondarie
- RTC sincronizzato con HA o con chip interno per mantenere eventuali programmazioni
- LED indica lo stato di connessione a HA
- Uptime

## 2. Struttura del progetto
- **[config/substitutions.yaml](../substitutions.yaml)**: Variabili modificabili alla bisogna
- **[config/main.yaml](../main.yaml)**: Configurazione principale hardware, networking, automazioni di base
- [config/blocks/Blk0_SystemIdentification.yaml](../blocks/Blk0_SystemIdentification.yaml): Identificazione sistema e firmware VMC
- [config/blocks/Blk1_MachineState.yaml](../blocks/Blk1_MachineState.yaml.yaml): Stato macchina, sonde, allarmi, modalità
- [config/blocks/Blk2_MachineParameters.yaml](../blocks/Blk2_MachineParameters.yaml.yaml): Parametri macchina, limiti, offset, setpoint
- [config/blocks/Blk3_Commands.yaml](../blocks/Blk3_Commands.yaml.yaml): Comandi e stato comandi VMC
- [config/blocks/Blk4_UserTimerProgram.yaml](../blocks/Blk4_UserTimerProgram.yaml.yaml): Programmi personalizzati dall'utente (da fare)
- [config/blocks/Blk8_TimeAndDay.yaml](../blocks/Blk8_TimeAndDay.yaml.yaml): Lettura orario e giorno dalla VMC
- [config/climate.yaml](../climate.yaml): Integrazione clima e controlli avanzati (in sviluppo)
- [config/modules/modbus_helpers.h](../modbus_helpers.h): Funzioni di supporto per parsing dati Modbus
- [config/modules/ethernet.yaml](../modules/ethernet.yaml)/[wifi.yaml](../modules/wifi.yaml): Configurazione metodo di connessione alla rete
- [config/modules/buzzer.yaml](../modules/buzzer.yaml): Modulo per gestire un piccolo altoparlante (disabilitato di default)
- [config/modules/digital_input.yaml](../modules/digital_input.yaml): Modulo per gestire gli input digitali (disabilitato di default)
- [config/modules/led.yaml](../modules/led.yaml): Modulo per gestire il led di stato presente sulla scheda
- [config/modules/logger.yaml](../modules/logger.yaml): Configurazione dei log (disabilitare se non necessario)
- [config/modules/modbus.yaml](../modules/modbus.yaml): Configurazione del protocollo ModBus
- [config/modules/relais.yaml](../modules/relais.yaml): Modulo per gestire i relè (disabilitato di default)
- [config/modules/rtc.yaml](../modules/rtc.yaml): Modulo per sincronizzare l'ora con HA

Per personalizzare il progetto è sufficiente modificare i file indicati in grassetto.

## 3. Task Explorer
Ho impostato l'estensione di VS Code affinché sia più agevole generare, avviare ed avviare il container docker.

## 4. Docker
Configurato affinché la cartella `config` venga copiata nel container e sia molto semplice compilare il firmware.