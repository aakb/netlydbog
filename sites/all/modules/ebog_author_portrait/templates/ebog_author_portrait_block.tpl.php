<?php
/**
 * @file
 * Template for author portrait block.
 */
?>
<div class="ebog-author-portrait">
  <div class="ebog-author-portrait-image">
    <?php echo theme_image($conf['image'], '' , '', array(), FALSE); ?>
  </div>
  <div class="ebog-author-portrait-descr">
    <h3 class="title"><?php echo $conf['portrait']['name']; ?></h3>
    <p class="teaser"><?php echo $conf['portrait']['teaser']; ?></p>
    <p class="forfatterweb-link"><?php echo l(theme_image(drupal_get_path('theme', 'ebog') . '/images/forfatterweb.jpg', '' , '', array(), FALSE), 'http://www.forfatterweb.dk/', array('html' => TRUE, 'absolute' => TRUE, 'attributes' => array('target' => '_blank', 'title' => 'Forfatterweb'))); ?></p>
  </div>
  <div class="clear"></div>
</div>
<div class="bottom-bar">
  <div class="see-more"><?php echo l($conf['portrait']['url'], $conf['portrait']['more'], array('attributes' => array('target' => '_blank', 'title' => $conf['portrait']['more']))); ?></div>
</div>
