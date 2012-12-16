<?php
// $Id$

/**
 * @file
 * Template to render a Ting collection of books.
 */

foreach ($collection->objects as $obj){
  $ting_object = $obj;
  $isbn = $obj->record['dc:identifier']['oss:PROVIDER-ID'][0];
  $alttext = t('@titel af @forfatter',array('@titel' => $ting_object->title, '@forfatter' => $ting_object->creators_string));
?>
  <li class="display-book ting-collection ruler-after line clear-block" id="<?php print $ting_object->id ?>">
    <?php if ($elib[$isbn]['elib_book_cover']) { ?>
      <div class="picture">
        <?php print l(theme('image', $elib[$isbn]['elib_book_cover'], $alttext, $alttext, null, false), $ting_object->url, array('html' => true)); ?>
      </div>
    <?php } ?>
    <div class="record">
      <div class="left">
        <h3>
          <?php print l($ting_object->title, $ting_object->url, array('attributes' => array('class' =>'title'))) ;?>
        </h3>
        <div class="meta">
          <?php if ($ting_object->creators_string) : ?>
            <span class="creator">
              <?php echo t('By !creator_name', array('!creator_name' => $elib[$isbn]['author'])); ?>
            </span>
          <?php endif; ?>
          <div id="<?php print $ting_object->objects[0]->localId ?>"></div>
          <?php if ($ting_object->date) : ?>
            <span class="publication_date">
              <?php echo t('(%publication_date%)', array('%publication_date%' => $ting_object->date)) /* TODO: Improve date handling, localizations etc. */ ?>
            </span>
          <?php endif; ?>
        </div>
        <div class="rating-for-faust">
          <div class="<?php print $ting_object->localId; ?>"></div>
        </div>
        <?php if ($ting_object->subjects) : ?>
          <div class="subjects">
            <h4><?php echo t('Subjects:') ?></h4>
            <ul>
              <?php foreach ($ting_object->subjects as $subject) : ?>
                <li><?php echo $subject ?></li>
              <?php endforeach; ?>
            </ul>
          </div>
        <?php endif; ?>
      </div>
      <div class="right">
        <?php if ($ting_object->abstract) : ?>
          <div class="abstract">
            <p>
              <?php print drupal_substr(check_plain($ting_object->abstract), 0, 200) . '...'; ?>
            </p>
          </div>
        <?php endif; ?>
        <div class="icons">
          <ul>
            <li class="sample"><?php print l(t('Sample'), 'publizon/' . $isbn . '/sample', array('html' => TRUE, 'attributes' => array('target' => '_blank', 'action' => 'sample'))) ?></li>
            <li class="seperator"></li>
            <li class="stream"><?php print l(t('Stream'), 'publizon/' . $isbn . '/stream', array('html' => TRUE,  'attributes' => array('class' => 'stream'))) ?></li>
            <?php
              $platform = publizon_get_client_platform();
              if ($platform == PUBLIZON_PLATFORM_GENERIC) {
                if ($is_loan) {
                  print '<li>' . l(t('Download'), 'publizon/' . $isbn . '/download', array('html' => true, 'attributes' => array('class' => 'download'))) . '</li>';
                }
                else {
                  print '<li>' . l(t('Loan'), 'publizon/' . $isbn . '/download', array('html' => true, 'attributes' => array('class' => 'download'))) . '</li>';
                }
              }
            ?>
          </ul>
        </div>
      </div>
    </div>

  </li>

<?php
	}
