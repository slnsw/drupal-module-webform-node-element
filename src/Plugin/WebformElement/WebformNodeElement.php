<?php

namespace Drupal\webform_node_element\Plugin\WebformElement;

use Drupal\Core\Form\FormStateInterface;
use Drupal\webform\Plugin\WebformElement\WebformMarkupBase;

/**
 * Provides a 'webform_node_element' element.
 *
 * @WebformElement(
 *   id = "webform_node_element",
 *   label = @Translation("Node"),
 *   description = @Translation("Provides an element that renders a node"),
 *   category = @Translation("Markup elements"),
 *   states_wrapper = TRUE,
 * )
 */
class WebformNodeElement extends WebformMarkupBase {

  /**
   * {@inheritdoc}
   */
  public function getDefaultProperties() {
    return parent::getDefaultProperties() + [
      'markup' => '',
      'nid' => NULL,
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function form(array $form, FormStateInterface $form_state) {
    $form = parent::form($form, $form_state);
    $form['node_element'] = [
      '#title' => $this->t('Node Details'),
      '#type' => 'fieldset',
    ];
    $form['node_element']['nid'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Node ID'),
      '#description' => $this->t('The ID of the node to render. Leave empty to listen to an event (tbd). Use a custom display mode called "webform_element".'),
    ];
    return $form;
  }

}
