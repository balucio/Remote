/*
  Portal.cpp - Handle the webserver interface
  Saul bertuccio 11 feb 2017
  Released into the public domain.
*/

#include "Portal.h"

Portal::Portal():message(String("")) {}

boolean Portal::setup() {

  IPAddress ip = ConnectionManager::getIpAddress();
 
  if (ip.toString() == "0.0.0.0") {
    Serial.println("Indirizzo IP non valido");
    return false;
  }

  ap_mode = ConnectionManager::isApMode();

  // Avvio captive portal
  if (ap_mode) {
    dnsServer.reset(new DNSServer());
    dnsServer->setErrorReplyCode(DNSReplyCode::NoError);

    if (dnsServer->start(DNS_PORT, "*", ip)) {
      Serial.println("Avviato servizio DNS");
      } else {
        Serial.println("Impossibile avviare servizio DNS");
    }
  }

  /* Configuro il webserver */
  server.reset(new ESP8266WebServer(SERVER_PORT));

  /* Bind delle url alle funzioni */
  server->on("/", std::bind(&Portal::handleRootPage, this));
  server->on("/setup", std::bind(&Portal::handleSetup, this));
  server->on("/setup/wifilist", std::bind(&Portal::handleWifiList, this));
  server->on("/setup/editDevice", std::bind(&Portal::handleEditDevice, this));
  server->on("/setup/getDeviceInfo", std::bind(&Portal::handleGetDeviceInfo, this));
  server->on("/setup/deleteDevice", std::bind(&Portal::handleDeleteDevice, this));

  server->on("/setup/savewifi", std::bind(&Portal::handleSaveWifi, this));
  server->onNotFound (std::bind(&Portal::handleNotFound, this));
  server->begin(); // Web server start

  dh = DeviceHandler();
  Serial.println("Avviato server HTTP");
/*
  if (!ap_mode) {
    MDNS.begin(AP_SSID);
    MDNS.addService("http", "tcp", 80);
    delay(1000);
    //Serial.println("Avviato servizio mDNS");
  }
*/
  return true;

}

boolean Portal::needRestart() { return need_restart; }

void Portal::handleRequest() {

  if (ap_mode)
    dnsServer->processNextRequest();
  server->handleClient();
}

void Portal::handleRootPage() {

  if (redirect())
    return;

  String page = this->basePage();
  
  page.replace("{script}", "");
  page.replace("{message}", message);
  page.replace("{body}", FPSTR(HTML_ROOT_BODY));
  server->send(200, "text/html", page); 
}

void Portal::handleSaveWifi() {

  // get post data
  String new_ssid = server->arg("ssid");
  String new_pass = server->arg("password");
  String msg;

  message = getMessageTpl();

  boolean success = ConnectionManager::connectWifi(new_ssid, new_pass, true);

  if (!success) {
    Serial.print("Impossibile collegarsi alla rete Wifi ");
    message.replace("{class}",  "error");
    msg = "Errore impossibile collegarsi alla rete senza fili ";
    msg += new_ssid;
  } else {
    message.replace("{class}",  "info");
    msg = "Connessione riuscita il dispositivo si riavvierà per completare la configurazione.";
    need_restart = true;
  }

  message.replace("{message}", msg);
  redirectHeaders("/setup");

}

void Portal::handleEditDevice() {

    String device_name="", device_type="";

    String msg;
    bool error = true;

    if(!server->hasArg("device_name") || !server->hasArg("device_type")) {
      
      msg = "Errore: nome o tipo dispositivo non specificati";

    } else {

      device_name = server->arg("device_name");
      device_type = server->arg("device_type");

      if (!DeviceHandler::isValidDeviceName(device_name)) {

        msg = "Errore: nome dispositivo non valido";

      } else if (!DeviceHandler::isValidDeviceType(device_type)) {

        msg = "Errore: tipo dispositivo non valido";

      } else if (dh.setDevice( device_name, device_type ))  {

        if (server->hasArg("new_device_name")) {

          String new_device_name = server->arg("new_device_name");

          if (DeviceHandler::isValidDeviceName(new_device_name)) {

            if (dh.renameDevice(new_device_name)) {
              error = false;
              msg = "Dispositivo rinominato";
              device_name = new_device_name;
            } else {
              msg = "Errore: impossibile rinominare il dispositivo";
            }
          } else {
            msg = "Errore: nuovo nome del dispositivo non valido"; 
          }

        } else {
          error = false;
          msg = "Dispositivo correttamente configurato";
        }

      } else {
        msg = "Errore: impossibile configurare il dispositivo";
      }
    }

    String json = "{ \"error\" :";
    json += (error ? String("true") : String("false"));
    json += String(",\"message\":\"") + msg + "\"}";

    server->send( 200, "text/json", json );
}


