<?php
/**
 * @file
 * Handles all related to generate data for the front page feed.
 */

/**
 * Menu callback to generate front page feed.
 */
function app_feeds_frontpage() {
  $feed = new AppFeed();

  // Add news to the feed.
  _app_feeds_build_news($feed);

  // Add video to the feed.
  _app_feeds_build_videos($feed);

  // Add author portraits to the feed.
  _app_feeds_build_portraits($feed);

  // Add recommendations to the feed.
  _app_feeds_build_reviews($feed);

  $feed->send();
}

/**
 * Helper function to generate news items and add the to the feed.
 *
 * @param AppFeed $feed
 *   The feed object to which the items are added.
 */
function _app_feeds_build_news(AppFeed $feed) {
  // Get last news nodes.
  $nodes = app_feeds_load_nodes('article', 2);

  // Add each node to the feed.
  foreach ($nodes as $node) {
    // Build content array.
    $content = array(
      'source' => array(
        'tag' => 'source',
        'text' => 'netlydbog',
      ),
      'type' => array(
        'tag' => 'type',
        'text' => 'news',
      ),
      'date' =>  array(
        'tag' => 'date',
        'text' => $node->created,
      ),
      'title' =>  array(
        'tag' => 'title',
        'text' => htmlspecialchars($node->title, ENT_QUOTES),
      ),
      'description' =>  array(
        'tag' => 'description',
        'text' => app_feeds_filter_content($node->teaser),
      ),
      'content' => array(
        'tag' => 'content',
        'text' =>  app_feeds_filter_content($node->body),
      ),
      'link_content' => array(
        'tag' => 'link',
        'text' => url('node/' . $node->nid, array('absolute' => TRUE)),
        'attributes' => array(
          'type' => 'content',
        ),
      ),
      'link_thumbnail' => array(
        'tag' => 'link',
        'text' => url($node->field_image[0]['filepath'], array('absolute' => TRUE)),
        'attributes' => array(
          'type' => 'thumbnail',
        ),
      ),
    );

    // Add object id's if any.
    if (!is_null($node->field_books[0]['ting_object_id'])) {
      module_load_include('client.inc', 'ting');
      $content['books'] = array(
        'tag' => 'isbns',
        'children' => array(),
      );
      foreach ($node->field_books as $book) {
        $object = ting_get_object_by_id($book['ting_object_id']);
        if ($object) {
          $isbn = $object->record['dc:identifier']['oss:PROVIDER-ID'][0];
          $product = new PublizonProduct($isbn);
          $content['books']['children'][] = array(
            'tag' => 'isbn',
            'text' => $isbn,
            'attributes' => array(
              'cover' => url($product->cover, array('absolute' => TRUE)),
              'faust' => $object->localId,
            ),
          );
        }
      }
    }

    $feed->addItem($node->nid, $content);
  }
}

/**
 * Helper function to generate review items and add the to the feed.
 *
 * @param AppFeed $feed
 *   The feed object to which the items are added.
 */
function _app_feeds_build_reviews(AppFeed $feed) {
  $reviews = litteratursiden_feed_compare_get_cached_feed(2);

  foreach ($reviews['data'] as $review) {
    $product = new PublizonProduct($review['isbn']);
    $object = $product->getTingObject();
    $content = array(
      'source' => array(
        'tag' => 'source',
        'text' => 'netlydbog',
      ),
      'type' => array(
        'tag' => 'type',
        'text' => 'review',
      ),
      'date' =>  array(
        'tag' => 'date',
        'text' => $review['created'],
      ),
      'title' =>  array(
        'tag' => 'title',
        'text' => htmlspecialchars($review['title'], ENT_QUOTES),
      ),
      'description' =>  array(
        'tag' => 'description',
        'text' => app_feeds_filter_content($review['abstract']),
      ),
      'link_content' => array(
        'tag' => 'link',
        'text' => $review['link'],
        'attributes' => array(
          'type' => 'content',
        ),
      ),
      'link_thumbnail' => array(
        'tag' => 'link',
        'text' => url($product->cover, array('absolute' => TRUE)),
        'attributes' => array(
          'type' => 'thumbnail',
        ),
      ),
      'isbn' => array(
        'tag' => 'isbn',
        'text' => $review['isbn'],
        'attributes' => array(
          'cover' => url($product->cover, array('absolute' => TRUE)),
          'faust' => $object->localId,
        ),
      ),
    );

    $feed->addItem($review['isbn'], $content);
  }
}

