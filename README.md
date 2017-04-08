# Remote
### ESP8266 - Universal Remote

**L'applicativo è ancora in via di sviluppo**

*****

#### Descrizione

Implementazione di un *telecomando* universale usando il modulo **ESP-12E** collegato a un trasmettitore/ricevitore RF 315/433 MHz e uno a Infrarossi (IR). In questo modo è possible comandare sia dispositivi che funzionano usando frequenze radio a 315 o 433 Mhz, sia quelli che usano infrarossi. In quest'ultimo caso è necessario che tra il sensore infrarossi e il dispositivo da comandare non ci siano ostacoli.

 La pagina web di configurazione consente di definire diversi *telecomandi* (devices), e per ciascuno di essi è sceglierne la tiplogia (*infrarossi* o *RF433*). Per ciascun _teleacomando_ sarà quindi possibile definire i *tasti* inserendo manualmente i loro codici oppure in modo più semplice usando la rilevazione automatica, che consente di rilevare automaticamente i dati di uno specifico tasto semplicemente puntando il telecomando originario verso il ricevitore collegato all'**ESP-12E**.

La configurazione viene salvata in modo permanentemente nella memoria flash dell'**ESP-12E**.

Configurati **telecomandi** e **tasti** il dispositivo è in grado di inviare il codice corretto al dispositivo semplicemente usando l'url: **http://<ip_esp-12e>/push/<nomedispositivo>/<nometasto>**

#### Implementazione

Il programma è stato svilupato usando l'IDE Arduino configurato per **ESP-12E**. Vengono usate librerie tipiche di questo ambiente come _ESP8266WebServer_ per servire le pagine web e _FS_ (_SPIFFS_) per gestire la memoria Flash del dispositivo come se fosse una memoria di massa. Quest'ultima viene usata sia sia per salvare le configurazioni dei telecomandi sia per memorizzare alcune funzioni javascript necessarie al funzionamento delle pagine di configurazione.

La gestione relativa al telecomando vero e proprio è implementata usando le seguenti librerie, che vanno installate nell'IDE di Arduino:

- rc-switch per gestire i trasmettitori/ricevitori RF che operano a frequeze di 315 e 433MHz. La libreria supporta i seguenti chipset:

    SC5262 / SC5272
    HX2262 / HX2272
    PT2262 / PT2272
    EV1527 / RT1527 / FP1527 / HS1527
    Intertechno outlets

- IRremoteESP8266 per gestire i ricevitori/trasmettitori a infrarossi.

Per poter trasferire i file presenti all'interno della directory _data_ nella memoria flash dell'**ESP-12E** è possibile usare il plugin _Sketch Data Uploader_ dell'ambiente Arduino.

#### Funzionamento

Per il corretto funzionamento della pagina di configurazione dei _telecomandi_ è necessario che l'**ESP-12E** venga prima connesso alla rete Wi-Fi casalinga come _client Wi-Fi_.

Al primo avvio, oppure quando l'**ESP-12E** non riesce a connettersi alla rete Wi-Fi configurata, entra in modalità _di configurazione provvisoria_. In questa modalità l'**ESP-12E** funziona in come _punto di accesso senza fili_ (access point) rendendo disponibile una rete Wi-Fi isolata il cui SSID è **esp8266_remote** mentre la password è **administrator**; in tale modalità viene attivato un servizio _DHCP server_ per assegnare automaticaemente gli indirizzi IP, e un _DNS server minimale_ che risolve qualsiasi nome web nell'indirizzo IP dell'**ESP-12E**.

Quando un dispositivo (PC, tablet, cellulare) dotato di scheda Wi-Fi, viene connesso alla rete **esp8266_remote**, gli verrà assegnato un indirizzo IP, e accedendo tramite un browser a qualsiasi sito internet si verrà rediretti verso la pagina web di configurazione minimale dell'**ESP-12E** dove si potrà impostare l'_SSID_ e la password della rete Wi-Fi a cui connettere l'**ESP-12E** stesso.

Per facilitare l'individuazione della rete Wi-Fi, **ESP-12E** esegue una scansione visualizzando in un controllo web di tipo _select/option_ tutti gli _SSID_ disponibili. Non è possibile inserire manualmente un _SSID_, pertanto non sono supportate reti Wi-Fi nascoste, che non espongono cioè il loro _SSID_. Non è inoltre supportata la configurazione manuale dell'indirizzo IP e degli altri parametri Internet, pertanto è necessario che la rete Wi-Fi a cui l'**ESP-12E** si connettererà disponga di un  server _DHCP_ che rilasci tali parametri automaticamente.

Solo in modalità _client Wi-Fi_ l'**ESP-12E** servirà le pagine di configurazione complete e le pagine **push** per il controllo dei dispositivi configurati. Le pagine di configurazione (_setup_) usano i framework per Bootsrtap e JQuery. Per non occupare memoria flash inutilmente, i due framework non sono presenti nella memoria flash del dispositivo: ma le pagine _html_ servite  contengono dei riferimenti esterni permanenti. È necessario pertanto che il dispositvo dal quale si sta configurando l'**ESP-12E** sia connesso a Internet.