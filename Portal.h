/*
  Portal.h - Handle the webserver interface
  Saul bertuccio 12 feb 2017
  Released into the public domain.
*/

#ifndef Portal_h
#define Portal_h

#include "ESP8266WiFi.h"
#include "ESP8266WebServer.h"
#include "ESP8266mDNS.h"
#include "DNSServer.h"
#include "memory"

#include "ConnectionManager.h"
#include "DeviceHandler.h"

#define APP_NAME "Remote"
#define APP_VER "0.1"
#define APP_AUTHOR "Saul Bertuccio"
#define APP_RIGHT "Copyright &copy; 2017"
#define SERVER_PORT 80
#define DNS_PORT 53

// Html page template
const char HTML_FULL_TPL[] PROGMEM = "<!DOCTYPE html><html><head><meta name=viewport content=\"width=device-width, initial-scale=1\"><meta charset=utf-8><title>{title}</title><link href=https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css rel=stylesheet></head><body class=\"bg-info\">{body}<div>{message}</div><script src=https://code.jquery.com/jquery-3.1.1.slim.min.js></script><script src=https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js></script><script type=\"text/javascript\">{script}</script></body></html>";
const char HTML_SIMPLE_TPL[] PROGMEM = "<!DOCTYPE html><html><head><meta name=viewport content=\"width=device-width, initial-scale=1\"><meta charset=utf-8><title>{title}</title><body class=\"bg-info\" style=\"font-family:'Lucida Grande',Tahoma,Arial,Verdana,sans-serif;font-size:small\">{body}<div>{message}</div><script type=\"text/javascript\">{script}</script></body></html>";

// Body for root page
const char HTML_ROOT_BODY[] PROGMEM = "<div class=\"jumbotron\"><h1>" APP_NAME " versione " APP_VER "</h1><h4>Autore " APP_AUTHOR " " APP_RIGHT "</h4><p><a href=\"/setup\">Impostazioni</a></p></div>";


// Warninig message for Wifi settings
const char HTML_WIFISAVE_WARNING[] PROGMEM = "<p>La modifica a queste impostazioni provoca la temporanea disconnessione del dispositivo.<br>Se le informazioni inserite non risultano valide il dispositivo si avvierà in modalità configurazione base per consentirne la correzione</p>";

// Root page header template
const char HTML_PAGE_HEADER[] PROGMEM ="<div class=\"container\"><div class=\"row\"><div class=\"col-sm-12 col-lg-12\"><h1>{btext}<small>{stext}</small></h1></div></div></div>";

// Wifi settings form
const char HTML_FULL_WIFI_FORM[] PROGMEM = "<div class=\"row\"><form name=\"wifidata\" action=\"/setup/savewifi\" method=\"POST\" class=\"form-horizontal col-sm-10 col-sm-offset-1\"><div class=\"panel panel-primary\"><div class=\"panel-heading\"><h3 class=\"panel-title\">Dati rete senza fili (Wifi)</h3>{warn}</div><div class=\"panel-body\"><div class=\"form-group col-sm-6\"><label for=\"ssid\" class=\"col-sm-3 control-label\">Nome rete Wifi</label><div class=\"col-sm-9\"><input type=\"text\" class=\"form-control\" id=\"ssid\" name=\"ssid\"placeholder=\"ssid\" value=\"{ssid}\"></div></div><div class=\"form-group col-sm-6\"><label for=\"password\" class=\"col-sm-3 control-label\">Password</label><div class=\"col-sm-9\"><input type=\"password\" class=\"form-control\" id=\"password\" name=\"password\" value=\"{password}\"></div></div></div><div class=\"panel-footer clearfix\"><div class=\"pull-right\"><button type=\"submit\" class=\"btn btn-warning\">Salva</button></div></div></div></form></div>{dev_list}";
const char HTML_SIMPLE_WIFI_FORM[] PROGMEM = "{warn}<style type=\"text/css\">label{width:10em;display:inline-block;}</style><form name=\"wifidata\" action=\"/setup/savewifi\" method=\"POST\"><div><h3>Dati rete Wifi</h3><label for=\"ssid\"> Nome rete Wifi:</label> <select id=\"ssid\" name=\"ssid\" value=\"{ssid}\"><option>Attendere caricamento...</option></select><button type=\"button\" onclick=\"getNetworkList()\">Rileva</button><br/><label for=\"password\">Password:</label><input type=\"password\" id=\"password\" name=\"password\" value=\"{password}\"></div><div><button type=\"submit\">Salva</button></div></form>";

// Alert message
const char HTML_FULL_ALERT_TPL[] PROGMEM = "<div class=\"alert alert-{class}\" role=\"alert\"><button type=\"button\" class=\"close\" data-dismiss=\"alert\" aria-label=\"Chiudi\"><span aria-hidden=\"true\">×</span></button>{message}</div>";
const char HTML_SIMPLE_ALERT_TPL[] PROGMEM = "<div class=\"alert alert-{class}\">{message}</div>";

