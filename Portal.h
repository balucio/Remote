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
#include "Validation.h"

#define APP_NAME "Remote"
#define APP_VER "0.1"
#define APP_AUTHOR "Saul Bertuccio"
#define APP_RIGHT "Copyright &copy; 2017"
#define SERVER_PORT 80
#define DNS_PORT 53

// Html page template
const char HTML_FULL_TPL[] PROGMEM = "<!DOCTYPE html><html><head><meta name=viewport content=\"width=device-width, initial-scale=1\"><meta charset=utf-8><title>{title}</title><link href=http://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css rel=stylesheet></head><body class=\"bg-info\">{body}<div>{message}</div><script src=http://code.jquery.com/jquery-3.1.1.min.js></script><script src=http://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js></script>{script}<script type=\"text/javascript\">{onready}</script></body></html>";
const char HTML_SIMPLE_TPL[] PROGMEM = "<!DOCTYPE html><html><head><meta charset=utf-8><title>{title}</title><body class=\"bg-info\" style=\"font-family:'Lucida Grande',Tahoma,Arial,Verdana,sans-serif;font-size:small\">{body}<div>{message}</div><script type=\"text/javascript\">{onready}</script></body></html>";
const char HTML_404_TPL[] PROGMEM = "<div class=\"jumbotron\"><h1 class=\"text-warning\">Pagina non trovata</h1><p><span class=\"text-muted\">Url Richiesta: </span><span class=\"text-success\">{uri}</span></p><p><span class=\"text-muted\">Metodo: </span><span class=\"text-success\">{method}</span></p><p><span class=\"text-muted\">Argomenti: </span><span class=\"text-success\">{args}</span></p><ul>{arglist}</ul></div>";

// Body for root page
const char HTML_ROOT_TPL[] PROGMEM = "<div class=\"jumbotron col-sm-10 col-sm-offset-1\"><h1>" APP_NAME " versione " APP_VER "</h1><h4>Autore " APP_AUTHOR " " APP_RIGHT "</h4>{menu}</div>";
const char HTML_WIFI_MENU[] PROGMEM =  "<h5><a href=\"/setup/wifi\">Impostazioni Wi-Fi</a></h5>";
const char HTML_DEVICE_MENU[] PROGMEM = "<h5><a href=\"/setup/devices\">Impostazioni Dispositivi e tasti</a></h5>";

// Warninig message for Wifi settings
const char HTML_WIFISAVE_WARNING[] PROGMEM = "<p>La modifica a queste impostazioni provoca la temporanea disconnessione del dispositivo.<br>Se le informazioni inserite non risultano valide il dispositivo si avvierà in modalità configurazione base per consentirne la correzione</p>";

// Root page header template
const char HTML_PAGE_HEADER[] PROGMEM = "<div class=\"container\"><div class=\"row\"><div class=\"col-sm-12 col-lg-12\"><h1>{btext}<small>{stext}</small></h1></div></div></div>";

// Wifi settings form
const char HTML_FULL_WIFI_FORM[] PROGMEM = "<div class=\"row\"><div class=\"col-sm-10 col-sm-offset-1\"><form name=\"wifidata\" action=\"/setup/savewifi\" method=\"POST\" class=\"form-horizontal\"><div class=\"panel panel-primary\"><div class=\"panel-heading\"><h3 class=\"panel-title\">Dati rete senza fili (Wifi)</h3>{warn}</div><div class=\"panel-body\"><div class=\"form-group col-sm-6\"><label for=\"ssid\" class=\"col-sm-3 control-label\">Nome rete Wifi</label><div class=\"col-sm-9\"><select id=\"ssid\" name=\"ssid\" value=\"{ssid}\"><option>Attendere caricamento...</option></select><button type=\"button\" onclick=\"getNetworkList()\">Rileva</button></div></div><div class=\"form-group col-sm-6\"><label for=\"password\" class=\"col-sm-3 control-label\">Password</label><div class=\"col-sm-9\"><input type=\"password\" class=\"form-control\" id=\"password\" name=\"password\" value=\"{password}\"></div></div></div><div class=\"panel-footer clearfix\"><div class=\"pull-left\"><a href=\"/\" class=\"btn btn-primary\">Menu</a></div><div class=\"pull-right\"><button type=\"submit\" class=\"btn btn-warning\">Salva</button></div></div></div></form></div></div>";
const char HTML_SIMPLE_WIFI_FORM[] PROGMEM = "{warn}<style type=\"text/css\">label{width:10em;display:inline-block;}</style><form name=\"wifidata\" action=\"/setup/savewifi\" method=\"POST\"><div><h3>Dati rete Wifi</h3><label for=\"ssid\"> Nome rete Wifi:</label><select id=\"ssid\" name=\"ssid\" value=\"{ssid}\"><option>Attendere caricamento...</option></select><button type=\"button\" onclick=\"getNetworkList()\">Rileva</button><br/><label for=\"password\">Password:</label><input type=\"password\" id=\"password\" name=\"password\" value=\"{password}\"></div><div><button type=\"submit\">Salva</button></div></form><a href=\"/\">Torna al menu</a>";

