/*
  Setup.h - Handle setup setting
  Saul bertuccio 5 feb 2017
  Released into the public domain.
*/

#ifndef Setup_h
#define Setup_h

#include "Arduino.h"

class Setup {
  
  const char HTTP_HTML[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><meta charset=\"utf-8\"><title>Remote: impostazioni</title>    <link href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" rel=\"stylesheet\">  </head>  <body>    <h2>Remote - Configurazione di base</h2>    <input id=\"devicemax\" type=\"hidden\" value=\"{devicemax}\">    <input id=\"devicenum\" type=\"hidden\" value=\"{devicenum}\">    {form}    {empty_device}    <script src=\"https://code.jquery.com/jquery-3.1.1.slim.min.js\"></script>    <script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\"></script>    <script>{script}</script>  </body></html>"
  const char HTTP_FORM[] PROGMEM   = "<form cla ss=\"form-horizontal col-sm-10 col-sm-offset-1\"><div class=\"panel panel-primary\"><div class=\"panel-heading\"><h3 class=\"panel-title\">Dati rete WIFY</h3></div><div class=\"panel-body\"><div class=\"form-group col-sm-6\"><label for=\"essid\" class=\"col-sm-3 control-label\">Nome rete Wify</label><div class=\"col-sm-9\"><input type=\"essid\" class=\"form-control\" id=\"essid\" placeholder=\"essid\" value=\"{essid}\"></div></div><div class=\"form-group col-sm-6\"><label for=\"password\" class=\"col-sm-3 control-label\">Password</label><div class=\"col-sm-9\"><input type=\"password\" class=\"form-control\" id=\"password\" placeholder=\"password\" value=\"{password}\"></div></div></div></div><div class=\"panel panel-info\"><div class=\"panel-heading clearfix\"><button id=\"adddevice\" type=\"button\" class=\"btn btn-success hidden pull-right\">Aggiungi</button><h3 class=\"panel-title\">Elenco dispositivi</h3></div><div class=\"panel-body\"><div id=\"devicelist\" class=\"col-sm-12\"></div></div></div><div class=\"form-group\"><div class=\"col-sm-offset-2 col-sm-10\"><button type=\"submit\" class=\"btn btn-default\">Sign in</button></div></div></form>";
  const char HTTP_DEVICE[] PROGMEM = "<div id=\"empty_device\" class=\"hidden\"><div class=\"panel panel-primary\"><div class=\"panel-heading clearfix\"><button type=\"button\" class=\"btn btn-warning btn-xs pull-right delete-device\">Elimina</button><h4 class=\"panel-title\">Dispositivo {num}</h4></div><div class=\"panel-body\"><div class=\"form-group\"><label class=\"col-sm-3 control-label\">Nome</label><div class=\"col-sm-9\"><input type=\"text\" class=\"form-control device-name\" placeholder=\"nome\" value=\"{name}\" name=\"name[]\"></div></div><div class=\"form-group\"><label class=\"col-sm-3 control-label\">Impulso (microsecondi)</label><div class=\"col-sm-9\"><input type=\"number\" class=\"form-control\" placeholder=\"millisec\" value=\"322\" name=\"pulse[]\"></div></div><div class=\"form-group\"><label class=\"col-sm-3 control-label\">Codice accensione</label><div class=\"col-sm-9\"><input type=\"number\" class=\"form-control\" value=\"{on_code}\" name=\"on_code[]\"></div></div><div class=\"form-group\"><label class=\"col-sm-3 control-label\">Codice spegnimento</label><div class=\"col-sm-9\"><input type=\"number\" class=\"form-control\" value=\"{off_code}\" name=\"on_code[]\"></div></div></div></div></div>" 
  const char HTTP_SCRIPT[] PROGMEM = "$(function(){var a=function(){return+$(\"#devicemax\").val()<=+$(\"#devicenum\").val()},b=function(){$(\"#devicenum\").val(function(a,b){return++b})},c=function(){$(\"#devicenum\").val(function(a,b){return--b})},d=function(){$(this).closest(\".panel .panel-primary\").remove(),c()},e=function(){if(a())$(\"#adddevice\").addClass(\"hidden\");else{b();var c=$(\"#empty_device\").children().clone(!0,!0);c.find(\"button.delete-device\").click(d);var e=c.find(\".panel-title\"),f=c.find(\".device-name\");e.text(e.text().replace(\"{num}\",$(\"#devicenum\").val())),f.val(e.text()),$(\"#devicelist\").append(c)}};a()||($(\"#adddevice\").removeClass(\"hidden\"),$(\"#adddevice\").click(e))});"

  public:
    Setup();

  private:
    int b;
};

#endif

