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
 *
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
    'title' =>  array(
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
