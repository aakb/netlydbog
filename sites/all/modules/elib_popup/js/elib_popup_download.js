(function ($) {
  // Close dialog when download link is pressed ("Hent").
  $('.ebog-dlink').live('click', function() {
    $('#ting-download-popup-info').dialog('close');
  });

  var href = '';
  var clicked = null;

  // Handle clicked loan link, those matching 'ting/object/%/download' pattern
  $(document).ready(function() {
    $('.icons a.download, .icons a.loan').live('click', function() {
      // Add spinner for the button clicked.
      elib_popup_spinner(true, $(this));

      // Process the loan.
      $.ajax({
        type : 'post',
        url : $('.js-clicked').attr('href'),
        dataType : 'json',
        success : elib_popup_process_loan
      });

      return false;
    });
  });
})(jQuery);

/**
 * Process the loan response from the server.
 *
 * @param response
 *   JSON response from the server.
 */
function elib_popup_process_loan(response) {
  "use strict";

  var button = null;
  var popup_buttons = {};
  var cancel_button = Drupal.t('Cancel');
  var download_button = Drupal.t('Proceed to download');

  // Close login dialog, if open.
  elib_popup_login_close();

  if (response.status == false) {
    // Something went wrong, display message.
    elib_popup_message(response.title, response.content);
    return;
  }
  else if (response.status === 'login') {
    // User needs to login.
    elib_popup_login_open(response.title, response.content, elib_popup_process_loan);
    return;
  }

  if (!response.final) {
    popup_buttons[download_button] = function() {
      button = $('#ting-download-popup').parents('.ui-dialog:first').find('button');
      button.css('visibility', 'hidden');
      button.parent().append('<div class="ajax-loader"></div>');

      // Finalize the loan.
      $.ajax({
        type : 'post',
        url : $('.js-clicked').attr('href') + '/request',
        dataType : 'json',
        success : function(response) {
          // Close dialog and call the function once more with the new response.
          $('#ting-download-popup').dialog('close');
          elib_popup_process_loan(response);
        }
      });
    };
  }
  popup_buttons[cancel_button] = function() {
    $('#ting-download-popup').dialog('close');
  };

  $('<div id="ting-download-popup" title="' + response.title + '">' + response.content + '</div>').dialog({
    modal : true,
    width: 'auto',
    height: 'auto',
    buttons: popup_buttons
  });

  // Remove spinner from the button.
  elib_popup_spinner(false, undefined);
}