// Device panel
const char HTML_DEV_PANEL[] PROGMEM = "<div class=\"panel panel-warning col-sm-10 col-sm-offset-1\"><div class=\"panel-heading row\"><div class=\"col-xs-8\"><h3 class=\"panel-title\">Dispositivi</h3></div><div class=\"col-xs-3 pull-right\"><div class=\"btn-group btn-group-sm\" role=\"group\"><button type=\"button\" class=\"btn btn-primary\" data-action=\"new-device\" data-toggle=\"modal\" data-target=\"#device-name-edit\"><span class=\"glyphicon glyphicon-plus\" aria-hidden=\"true\"></span></button><button type=\"button\" class=\"btn btn-success \" data-action=\"edit-device\" data-toggle=\"modal\" data-target=\"#device-name-edit\"><span class=\"glyphicon glyphicon-pencil\" aria-hidden=\"true\"></span></button><button type=\"button\" class=\"btn btn-danger\" data-action=\"delete-device\" data-toggle=\"modal\" data-target=\"#confirm-delete\"><span class=\"glyphicon glyphicon-trash\" aria-hidden=\"true\"></span></button></div></div></div><div class=\"panel-body\">{panel_body}</div></div>";
const char HTML_DEV_LIST_TPL[] PROGMEM ="<div class=\"row\"><div class=\"col-md-3 col-sm-3 col-xs-3\"><ul class=\"nav nav-pills nav-stacked\" id=\"devices-list\">{dev_li_list}</ul></div><div class=\"col-md-9 col-sm-9 col-xs-9\"><div class=\"tab-content\">{dev_pane_list}</div></div></div>";  
const char HTML_DEV_LI_TPL[] PROGMEM = "<li><a href=\"#{dev_name}\" data-toggle=\"pill\">{dev_name}</a></li>";
const char HTML_DEV_PANE_TPL[] PROGMEM = "<div class=\"tab-pane\" id=\"{dev_name}\">{dev_name}</div>";

// Modal change device name
const char HTML_MODAL_DEV_NAME[] PROGMEM = "<div class=\"modal fade\" id=\"device-name-edit\" tabindex=\"-1\" role=\"dialog\" aria-labelledby=\"device-name-edit\"> <div class=\"modal-dialog\" role=\"document\"><div class=\"modal-content\"><div class=\"modal-header\"><button type=\"button\" class=\"close\" data-dismiss=\"modal\" aria-label=\"Annulla\"><span aria-hidden=\"true\">&times;</span></button><h4 class=\"modal-title\" id=\"device-name-edit-title\">Nome dispositivo</h4></div><div class=\"modal-body\"><form class=\"form-horizontal\" role=\"form\"><div class=\"form-group\"><label  class=\"col-sm-2 control-label\" for=\"device-name\">Nome</label><div class=\"col-sm-10\"><input type=\"text\" class=\"form-control\"  id=\"device-name\" placeholder=\"Nome dispositivo\"/></div></div></form><div class=\"modal-footer\"><button type=\"button\" class=\"btn btn-default\" data-dismiss=\"modal\">Annulla</button><button type=\"button\" class=\"btn btn-primary\">Salva</button></div></div></div></div></div><script type=\"text/javascript\">$('#device-name-edit').on('shown.bs.modal', function() {})</script>";
// Modal device or command key delete
const char HTML_MODAL_DELETE[] PROGMEM = "<div class=\"modal fade\" id=\"confirm-delete\" tabindex=\"-1\" role=\"dialog\" aria-labelledby=\"confirm-delete\" aria-hidden=\"true\"><div class=\"modal-dialog\"><div class=\"modal-content\"><div class=\"modal-header\">Conferma eliminazione</div><div class=\"modal-body\"><div class=\"device hidden\">Procedendo verrà eliminato il dispositivo e tutti i comandi eventualmente configurati</div><div class=\"key hidden\">Eliminare il comando del dispositivo?</div></div><div class=\"modal-footer\"><button type=\"button\" class=\"btn btn-default\" data-dismiss=\"modal\">Annulla</button><a class=\"btn btn-danger btn-ok\">Elimina</a></div></div></div></div><script type=\"text/javascript\">$('#confirm-delete').on('shown.bs.modal', function() {})</script>";

// Javascript funciont
const char JAVASCRIPT_NETLIST[] PROGMEM ="function getNetworkList(){var a=new XMLHttpRequest;a.onreadystatechange=function(){if(a.readyState==XMLHttpRequest.DONE)if(200==a.status){for(var b=document.getElementById(\"ssid\");b.firstChild;)b.removeChild(b.firstChild);try{for(var c=JSON.parse(a.responseText),d=c.networks,e=0;e<d.length;e++){var f=document.createElement(\"option\");f.value=d[e].name,f.text=d[e].name,\"\"!=d[e].protection&&(f.text+=\"(*)\"),f.text+=\" db: \"+d[e].quality,b.appendChild(f)}}catch(a){alert(\"Errore risposta server\"+a)}}else 400==a.status?alert(\"Errore 400 richiesta non valida\"):alert(\"Errore di comunicazione con il server\")},a.open(\"GET\",\"/setup/wifilist\",!0),a.send()};document.addEventListener(\"DOMContentLoaded\", function(event) {getNetworkList();});";


class Portal {

  public:
 
    Portal();
    boolean setup();
    void handleRequest();

    boolean needRestart();

  private:
    boolean ap_mode = false;
    boolean need_restart = false;
    String message;

    String getDevPanel();

    std::unique_ptr<DNSServer> dnsServer;
    std::unique_ptr<ESP8266WebServer> server;
    MDNSResponder mdns;

    void setWifi();

    void handleRootPage();
    void handleNotFound();
    void handleSetup();
    void handleSaveWifi();
    void handleEditDevice();
    void redirectHeaders(String where);


    void handleWifiList();

    String basePage();
    String getMessageTpl();
    boolean redirect();

    // Check
    boolean parseAlphaNumString(String &argument, int len);
};

#endif


