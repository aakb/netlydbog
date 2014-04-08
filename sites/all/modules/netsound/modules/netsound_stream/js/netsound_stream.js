/**
 * @file
 * This implements the dialog part of the stream player.
 */
(function ($) {
  "use strict";

  $(document).ready(function() {

    var clicked = undefined;
    var href = undefined;

    $('.js-stream').live('click', function(e) {
      e.preventDefault();

      // Add spinner for the button clicked.
      elib_popup_spinner(true, $(this));

      $.ajax({
        type : 'post',
        url : $('.js-clicked').attr('href'),
        dataType : 'json',
        success : netsound_stream_process
      });

      return false;
    });
  });

  function netsound_stream_process(response) {
    // Close login dialog, if open.
    elib_popup_login_close();

    if (response.status == false) {
      // Something went wrong, display message.
      elib_popup_message(response.title, response.content);
      return;
    }
    else if (response.status === 'login') {
      // User needs to login.
      elib_popup_login_open(response.title, response.content, netsound_stream_process);
      return;
    }

    var popup_buttons = {};
    var download_button = Drupal.t('Ok');
    var cancel_button = Drupal.t('Cancel');

    if (response.final === undefined) {
      popup_buttons[download_button] = function() {
        var button = $('#ting-download-popup').parents('.ui-dialog:first').find('button');
        button.css('visibility', 'hidden');
        button.parent().append('<div class="ajax-loader"></div>');

        // Finalize the loan.
        $.ajax({
          type : 'post',
          url : $('.js-clicked').attr('href') + '/request',
          dataType : 'json',
          success : function(response) {
            $('#ting-download-popup').dialog('close');
            netsound_stream_process(response);
          }
        });
      };
      popup_buttons[cancel_button] = function() {
        $('#ting-download-popup').dialog('close');
      };
    }

    if (response.processed && response.processed == true) {
      // Prepare the stream popup.
      popup_buttons = {};
      popup_buttons[cancel_button] = function() {
        $('#ting-download-popup').dialog('close');
      };

      // Remove spinner from the button.
      elib_popup_spinner(false, undefined);
    }

    $('<div id="ting-download-popup" title="' + response.title + '">' + response.content + '</div>').dialog({
      modal : true,
      width: 'auto',
      height: 'auto',
      buttons: popup_buttons
    });
  }
})(jQuery);
