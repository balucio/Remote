/* Device Edit*/

document.addEventListener('DOMContentLoaded', function(event) {

    /* Evento show su modale di edit/new device */
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

    /* Evento hide su modale edit/new */
    $('#device-edit').on('hidden.bs.modal', function () {
        $('#frm_edit_dev_name')[0].setCustomValidity('');
        $(this).data('bs.modal', null);
    });

    /* Evento di input su widget dev name */
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
    /* Evento show su tab elenco device */
    $('#devices-list a[data-toggle=pill]').on('shown.bs.tab', show_device_info );

    /* Evento show del modale Delete device */
    $('#device-delete').on('shown.bs.modal', function() {
        $('#device-delete').find('b.device-name').text(
            $('#devices-list').find('li.active a').attr('href').substring(8)
        );
    });

    /* Edit (nuovo) tasto */
    $('#device-key-edit').on('shown.bs.modal', function() {
        var dev_pane_id = $('#devices-list').find('li.active a').attr('href');
        var dev_name = $(dev_pane_id).children('input[name=device_name]').eq(0).val();
        var dev_type = $(dev_pane_id).children('input[name=device_type]').eq(0).val();
        $("#frm_edit_dev_key_dev_type").val(dev_type);
        $('#frm_edit_dev_key_dev_name').val(dev_name);
        $('#frm_edit_dev_key_name')[0].setCustomValidity('');
    });

    /* Device key delete on show modal */
    $('#device-key-delete').on('shown.bs.modal', function(e) {
        var modal = $('#device-key-delete');
        modal.find('i.device-name').text(
            $('#devices-list').find('li.active a').attr('href').substring(8)
        );
        modal.find('b.device-key-name').text(
            $(e.relatedTarget).closest("tr.key-row").children('td:eq(1)').text().trim()
        );
    });
    /* evento di input sul nome del tasto */
    $('#frm_edit_dev_key_name').on('input',  function() {
        var dev_pane_id = $('#devices-list').find('li.active a').attr('href');
        var found = false;
        var key_name = $(this).val();
        $(dev_pane_id).find("tbody.device-keys-list tr.key-row").children(":nth-child(2)").each(
            function() {
                found = $(this).text() == key_name;
                return !found;
            }
        );
        this.setCustomValidity( found ? 'Il nome del tasto è già in uso' : '');
    });

});