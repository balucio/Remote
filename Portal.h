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
const char HTML_FULL_TPL[] PROGMEM = "<!DOCTYPE html><html><head><meta name=viewport content=\"width=device-width, initial-scale=1\"><meta charset=utf-8><title>{title}</title><link href=http://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css rel=stylesheet></head><body class=\"bg-info\">{body}<div>{message}</div><script src=http://code.jquery.com/jquery-3.1.1.min.js></script><script src=http://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js></script><script type=\"text/javascript\">{script}</script></body></html>";
const char HTML_SIMPLE_TPL[] PROGMEM = "<!DOCTYPE html><html><head><meta name=viewport content=\"width=device-width, initial-scale=1\"><meta charset=utf-8><title>{title}</title><body class=\"bg-info\" style=\"font-family:'Lucida Grande',Tahoma,Arial,Verdana,sans-serif;font-size:small\">{body}<div>{message}</div><script type=\"text/javascript\">{script}</script></body></html>";

// Body for root page
const char HTML_ROOT_BODY[] PROGMEM = "<div class=\"jumbotron\"><h1>" APP_NAME " versione " APP_VER "</h1><h4>Autore " APP_AUTHOR " " APP_RIGHT "</h4><p><a href=\"/setup\">Impostazioni</a></p></div>";


// Warninig message for Wifi settings
const char HTML_WIFISAVE_WARNING[] PROGMEM = "<p>La modifica a queste impostazioni provoca la temporanea disconnessione del dispositivo.<br>Se le informazioni inserite non risultano valide il dispositivo si avvierà in modalità configurazione base per consentirne la correzione</p>";

// Root page header template
const char HTML_PAGE_HEADER[] PROGMEM ="<div class=\"container\"><div class=\"row\"><div class=\"col-sm-12 col-lg-12\"><h1>{btext}<small>{stext}</small></h1></div></div></div>";

// Wifi settings form
const char HTML_FULL_WIFI_FORM[] PROGMEM = "<div class=\"row\"><div class=\"col-sm-10 col-sm-offset-1\"><form name=\"wifidata\" action=\"/setup/savewifi\" method=\"POST\" class=\"form-horizontal\"><div class=\"panel panel-primary\"><div class=\"panel-heading\"><h3 class=\"panel-title\">Dati rete senza fili (Wifi)</h3>{warn}</div><div class=\"panel-body\"><div class=\"form-group col-sm-6\"><label for=\"ssid\" class=\"col-sm-3 control-label\">Nome rete Wifi</label><div class=\"col-sm-9\"><select id=\"ssid\" name=\"ssid\" value=\"{ssid}\"><option>Attendere caricamento...</option></select><button type=\"button\" onclick=\"getNetworkList()\">Rileva</button></div></div><div class=\"form-group col-sm-6\"><label for=\"password\" class=\"col-sm-3 control-label\">Password</label><div class=\"col-sm-9\"><input type=\"password\" class=\"form-control\" id=\"password\" name=\"password\" value=\"{password}\"></div></div></div><div class=\"panel-footer clearfix\"><div class=\"pull-right\"><button type=\"submit\" class=\"btn btn-warning\">Salva</button></div></div></div></form></div></div>{dev_list}";
const char HTML_SIMPLE_WIFI_FORM[] PROGMEM = "{warn}<style type=\"text/css\">label{width:10em;display:inline-block;}</style><form name=\"wifidata\" action=\"/setup/savewifi\" method=\"POST\"><div><h3>Dati rete Wifi</h3><label for=\"ssid\"> Nome rete Wifi:</label><select id=\"ssid\" name=\"ssid\" value=\"{ssid}\"><option>Attendere caricamento...</option></select><button type=\"button\" onclick=\"getNetworkList()\">Rileva</button><br/><label for=\"password\">Password:</label><input type=\"password\" id=\"password\" name=\"password\" value=\"{password}\"></div><div><button type=\"submit\">Salva</button></div></form>";

