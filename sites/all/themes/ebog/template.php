<?php
/**
 * @file
 *
 */

/**
 * @TODO what do this do ?
 *
 * @param type $node
 * @param type $count
 * @return type
 */
function fetchNewestComment($node = false, $count = 1) {
  $query = db_query_range("SELECT c.*
                             FROM {comments} AS c
                            WHERE c.nid = %d", $node->nid, 0, $count);

  $result = db_fetch_object($query);
  return $result;
}

/**
 * Fetch all terms from specified vocabulary
 *
 * @param Object $node
 * @param Integer $vid
 * @return Arrau
 * @author Hasse R. Hansen
 **/
function showEntriesFromVocab($node, $vid) {
  $entries = array();
  $items = taxonomy_node_get_terms_by_vocabulary($node, $vid);
  foreach ($items as $value) {
    $entries[] = $value->name;
  }
  return $entries;
}

/**
 * Implements hook_preprocess_ting_object().
 *
 * Add extra information form elib to the ting object.
 */
function ebog_preprocess_ting_object(&$vars) {
  $isbn = $vars[object]->record['dc:identifier']['oss:PROVIDER-ID'][0];

  // Override ting object page title.
  drupal_set_title(check_plain($vars['object']->title . ' ' . t('af') . ' ' . $vars['object']->creators_string));

  // Create the author field
  $vars['author'] = publizon_get_authors($vars['object']);

  // Load the product.
  try {
    $product = new PublizonProduct($isbn);

    // Get cover image.
    $vars['cover'] = $product->getCover('170_x');

    // Get ebook sample link.
    if (!empty($product->teaser_link)) {
      $vars['elib_sample_link'] = $product->teaser_link;
    }

    // Check if the book is loaned by the user.
    global $user;
    if ($user->uid > 0) {
      $user_loans = new PublizonUserLoans($user->uid);
      $vars['is_loan'] = $user_loans->isLoan($isbn, TRUE);
    }

    // Set new header meta data (why do we need this?).
    $head .= '<meta property="og:url" content="' . $base_url .'/ting/object/' . $vars['object']->id . '" />' . "\n";
    $head .= '<meta property="og:type" content="book" />' . "\n";
    $head .= '<meta property="og:title" content="' . $vars['object']->title .'" />' . "\n";
    $head .= '<meta property="og:description" content="Lån `' . $vars['object']->title . '` på Netlydbog.dk: ' . $vars['object']->abstract .'" />' . "\n";
    $head .= '<meta property="og:image" content="' . $vars['cover'] . '" />' . "\n";
    $head .= '<meta property="og:image:secure_url" content="' . $vars['cover'] . '" />' . "\n";
    $head .= '<meta property="og:site_name" content="Netlydbog.dk" />' . "\n";
    $head .= '<meta property="og:locale" content="da_DK" />' . "\n";
    $head .= '<meta property="fb:admins" content="694811338" />' . "\n";
    $head .= '<meta property="book:isbn" content="' . $isbn . '" />' . "\n";
    $head .= '<meta property="book:release_date" content="' . $vars['object']->date . '" />';
    drupal_set_html_head($head);
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage(), 'error');
  }

  // Add Javascript to handle stream links.
  drupal_add_js(drupal_get_path('module', 'netsound_stream') . '/js/netsound_stream.js');
}

/**
 * Implements hook_preprocess_ting_search_collection().
 *
 * Add extra information from elib to the ting objects.
 */
function ebog_preprocess_ting_search_collection(&$vars) {
  foreach ($vars['collection']->objects as $obj) {
    $isbn = $obj->record['dc:identifier']['oss:PROVIDER-ID'][0];
    if (isset($vars['elib'])) {
      $vars['elib'][$isbn] = array();
    }
    else {
      $vars['elib'] = array();
    }

    // Get authors.
    $vars['elib'][$isbn]['author'] = publizon_get_authors($obj);

    try {
      $product = new PublizonProduct($isbn);

      // Get cover image.
      $vars['elib'][$isbn]['elib_book_cover'] = $product->getCover('170_x');

      // Get ebook sample link.
      if (!empty($product->teaser_link)) {
        $vars['elib'][$isbn]['elib_sample_link'] = $product->teaser_link;
      }
    }
    catch (Exception $e) {
      drupal_set_message($e->getMessage(), 'error');
    }
  }
}

/**
 * Override or insert variables into the page templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("page" in this case.)
 */
function ebog_preprocess_page(&$vars, $hook) {
  global $user;

  array_pop($vars['primary_links']) ;
  if ($user->uid != 0) {
    $vars['primary_links']['account-link'] = array(
      'href'  => 'min_side',
      'title' => t('Min side'),
    );
  }
  else {
    $vars['primary_links']['login-link'] = array(
      'href'  => 'user',
      'title' => t('Login')
    );
  }

  $rendered_primary_links = theme('links', $vars['primary_links'], array('class' => 'menu'));
  $vars['navigation'] = '<div class="block block-menu" id="block-menu-primary-links"><div class="content">' . $rendered_primary_links . '</div></div>';

  if (arg(0) == 'min_side' && $user->uid == 0){
    drupal_goto('user',drupal_get_destination());
  }

  if (arg(3) == 'stream' || arg(3) == 'download' || (isset($_GET['clean']) && $_GET['clean'] == 1)){
    $vars['template_files'] = array('page-clean');
    $vars['css']['all']['theme']['sites/all/themes/ebog/css/style.css'] = false;
  }
}

/**
 * Override or insert variables into the comment templates.
 *
 * @param $vars
 *   An array of variables to pass to the theme template.
 * @param $hook
 *   The name of the template being rendered ("comment" in this case.)
 */
function ebog_preprocess_comment(&$variables) {
  $comment = $variables['comment'];
  $node = $variables['node'];

  $variables['author']    = netsound_get_username($comment->uid);
  $variables['content']   = $comment->comment;
  $variables['date']      = format_date($comment->timestamp);
  $variables['new']       = $comment->new ? t('new') : '';
  $variables['picture']   = theme_get_setting('toggle_comment_user_picture') ? theme('user_picture', $comment) : '';
  $variables['signature'] = $comment->signature;
  $variables['submitted'] = theme('comment_submitted', $comment);
  $variables['title']     = l($comment->subject, $_GET['q'], array('fragment' => "comment-$comment->cid"));
  $variables['template_files'][] = 'comment-'. $node->type;

  // Set status to a string representation of comment->status.
  if (isset($comment->preview)) {
    $variables['status']  = 'comment-preview';
  }
  else {
    $variables['status']  = ($comment->status == COMMENT_NOT_PUBLISHED) ? 'comment-unpublished' : 'comment-published';
  }
}


/**
 * Create a string of attributes form a provided array.
 *
 * @param $attributes
 * @return string
 */
function ebog_render_attributes($attributes) {
  return omega_render_attributes($attributes);
}
