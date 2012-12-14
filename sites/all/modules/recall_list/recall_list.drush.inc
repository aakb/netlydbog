<?php

/**
 * @file
 * This file implements support for drush into the module.
 */

/**
 * Adds a cache clear option for the recall list cache.
 */
function recall_list_drush_cache_clear(&$types) {
  $types['recall-list'] = 'recall_list_cache_flush';
}

/**
 * Clears out all cache entries about recal lists.
 */
function recall_list_cache_flush() {
  cache_clear_all('*', 'cache_recall_list', TRUE);
}

/**
 * @todo: Clean single user recall list.
 */