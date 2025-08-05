# Come usare
1. Carica i file YAML e il file di helper C++ su ESPHome
2. Configura le credenziali di rete e Home Assistant nei file `secrets.yaml` (vedi capitolo successivo)
3. Collega ESP32 alla VMC Sabiana tramite interfaccia Modbus (RS485)
4. Carica il firmware sulla scheda tramite ESPHome
5. Integra il dispositivo in Home Assistant
6. Personalizza automazioni e dashboard secondo le tue esigenze

## Personalizzazione
- Ho suddiviso ogni sezione in modo che sia facilmente utilizzabile e che sia semplice rimuovere le parti che non interessano.
- Per utilizzare il WiFi è necessario togliere il commento nel file `main.yaml` al package `wifi.yaml` e commentare `ethernet.yaml`.
- Nel file `substitutions.yaml` è possibile modificare il nome visualizzato su HA oppure modificare l'indirizzo della macchina (verificare i dip switches).

## Setup Secrets
Ricordarsi di creare il file `secrets.yaml` con i parametri:
- wifi_ssid
- wifi_password
- fallback_password
- ota_password
- api_encryption_key

Per semplicità ho creato degli script che aiutano alla generazione del file.

### Metodo 1: Python
```bash
python3 secrets_setup.py
```
### Metodo 2: Windows (Doppio click)
```bash
secrets_setup.bat
```