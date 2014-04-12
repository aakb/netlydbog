<?php
/**
 * @file
 */
?>
<div class="feed-and-compare-page clear-block">
  <?php if ($items['status'] == 'empty') : ?>
    <div class="feed_and_compare_empty">
      <h1>Der var desv&aelig;rre ingen materialer som matchede anmeldelser hos Litteratursiden.</h1>
    </div>
  <?php endif ?>

  <?php if ($items['status'] == 'error') : ?>
    <div class="feed_and_compare_error">
      <b>Error:</b> <?php echo $items['message']; ?>
    </div>
  <?php endif; ?>

  <?php if (($items['status'] == 'ok') || ($items['status'] == 'notfull')) : ?>
    <?php foreach ($items['data'] as $key => $item) : ?>
      <div class="feed_and_compare_item display-book">
        <div class="left">
        <?php
          $alttext = t('@titel af @forfatter', array('@titel' => $item['title'], '@forfatter' => $item['author']));
          $image = theme('image', $item['image'], $alttext, $alttext, array('width' => '170px'), FALSE);
          echo l($image, $item['url'], array('html' => TRUE));
        ?>
        </div>
        <div class="record right">
          <h3 class="title">
            <?php echo l($item['title'], $item['url'], array('html' => TRUE)); ?>
          </h3>
          <div class="author">
            <?php echo l($item['author'], 'ting/search/' . urlencode($item['author']), array('html' => TRUE)); ?>
          </div>
          <div class="descr">
            <?php echo $item['abstract']; ?>
          </div>
        </div>
      </div>
    <?php endforeach; ?>
  <?php endif; ?>
</div>
