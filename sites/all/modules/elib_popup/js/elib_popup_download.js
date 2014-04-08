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
