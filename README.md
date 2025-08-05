# Sabiana VMC ESPHome Integration

Questo progetto permette l'integrazione avanzata di una VMC Sabiana ENY-SP-180 
con Home Assistant tramite ESPHome, sfruttando la comunicazione Modbus e 
l'automazione locale.

## Funzionalità principali
- Lettura e pubblicazione di tutti i parametri principali della VMC tramite Modbus
- Controllo remoto di accensione/spegnimento, modalità operative, velocità ventole, 
  soglie di temperatura e altre funzioni avanzate
- Integrazione con Home Assistant tramite API nativa ESPHome
- Automazioni locali per gestione stagionale, modalità rapide, allarmi e notifiche
- Visualizzazione di stato, allarmi, timer, parametri macchina e identificazione sistema

## 📚 Guide dettagliate
- 📖 [Guida all'utilizzo](config/INSTALLATION.md)
- 🔧 [Hardware](config/HARDWARE_GUIDE.md)
- ⚙️ [Per sviluppatori](config/TECHNICAL_DETAILS.md)

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

## 💬 Supporto

- 🐛 **Bug o problemi?** [Apri un Issue](../../issues)
- 💡 **Domande?** [Discussioni](../../discussions)  
- 📧 **Altro?** Contatta @NWItaly

---
⭐ **Ti piace il progetto?** Metti una stella! Aiuta altri utenti a trovarlo.