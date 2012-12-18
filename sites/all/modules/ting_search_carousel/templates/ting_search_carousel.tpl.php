<?php
/**
 * @file
 *
 *
 * * Available variables:
 * - $searches: Array with each tab search.
 *
 */
?>

<div class="carousel-outer">
  <div id="carousel" class="carousel-wrapper">
    <div class="ajax-loader"></div>
    <div class="carousel-inner">
      <ul class="carousel-runner">
      </ul>
    </div>
  </div>
  <!-- Only print tabs if there is more than 1 -->
  <?php if (count($searches) > 1): ?>
  <div class="carousel-tabs">
    <ul>
      <?php foreach ($searches as $i => $search): ?>
        <li class="<?php echo ($i == 0) ? 'active' : ''; ?>" rel="<?php echo $i ?>">
          <a href="#"><?php echo $search['title'] ?></a>
        </li>
      <?php endforeach; ?>
    </ul>
  </div>
  <?php endif; ?>
</div>