// Alert message
const char HTML_FULL_ALERT_TPL[] PROGMEM = "<div class=\"alert alert-{class}\" role=\"alert\"><button type=\"button\" class=\"close\" data-dismiss=\"alert\" aria-label=\"Chiudi\"><span aria-hidden=\"true\">×</span></button>{message}</div>";
const char HTML_SIMPLE_ALERT_TPL[] PROGMEM = "<div class=\"alert alert-{class}\">{message}</div>";

// Device panel
const char HTML_DEV_PANEL[] PROGMEM = "<div class=\"panel panel-warning col-sm-10 col-sm-offset-1\"><div class=\"panel-heading row\"><div class=\"col-xs-8\"><h3 class=\"panel-title\">Dispositivi</h3></div><div class=\"col-xs-4\"><div class=\"btn-group btn-group-sm  pull-right\" role=\"group\"><button type=\"button\" class=\"btn btn-primary\" data-action=\"new-device\" data-toggle=\"modal\" data-target=\"#device-edit\"><span class=\"glyphicon glyphicon-plus\" aria-hidden=\"true\"></span></button><button type=\"button\" class=\"btn btn-success \" data-action=\"edit-device\" data-toggle=\"modal\" data-target=\"#device-edit\" disabled><span class=\"glyphicon glyphicon-pencil\" aria-hidden=\"true\"></span></button><button type=\"button\" class=\"btn btn-danger\" data-action=\"device-delete\" data-toggle=\"modal\" data-target=\"#device-delete\" disabled><span class=\"glyphicon glyphicon-trash\" aria-hidden=\"true\"></span></button></div></div></div><div class=\"panel-body\">{panel_body}</span></button></div></div>";
const char HTML_DEV_LIST_TPL[] PROGMEM = "<div class=\"row\"><div class=\"col-md-3 col-sm-3 col-xs-3\"><ul class=\"nav nav-pills nav-stacked\" id=\"devices-list\">{dev_li_list}</ul></div><div class=\"col-md-9 col-sm-9 col-xs-9\"><div id=\"device-info\"class=\"tab-content\">{dev_pane_list}</div></div></div>";
const char HTML_DEV_LI_TPL[] PROGMEM = "<li><a href=\"#device_{dev_name}\" data-toggle=\"pill\">{dev_name}</a></li>";
const char HTML_DEV_PANE_TPL[] PROGMEM = "<div class=\"tab-pane\" id=\"device_{dev_name}\"><input type=\"hidden\" name=\"device_name\" value=\"\"><input type=\"hidden\" name=\"device_type\" value=\"\"><div class=\"table-responsive\"><style>.btn-outline{background-color:transparent;border-color:none;}</style><table class=\"table\"><caption><button type=\"button\" class=\"btn btn-outline\" data-action=\"new-device-key\" data-toggle=\"modal\" data-target=\"#device-key-edit\"><span class=\"glyphicon glyphicon-plus-sign\"></span> Aggiungi tasto</button></div></caption><thead class=\"device-keys-header\"></thead><tbody class=\"device-keys-list\"></tbody></table></div></div>";
const char HTML_DEV_PANE_RF433_TPL[] PROGMEM = "<table id=\"rf433_dev_pane\" class=\"hidden\"><thead class=\"pane_header\"><tr><th class=\"toolbar\"></th><th class=\"col-sm-5\">Nome</th><th>L. Codice</th><th>Codice</th></tr></thead><tbody class=\"pane_body\"><tr class=\"key-row\"><td class=\"text-nowrap\"><button type=\"button\" class=\"btn btn btn-outline\" data-action=\"device-key-delete\" data-toggle=\"modal\" data-target=\"#device-key-delete\"><span class=\"glyphicon glyphicon-minus-sign\"></span></button></td><td class=\"device_key_name\">{name}</td><td class=\"device_key_length\">{length}</td><td class=\"device_key_code\">{code}</td></tr></tbody></table>";

// Modal
const char HTML_MODAL_TPL[] PROGMEM = "<div class=\"modal fade\" id=\"{id}\" tabindex=\"-1\" role=\"dialog\" aria-labelledby=\"{id}\"><div class=\"modal-dialog\" role=\"document\"><div class=\"modal-content\"><div class=\"modal-header\"><button type=\"button\" class=\"close\" data-dismiss=\"modal\" aria-label=\"Annulla\"><span aria-hidden=\"true\">&times;</span></button><h4 class=\"modal-title\" id=\"{id}-title\">{title}</h4></div><div id=\"{id}-body\" class=\"modal-body\">{body}</div><div class=\"modal-footer\"><button type=\"button\" class=\"btn btn-default\" data-dismiss=\"modal\">Annulla</button><button type=\"button\" class=\"btn btn-primary\" onclick=\"return {ok-callback}()\">Conferma</button></div></div></div></div>";

