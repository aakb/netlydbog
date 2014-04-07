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

      href = $(this).attr('href');

      // Fix ajax spinner for the clicked on button.
      clicked = $(this);
      clicked.parent().find('.ajax-loader').remove();
      clicked.hide();
      clicked.parent().append('<div class="ajax-loader"></div>');


      return false;
    });
  });

})(jQuery);
