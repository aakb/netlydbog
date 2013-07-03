<?php
/**
 * @file
 * Template for banner block.
 */
?>
<div class="ebog-banner">
  <div class="ebog-banner-image">
    <?php echo l(theme_image($conf['image'], '' , '', array(), FALSE), $conf['url'], array('html' => TRUE, 'absolute' => TRUE)); ?>
  </div>
  <div class="ebog_banner_descr"><?php echo $conf['text']; ?></div>
</div>
<div class="bottom-bar">
  <div class="see-more"><?php echo l($conf['url_text'], $conf['url']); ?></div>
</div>
