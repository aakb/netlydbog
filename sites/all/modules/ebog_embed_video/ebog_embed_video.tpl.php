<?php
/**
 * @file
 * Default template file for embed video block.
 */
?>
<div class="ebog_embed_video">
  <?php if ($type == 'youtube') : ?>
    <div class="ebog_embed_youtube">
      <object>
        <param name="movie" value="http://www.youtube.com/v/<?php echo $embed_code; ?>" />
        <param name="allowFullScreen" value="true" />
        <param name="allowScriptAccess" value="always" />
        <param name="wmode" value="opaque" />
        <embed
          src="https://www.youtube.com/v/<?php echo $embed_code; ?>"
          type="application/x-shockwave-flash"
          allowfullscreen="true"
          allowScriptAccess="always"
          wmode="opaque"
          width="100%"
          >
      </object>
    </div>
  <?php endif; ?>

  <?php if ($type == 'vimeo') : ?>
    <div class="ebog_embed_vimeo">
      <iframe
        src="http://player.vimeo.com/video/<?php echo $embed_code; ?>"
        width="100%"
        frameborder="0"
        wmode="opaque"
        webkitAllowFullScreen
        allowfullscreen="true"
        >
      </iframe>
    </div>
  <?php endif; ?>

  <div class="ebog_embed_descr">
    <?php echo $description; ?>
  </div>
</div>

<div class="bottom-bar">
  <div class="see-more ebog_embed_see_more">
    <?php echo $more_link; ?>
  </div>
</div>
