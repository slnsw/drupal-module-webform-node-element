<?php

namespace Drupal\webform_node_element\Element;

use Drupal\Core\Render\Element\RenderElementBase;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\webform_node_element\Event\WebformNodeElementPreRender;


/**
 * Provides a render element to display a node.
 *
 * @FormElement("webform_node_element")
 */
class WebformNodeElement extends RenderElementBase {

  /**
   * {@inheritdoc}
   */
  public function getInfo() {
    $class = get_class($this);
    return [
      '#pre_render' => [[$class, "preRenderWebformNodeElement"]],
      '#webform_node_element_nid' => NULL,
    ];
  }

  /**
   * Add the rendered node to the element.
   *
   * @param array $element
   *   An associative array containing the properties of the element.
   *
   * @return array
   *   The $element.
   */
  public static function preRenderWebformNodeElement(array $element) {
    $element['#markup'] = "";

    $nid = $element['#webform_node_element_nid'];
    $element_id = $element['#webform_id'];
    $dispatcher = \Drupal::service('event_dispatcher');
    $event =  new WebformNodeElementPreRender($element_id, $nid, 'webform_element');
    
    $dispatcher->dispatch($event,WebformNodeElementPreRender::PRERENDER);
    $nid = $event->getNid();
    // This guarantees the nid is valid.
    $nid_array = \Drupal::entityQuery('node')->condition('nid', $nid)->accessCheck(TRUE)->execute();
    $nid = reset($nid_array);

    $display_mode = $event->getDisplayMode();

    if ($nid && $display_mode) {
      $node = \Drupal::entityTypeManager()->getStorage('node')->load($nid);
      if ($node->access('view')) {
        $view_builder = \Drupal::entityTypeManager()->getViewBuilder('node');

        if ($node && $view_builder) {
          if ($render_array = $view_builder->view($node, $display_mode)) {
            $element['#markup'] = \Drupal::service('renderer')->render($render_array, false);
          }
        }
      }
    }
    else {
      \Drupal::logger('webform_node_element')->notice('webform_node_element @element_id not rendered because nid was set to "@nid" and display_mode was set to "@display_mode"',
        [
          '@element_id' => $element_id,
          '@nid' => $nid,
          '@display_mode' => $display_mode,
        ]);
    }

    return $element;
  }

}
