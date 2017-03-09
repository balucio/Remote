# Remote
### ESP8266 - Universal Remote

**L'applicativo è ancora in via di sviluppo**

*****
#### Descrizione
Implementazione di un *telecomando* universale usando il modulo **ESP-12E** correlato di un trasmettitore/ricevitore RF 315/433 MHz e di uno a Infrarossi (IR), per comandare entrambe le tipologie di dispositivi.

Tramite una pagina web di configurazione è possibile procedere alla configurazione dei *telecomandi* (devices), della loro tiplogia (*infrarossi* o *RF433*) di ciascun *tasto*.
La configurazione viene salvata in modo permanentemente nella memoria flash dell'**ESP-12E**.

#### Implementazione
Il programma è stato svilupato usando l'IDE Arduino configurato per **ESP-12E**. Vengono usate librerie tipiche di questo ambiente come _ESP8266WebServer_ per servire le pagine web e _FS_ (_SPIFFS_). Quest'ultima consente la gestione di gestire la  memora flash dell'**ESP-12E** come un file system, e viene usata nello specifico sia per salvare le configurazioni dei telecomandi sia per memorizzare i javascript necessari al funzionamento delle pagine di configurazione.
I file all'interno della directory _data_ dell'applicativo devono quindi essere trasferiti nella memoria flash dell'**ESP-12E**, allo scopo è stato usato il plugin _Sketch Data Uploader_ dell'ambiente Arduino.

#### Funzionamento

Per il corretto funzionamento della pagina di configurazione dei _telecomandi_ è necessario che l'**ESP-12E** venga prima connesso a una rete Wi-Fi, funzioni quindi in modalità _client Wi-Fi_.

Al primo avvio, oppure quando l'**ESP-12E** non riesce a connettersi alla rete Wi-Fi configurata, entra in modalità _di configurazione provvisoria_.
In questa modalità l'**ESP-12E** funziona in come _punto di accesso senza fili_ rendendo disponibile una rete Wi-Fi **stub** (cioè completamente isolata) con SSID **esp8266_remote** e password **administrator**; sono inoltre attivi servizi di _DHCP server_ per l'assegnazione di un indirizzo IP, e un _DNS server minimale_ che risolve qualsiasi nome web nell'indirizzo IP dell'**ESP-12E**.
Pertanto connettendosi alla rete **esp8266_remote** con un PC dotato di scheda Wi-Fi, e accedendo con un browser a qualsiasi sito internet si verrà rediretti verso una pagina web minimale dove sarà possibile impostare l'_SSID_ e la password di una rete Wi-Fi a cui l'**ESP-12E** dovrà connettersi.
L'**ESP-12E** esegue una scansione per trovare gli _SSID_ disponibili, non è previsto al momento il supporto a _SSID_ nascosti, così come non è prevista l'impostazione manuale di un indirizzo IP. Pertanto è necessario che sulla rete Wi-Fi a cui l'**ESP-12E** dovrà connettersi sia presente un server _DHCP_.

Solo in modalità _client Wi-Fi_ l'**ESP-12E** serve le pagine di configurazione complete e la pagina **api** per il controllo dei dispositivi configurati.