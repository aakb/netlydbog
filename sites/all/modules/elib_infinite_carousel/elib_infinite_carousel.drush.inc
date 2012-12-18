<?php

/**
 * @file
 * This file implements support for drush into the module.
 */

/**
 * Adds a cache clear option for the carousel cache.
 */
function elib_infinite_carousel_drush_cache_clear(&$types) {
  $types['infinite-carousel'] = 'elib_infinite_carousel_cache_flush';
}

/**
 * Clears out all cache entries about infinite carousel.
 */
function elib_infinite_carousel_cache_flush() {
  cache_clear_all('elib_infinite_carousel_cache_*', 'cache', TRUE);
}