// Alert message
const char HTML_FULL_ALERT_TPL[] PROGMEM = "<div class=\"alert alert-{class}\" role=\"alert\"><button type=\"button\" class=\"close\" data-dismiss=\"alert\" aria-label=\"Chiudi\"><span aria-hidden=\"true\">×</span></button>{message}</div>";
const char HTML_SIMPLE_ALERT_TPL[] PROGMEM = "<div class=\"alert alert-{class}\">{message}</div>";

// Device panel
const char HTML_DEV_PANEL[] PROGMEM = "<div class=\"panel panel-warning col-sm-10 col-sm-offset-1\"><div class=\"panel-heading row\"><div class=\"col-xs-8\"><h3 class=\"panel-title\">Dispositivi</h3></div><div class=\"col-xs-4\"><div class=\"btn-group btn-group-sm  pull-right\" role=\"group\"><button type=\"button\" class=\"btn btn-primary\" data-action=\"new-device\" data-toggle=\"modal\" data-target=\"#device-edit\"><span class=\"glyphicon glyphicon-plus\" aria-hidden=\"true\"></span></button><button type=\"button\" class=\"btn btn-success \" data-action=\"edit-device\" data-toggle=\"modal\" data-target=\"#device-edit\" disabled><span class=\"glyphicon glyphicon-pencil\" aria-hidden=\"true\"></span></button><button type=\"button\" class=\"btn btn-danger\" data-action=\"device-delete\" data-toggle=\"modal\" data-target=\"#device-delete\" disabled><span class=\"glyphicon glyphicon-trash\" aria-hidden=\"true\"></span></button></div></div></div><div class=\"panel-body\">{panel_body}</span></button></div></div>";
const char HTML_DEV_LIST_TPL[] PROGMEM ="<div class=\"row\"><div class=\"col-md-3 col-sm-3 col-xs-3\"><ul class=\"nav nav-pills nav-stacked\" id=\"devices-list\">{dev_li_list}</ul></div><div class=\"col-md-9 col-sm-9 col-xs-9\"><div id=\"device-info\"class=\"tab-content\">{dev_pane_list}</div></div></div>";  
const char HTML_DEV_LI_TPL[] PROGMEM = "<li><a href=\"#device_{dev_name}\" data-toggle=\"pill\">{dev_name}</a></li>";
const char HTML_DEV_PANE_TPL[] PROGMEM = "<div class=\"tab-pane\" id=\"device_{dev_name}\"><input type=\"hidden\" name=\"device_name\" value=\"{dev_name}\"><input type=\"hidden\" name=\"device_type\" value=\"{dev_type}\"><div class=\"table-responsive\"><style>.btn-outline{background-color:transparent;border-color:none;}</style><table class=\"table\"><caption><b>Elenco tasti configurati</b></caption><thead><tr><th><button type=\"button\" class=\"btn btn-outline\" data-action=\"new-device-key\" data-toggle=\"modal\" data-target=\"#device-key-edit\"><span class=\"glyphicon glyphicon-plus-sign\"></span></button></th><th class=\"col-sm-5\">Nome</th><th>Impulso</th><th>Codice</th></tr></thead><tbody class=\"device-keys-list\"><tr class=\"key-row\"></tr><tr class=\"hidden key-row-template\"><td class=\"text-nowrap\"><button type=\"button\" class=\"btn btn-outline\" data-action=\"edit-device-key\" data-toggle=\"modal\" data-target=\"#device-key-edit\"><span class=\"glyphicon glyphicon-info-sign\"></span></button><button type=\"button\" class=\"btn btn btn-outline\" data-action=\"device-key-delete\" data-toggle=\"modal\" data-target=\"#device-key-delete\" disabled><span class=\"glyphicon glyphicon-minus-sign\"></span></button></td><td>{name}</td><td>{pulse}</td><td>{code}</td></tr></tbody></table></div></div>";

