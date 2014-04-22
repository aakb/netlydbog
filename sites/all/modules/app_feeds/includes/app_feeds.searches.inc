<?php
/**
 * @file
 * Handles all related to generate data for the search/category feed.
 */

/**
 * Menu callback to generate search/category feed.
 */
function app_feeds_searchs() {
  $feed = new AppFeed();

  $tree = menu_tree_all_data('menu-elib-categories');
  foreach ($tree as $menu_subtree) {
    // Only generate links if menu item is active and have sub-tree.
    if (!$menu_subtree['link']['hidden'] && $menu_subtree['below'] != FALSE) {
      $categories = _app_feed_searches_pares_categories($menu_subtree);
      _app_feed_searches_add_item($feed, $menu_subtree['link']['mlid'], $menu_subtree['link']['title'], $categories);
    }
  }

  $feed->send();
}

/**
 * Filter menu tree to collect title and link path (searches).
 *
 * @param array $menu_subtree
 *   Partly menu tree.
 *
 * @return array
 *   The searches indexed by title.
 */
function _app_feed_searches_pares_categories($menu_subtree) {
  $categories = array();

  foreach ($menu_subtree['below'] as $link) {
    // Only generate category if menu item is active.
    if (!$link['link']['hidden']) {
      $categories[$link['link']['title']] = strtr($link['link']['link_path'], array('ting/search/' => ''));
    }
  }

  return $categories;
}

/**
 * Add item to feed.
 *
 * @param Appfeed $feed
 *   The feed object to which the items are added.
 * @param string $id
 *   ID for the feed item (menu link id).
 * @param string $title
 *   Item title
 * @param array $categories
 *   The items to generate as searches/queries.
 */
function _app_feed_searches_add_item(AppFeed $feed, $id, $title, $categories) {
  $content = array(
    'source' => array(
      'tag' => 'source',
      'text' => 'netlydbog',
    ),
    'title' => array(
      'tag' => 'title',
      'text' => $title,
    ),
    'searches' => array(
      'tag' => 'searches',
      'children' => array(),
    ),
  );

  // Loop over categories and build searches.
  foreach ($categories as $title => $query) {
    // If query is shorten by google, find the query.
    if (strpos($query, 'http://goo.gl') !== FALSE) {
      $query = app_feeds_decode_google_urls($query);
    }

    $content['searches']['children'][] = array(
      'tag' => 'query',
      'text' => $query,
      'attributes' => array(
        'title' => htmlspecialchars($title, ENT_QUOTES),
      ),
    );
  }

  $feed->addItem($id, $content);
}

/**
 * Helper function to get long URLs from google short urls.
 *
 * @param string $url
 *   The short google url (http://goo.gl/fbsS).
 *
 * @return string
 *   The long URL behind the short URL.
 */
function app_feeds_decode_google_urls($url) {
  // Use CURL to call google API to get long URL.
  $ch = curl_init();
  curl_setopt($ch, CURLOPT_URL, 'https://www.googleapis.com/urlshortener/v1/url?shortUrl=' . $url);
  curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
  curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, FALSE);
  $json = curl_exec($ch);
  curl_close($ch);

  // Decode json and parse URL
  global $base_url;
  $json = json_decode($json);
  $query = str_replace($base_url . '/ting/search/', '', $json->longUrl);

  // return response as object
  return urldecode($query);
}
