<?php

/**
 * Rules editing page callback.
 *
 * Simple wrapper for the form, only required to add the JavaScript.
 */
function ting_search_carousel_admin_page() {
  $path = drupal_get_path('module', 'ting_search_carousel');

  drupal_add_js($path . '/js/ting_search_carousel.admin.js');
  drupal_add_css($path . '/css/ting_search_carousel.css');

  return drupal_get_form('ting_search_carousel_admin_form');
}


/**
 * Ting search carousel admin form.
 */
function ting_search_carousel_admin_form() {
  $form = array();

  $form['ting_carousel_search_queries'] = array(
    '#type' => 'fieldset',
    '#title' => t('Searches'),
    '#attributes' => array('id' => 'ting-search-carousels'),
    '#tree' => TRUE,
    '#theme' => 'ting_search_carousel_admin_form',
  );

  $searches = variable_get('ting_carousel_search_queries', array());
  if ($searches) {
    foreach ($searches as $key => $search) {
      // Filter out searches with empty title. Previous versions created
      // those, left here for compatibility.
    	if ($search['title']) {
	      $form['ting_carousel_search_queries'][$key] = array(
	        '#type' => 'ting_search_carousel_search',
	        '#value' => $search
	      );
    	}
    }
  }
  else {
    $form['ting_carousel_search_queries'][0] = array(
      '#type' => 'ting_search_carousel_search'
    );
  }

  $form['new_search_carousel_button'] = array(
    '#type' => 'button',
    '#value' => 'Add another',
    '#ahah' => array(
      'path' => 'admin/settings/ting_search_carousel/form/ahah',
      'wrapper' => 'add-another-fields',
      'method' => 'append',
      'effect' => 'slide'
    ),
  );

  // Get systemet form elements
  $form = system_settings_form($form);

  // Override submit, need to sort data before save
  $form['#submit'] = array('ting_search_carousel_search_submit');
  return $form;
}

/*
  This function is largely based on the poll module, its been simplified for reuse.
  $fields is the specific form elements you want to attach via ahah,
  $name is the form fields array key... e.g. the name for $form['title'] is "title"
*/
function ting_search_carousel_form_ahah($form_state) {

  $form_state = array('submitted' => FALSE);
  $form_build_id = $_POST['form_build_id'];
  // Add the new element to the stored form. Without adding the element to the
  // form, Drupal is not aware of this new elements existence and will not
  // process it. We retreive the cached form, add the element, and resave.
  $form = form_get_cache($form_build_id, $form_state);

  // Delta is assigned to the count of current elements, so it'll be the
  // next in the zero-based list.
  $key = count(element_children($form['ting_carousel_search_queries']));
  $key++;

  $form['ting_carousel_search_queries'][$key] = array(
    '#type' => 'ting_search_carousel_search',
    '#sort' => 10,
  );
  form_set_cache($form_build_id, $form, $form_state);

  $form += array(
    '#post' => $_POST,
    '#programmed' => FALSE,
  );

  // Rebuild the form.
  $form = form_builder($_POST['form_id'], $form, $form_state);

  // Render and return the new field.
  drupal_json(array(
    'data' => drupal_render($form['ting_carousel_search_queries'][$key]),
    'status' => TRUE));
  exit();
}

function ting_search_carousel_search_submit($form, &$form_state) {
  foreach ($form_state['values']['ting_carousel_search_queries'] as $key => $val) {
    if (!$val['title']) {
      unset($form_state['values']['ting_carousel_search_queries'][$key]);
    }
  }
  // Sort searches, before saveing (preformance).
  usort($form_state['values']['ting_carousel_search_queries'], "ting_search_carousel_usort_cmp");

  // Return control to system settings form submit.
  system_settings_form_submit($form, &$form_state);

  // Clear search cache for the carousel.
  cache_clear_all('ting_search_carousel_result', 'cache');
}

/**
 * Custom sort function, to sort form by weight (sort col.)
 *
 * @param array $a
 * @param array $b
 * @return <type>
 */
function ting_search_carousel_usort_cmp($a, $b) {
    if ($a['sort'] == $b['sort']) {
        return 0;
    }
    return ($a['sort'] < $b['sort']) ? -1 : 1;
}
