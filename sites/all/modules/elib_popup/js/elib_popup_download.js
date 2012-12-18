(function ($) {
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
    $('.icons a.download, .icons a.stream, .icons a.loan').live('click', function() {
      href = $(this).attr('href');

      clicked = $(this);
      clicked.parent().find('.ajax-loader').remove();
      clicked.hide();
      clicked.parent().append('<div class="ajax-loader"></div>');

      if (clicked.hasClass('re-loan')) {
        $('#ting-download-popup').remove();
        clicked.parent().find('.ajax-loader').remove();
        clicked.show();

        popup_buttons = {};
        popup_buttons[ok_button] = function() {
          button = $('#ting-download-popup').parents('.ui-dialog:first').find('button');
          button.css('visibility', 'hidden');
          button.parent().append('<div class="ajax-loader"></div>');
          process_loan();
        }

        popup_buttons[cancel_button] = function() {
          $('#ting-download-popup').dialog('close');
        }

        $('<div id="ting-download-popup" title="' + Drupal.t('Confirm reloan') + '">' + Drupal.t('Are you sure you want to reloan this item') + ' (<a href=' + '"' + '/faq/generelt-0#31n128' + '">' + Drupal.t('read more') + '</a>)?' + '</div>').dialog({
          modal : true,
          width: 'auto',
          height: 'auto',
          buttons: popup_buttons
        });
      }
      else {
        // This was now a re-load so process the loan, this may trigge a
        // login request.
        process_loan();
      }

      return false;
    });

    // Process loan/reloan/download
    var process_loan = function() {
      $.ajax({
        type : 'post',
        url : href + '/popup',
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
            // Login is required, so display login form.
            popup_buttons = {};

            // Hide login button from the form.
            var content = $(response.content);
            $('#edit-submit', content).remove();

            // Add action to the login dialog button.
            popup_buttons[login_button] = function() {
              // Add ajax loader to replace buttons.
              button = $('#ting-login-popup').parents('.ui-dialog:first').find('button');
              button.css('visibility', 'hidden');
              button.parent().append('<div class="ajax-loader"></div>');

              // Collect form values.
              var data = $('#elib-popup-login-form').formSerialize();

              // Make login ajax callback.
              $.ajax({
                type : 'POST',
                url : $('#elib-popup-login-form').attr('action'),
                dataType : 'json',
                data: data,
                success : function(response) {
                  // If not logged in handle errors.
                  if (response.status !== 'loggedin') {

                    // Display error message.
                    if ($('#ting-login-popup .messages').length) {
                      $('#ting-login-popup .messages').fadeOut('fast', function () {
                        $(this).remove();
                        $('#elib-popup-login-form #edit-name-wrapper').prepend(response.content);
                      });
                    }
                    else {
                      $('#elib-popup-login-form #edit-name-wrapper').prepend(response.content);
                    }

                    // Enable login buttons and remove ajax loader.
                    button.css('visibility', 'visible');
                    button.parent().find('.ajax-loader').remove();
                    return;
                  }
                  else {
                    // Upload login menu link.
                    var link = $('.menu .login-link');
                    link.removeClass('login-link').addClass('account-link');
                    link = $('a', link);
                    link.val(Drupal.t('My page'));
                    link.attr("href", "/min_side");

                    // Add logout block to the site.
                    var block = $('<div id="block-publizon_user-logout" class="block block-publizon_user"><h2>' + Drupal.t('My profile') + '</h2><div class="content"><p>' + Drupal.t('You are now logged in') + ': <a class="biglogoutbutton" href="/logout">' + Drupal.t('Logout') + '</a></p></div></div>');
                    $('#sidebar-first').prepend(block);

                    // Try to process the loan once more.
                    process_loan();
                  }
                }
              });
              return false;

            }

            popup_buttons[cancel_button] = function() {
              // Close the form an remove it from the dom or close will not work
              // if displayed once more.
              $('#ting-login-popup').dialog('close');
              $('#ting-login-popup').remove();
            }

            options = {
              modal: true,
              width: 'auto',
              height: 'auto',
              buttons: popup_buttons
            }

            $('<div id="ting-login-popup" title="' + response.title + '">' + content[0].outerHTML + '</div>').dialog(options);

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
                check_rules();
              }
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
    }

    // Check those checkboxes
    var check_rules = function() {
      $.ajax({
        type : 'post',
        url : href + '/request',
        dataType : 'json',
        success : function(response) {
          button.css('visibility', 'visible');
          button.parent().find('.ajax-loader').remove();
          $('#ting-download-popup').dialog('close');
          $('#ting-download-popup-info').remove();
          var options = {};
          if (response.status == false) {
            popup_buttons = {};
            popup_buttons[ok_button] = function() {
              $('#ting-download-popup-info').dialog('close');
            }

            options = {
              modal: true,
              width: 'auto',
              height: 'auto',
              buttons: popup_buttons
            }

            $('<div id="ting-download-popup-info" title="' + response.title + '">' + response.content + '</div>').dialog(options);
          }
          else {
            if (response.processed && response.processed == true) {
              if (response.platform == 2) {
                popup_buttons[ok_button] = function() {
                  $('#ting-download-popup-info').dialog('close');
                }
                popup_buttons[cancel_button] = function() {
                  $('#ting-download-popup-info').dialog('close');
                }
              }
              else {
              // Prepare the stream popup
              popup_buttons = {};
              popup_buttons[cancel_button] = function() {
                $('#ting-download-popup-info').dialog('close');
              }

              // Prepare popup content
              response.content = Drupal.t('Click on the Strem link in order to start streaming of this audiobook.') + '<br />' + '<a href="' + response.stream + '" target="_blank">' + Drupal.t('Stream') + '</a>';
              }
            } else {
              popup_buttons[download_button] = function() {
                button = $('#ting-download-popup').parents('.ui-dialog:first').find('button');
                button.css('visibility', 'hidden');
                button.parent().append('<div class="ajax-loader"></div>');
                $('#ting-download-popup-info').dialog('close');
              }

              popup_buttons[cancel_button] = function() {
                $('#ting-download-popup-info').dialog('close');
              }
            }
            $('<div id="ting-download-popup-info" title="' + response.title + '">' + response.content + '</div>').dialog({
              modal : true,
              width: 'auto',
              height: 'auto',
              buttons: popup_buttons
            });
          }
        }
      });
      return false;
    }
  });
})(jQuery);
