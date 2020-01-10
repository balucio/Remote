# Remote
### ESP8266 - Universal Remote

**Prima versione stabile 0.5**

*****

#### Descrizione

Implementazione di un *telecomando* universale usando il modulo **ESP-12E** collegato a un trasmettitore/ricevitore RF 315/433 MHz e Infrarossi Seriale tipo YS-IRTM. Ciò dovrebbe consentire di comandare un gran numero di dispositivi, sia che usino RF 315Mhz e 433Mhz, sia infrarossi (IR). In caso di dispositivi IR non devono esserci ostacoli tra il trasmettitore IR e il dispositivo da comandare.

La pagina web di configurazione consente di definire diversi *telecomandi* (devices) di tipo *IR* o *RF433*. Per ciascun _teleacomando_ sarà possibile definire i *tasti* (keys) inserendo manualmente i relativi codici o usando la rilevazione automatica: puntare il telecomando originario verso il ricevitore sull'**ESP-12E** e premere il tasto da rilevare.

La configurazione viene salvata in modo permanentemente nella memoria flash dell'**ESP-12E**.

Configurati **telecomandi** e i relativi **tasti** il dispositivo è in grado di inviare il codice corretto al dispositivo semplicemente usando l'url: 

**http://<ip_esp-12e>/sendKey/\<nomedispositivo\>/\<nometasto\>**

#### Implementazione

Il programma è stato svilupato usando l'IDE Arduino configurato per **ESP-12E**. Vengono usate le seguenti librerie:
* rc-switch (2.6.2)
* ESP8266WiFi (1.0)
* ESP8266WebServer (1.0)
* ESP8266mDNS
* DNSServer (1.1.0)
Viene inoltre usata la libreria _FS_ (_SPIFFS_) per gestire la memoria Flash del dispositivo come se fosse una memoria di massa. Quest'ultima viene usata sia sia per salvare le configurazioni dei telecomandi sia per memorizzare alcune funzioni javascript necessarie al funzionamento delle pagine di configurazione.

Tutte le precedenti librerie vanno installare sull'IDE di Arduino prima di compilare il progetto.

##### Modulo RF315/433Mhz
Allo scopo sono stati usati i moduli MX-05V e MX-FS-03V, rispettivamente un ricevitore e un trasmettitore RF314/433Mth facilmente reperibili su Amazon o su Ebay. Il ricevitore è stato collegato al PIN **D2** dell'**ESP-12E** mentre il trasmettitore è stato collegato al PIN **D1**. Vengono pilotati tramite la libreria _rc-switch_ che dovrebbe essere in grado di gestire trasmettitori/ricevitori RF che operano a frequeze di 315 e 433MHz. La libreria supporta i seguenti chipset:

* SC5262 / SC5272
* HX2262 / HX2272
* PT2262 / PT2272
* EV1527 / RT1527 / FP1527 / HS1527
* Intertechno outlets
##### Modulo YS-IRTM
Si tratta di una schedina di fabbricazione cinese che include sia un ricevitore che un trasmettitore IR (infrarossi). Questa andrebbe collegata a un'interfaccia UART (seriale). Tuttavia per evitare di impegnare i PIN seriali dell'**ESP-12E** è stata usata la libreria **SoftwareSerial** che permette di usare due GPIO per inviare o ricevere segnali di tipo seriale. Nello specifico il PIN *D5* è stato usato come *RXD* e va collegato al PIN *IR-TXD* del modulo, il PIN *D6* è stato invece usato come trasmettitore *TXD* e va collegato al PIN *IR-RXD* del modulo. Il circuito di ricezione IR del modulo è sempre attivo e decodifica ogni segnale Infrarosso autotomaticamente inviando sulla seriale il relativo codice come coppie di numeri esadecimali.

##### JavaScript

Le pagine Web servite dall'**ESP-12E** fanno largo uso di Javascript.
Nello specifico le pagine di configurazione (_setup_) usano i framework per Bootsrtap e JQuery. Per non occupare memoria inutilmente, i file relativi a questi framework non sono memorizzati nella memoria flash dell'**ESP-12E**, ma vengono serviti come riferimenti esterni permanenti. È necessario pertanto che il dispositvo dal quale si sta configurando l'**ESP-12E** sia connesso a Internet.

Tutte le funzioni javascript scritte _ad hoc_ sono state raccolte nel file _devicesFunc.js_ presente nella directory _data_ del progetto. Tale file va trasferito nella memoria flash dell'**ESP-12E**, per tale operazione è possibile usare il plugin _Sketch Data Uploader_ dell'ambiente Arduino.

#### Funzionamento

Per il corretto funzionamento della pagina di configurazione dei _telecomandi_ è necessario che l'**ESP-12E** venga prima connesso alla rete Wi-Fi casalinga come _client Wi-Fi_.

##### Modalità provvisoria

Al primo avvio, oppure quando l'**ESP-12E** non riesce a connettersi alla rete Wi-Fi configurata, entra in modalità _di configurazione provvisoria_. In questa modalità l'**ESP-12E** funziona come _punto di accesso senza fili_ (access point) rendendo disponibile una rete Wi-Fi isolata il cui SSID è **esp8266_remote** e password **administrator**. In questa modalità è attivo anche un servizio _DHCP_ per assegnare indirizzi IP, e un _DNS server_ minimale che risolve qualsiasi nome web nell'indirizzo IP dell'**ESP-12E**.

In _modalitò provvisoria_ i dispositivi Wi-Fi (PC, tablet, cellulare) connessi alla rete **esp8266_remote**, riceveranno una configurazione IP, potranno accedere alla pagina di configurazione aprendo una pagina web qualunque in un browser. La pagina web di configurazione minimale dell'**ESP-12E** consente di impostare l'_SSID_ e la password della rete Wi-Fi principale a cui connettere l'**ESP-12E** per il funzionamento normale.

Per facilitare l'individuazione della rete Wi-Fi, l'**ESP-12E** esegue una scansione visualizzando in un controllo web di tipo _select/option_ tutti gli _SSID_ rilevati. Al momento non è possibile inserire manualmente un _SSID_, pertanto non sono supportate reti Wi-Fi che non espongono il loro _SSID_ (cioè nascoste). Non è inoltre supportata la configurazione manuale dell'indirizzo IP e degli altri parametri Internet, pertanto è necessario che la rete Wi-Fi a cui l'**ESP-12E** si dovrà connettere disponga di un server _DHCP_ che rilasci tali parametri automaticamente.

##### Funzionamento normale

Solo in modalità _client Wi-Fi_ l'**ESP-12E** servirà le pagine di configurazione complete.
Tuttavia l'invio dei tasti precedentemente configurati rimane disponibile attravarso la pagina **sendKey**.