void Portal::handleDeleteDevice() {
  
  String device_name="", msg="\"\"";
  bool error = true;

  if(!server->hasArg("device_name")) {
      
    msg = "Errore: nome dispositivo non specificato";

  } else {

    device_name = server->arg("device_name");

    if (!DeviceHandler::isValidDeviceName(device_name)) {

      msg = "Errore: nome dispositivo non valido";

    } else if (dh.setDevice(device_name)) {

      if (dh.deleteDevice()) {
        error = false;
        msg = "Dispositivo eliminato correttamente";
      } else {
        msg = "Errore: impossibile eliminare il dispositivo";
      }
    }
  }

  String json = "{\"error\":";
  json += (error ? String("true") : String("false"));
  json += String(",\"message\":\"") + msg + "\"}";

  server->send( 200, "text/json", json );
}

void Portal::handleGetDeviceInfo() {
  
  String device_name="", msg="";
  String dev_data="\"name\":\"\",\"type\":\"\", \"key\":[]";
  bool error = true;

  if(!server->hasArg("device_name")) {
      
    msg = "Errore: nome dispositivo non specificato";

  } else {

    device_name = server->arg("device_name");

    if (!DeviceHandler::isValidDeviceName(device_name)) {

      msg = "Errore: nome dispositivo non valido";

    } else if (dh.setDevice(device_name)) {

      dev_data = getDeviceData(dh.getDevice());
      error = false;
    } else {

      msg = "Errore: impossibile selezionare il dispositivo";

    }
  }

  String json = "{\"error\":";
  json += (error ? String("true") : String("false"));
  json += String(",\"device\": {") + dev_data + '}';
  json += String(",\"message\":\"") + msg + "\"}";

  server->send( 200, "text/json", json );
}

void Portal::handleWifiList() {

  WifiNetworks nets = ConnectionManager::getNetworkList();

  String json = "{\"networks\":[";
  String prot;
  for (int i = 0; i < nets.netNum(); i++) {
    prot = ConnectionManager::encryptionCodeToString(nets.getProtection(i));
    json += String("{ \"name\" : \"") + nets.getName(i) + "\",";
    json += String("\"protection\":\"") + prot + "\",";
    json += String("\"quality\":\"") + nets.getQuality(i) + "\"},";
  }
  json.setCharAt(json.length() - 1, ' ');
  json += "]}";
  server->send( 200, "text/json", json );

}

void Portal::handleSetup() {

    String page = this->basePage();
    String body = FPSTR(HTML_PAGE_HEADER);
    
    body += (ap_mode ? FPSTR(HTML_SIMPLE_WIFI_FORM) : FPSTR(HTML_FULL_WIFI_FORM));
    page.replace("{body}",  body);
    page.replace("{btext}", "Impostazioni");
    page.replace("{stext}", "");
    page.replace("{ssid}", ConnectionManager::getSsid());
    page.replace("{password}", ConnectionManager::getPassword());
    page.replace("{script}", perpareJavascript(!ap_mode));
    page.replace("{warn}", FPSTR(HTML_WIFISAVE_WARNING));

    if (!ap_mode) {
      page.replace("{dev_list}", getDevPanel());
    }
    
    server->send(200, "text/html", page);
}

void Portal::handleNotFound() {

  if (redirect())
    return;
    
  String page = this->basePage();
  page.replace("{script}", "");
  
  String body = "<div class=\"jumbotron\"><h1 class=\"text-warning\">Pagina non trovata</h1>";
  body += "<p><span class=\"text-muted\">Url Richiesta: </span><span class=\"text-success\">";
  body +=  server->uri() + "</span></p>";
  body += "<p><span class=\"text-muted\">Metodo: </span><span class=\"text-success\">";
  body += (( server->method() == HTTP_GET ) ? String("GET") : String("POST")) + "</span></p>";
  
  int sargs = server->args();

  body += "<p><span class=\"text-muted\">Argomenti: </span><span class=\"text-success\">";
  body +=  String(sargs) + "</span></p>";
  body += "<ul>";
      
  for ( int i = 0; i < sargs; i++ ) {
    body += "<li><span class=\"text-muted\">" + server->argName( i ) +": </span>";
    body += "<span class=\"text-success\">" + server->arg( i ) + "</span></li>";
  }
  body += "</ul></div>";

  page.replace("{body}", body);
  
  server->sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
  server->sendHeader("Pragma", "no-cache");
  server->sendHeader("Expires", "-1");
  server->send( 404, "text/html", page );

}

String Portal::basePage() {
  
  String page = FPSTR(ap_mode ? HTML_SIMPLE_TPL : HTML_FULL_TPL);
  String appdata = FPSTR(APP_NAME);
  appdata += " ver .";
  appdata += FPSTR(APP_VER);
  page.replace("{title}", appdata);
  page.replace("{message}", message);
  return page;
}

