/**
 * @file
 * Helper functions to handle download and stream popups.
 */

/**
 * Display or remove spinner from button.
 *
 * @param display
 *   If true the spinner is displayed else removed.
 * @param context
 *   If no button with the class 'js-clicked' exists context should be the
 *   button.
 */
function elib_popup_spinner(display, context) {
  "use strict";

  var button = $('.js-clicked');
  if (button.length == 0) {
    context.addClass('js-clicked');
    button = context;
  }

  if (display === true) {
    var spinner = $('<div class="ajax-loader"></div>');
    button.hide();
    button.parent().append(spinner);
  }
  else {
    button.parent().find('.ajax-loader').remove();
    button.show();
    button.removeClass('js-clicked');
  }
}

/**
 * Display message in dialog with ok button.
 *
 * @param title
 *   Title to give the dialog.
 * @param content
 *   HTML content to insert into the dialog.
 */
function elib_popup_message(title, content) {
  "use strict";

  // Build ok button.
  var ok_button = Drupal.t('Ok', {});
  var popup_buttons = {};
  popup_buttons[ok_button] = function() {
    $('#ting-download-popup').dialog('close');
  };

  // Display dialog with message.
  $('<div id="ting-download-popup" title="' + title + '">' + content + '</div>').dialog({
    modal : true,
    width: 'auto',
    height: 'auto',
    buttons: popup_buttons,
    close: function () {
      $(this).dialog('destroy').remove ();

      // Remove spinner from the button.
      elib_popup_spinner(false, undefined);
    }
  });
}