// Modal 
const char HTML_MODAL_TPL[] PROGMEM = "<div class=\"modal fade\" id=\"{id}\" tabindex=\"-1\" role=\"dialog\" aria-labelledby=\"{id}\"><div class=\"modal-dialog\" role=\"document\"><div class=\"modal-content\"><div class=\"modal-header\"><button type=\"button\" class=\"close\" data-dismiss=\"modal\" aria-label=\"Annulla\"><span aria-hidden=\"true\">&times;</span></button><h4 class=\"modal-title\" id=\"{id}-title\">{title}</h4></div><div class=\"modal-body\">{body}</div><div class=\"modal-footer\"><button type=\"button\" class=\"btn btn-default\" data-dismiss=\"modal\">Annulla</button><button type=\"button\" class=\"btn btn-primary\" onclick=\"return {ok-callback}()\">Conferma</button></div></div></div></div>";

// Form device name
const char HTML_FORM_EDIT_DEVICE[] PROGMEM = "<form class=\"form-horizontal edit-device-form\" role=\"form\" action=\"javascript:0\"><div class=\"form-group\"><label class=\"col-sm-2 control-label\" for=\"frm_edit_dev_name\">Nome</label><div class=\"col-sm-10\"><input type=\"text\" class=\"form-control\" placeholder=\"Nome dispositivo\" id=\"frm_edit_dev_name\" pattern=\"^[\\w+]{1,16}$\" title=\"Inserire un nome valido, max 16 caratteri\" required/></div></div><div class=\"form-group\"><label class=\"col-sm-2 control-label\" for=\"frm_edit_dev_type\">Tipo</label><div class=\"col-sm-10\"><select id=\"frm_edit_dev_type\" class=\"form-control\" required>{device_types}</select></div></div><input type=\"hidden\" id=\"frm_edit_curr_dev_name\"/></form>";

// HTML Spare
const char HTML_OPTION_TPL[] PROGMEM = "<option value=\"{value}\">{name}</option>";

