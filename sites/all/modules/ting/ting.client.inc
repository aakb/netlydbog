<?php
/**
 * @file
 * Wrapper functions for Ting client.
 */

// Ting client has a ton of includes. Let's load them.
$includes = array(
  'addi-client/AdditionalInformationService.php',
  'ting-client/lib/TingClient.php',
  'ting-client/lib/adapter/TingClientRequestAdapter.php',
  'ting-client/lib/request/TingClientRequestFactory.php',
  'ting-client/lib/log/TingClientDrupalWatchDogLogger.php'

);

$path = drupal_get_path('module', 'ting') . '/lib/';

foreach ($includes as $include) {
  require_once($path . $include);
}

/**
 * Load an object from Ting by ID.
 *
 * @param string $object_id
 *    Ting object ID.
 * @param bool $enrich
 *    Whether to enrich the object with additional information, covers etc.
 * @return mixed
 *    Object, if found - boolean FALSE if not.
 */
function ting_get_object_by_id($object_id, $enrich = FALSE, $reset = FALSE) {
  $object = FALSE;
  if (!empty($object_id)) {
    static $static_cache = array();
    if (!isset($static_cache[$object_id]) || $reset) {
      if (!$reset && ($cache = cache_get($object_id, 'cache_ting'))) {
        if ($cache->expire < time()) {
          // Cache expire, so delete entry and call self once more.
          cache_clear_all($object_id, 'cache_ting');
          return ting_get_object_by_id($object_id, $enrich, $reset);
        }
        $object =  $cache->data;
      }
      else {
        $request = ting_get_request_factory()->getObjectRequest();
        $request->setObjectId($object_id);

        // Add agency and profile
        $request = ting_add_agency($request);
        $request = ting_add_profile($request);

        if ($enrich) {
          $request = ting_add_relations($request);
        }
        $object = ting_execute($request);

        if ($object) {
          $object = ting_add_object_info($object);
        }

        // If object have to be enriched, wait with caching.
        if (!$enrich) {
          // Set static cache.
          $static_cache[$object_id] = $object;

          // Set database cache for 1 hour.
          cache_set($object_id, $object, 'cache_ting', time() + 3600);
        }
      }
    }
    else {
      $object = $static_cache[$object_id];
    }
  }

  if ($object && $enrich && empty($object->enriched)) {
    $object = array_shift(ting_add_additional_info(array($object)));
    $object->enriched = TRUE;

    // Update static and database cache.
    $static_cache[$object_id] = $object;
    cache_set($object_id, $object, 'cache_ting', time() + 3600);
  }

  return $object;
}

/**
 * Load an object from Ting by local ID (faust number).
 *
 * @param string $local_id
 *    Local identifier
 * @param bool $enrich
 *    Whether to enrich the object with additional information, covers etc.
 * @return mixed
 *    TingClientObject if found, FALSE if not.
 */
function ting_get_object_by_local_id($local_id, $enrich = FALSE) {
  if (empty($local_id)) { return FALSE; }
  $request = ting_get_request_factory()->getObjectRequest();
  $request->setLocalId($local_id);

  // Add agency and profile
  $request = ting_add_agency($request);
  $request = ting_add_profile($request);

  if ($enrich) {
    $request = ting_add_relations($request);
  }
  $object = ting_execute($request);

  if ($object) {
    $object = ting_add_object_info($object);
    $object = ($enrich) ? array_shift(ting_add_additional_info(array($object))) : $object;
  }
  return ($object) ? $object : FALSE;
}

/**
 * Display a Ting collection of objects.
 *
 * @param string $object_id
 *   Ting object ID.
 * @param bool $enrich
 *   Whether to enrich objects in the collection with additional information,
 *   covers etc.
 *
 * @return mixed
 *   Collection object, if found - boolean FALSE if not.
 */
function ting_get_collection_by_id($object_id, $enrich = FALSE) {
  // If passed object_id is empty, save ourselves the trouble of trying
  // to load the Ting client, do a request, etc. Big speedup for empty
  // Ting reference fields.
  if (empty($object_id)) { return FALSE; }

  static $cache = array();

  if (isset($cache[$object_id])) {
    $collection = $cache[$object_id];
  }
  else {
    $request = ting_get_request_factory()->getCollectionRequest();
    $request->setObjectId($object_id);

    // Add agency and profile
    $request = ting_add_agency($request);
    $request = ting_add_profile($request);

    $collection = ting_execute($request);

    if ($collection) {
      $collection = ting_add_collection_info($collection);
    }
  }

  if ($collection && $enrich && empty($collection->enriched)) {
    $collection = ting_add_additional_info($collection);
    $collection->enriched = TRUE;
  }

  $cache[$object_id] = $collection;

  return $collection;
}