/**
 * Helper function to generate video items and add the to the feed.
 *
 * @param AppFeed $feed
 *   The feed object to which the items are added.
 */
function _app_feeds_build_videos(AppFeed $feed) {
  // Get last news nodes.
  $nodes = app_feeds_load_nodes('video', 2);

  // Add each node to the feed.
  foreach ($nodes as $node) {
    // Build content array.
    $content = array(
      'source' => array(
        'tag' => 'source',
        'text' => 'netlydbog',
      ),
      'type' => array(
        'tag' => 'type',
        'text' => 'video',
      ),
      'date' =>  array(
        'tag' => 'date',
        'text' => $node->created,
      ),
      'title' =>  array(
        'tag' => 'title',
        'text' => htmlspecialchars($node->title, ENT_QUOTES),
      ),
      'description' =>  array(
        'tag' => 'description',
        'text' => app_feeds_filter_content($node->field_video_description[0]['value']),
      ),
      'link_content' => array(
        'tag' => 'link',
        'text' => $node->field_video_url[0]['value'],
        'attributes' => array(
          'type' => 'content',
        ),
      ),
    );


    // Get video image link.
    $video = ebog_embed_video_parse_url($node->field_video_url[0]['value']);
    $content['link_thumbnail'] = array(
      'tag' => 'link',
      'text' =>  'http://img.youtube.com/vi/' . $video['embed_code'] . '/0.jpg',
      'attributes' => array(
        'type' => 'thumbnail',
      ),
    );

    // Add object id's if any.
    if (!is_null($node->field_video_book[0]['ting_object_id'])) {
      module_load_include('client.inc', 'ting');
      $object = ting_get_object_by_id($node->field_video_book[0]['ting_object_id']);
      if ($object) {
        $isbn = $object->record['dc:identifier']['oss:PROVIDER-ID'][0];
        $product = new PublizonProduct($isbn);

        $content['isbn'] = array(
          'tag' => 'isbn',
          'text' => $isbn,
          'attributes' => array(
            'cover' => url($product->cover, array('absolute' => TRUE)),
            'faust' => $object->localId,
          ),
        );
      }
    }

    $feed->addItem($node->nid, $content);
  }
}

/**
 * Helper function to generate portrait items and add the to the feed.
 *
 * @param AppFeed $feed
 *   The feed object to which the items are added.
 */
function _app_feeds_build_portraits(AppFeed $feed) {
  // Get last news nodes.
  $nodes = app_feeds_load_nodes('portrait', 2);

  // Add each node to the feed.
  foreach ($nodes as $node) {
    // Build content array.
    $content = array(
      'source' => array(
        'tag' => 'source',
        'text' => 'netlydbog',
      ),
      'type' => array(
        'tag' => 'type',
        'text' => 'author',
      ),
      'date' =>  array(
        'tag' => 'date',
        'text' => $node->created,
      ),
      'title' =>  array(
        'tag' => 'title',
        'text' => htmlspecialchars($node->field_portrait_author[0]['value'], ENT_QUOTES),
      ),
      'description' =>  array(
        'tag' => 'description',
        'text' => app_feeds_filter_content($node->body),
      ),
      'link_content' => array(
        'tag' => 'link',
        'text' => $node->field_portrait_more_link_url[0]['value'],
        'attributes' => array(
          'type' => 'content',
        ),
      ),
      'link_thumbnail' => array(
        'tag' => 'link',
        'text' => url($node->field_portrait_image[0]['filepath'], array('absolute' => TRUE)),
        'attributes' => array(
          'type' => 'thumbnail',
        ),
      ),
    );

    $feed->addItem($node->nid, $content);
  }
}
