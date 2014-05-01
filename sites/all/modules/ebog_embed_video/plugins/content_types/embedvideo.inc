<?php
/**
 * @file
 *
 */


/**
 * Prepare plugin info.
 */
function ebog_embed_video_embedvideo_ctools_content_types() {
  return array(
    'title' => t('Ebog Embed video'),
    'description' => t('Ebog Embed video description.'),
    'single' => TRUE,
    'category' => array('INLEAD'),
  );
}

/**
 * Rendering of the block.
 */
function ebog_embed_video_embedvideo_content_type_render($subtype, $conf, $args, $context) {
  $block = new stdClass();
  $block->title = check_plain('Ebog Embed video');
  $block->content = _ebog_embed_video_block_view();
  return $block;
}

/**
 * 'Edit form' callback for the content type.
 */
function ebog_embed_video_embedvideo_content_type_edit_form(&$form, &$form_state) {
   return $form;
}