// Form device edit
const char HTML_FORM_EDIT_DEVICE[] PROGMEM = "<form id=\"edit-device-form\" class=\"form-horizontal\" role=\"form\" action=\"javascript:0\"><div class=\"form-group\"><label class=\"col-sm-2 control-label\">Nome</label><div class=\"col-sm-10\"><input type=\"text\" class=\"form-control\" placeholder=\"Nome dispositivo\" name=\"device_name\" pattern=\"^[\\w+]{1,16}$\" title=\"Inserire un nome valido, max 16 caratteri\" required/></div></div><div class=\"form-group\"><label class=\"col-sm-2 control-label\">Tipo</label><div class=\"col-sm-10\"><select name=\"device_type\" class=\"form-control\" required>{device_types}</select></div></div><input type=\"hidden\" name=\"current_device_name\"/></form>";
// From device key edit
const char HTML_FORM_EDIT_RF433_KEY[] PROGMEM = "<form class=\"hidden form-horizontal\" id=\"edit-rf433-key-form\" action=\"javascript:0\"><div class=\"form-group\"><label class=\"col-sm-4 control-label\">Tasto</label><div class=\"col-sm-8\"><input type=\"text\" class=\"form-control\" placeholder=\"Nome tasto\" name=\"device_key_name\" pattern=\"^[\\w+]{1,10}$\" title=\"Inserire un nome di max. 10 caratteri.\" required/></div></div><div class=\"form-group\"><label class=\"col-sm-4 control-label\">Dim. Codice</label><div class=\"col-sm-6\"><input type=\"number\" class=\"form-control\" name=\"device_key_length\" pattern=\"^[0-9]+{1,10}$\" title=\"Inserire un numero intero.\" required/></div><div class=\"col-sm-2\"><button type=\"button\" onclick=\"return detectKeyData(this)\" class=\"btn btn-success\" title=\"Rileva automaticamente\"><span class=\"glyphicon glyphicon-refresh\"></span></button></div></div><div class=\"form-group\"><label class=\"col-sm-4 control-label\">Codice</label><div class=\"col-sm-6\"><input type=\"number\" class=\"form-control\" name=\"device_key_code\" pattern=\"^[0-9]+{1,10}$\" title=\"Inserire un numero intero.\" required/></div></div><input type=\"hidden\" name=\"device_name\"/><input type=\"hidden\" name=\"device_type\"/></form>";
// HTML Spare
const char HTML_OPTION_TPL[] PROGMEM = "<option value=\"{value}\">{name}</option>";

