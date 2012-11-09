<?php

/**
 * @file
 * Implements a very simple interface to find users that have problems with
 * loggin in. In many cases it helps to delete the Drupal user, so this file
 * allows the administrator to do this through the backend. 
 */

/**
 * Display a form that takes an card no. or CPR no. and find basic information 
 * in Drupal about the user.
 * 
 */
function elib_user_search($uid = FALSE) {
  $output = drupal_get_form('elib_user_search_form');
  $info = array();
  
  // Check the supplied user id.
  $uid = check_plain($uid);
  
  // If set and is a number load extra information about the user.
  if ($uid && is_numeric($uid)) {
    $info['elib_id'] = elib_user_elib_id($_SESSION['elib_user_search_cardno']);
    
    // Load user account.
    $info['user'] = user_load(array('uid' => $uid));
    
    // Load the ting client and use it to get more information about the books 
    // the user may have request in the cart etc,
    module_load_include('client.inc', 'ting');
    
    // Search for cart content.
    $result = db_query('SELECT DISTINCT ting_object_id
                                      FROM {netsound_cart}
                                      WHERE uid = %d', array('%d' => $uid));
    if ($result) {
      $info['cart'] = array();
      while ($row = db_fetch_array($result)) {
        $info['cart'][] = ting_get_object_by_id($row['ting_object_id']);
      }
    }
    
    // Add information to the output.
    $output .= theme('elib_user_search', $uid, $info);    
    $output .= l(t('Remove user'), 'admin/user/elib/search/' . $uid . '/delete') . ' (You will not be aksed to confirm the deleteion)';
  }
    
  return $output;
}

/**
 * Search form for elib user.
 * 
 * @return string 
 */
function elib_user_search_form() {
  $form = array();
  
  $form['cardno'] = array(
    '#type' => 'textfield',
    '#title' => t('User number'),
    '#description' => t('Enter the user library card number or social securty number'),
    '#size' => 20,
    '#default_value' => $_SESSION['elib_user_search_cardno'] ? $_SESSION['elib_user_search_cardno'] : '',
  );
  
  $form['search'] = array(
    '#type' => 'submit',
    '#value' => 'Search for user',
  );
  
  return $form;
}

/**
 * Validation for the elib search form. It checkes that the number enter is 
 * either a library card no. or a CPR no. 
 */
function elib_user_search_form_validate($form, $form_state) {
  // Check if card number or cpr was entered.
  if (!preg_match('/^\d{6}-?\d{4}$/', $form_state['values']['cardno'])) {
    form_set_error('cardno', t('The value entered is not valided ID for an user.'));    
  }
}

function elib_user_search_form_submit($form, $form_state) {  
  // Perform search and save data into the current session
  $statement = "SELECT uid FROM {elib_user} WHERE elib_id = '%s'";
  $uid = db_result(db_query($statement, elib_user_elib_id($form_state['values']['cardno'])));
  
  // Save information and make a redirect.
  $_SESSION['elib_user_search_cardno'] = $form_state['values']['cardno'];
  if ($uid) {
    drupal_goto('admin/user/elib/search/' . $uid);
  }
}

/**
 * Menu callback that deletes a user.
 */
function elib_user_search_delete($uid = FALSE) {
  if ($uid && is_numeric($uid)) {   
    $statement = "DELETE FROM {elib_user} WHERE elib_id = '%s'";
    db_query($statement, elib_user_elib_id($_SESSION['elib_user_search_cardno']));
    unset($_SESSION['elib_user_search_cardno']);
    user_delete(array(), $uid);
    drupal_set_message(t('The user have been deleted'));
    drupal_goto('admin/user/elib/search');
  }
  else {
    drupal_set_message(t('You are not allowed to delete this user'));
    drupal_goto('admin/user/elib/search');
  }
}