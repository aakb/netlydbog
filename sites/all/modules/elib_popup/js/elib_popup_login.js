/**
 * @file
 * Handle login dialog.
 */

function elib_popup_login_open(title, content, callback) {
  "use strict";

  var popup_buttons = {};
  var cancel_button = Drupal.t('Cancel');
  var login_button = Drupal.t('Login');

  // Hide login button from the form.
  content = $(content);
  $('#edit-submit', content).remove();

  // Add action to the login dialog button.
  popup_buttons[login_button] = function() {
    // Add ajax loader to replace buttons.
    var button = $('#ting-login-popup').parents('.ui-dialog:first').find('button');
    button.css('visibility', 'hidden');
    button.parent().append('<div class="ajax-loader"></div>');

    // Collect form values.
    var form = $('#elib-popup-login-form');
    var data = form.formSerialize();

    // Make login ajax callback.
    $.ajax({
      type : 'POST',
      url : form.attr('action'),
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
          // Update login menu link (HACK).
          var link = $('.menu .login-link');
          link.removeClass('login-link').addClass('account-link');
          link = $('a', link);
          link.val(Drupal.t('My page'));
          link.attr("href", "/min_side");

          // Add logout block to the site (HACK).
          var block = $('<div id="block-publizon_user-logout" class="block block-publizon_user"><h2>' + Drupal.t('My profile') + '</h2><div class="content"><p>' + Drupal.t('You are now logged in') + ': <a class="biglogoutbutton" href="/logout">' + Drupal.t('Logout') + '</a></p></div></div>');
          $('#sidebar-first').prepend(block);

          // Try to process the loan once more.
          callback();
        }
      }
    });

    return false;
  };

  popup_buttons[cancel_button] = function() {
    // Close the form an remove it from the dom or close will not work
    // if displayed once more.
    $('#ting-login-popup').dialog('close');
    $('#ting-login-popup').remove();
  };

  var options = {
    modal: true,
    width: 'auto',
    height: 'auto',
    buttons: popup_buttons
  };

  $('<div id="ting-login-popup" title="' + title + '">' + content[0].outerHTML + '</div>').dialog(options);
}


function elib_popup_login_close() {
  "use strict";

  // Check if login dialog is open, if it is close it.
  var login_dialog = $('#ting-login-popup');
  if (login_dialog.length) {
    login_dialog.dialog('close');
    login_dialog.remove();
  }
}