/**
 * Return the possible sort keys and their corresponding labels.
 *
 * @return
 *   A key/value array of sort keys and labels.
 */
function ting_search_rank_options() {
  return array(
    'rank_creator' => t('Rank by creator'),
    'rank_general' => t('Rank by general relevance'),
    'rank_title' => t('Rank by title'),
  );
}

/**
 * Return the possible sort keys and their corresponding labels.
 *
 * @return
 *   A key/value array of sort keys and labels.
 */
function ting_search_sort_options() {
  return array(
    '' => t('Relevance'),
    'title_ascending' => t('Title – A → Z'),
    'title_descending' => t('Title – Z → A'),
    'creator_ascending' => t('Author – A → Z'),
    'creator_descending' => t('Author – Z → A'),
    'date_descending' => t('Year of publication – newest first'),
    'date_ascending' => t('Year of publication – oldest first'),
  );
}

/**
 * Performs a search agains the
 *
 * @param string $query
 *    The search query
 * @param int $page
 *    The page number to retrieve search results for
 * @param int $resultsPerPage
 *    The number of results to include per page
 * @param array $options
 *    Options to pass to the search. Possible options are:
 *    - facets: Array of facet names for which to return results. Default: facet.subject, facet.creator, facet.type, facet.date, facet.language
 *    - numFacets: The number of terms to include with each facet. Default: 10
 *    - enrich: Whether to include additional information and cover images with each object. Default: false
 *    - sort: The key to sort the results by. Default: "" (corresponds to relevance). The possible values are defined by the sortType type in the XSD.
 * @return TingClientSearchResult
 *    The search result
 */
function ting_do_search($query, $page = 1, $resultsPerPage = 10, $options = array()) {
  $request = ting_get_request_factory()->getSearchRequest();
  $request->setQuery($query);
  $request->setStart($resultsPerPage * ($page - 1) + 1);
  $request->setNumResults($resultsPerPage);

  $request->setFacets((isset($options['facets'])) ? $options['facets'] : array('facet.subject', 'facet.creator', 'facet.type', 'facet.category', 'facet.language', 'facet.date', 'facet.acSource'));
  $request->setNumFacets((isset($options['numFacets'])) ? $options['numFacets'] : ((sizeof($request->getFacets()) == 0) ? 0 : 10));
  $request->setRank((isset($options['rank']) && $options['rank']) ? $options['rank'] : 'rank_general');
  $request->setSort((isset($options['sort']) && $options['sort']) ? $options['sort'] : NULL);
  $request->setAllObjects(isset($options['allObjects']) ? $options['allObjects'] : FALSE);

  // Add agency and profile
  $request = ting_add_agency($request);
  $request = ting_add_profile($request);

  // Apply custom ranking if enabled.
  if (variable_get('ting_ranking_custom', FALSE)) {
    $fields = array();
    foreach (variable_get('ting_ranking_fields', array()) as $field) {
      $fields[] = array(
        'fieldName' => $field['field_name'],
        'fieldType' => $field['field_type'],
        'weight' => $field['weight'],
      );
    }

    if (!empty($fields)) {
      // Add the default anyIndex ranks. Without these, the result set
      // will be mangled.
      $fields[] = array(
        'fieldName' => 'cql.anyIndexes',
        'fieldType' => 'phrase',
        'weight' => 1,
      );

      $fields[] = array(
        'fieldName' => 'cql.anyIndexes',
        'fieldType' => 'word',
        'weight' => 1,
      );

      $request->setUserDefinedRanking(array('tieValue' => 0.1, 'rankField' => $fields));
    }
  }
  // Otherwise, use the ranking setting.
  else {
    $request->setRank((isset($options['rank']) && $options['rank']) ? $options['rank'] : 'rank_general');
  }

  // Apply custom boosts if any.
  $boosts = variable_get('ting_boost_fields', array());
  if ($boosts) {
    $request->setUserDefinedBoost($boosts);
  }

  $searchResult = ting_execute($request);

  //Decorate search result with additional information
  if (is_array($searchResult->collections)) {
    foreach ($searchResult->collections as &$collection) {
      $collection = ting_add_collection_info($collection);
      if (isset($options['enrich']) && $options['enrich']) {
        $collection = ting_add_additional_info($collection);
      }
    }
  }

  return $searchResult;
}

/**
 * @param string $query The prefix to scan for
 * @param int $numResults The numver of results to return
 * @return TingClientScanResult
 */
