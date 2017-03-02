document.addEventListener('DOMContentLoaded', function(event) {
    $('#device-delete').on('shown.bs.modal', function() {
        $('#device-delete').find('b.device-name').text(
            $('#devices-list').find('li.active a').attr('href').substring(8)
        );
    });
});

document.addEventListener('DOMContentLoaded', function(event) {
    $('#device-edit').on('shown.bs.modal', function(e) {
        var action = $(e.relatedTarget).data('action');
        if (action == 'new-device') {
            $('#frm_edit_dev_name').val('');
            $('#frm_edit_curr_dev_name').val('');
        } else if (action == 'edit-device') {
            var dev_pane_id = $('#devices-list').find('li.active a').attr('href');
            var dev_name = $(dev_pane_id).children('input[name=device_name]').eq(0).val();
            var dev_type = $(dev_pane_id).children('input[name=device_type]').eq(0).val();
            $('#frm_edit_dev_name').val(dev_name);
            $('#frm_edit_curr_dev_name').val(dev_name);
            $('#frm_edit_dev_type').val(dev_type);
        }
    });

    $('#device-edit').on('hidden.bs.modal', function () {
        $('#frm_edit_dev_name')[0].setCustomValidity('');
        $(this).data('bs.modal', null);
    });

    $('#frm_edit_dev_name').on('input',  function() {
        var old_name = $('#frm_edit_curr_dev_name').val().trim();
        var new_name = $(this).val().trim();

        if ((!old_name || new_name != old_name)) {
            var found = false;
            $('#devices-list li a').each(function( index ) {
                found = $( this ).text() == new_name;
                return !found;
            });
            this.setCustomValidity(found ? 'Il nome del dispositivo è già in uso' : '');
        }
    });
    $('#devices-list a[data-toggle=pill]').on('shown.bs.tab', function (e) {
        var sel_dev = e.target; // activated tab

        // l'id del div che inizia per #device_
        var device_div = $(sel_dev).attr('href');
        var dev_input_name = $(device_div).children('input[name=device_name]').eq(0);
        var dev_input_type = $(device_div).children('input[name=device_type]').eq(0);
        if ( device_name.val() != '' && device_type.val() != '')
            return;
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
            $('button[data-action="device-delete"]').prop( 'disabled', false );
            $('button[data-action="edit-device"]').prop( 'disabled', false );
          },
          error : function() {alert('Errore di comunicazione con il server');}
        });
    });
});

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
        $('button[data-action=\"device-delete\"]').prop( 'disabled', false );
        $('button[data-action=\"edit-device\"]').prop( 'disabled', false );
        $(dev_id).remove();
        if ($('#devices-list').find('li').length == 0)
             $('.no-device-alert').removeClass('hidden');

      },
      error : function() {alert('Errore di comunicazione con il server');}
    });

}