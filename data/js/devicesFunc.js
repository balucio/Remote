/* Disattiva toolbar */
var disable_tool_button = function() {
    $('button[data-action="device-delete"]').prop( 'disabled', false );
    $('button[data-action="edit-device"]').prop( 'disabled', false );
}

var enable_tool_button = function() {
    $('button[data-action="device-delete"]').prop( 'disabled', false );
    $('button[data-action="edit-device"]').prop( 'disabled', false );
}

var get_table_key_section = function(dev_type, section) {

    var id = "#" + dev_type.toLowerCase() + "_dev_pane .pane_" + section;
    return $(id);
}

var render_table_row_data = function(tpl, key) {
    var row = tpl;
    for (var prop in key) {
        if (key.hasOwnProperty(prop)) {
            row = row.replace("{"+prop+"}",key[prop]);
        }
    }
    return row;
}

var show_device_info = function (e) {
    var sel_dev = e.target; // activated tab

    // l'id del div per #device_
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
            $(device_div)
                .find("thead.device-keys-header")
                .append(get_table_key_section(d.device.type, 'header').html());

            var tpl = get_table_key_section(d.device.type, 'body').html();
            var tbody = $(device_div).find("tbody.device-keys-list");
            for (var k=0; k < keys.length; k++ ){
                tbody.append(render_table_row_data(tpl,keys[k]));
            }
        }
        enable_tool_button();
      },
      error : function() {alert('Errore di comunicazione con il server');}
    });
}

var saveDeviceData = function() {

    var form = $('#edit-device-form');
    if (!form[0].checkValidity()) {
        // submitting form to show error message
        $('<input type="submit">').hide().appendTo(form).click().remove();
        return false;
    }

    var device_name = form.find('[name=device_name]').val().trim();
    var curr_name = form.find('[name=current_device_name]').val().trim();
    var device_type = form.find('[name=device_type]').val().trim();

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
        param.new_device_name = new_device_name;

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

var deleteDevice = function() {
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

var detectKeyData = function() {
    var form = $(this).closest('form');
    $.ajax({
        method: 'POST',
        url : '/detectKeyValue',
        data : { type : form.find('input[name=device_type]').val().trim() },
        dataType : 'json',
        error : function() {alert('Errore di comunicazione con il server');},
        success : function(data) {
            if (data.error == true){
                alert(data.message);
            } else {
                delete data.error, data.message;
                for (var k in data) {
                    if (data.hasOwnProperty(k)) {
                        var input_name = 'input[name=device_key_' + k + ']';
                        form.find(input_name).val(data[k]);
                    }
                }
            }
        }
    });
}

var saveDeviceKey = function() {
    var form = $('#device-key-edit').find('form');
    if (!form[0].checkValidity()) {
        // submitting form to show error message
        $('<input type="submit">').hide().appendTo(form).click().remove();
        return false;
    }

    $.ajax({
        method: 'POST',
        url : '/setup/editDeviceKey',
        data : form.serialize(),
        dataType : 'json',
        beforeSend : function() { $('#device-key-edit').modal('hide'); return true; },
        success : function(data) {
            if (data.error == true){
                alert(data.message);
            } else {
                var dev_pane_id = $('#devices-list').find('li.active a').attr('href');
                var type = form.find('input[name=device_type]').val();
                var thead = $(dev_pane_id).find("thead.device-keys-header");
                if (thead.children().length == 0)
                    thead.append(get_table_key_section(type, 'header').html());
                var tbody = $(dev_pane_id).find("tbody.device-keys-list");
                tbody.append(render_table_row_data(get_table_key_section(type, 'body').html(),data.key));
            }
        },
        error : function () { alert ('Errore di comunicazione con il server.');}
    });
}

var deleteDeviceKey = function() {
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