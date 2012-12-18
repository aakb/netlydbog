<?php

/**
 * @file
 * Theme functions used with the ting search carousel.
 */

/**
 * Implements default preprocess function for the search carsousel.
 */
function template_preprocess_ting_search_carousel(&$vars, $hook) {
  // Add JavaScript and CSS using wrapper functions.
  _ting_search_carousel_add_javascript();
  _ting_search_carousel_add_css();

  // Fixes the naming from the administration interface.
  $vars['searches'] = $vars['searches']['searches'];
}

/**
 * Implements default preprocess function for the search carsousel collection.
 */
function template_preprocess_ting_search_carousel_collection(&$vars, $hook) {
  $vars['collection'] = $vars['collection']['collection'];
}

/**
 * Theme function for the admin form, enables drag and drop of form elements.
 *
 * @param array $form
 * @return string
 */
function theme_ting_search_carousel_admin_form($form) {
  drupal_add_tabledrag('ting-search-carousel-admin-sort', 'order', 'sibling', 'sort');

  $header = array('', 'Searches', 'sort');
  foreach (element_children($form) as $key) {
    // Add class to group weight fields for drag and drop.
    $form[$key]['sort']['#attributes']['class'] = 'sort';

    // Get sort element
    $sort = $form[$key]['sort'];
    unset($form[$key]['sort']);

    // Build rows
    $row = array('');
    $row[] = drupal_render($form[$key]);
    $row[] = drupal_render($sort);
    $rows[] = array('data' => $row, 'class' => 'draggable');
  }
  $output = theme('table', $header, $rows, array('id' => 'ting-search-carousel-admin-sort'));

  // Add placeholder for ahah objects
  $output .= '<div id="add-another-fields" class="clear-block"></div>';

  $output .= drupal_render($form);

  return $output;
}

/**
 * Theme function to format the our custom form element.
 *
 * We use the container-inline class so that all three of the HTML elements
 * are placed next to each other, rather than on separate lines.
 */
function theme_ting_search_carousel_search($element) {
  return theme('form_element', $element, '<div class="ting-search-carousel-search-wrap">'. $element['#children'] .'</div>');
}