String Portal::getMessageTpl() {
  return FPSTR(ap_mode ? HTML_SIMPLE_ALERT_TPL : HTML_FULL_ALERT_TPL);
}

boolean Portal::redirect() {


  if ( ap_mode && !ConnectionManager::isIp(server->hostHeader())) {
    Serial.println("Richiesto un hostname redirigo la richiesta verso l'ip locale.");
    this->redirectHeaders(String("http://") + server->client().localIP().toString());
    return true;
  }
  return false;
}

void Portal::redirectHeaders(String where) {
  
      Serial.println("Redirigo la richiesta verso ");
      Serial.println(where);

      server->sendHeader("Location", where, true);
      // nessuna intestazione Content-length, perchè non inviamo contenuto
      server->send ( 302, "text/plain", ""); 
      // è necessario chiudere la connessione manualmente
      server->client().stop();
}

String Portal::getDevPanel() {
  
  String devpane = FPSTR(HTML_DEV_PANEL);
  String * list = DeviceHandler::getDevicesName();
  int dn = DeviceHandler::getDevicesNum();

  String panelbody;

  devpane += prepareModal(
    String("device-edit"),
    String("Modifica/Crea dispositivo"),
    prepareDeviceForm(),
    "saveDeviceData"
  );
  devpane += prepareModal(
    String("device-delete"),
    String("Elimina dispositivo"),
    String("Eliminare il dispositivo <b class=\"device-name\"></b> e i comandi configurati?"),
    "deleteDevice"
  );

  panelbody = FPSTR(HTML_DEV_LIST_TPL);
  panelbody += "<div id=\"device-template\" class=\"hidden\">";
  panelbody += FPSTR(HTML_DEV_LI_TPL);
  panelbody += FPSTR(HTML_DEV_PANE_TPL);
  panelbody += "</div>";

  String alert = FPSTR(HTML_FULL_ALERT_TPL);
  alert.replace("{message}", "Nessun dispositivo configurato");

  if (dn == 0) {
    panelbody.replace("{dev_li_list}", "");
    panelbody.replace("{dev_pane_list}","");
    alert.replace("{class}", "danger no-device-alert");
    panelbody += alert;
    devpane.replace("{panel_body}", panelbody);
    return devpane;
  }

  String li = "", pane = "", wrk;

  for (int i = 0; i < dn; i++) {
        wrk = FPSTR(HTML_DEV_LI_TPL);
        wrk.replace("{dev_name}", list[i]);
        li += wrk;
        wrk = FPSTR(HTML_DEV_PANE_TPL);
        wrk.replace("{dev_name}", list[i]);
        wrk.replace("{dev_type}", "");
        pane += wrk;
  }

  delete [] list;

  panelbody.replace("{dev_li_list}", li);
  panelbody.replace("{dev_pane_list}", pane);
  alert.replace("{class}", "danger no-device-alert hidden");
  panelbody += alert;
  devpane.replace("{panel_body}", panelbody);

  return devpane;
    
}

String Portal::prepareDeviceForm() {

  String form = FPSTR(HTML_FORM_EDIT_DEVICE);
  int qt = DeviceHandler::getDeviceTypesNum();
  String tpl, options = "";

  for( int i=0; i < qt; i++){
    tpl = FPSTR(HTML_OPTION_TPL);
    tpl.replace("{value}", DeviceHandler::getDeviceTypes()[i]);
    tpl.replace("{name}", DeviceHandler::getDeviceTypesDescription()[i]);
    options += tpl;
  }
  form.replace("{device_types}", options);
  return form;
}

String Portal::prepareModal(String const &id, String const &title, String const &body, String const &ok_callback) {

  String modal = FPSTR(HTML_MODAL_TPL);
  modal.replace("{id}", id);
  modal.replace("{title}", title);
  modal.replace("{body}", body);
  modal.replace("{ok-callback}", ok_callback);
  return modal;
}


String Portal::getDeviceData(Device &device) {

  String json = "\"name\":\"" + device.getName() + "\",";
  json += "\"type\":\"" + device.getType() + "\",";
  json += "\"keys\":[ ";
  Key * key = device.getKeys();

  while (key) {
    json += "{\"name\":\"" + key->getName() + "\",";
    json += String("\"pulse\":\"") + key->getPulse() + "\",";
    json += String("\"code\":\"") + key->getCode() + "\"},";
    key = key->getNext();
  }
  json.remove(json.length() - 1);
  json += "]";
  return json;
}

String Portal::perpareJavascript(boolean full) {

  String javascript = FPSTR(JAVASCRIPT_NETLIST);

  if (full) {
    javascript += FPSTR(JAVASCRIPT_EDIT_DEVICE);
    javascript += FPSTR(JAVASCRIPT_DELETE_DEVICE);
  }

  return javascript;
}