// Javascript functionts
const char JAVASCRIPT_NETLIST[] PROGMEM ="function getNetworkList(){var a=new XMLHttpRequest;a.onreadystatechange=function(){if(a.readyState==XMLHttpRequest.DONE)if(200==a.status){var b=document.getElementById(\"ssid\");for(v=b.getAttribute(\"value\"),b;b.firstChild;)b.removeChild(b.firstChild);try{for(var c=JSON.parse(a.responseText),d=c.networks,e=0;e<d.length;e++){var f=document.createElement(\"option\");f.value=d[e].name,f.text=d[e].name,\"\"!=d[e].protection&&(f.text+=\"(*)\"),f.text+=\" db: \"+d[e].quality,b.appendChild(f),d[e].name==v&&(f.selected=!0)}}catch(a){alert(\"Errore risposta server\"+a)}}else 400==a.status?alert(\"Errore 400 richiesta non valida\"):alert(\"Errore di comunicazione con il server\")},a.open(\"GET\",\"/setup/wifilist\",!0),a.send()}document.addEventListener(\"DOMContentLoaded\",function(a){getNetworkList()});";
const char JAVASCRIPT_DELETE_DEVICE[] PROGMEM = ""
"document.addEventListener('DOMContentLoaded', function(event) {\n"
"    $('#device-delete').on('shown.bs.modal', function() {\n"
"        $('#device-delete').find('b.device-name').text(\n"
"            $('#devices-list').find('li.active a').attr('href').substring(8)\n"
"        );\n"
"    });\n"
"});\n"
"\n"
"function deleteDevice() {\n"
"    var dev_name = $('#device-delete').find('b.device-name').text();\n"
"    $.ajax({\n"
"      dataType: 'json',\n"
"      url: '/setup/deleteDevice',\n"
"      data: { device_name : dev_name},\n"
"      beforeSend : function() { $('#device-delete').modal('hide'); return true; },\n"
"      success: function(d) {\n"
"        if (d.error) {\n"
"            alert(d.message);\n"
"            return;\n"
"        }\n"
"        var dev_id = '#device_' + dev_name;\n"
"        $('#devices-list').find('a[href=\"' + dev_id + '\"]').parent().remove();\n"
"        disable_tool_button();\n"
"        $(dev_id).remove();\n"
"        if ($('#devices-list').find('li').length == 0)\n"
"             $('.no-device-alert').removeClass('hidden');\n"
"\n"
"      },\n"
"      error : function() {alert('Errore di comunicazione con il server');}\n"
"    });\n"
"\n"
"}\n"
;
const char JAVASCRIPT_EDIT_DEVICE[] PROGMEM = ""
"document.addEventListener('DOMContentLoaded', function(event) {\n"
"\n"
"    /* Evento show su modale di edit/new device */\n"
"    $('#device-edit').on('shown.bs.modal', function(e) {\n"
"        var action = $(e.relatedTarget).data('action');\n"
"        if (action == 'new-device') {\n"
"            $('#frm_edit_dev_name').val('');\n"
"            $('#frm_edit_curr_dev_name').val('');\n"
"        } else if (action == 'edit-device') {\n"
"            var dev_pane_id = $('#devices-list').find('li.active a').attr('href');\n"
"            var dev_name = $(dev_pane_id).children('input[name=device_name]').eq(0).val();\n"
"            var dev_type = $(dev_pane_id).children('input[name=device_type]').eq(0).val();\n"
"            $('#frm_edit_dev_name').val(dev_name);\n"
"            $('#frm_edit_curr_dev_name').val(dev_name);\n"
"            $('#frm_edit_dev_type').val(dev_type);\n"
"        }\n"
"    });\n"
"\n"
"    /* Evento hide su modale edit/new */\n"
"    $('#device-edit').on('hidden.bs.modal', function () {\n"
"        $('#frm_edit_dev_name')[0].setCustomValidity('');\n"
"        $(this).data('bs.modal', null);\n"
"    });\n"
"\n"
"    /* Evento di input su widget dev name */\n"
"    $('#frm_edit_dev_name').on('input',  function() {\n"
"        var old_name = $('#frm_edit_curr_dev_name').val().trim();\n"
"        var new_name = $(this).val().trim();\n"
"\n"
"        if ((!old_name || new_name != old_name)) {\n"
"            var found = false;\n"
"            $('#devices-list li a').each(function( index ) {\n"
"                found = $( this ).text() == new_name;\n"
"                return !found;\n"
"            });\n"
"            this.setCustomValidity(found ? 'Il nome del dispositivo è già in uso' : '');\n"
"        }\n"
"    });\n"
"    /* Evento show su tab elenco device */\n"
"    $('#devices-list a[data-toggle=pill]').on('shown.bs.tab', show_device_info );\n"
"});\n"
"\n"
"/* Disattiva toolbar */\n"
"var disable_tool_button = function() {\n"
"    $('button[data-action=\"device-delete\"]').prop( 'disabled', true );\n"
"    $('button[data-action=\"edit-device\"]').prop( 'disabled', true );\n"
"}\n"
"\n"
"var enable_tool_button = function() {\n"
"    $('button[data-action=\"device-delete\"]').prop( 'disabled', false );\n"
"    $('button[data-action=\"edit-device\"]').prop( 'disabled', false );\n"
"}\n"
" var show_device_info = function (e) {\n"
"        var sel_dev = e.target; // activated tab\n"
"\n"
"        // l'id del div che inizia per #device_\n"
"        var device_div = $(sel_dev).attr('href');\n"
"        var dev_input_name = $(device_div).children('input[name=device_name]').eq(0);\n"
"        var dev_input_type = $(device_div).children('input[name=device_type]').eq(0);\n"
"        if ( dev_input_name.val() != '' && dev_input_type.val() != '') {\n"
"            enable_tool_button();\n"
"            return;\n"
"        };\n"
"        $.ajax({\n"
"          dataType: 'json',\n"
"          url: '/setup/getDeviceInfo',\n"
"          data: { device_name : device_div.substring(8)},\n"
"          success: function(d) {\n"
"            if (d.error) {\n"
"                alert(d.message);\n"
"                return;\n"
"            }\n"
"            dev_input_name.val(d.device.name);\n"
"            dev_input_type.val(d.device.type);\n"
"            enable_tool_button();\n"
"          },\n"
"          error : function() {alert('Errore di comunicazione con il server');}\n"
"        });\n"
"    }\n"
"\n"
"function saveDeviceData() {\n"
"\n"
"    var form = $('.edit-device-form');\n"
"    if (!form[0].checkValidity()) {\n"
"        // submitting form to show error message\n"
"        $('<input type=\"submit\">').hide().appendTo(form).click().remove();\n"
"        return false;\n"
"    }\n"
"\n"
"    var device_name = $('#frm_edit_dev_name').val().trim();\n"
"    var curr_name = $('#frm_edit_curr_dev_name').val().trim();\n"
"    var device_type = $('#frm_edit_dev_type').val().trim();\n"
"\n"
"    var new_device_name = null;\n"
"\n"
"    if (curr_name && curr_name != device_name) {\n"
"        new_device_name = device_name;\n"
"        device_name = curr_name;\n"
"    }\n"
"\n"
"    var param = {\n"
"        device_name: device_name,\n"
"        device_type: device_type\n"
"    };\n"
"\n"
"    if (new_device_name)\n"
"        param['new_device_name'] = new_device_name;\n"
"\n"
"    $.ajax({\n"
"        method: 'POST',\n"
"        url : '/setup/editDevice',\n"
"        data : param,\n"
"        dataType : 'json',\n"
"        beforeSend : function() { $('#device-edit').modal('hide'); return true; },\n"
"        success : function(data) {\n"
"            if (data.error == true){\n"
"                alert(data.message);\n"
"            } else {\n"
"                var dev_a = $('#devices-list').find('a[href=\"#device_' + device_name + '\"]');\n"
"                var dev_pane = $('#device_' + device_name);\n"
"                if (!dev_a.length) {\n"
"                    var dev_tab = $('#device-template').children().eq(0).clone();\n"
"                    dev_tab.appendTo($('#devices-list'));\n"
"                    dev_a = $(':first-child', dev_tab);\n"
"                }\n"
"                if (!dev_pane.length) {\n"
"                    dev_pane = $('#device-template').children().eq(1).clone();\n"
"                    dev_pane.appendTo($('#device-info'))\n"
"                }\n"
"                var actual_name = (new_device_name ? new_device_name : device_name);\n"
"                dev_a.attr('href', '#device_' + actual_name);\n"
"                dev_a.text(actual_name);\n"
"                dev_a.on('shown.bs.tab', show_device_info );\n"
"                dev_pane.attr('id','device_' + actual_name);\n"
"                dev_pane.children().eq(0).val(actual_name);\n"
"                dev_pane.children().eq(1).val(device_type);\n"
"                $('.no-device-alert').addClass('hidden');\n"
"            }\n"
"        },\n"
"        error : function () { alert ('Errore di comunicazione con il server.');}\n"
"    });\n"
"}\n"
;

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

    void handleRootPage();
    void handleNotFound();
    void handleSetup();
    void handleSaveWifi();
    void handleEditDevice();
    void handleGetDeviceInfo();
    void handleDeleteDevice();
    void redirectHeaders(String where);

    void handleWifiList();

    String basePage();
    String getMessageTpl();
    boolean redirect();

    String prepareModal(String const &id, String const &title, String const &body, String const &ok_callback ="");
    String perpareJavascript(boolean full);

    String getDeviceData(Device &device);
};

#endif


