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
    $this->feed->formatOutput = true;

    // Create the root element.
    $this->root = $this->feed->appendChild($this->feed->createElement('feed'));
  }

  /**
   * Generates an item tag and add it to the feed.
   *
   * @param string $guid
   *   Global ID for the element.
   *
   * @return DOMElement
   *   A reference to the created DOMElement.
   */
  private function createItem($guid) {
    $item = $this->feed->createElement('item');

    $item->appendChild($this->feed->createElement('GUID', $guid));

    $this->root->appendChild($item);
    return $item;
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
   *        'attributes' => array(
   *          'type' => 'content',
   *        ),
   *        'text' => 'value',
   *      ),
   *    )
   */
  public function addItem($guid, array $content) {
    $item = $this->createItem($guid);

    foreach ($content as $key => $value) {
      $element = $this->feed->createElement($key, $value['text']);

      // Add attributes to the element if they exists.
      if (!empty($value['attributes'])) {
        foreach ($value['attributes'] as $attribute => $attribute_value) {
          $element->appendChild(
            $this->feed->createAttribute($attribute)->appendChild(
              $this->feed->createTextNode($attribute_value)
            )
          );
        }
      }

      // Add element to the item.
      $item->appendChild($element);
    }
  }
}
