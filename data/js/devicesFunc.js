/* Disattiva toolbar */
var disable_tool_button = function() {
    $('button[data-action="device-delete"]').prop( 'disabled', false );
    $('button[data-action="edit-device"]').prop( 'disabled', false );
}

var enable_tool_button = function() {
    $('button[data-action="device-delete"]').prop( 'disabled', false );
    $('button[data-action="edit-device"]').prop( 'disabled', false );
}

var show_device_info = function (e) {
    var sel_dev = e.target; // activated tab

    // l'id del div che inizia per #device_
    var device_div = $(sel_dev).attr('href');
    var dev_input_name = $(device_div).children('input[name=device_name]').eq(0);
    var dev_input_type = $(device_div).children('input[name=device_type]').eq(0);
    if ( dev_input_name.val() != '' && dev_input_type.val() != '') {
        enable_tool_button();
        return;
    };
    $.ajax({
      dataType: 'json',
      url: '/setup/getDeviceInfo',
      data: { device_name : device_div.substring(8)},
      success: function(d) {
        if (d.error) {
            alert(d.message);
            return;
        }
        dev_input_name.val(d.device.name);
        dev_input_type.val(d.device.type);
        var keys = d.device.keys;
        if (keys && keys.length) {
            for (var k=0; k < keys.length; k++ ){
                var tpl = setupKeyRowTemplate($(device_div),{
                    device_key_name : keys[k].name,
                    device_key_pulse : keys[k].pulse,
                    device_key_code : keys[k].code,
                });
                $(device_div).find("tbody.device-keys-list").append(tpl);
            }
        }
         enable_tool_button();
      },
      error : function() {alert('Errore di comunicazione con il server');}
    });
}

function saveDeviceData() {

    var form = $('.edit-device-form');
    if (!form[0].checkValidity()) {
        // submitting form to show error message
        $('<input type="submit">').hide().appendTo(form).click().remove();
        return false;
    }

    var device_name = $('#frm_edit_dev_name').val().trim();
    var curr_name = $('#frm_edit_curr_dev_name').val().trim();
    var device_type = $('#frm_edit_dev_type').val().trim();

    var new_device_name = null;

    if (curr_name && curr_name != device_name) {
        new_device_name = device_name;
        device_name = curr_name;
    }

    var param = {
        device_name: device_name,
        device_type: device_type
    };

    if (new_device_name)
        param['new_device_name'] = new_device_name;

    $.ajax({
        method: 'POST',
        url : '/setup/editDevice',
        data : param,
        dataType : 'json',
        beforeSend : function() { $('#device-edit').modal('hide'); return true; },
        success : function(data) {
            if (data.error == true){
                alert(data.message);
            } else {
                var dev_a = $('#devices-list').find('a[href="#device_' + device_name + '"]');
                dev_a.on('shown.bs.tab', show_device_info );
                var dev_pane = $('#device_' + device_name);
                if (!dev_a.length) {
                    var dev_tab = $('#device-template').children().eq(0).clone();
                    dev_tab.appendTo($('#devices-list'));
                    dev_a = $(':first-child', dev_tab);
                }
                if (!dev_pane.length) {
                    dev_pane = $('#device-template').children().eq(1).clone();
                    dev_pane.appendTo($('#device-info'))
                }
                var actual_name = (new_device_name ? new_device_name : device_name);
                dev_a.attr('href', '#device_' + actual_name);
                dev_a.text(actual_name);
                dev_pane.attr('id','device_' + actual_name);
                dev_pane.children().eq(0).val(actual_name);
                dev_pane.children().eq(1).val(device_type);
                $('.no-device-alert').addClass('hidden');
            }
        },
        error : function () { alert ('Errore di comunicazione con il server.');}
    });
}


function deleteDevice() {
    var dev_name = $('#device-delete').find('b.device-name').text();
    $.ajax({
      dataType: 'json',
      url: '/setup/deleteDevice',
      data: { device_name : dev_name},
      beforeSend : function() { $('#device-delete').modal('hide'); return true; },
      success: function(d) {
        if (d.error) {
            alert(d.message);
            return;
        }
        var dev_id = '#device_' + dev_name;
        $('#devices-list').find('a[href="' + dev_id + '"]').parent().remove();
        disable_tool_button();
        $(dev_id).remove();
        if ($('#devices-list').find('li').length == 0)
             $('.no-device-alert').removeClass('hidden');

      },
      error : function() {alert('Errore di comunicazione con il server');}
    });

}


function detectKeyData() {
    $.ajax({
        method: 'POST',
        url : '/detectKeyValue',
        data : { type : $('#frm_edit_dev_key_dev_type').val() },
        dataType : 'json',
        success : function(data) {
            if (data.error == true){
                alert(data.message);
            } else {
                $("#frm_edit_dev_key_pulse").val(data.pulse);
                $("#frm_edit_dev_key_code").val(data.code);
            }
        }
    });
}

function setupKeyRowTemplate(dev_pane, kdata) {
    var tpl = dev_pane.find("tbody.device-keys-list").find(".key-row-template").clone();
    tpl.removeClass('key-row-template hidden');
    tpl.addClass('key-row');
    tpl.find('td:eq(1)').text(kdata.device_key_name);
    tpl.find('td:eq(2)').text(kdata.device_key_pulse);
    tpl.find('td:eq(3)').text(kdata.device_key_code);
    tpl.find('td:eq(0) button').click(deleteDeviceKey);
    return tpl;
}

function saveDeviceKey() {

    var form = $('.edit-device-key-form');
    if (!form[0].checkValidity()) {
        // submitting form to show error message
        $('<input type="submit">').hide().appendTo(form).click().remove();
        return false;
    }

    var param = {
        device_name: $('#frm_edit_dev_key_dev_name').val().trim(),
        device_key_name : $("#frm_edit_dev_key_name").val().trim(),
        device_key_pulse : $("#frm_edit_dev_key_pulse").val().trim(),
        device_key_code : $("#frm_edit_dev_key_code").val().trim()
    };

    $.ajax({
        method: 'POST',
        url : '/setup/editDeviceKey',
        data : param,
        dataType : 'json',
        beforeSend : function() { $('#device-key-edit').modal('hide'); return true; },
        success : function(data) {
            if (data.error == true){
                alert(data.message);
            } else {
                var dev_pane_id = $('#devices-list').find('li.active a').attr('href');
                var tbody = $(dev_pane_id).find("tbody.device-keys-list");
                tbody.append(setupKeyRowTemplate($(dev_pane_id), param));
            }
        },
        error : function () { alert ('Errore di comunicazione con il server.');}
    });
}

function deleteDeviceKey() {
    var modal = $('#device-key-delete');
    var key_name = modal.find("b.device-key-name").text().trim();
    var dev_name = modal.find('i.device-name').text().trim();
    $.ajax({
      dataType: 'json',
      url: '/setup/deleteDeviceKey',
      data: { device_name : dev_name, device_key_name : key_name },
      beforeSend : function() { modal.modal('hide'); return true; },
      success: function(d) {
        if (d.error) {
            alert(d.message);
            return;
        }
        var dev_id = '#device_' + dev_name;
        $(dev_id).find('tbody tr.key-row').each(function() {
            var kname = $(this).children(':eq(1)').text().trim();
            if (kname == key_name) {
                $(this).remove();
                return false;
            }
        });
      },
      error : function() {alert('Errore di comunicazione con il server');}
    });
}