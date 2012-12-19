<?php
/**
 * @file
 * Defualt template file for the publizon library field.
 *
 */
?>
<div class="icons">
  <ul>
    <?php foreach ($actions as $link) : ?>
      <li class="<?php echo $link['#class']; ?>"><?php echo $link['#link']; ?></li>
      <li class="seperator"></li>
    <?php endforeach; ?>
  </ul>
</div>