function ting_do_scan($query, $numResults = 10) {
  $request = ting_get_request_factory()->getScanRequest();
  $request->setField('phrase.anyIndexes');
  $request->setLower($query);
    $request = ting_add_agency($request);
  $request->setNumResults($numResults);
  return ting_execute($request);
}

/**
 * @param string $word The word to get spell suggestions for
 * @param $numResults The number of results to return
 * @return array An array of TingClientSpellSuggestion objects
 */
function ting_get_spell_suggestions($word, $numResults = 10) {
  $request = ting_get_request_factory()->getSpellRequest();
  $request->setWord($word);
  $request->setNumResults($numResults);
  return ting_execute($request);
}

/**
 * @param string $isbn ISBN number to get recommendations from
 * @param $numResults The number of results to return
 * @return array An array of TingClientObjectRecommendation objects
 */
function ting_get_object_recommendations($isbn, $numResults = 10) {
  $request = ting_get_request_factory()->getObjectRecommendationRequest();
  $request->setIsbn($isbn);
  $request->setNumResults($numResults);
  return ting_execute($request);
}

/**
 * Perform a request against Ting and perform error handling if necessary
 *
 * @param $request The request
 * @return mixed Result of the request or false if an error occurs
 */
function ting_execute($request) {
  try {
    // Allow other modules to modify the request just before executing
    // and the response immediatly after returning
    drupal_alter('ting_client_request', $request);
    $response = ting_get_client()->execute($request);
    drupal_alter('ting_client_response', $response);
    return $response;
  } catch (TingClientException $e) {
    watchdog('ting client', 'Error performing request: '.$e->getMessage(), NULL, WATCHDOG_ERROR, 'http://'.$_SERVER["HTTP_HOST"].$_SERVER["REQUEST_URI"]);
    return false;
  }
}

/**
 * Retrieves an initialized Ting client with appropriate request adapter and logger
 *
 * @return TingClient
 */
function ting_get_client() {
  static $client;

  if (!isset($client)) {
    $logger = (variable_get('ting_enable_logging', false)) ? new TingClientDrupalWatchDogLogger() : new TingClientVoidLogger();
    $client = new TingClient(new TingClientRequestAdapter(), $logger);
  }

  return $client;
}

/**
 * Retrieves an initialized Ting client request factory.
 *
 * @return TingClientRequestFactory
 */
function ting_get_request_factory() {
  static $requestFactory;

  if (!isset($requestFactory)) {
    $urlVariables = array(
      'search' => 'ting_search_url',
      'scan' => 'ting_scan_url',
      'object' => 'ting_search_url',
      'collection' => 'ting_search_url',
      'spell' => 'ting_spell_url',
      'recommendation' => 'ting_recommendation_server',
    );

    $urls = array();
    foreach ($urlVariables as $name => $setting) {
      $urls[$name] = variable_get($setting, false);
      if (!$urls[$name]) {
        throw new TingClientException('No Ting webservice url defined for '.$name);
      }
    }

    $requestFactory = new TingClientRequestFactory($urls);
  }

  return $requestFactory;
}

function ting_add_collection_info(TingClientObjectCollection $collection) {
  $types = array();
  $subject_count = array();

  foreach ($collection->objects as $object) {
    $object = ting_add_object_info($object);
    $types[] = $object->type;
    foreach ($object->subjects as $subject) {
      if (!isset($subject_count[$subject])) {
        $subject_count[$subject] = 0;
      }
      $subject_count[$subject]++;
    }
  }
  $collection->types = array_unique($types);

  asort($subject_count);
  // All subjects from all objects with the most common ones first.
  $collection->subjects = array_keys($subject_count);

  $common_object = $collection->objects[0];

  $collection->id = $common_object->id;
  $collection->title = $common_object->title;
  $collection->abstract = $common_object->abstract;
  $collection->creators = $common_object->creators;
  $collection->creators_string = $common_object->creators_string;
  $collection->date = $common_object->date;

  $collection->url = url('ting/collection/' . $collection->id, array('absolute' => true));

  return $collection;
}