// Javascript functionts
const char JAVASCRIPT_NETLIST[] PROGMEM = "function getNetworkList(){var a=new XMLHttpRequest;a.onreadystatechange=function(){if(a.readyState==XMLHttpRequest.DONE)if(200==a.status){var b=document.getElementById(\"ssid\");for(v=b.getAttribute(\"value\"),b;b.firstChild;)b.removeChild(b.firstChild);try{for(var c=JSON.parse(a.responseText),d=c.networks,e=0;e<d.length;e++){var f=document.createElement(\"option\");f.value=d[e].name,f.text=d[e].name,\"\"!=d[e].protection&&(f.text+=\"(*)\"),f.text+=\" db: \"+d[e].quality,b.appendChild(f),d[e].name==v&&(f.selected=!0)}}catch(a){alert(\"Errore risposta server\"+a)}}else 400==a.status?alert(\"Errore 400 richiesta non valida\"):alert(\"Errore di comunicazione con il server\")},a.open(\"GET\",\"/setup/wifilist\",!0),a.send()}document.addEventListener(\"DOMContentLoaded\",function(a){getNetworkList()});";
const char JAVASCRIPT_DEVICE_ONLOAD[] PROGMEM =
  "document.addEventListener(\"DOMContentLoaded\", function(a) {\n"
  "    $(\"#device-edit\").on(\"shown.bs.modal\", function(a) {\n"
  "        var b = $(a.relatedTarget).data(\"action\");\n"
  "        var dev_name = $(\"#edit-device-form input[name='device_name']\");\n"
  "        var dev_type = $(\"#edit-device-form input[name='device_type']\");\n"
  "        var c_dev_name = $(\"#edit-device-form input[name='current_device_name']\");\n"
  "        if (\"new-device\" == b) {\n"
  "            dev_name.val(\"\");\n"
  "            dev_type.val(\"\");\n"
  "        } else if (\"edit-device\" == b) {\n"
  "            var c = $(\"#devices-list\").find(\"li.active a\").attr(\"href\"),\n"
  "                d = $(c).children(\"input[name=device_name]\").eq(0).val(),\n"
  "                e = $(c).children(\"input[name=device_type]\").eq(0).val();\n"
  "            dev_name.val(d), c_dev_name.val(d), dev_type.val(e);\n"
  "        }\n"
  "    }),\n"
  "    $(\"#device-edit\").on(\"hidden.bs.modal\", function() {\n"
  "        $(\"#edit-device-form input[name='device_name']\")[0].setCustomValidity(\"\"),\n"
  "        $(this).data(\"bs.modal\", null)\n"
  "    }),\n"
  "    $(\"#edit-device-form input[name='device_name']\").on(\"input\", function() {\n"
  "        var a = $(\"#edit-device-form input[name='current_device_name']\").val().trim(),\n"
  "            b = $(this).val().trim();\n"
  "        if (!a || b != a) {\n"
  "            var c = !1;\n"
  "            $(\"#devices-list li a\").each(function(a) {\n"
  "                return c = $(this).text() == b, !c\n"
  "            }), this.setCustomValidity(c ? \"Il nome del dispositivo è già in uso\" : \"\")\n"
  "        }\n"
  "    }), $(\"#devices-list a[data-toggle=pill]\").on(\"shown.bs.tab\", show_device_info), $(\"#device-delete\").on(\"shown.bs.modal\", function() {\n"
  "        $(\"#device-delete\").find(\"b.device-name\").text($(\"#devices-list\").find(\"li.active a\").attr(\"href\").substring(8))\n"
  "    }), $(\"#device-key-edit\").on(\"shown.bs.modal\", function() {\n"
  "        var a = $(\"#devices-list\").find(\"li.active a\").attr(\"href\"),\n"
  "            b = $(a).children(\"input[name=device_name]\").eq(0).val(),\n"
  "            c = $(a).children(\"input[name=device_type]\").eq(0).val();\n"
  "            var form  = $(\"#edit-\"+ c.toLowerCase() +\"-key-form\").clone();\n"
  "            form.attr('id','');\n"
  "            $(\"#device-key-edit-body\").empty();\n"
  "            $(\"#device-key-edit-body\").append(form);\n"
  "            form.children(\"input[name=device_name]\").val(b);\n"
  "            form.children(\"input[name=device_type]\").val(c);\n"
  "            form.children(\"input[name=device_key_name]\").on(\"input\", function() {\n"
  "                var kname = $(this).val();\n"
  "                var found = false;\n"
  "                $(a).find(\"tbody.device-keys-list tr.key-row\").children(\":nth-child(2)\").each(function() {\n"
  "                    found = $(this).text() == kname;\n"
  "                    return !found;\n"
  "                });\n"
  "                this.setCustomValidity( found ? \"Il nome del tasto è già in uso\" : \"\");\n"
  "            });\n"
  "            form.removeClass('hidden');\n"
  "    }), $(\"#device-key-delete\").on(\"shown.bs.modal\", function(a) {\n"
  "        var b = $(\"#device-key-delete\");\n"
  "        b.find(\"i.device-name\").text($(\"#devices-list\").find(\"li.active a\").attr(\"href\").substring(8)),\n"
  "        b.find(\"b.device-key-name\").text($(a.relatedTarget).closest(\"tr.key-row\").children(\"td:eq(1)\").text().trim())\n"
  "    });\n"
  "});\n";

const char JAVASCRIPT_DEVICE_FUNCTIONS[] PROGMEM = "<script src=\"/js/devicesFunc.js\"></script>";

class Portal {

  public:

    Portal();
    boolean setup();
    void handleRequest();

    boolean needRestart();

  private:
    boolean ap_mode = false;
    boolean need_restart = false;
    DeviceHandler dh;
    String message;

    String getDevPanel();
    String prepareDeviceForm();

    std::unique_ptr<DNSServer> dnsServer;
    std::unique_ptr<ESP8266WebServer> server;
    MDNSResponder mdns;

    void setWifi();

    void handleSendKey();
    void handleRootPage();
    void handleNotFound();
    void handleSaveWifi();
    void handleSetupWifi();
    void handleEditDevice();
    void handleSetupDevices();
    void handleDeleteDevice();
    void handleGetDeviceInfo();
    void handleEditDeviceKey();
    void handleAcquireKeyData();
    void handleDeleteDeviceKey();
    void redirectHeaders(String where);

    void handleWifiList();

    String basePage();
    String getMessageTpl();
    boolean redirect();

    String prepareModal(String const &id, String const &title, String const &body, String const &ok_callback = "");

    String getDeviceData(Device &d);
    String getDeviceKeyAttrs(Device &d);
    String getMainMenu();

    static String keyAttrToJson(int num, Key * key);

};

#endif



