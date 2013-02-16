<div class="user-review clear-block">
  <?php
    $object = ting_get_object_by_id($node->field_book[0]['ting_object_id']);
    $product = new PublizonProduct($object->record['dc:identifier']['oss:PROVIDER-ID'][0]);
  ?>
  <div style="width:230px;margin-left:10px;float:right;">
    <?php print theme('publizon_product', $product); ?>
  </div>
  <?php print $n->field_review[0]['value'] ?>
  <div class="meta">
    <?php $u = user_load($node->uid);?>
    <br />
    <?php print t('Skrevet af: !name / !email - Kl. !time',array('!name' => $u->name, '!email' => $u->mail, '!time' => format_date($node->created, 'custom', "H.i, j F Y")))?>
    <?php if (count($taxonomy)) { ?>
       <?php print $terms ?>
    <?php } ?>
  </div>
</div>