function ting_add_object_info(TingClientObject $object) {
  $object->type = isset($object->record['dc:type']['dkdcplus:BibDK-Type'][0]) ? $object->record['dc:type']['dkdcplus:BibDK-Type'][0] : NULL;
  $object->language = !empty($object->record['dc:language'][''][0]) ? $object->record['dc:language'][''][0] : FALSE;

  $object->title = $object->record['dc:title'][''][0];
  $object->abstract = !empty($object->record['dcterms:abstract'][''][0]) ? $object->record['dcterms:abstract'][''][0] : FALSE;
  $object->date = !empty($object->record['dc:date'][''][0]) ? $object->record['dc:date'][''][0] : FALSE;

  $object->creators = array();
  if (!empty($object->record['dc:creator'])) {
    foreach ($object->record['dc:creator'] as $type => $dc_creator) {
      if ($type != 'oss:sort') {
        $object->creators = array_merge($object->creators, $dc_creator);
      }
    }
  }
  $object->creators_string = implode(', ', $object->creators);

  $object->subjects = array();
  if (!empty($object->record['dc:subject'])) {
    foreach ($object->record['dc:subject'] as $type => $dc_subject) {
      if (in_array($type, array('dkdcplus:DBCF', 'dkdcplus:DBCS', 'dkdcplus:DBCM', 'dkdcplus:DBCO', 'dkdcplus:DBCN'))) {
        $object->subjects = array_merge($object->subjects, $dc_subject);
      }
    }
  }

  $object->url = url('ting/object/'.$object->id, array('absolute' => TRUE));

  if (function_exists('ting_proxy_rewrite_download_url')) {
    if (isset($object->record['dc:identifier']['dcterms:URI']) && $object->record['dc:identifier']['dcterms:URI']) {
      foreach ($object->record['dc:identifier']['dcterms:URI'] as $url) {
        $object->online_url = ting_proxy_rewrite_download_url($url);
      }
    }
  }

  if ($object->relations) {
    // Add details to relation objects.
    foreach ($object->relations as $key => $relation) {
      $object->relations[$key] = ting_add_object_info($relation);
    }
  }

  return $object;
}

/**
 * Add additional information info for cover images.
 *
 * @param object $collection
 * @return object
 */
function ting_add_additional_info($collection) {
  /**
   *
   * THIS IS A HACK AND SHOULD BE LOOK AT LATER ON. WE DON'T USE THE ADDITIONAL.
   *
   */

//  $faust_numbers = array();
//
  $objects = (isset($collection->objects)) ? $collection->objects : $collection;
//
//  foreach ($objects as $object) {
//    if ($object->localId) {
//      $faust_numbers[] = $object->localId;
//    }
//  }
//
//  if (sizeof($faust_numbers) > 0) {
//    foreach (ting_get_additional_info($faust_numbers) as $faust_number => $ai) {
//      foreach ($objects as &$object) {
//        if ($faust_number == $object->localId) {
//          $object->additionalInformation = $ai;
//        }
//      }
//    }
//  }
//
  if (isset($collection->objects)) {
    $collection->objects = $objects;
  }
  else {
    $collection = $objects;
  }

  return $collection;
}

/**
 * Get additional info for a number of FAUST numbers.
 *
 * @param array $faust_numbers
 *    Array of the FAUST numbers to look up.
 * @return array
 *    Additional info keyed by FAUST number.
 */
function ting_get_additional_info($faust_numbers) {
  $addiVariables = array(
    'wsdlUrl' => 'addi_wsdl_url',
    'username' => 'addi_username',
    'group' => 'addi_group',
    'password' => 'addi_password',
  );

  foreach ($addiVariables as $name => &$setting) {
    $setting = variable_get($setting, false);
    if (!$setting) {
      watchdog('TingClient', 'Additional Information service setting '.$name.' not set', array(), WATCHDOG_WARNING);
      return $collection;
    }
  }

  extract($addiVariables);

  $additionalInformationService = new AdditionalInformationService($wsdlUrl, $username, $group, $password);

  try {
    return $additionalInformationService->getByFaustNumber($faust_numbers);
  }
  catch (AdditionalInformationServiceException $e) {
    watchdog('TingClient', 'Error retrieving additional information and covers: '.$e->getMessage(), $isbns, WATCHDOG_ERROR);
  }
  // TODO: SoapFault should probably be catched within the Ting client.
  catch (SoapFault $e) {
    watchdog('TingClient', 'Error retrieving additional information and covers: '.$e->getMessage(), $isbns, WATCHDOG_ERROR);
  }

  return array();
}

function ting_add_agency(TingClientRequest $request) {
  if ($agency = variable_get('ting_agency', false)) {
    $request->setAgency($agency);
  }
  return $request;
}

function ting_add_profile(TingClientRequest $request) {
  if ($profile = variable_get('ting_profile', 'opac')) {
    $request->setProfile($profile);
  }

  return $request;
}

function ting_add_relations($request, $type = 'full') {
  $request->setAllRelations(TRUE);
  $request->setRelationData($type);
  return $request;
}
