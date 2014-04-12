<?php
/**
 * @file
 * AppFeed object can be used to help build the feeds.
 */

class AppFeed {
  private $feed = NULL;
  private $root = NULL;

  /**
   * Default constructor that initializes the XML feed.
   */
  public function __construct() {
    // Create XML dom.
    $this->feed = new DOMDocument('1.0', 'UTF-8');

    // Add some configuration to the XML document
    $this->feed->formatOutput = TRUE;

    // Create the root element.
    $this->root = $this->feed->appendChild($this->feed->createElement('feed'));
  }

  /**
   * Generates an item tag and add it to the feed.
   *
   * @param string $id
   *   ID for the element (nid or ISBN could be used).
   *
   * @return DOMElement
   *   A reference to the created DOMElement.
   */
  private function createItem($id) {
    $item = $this->feed->createElement('item');
    $item->appendChild($this->feed->createElement('guid', $this->createGuid($id)));
    $this->root->appendChild($item);

    return $item;
  }

  /**
   * Adds attributes to an element.
   *
   * @param $attributes
   *   An array with attributes.
   * @param $element
   *   The element to add the attributes to.
   */
  private function addAttributes($attributes, &$element) {
    // Add attributes to the element if they exists.
    if (!empty($attributes)) {
      foreach ($attributes as $attribute => $attribute_value) {
        $att_element = $this->feed->createAttribute($attribute);
        $att_element->value = $attribute_value;
        $element->appendChild($att_element);
      }
    }
  }

  /**
   * Generate GUID based on private key and ID.
   *
   * @param $id
   *   The id to used to generate GUID.
   *
   * @return string
   *   MD5 hash of the GUID.
   */
  private function createGuid($id) {
    return md5(drupal_get_private_key() + $id);
  }

  /**
   * Sends the XML content by printing it and setting headers.
   */
  public function send() {
    header("Content-Type: application/xml; charset=UTF-8");
    echo $this->feed->saveXML();
  }

  /**
   * Add new item to the feed.
   *
   * @param $guid
   *   Global ID for the item.
   * @param array $content
   *   The content to write into the item element. The format of the content
   *   should be:
   *    array(
   *      'key' =>  array(
   *        'tag' => 'title',
   *        'attributes' => array(
   *          'type' => 'content',
   *        ),
   *        'text' => 'value',
   *      ),
   *      'key' =>  array(
   *        'tag' = 'isbns',
   *        'children' => array (
   *          array(
   *            'tag' = 'isbn',
   *            'text' => 'value',
   *            'attributes' => array(
   *              'type' => 'content',
   *            ),
   *          ),
   *          array(),
   *          .
   *          .
   *          .
   *          ),
   *        ),
   *      ),
   *    )
   */
  public function addItem($guid, array $content) {
    $item = $this->createItem($guid);

    foreach ($content as $value) {
      // Normal element.
      if (empty($value['children'])) {
        $element = $this->feed->createElement($value['tag'], $value['text']);

        // Add attributes to the element if they exists.
        if (!empty($value['attributes'])) {
          $this->addAttributes($value['attributes'], $element);
        }
      }

      if (!empty($value['children'])) {
        $element = $this->feed->createElement($value['tag']);
        foreach ($value['children'] as $child) {
          // Create child element.
          $child_element = $this->feed->createElement($child['tag'], $child['text']);
          if (!empty($child['attributes'])) {
            // Add attributes to the child.
            $this->addAttributes($child['attributes'], $child_element);
          }
          $element->appendChild($child_element);
        }
      }

      // Add element to the item.
      $item->appendChild($element);
    }
  }
}
