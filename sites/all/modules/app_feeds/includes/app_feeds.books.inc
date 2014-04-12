<?php
/**
 * @file
 * Handles all related to generate data for the carousel shelf feed.
 */

/**
 * Menu callback to generate book shelf feed.
 */
function app_feeds_books() {
  $feed = new AppFeed();

  // Get search queries and create book shelf based on theme.
  $queries = variable_get('ting_carousel_search_queries', array());
  foreach ($queries as $id => $query) {
    _app_feed_book_shelf_books($feed, $id, $query);
  }

  $feed->send();
}

/**
 * Add search carousel results into XML feed.
 *
 * @param AppFeed $feed
 *  The feed object to which the items are added.
 * @param $id
 *   The ID in the carousel order (tabs).
 * @param $query
 *   The query information used to find the books in the data well.
 */
function _app_feed_book_shelf_books(AppFeed $feed, $id, $query) {
  // Get books in the carousel tab with the id given.
  $books = _ting_search_carousel_get_content($id);

  $content = array(
    'source' => array(
      'tag' => 'source',
      'text' => 'netlydbog',
    ),
    'title' =>  array(
      'tag' => 'title',
      'text' => $query['title'],
    ),
    'query' =>  array(
      'tag' => 'query',
      'text' => $query['query'],
    ),
  );

  // Add object id's if any.
  if (!empty($books)) {
    module_load_include('client.inc', 'ting');
    $content['books'] = array(
      'tag' => 'books',
      'children' => array(),
    );
    foreach ($books as $book) {
      $object = ting_get_object_by_id($book->id);
      if ($object) {
        $isbn = $object->record['dc:identifier']['oss:PROVIDER-ID'][0];
        $product = new PublizonProduct($isbn);
        $content['books']['children'][] = array(
          'tag' => 'book',
          'text' => htmlspecialchars($book->title, ENT_QUOTES),
          'attributes' => array(
            'cover' => url($product->cover, array('absolute' => TRUE)),
            'faust' => $object->localId,
            'isbn' => $isbn,
          ),
        );
      }
    }
  }

  $feed->addItem(md5($query['query']), $content);
}
