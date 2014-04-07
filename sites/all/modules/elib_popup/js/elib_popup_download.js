(function ($) {
  // Close dialog when download link is pressed ("Hent").
  $('.ebog-dlink').live('click', function() {
    $('#ting-download-popup-info').dialog('close');
  });

  var href = '';
  var clicked = null;
  var button = null;
  var popup_buttons = null;
  var ok_button = Drupal.t('Ok');
  var cancel_button = Drupal.t('Cancel');
  var login_button = Drupal.t('Login');
  var download_button = Drupal.t('Proceed to download');
  var loggedin = false; // If true the site should be reloaded.

  // Handle clicked loan link, those matching 'ting/object/%/download' pattern
  $(document).ready(function() {
    $('.icons a.download, .icons a.loan').live('click', function() {
      href = $(this).attr('href');

      clicked = $(this);
      clicked.parent().find('.ajax-loader').remove();
      clicked.hide();
      clicked.parent().append('<div class="ajax-loader"></div>');


      process_loan();

      return false;
    });

    // Process loan/reloan/download
    var process_loan = function() {
      $.ajax({
        type : 'post',
        url : href,
        dataType : 'json',
        success : function(response) {
          // Check if login dialog is open, if it is close it.
          var login_dialog = $('#ting-login-popup');
          if (login_dialog.length) {
            $('#ting-login-popup').dialog('close');
            $('#ting-login-popup').remove();
          }

          // Remove ajax loader.
          $('#ting-download-popup').remove();
          clicked.parent().find('.ajax-loader').remove();
          clicked.show();

          popup_buttons = {};

          if (response.status == false) {
            popup_buttons[ok_button] = function() {
              $('#ting-download-popup').dialog('close');
            }

            $('<div id="ting-download-popup" title="' + response.title + '">' + response.content + '</div>').dialog({
              modal : true,
              width: 'auto',
              height: 'auto',
              buttons: popup_buttons
            });
            return;
          }
          else if (response.status === 'login') {
            elib_popup_login_open(response.title, response.content, process_loan);
            return;
          }

          if (response.processed && response.processed == true) {
            // Prepare download popup buttons
            if (response.platform == 2) {
              popup_buttons[ok_button] = function() {
                $('#ting-download-popup').dialog('close');
              }
              popup_buttons[cancel_button] = function() {
                $('#ting-download-popup').dialog('close');
              }
            }
            else {
              // Prepare the stream popup
              popup_buttons = {};
              popup_buttons[cancel_button] = function() {
                $('#ting-download-popup').dialog('close');
              }

              // Prepare popup content
              response.content = Drupal.t('Click on the Strem link in order to start streaming of this audiobook.') + '<br />' + '<a href="' + response.stream + '" target="_blank">' + Drupal.t('Stream') + '</a>';
            }
          } else {
            if (!response.final) {
              popup_buttons[download_button] = function() {
                button = $('#ting-download-popup').parents('.ui-dialog:first').find('button');
                button.css('visibility', 'hidden');
                button.parent().append('<div class="ajax-loader"></div>');
              };
            }
            popup_buttons[cancel_button] = function() {
              $('#ting-download-popup').dialog('close');
            }
          }
          $('<div id="ting-download-popup" title="' + response.title + '">' + response.content + '</div>').dialog({
            modal : true,
            width: 'auto',
            height: 'auto',
            buttons: popup_buttons
          });
        }
      });
    };
  });
})(jQuery);
