<?php
/**
 * @file
 * Webform module node component.
 */

/**
 * Implements _webform_defaults_component().
 */
function _webform_defaults_node() {
  return array(
    'name' => '',
    'form_key' => NULL,
    'pid' => 0,
    'weight' => 0,
    'value' => '',
    'extra' => array(
      'title' => 0,
      'view_mode' => 'full',
    ),
  );
}

/**
 * Implements _webform_theme_component().
 */
function _webform_theme_node() {
  return array(
    'webform_display_node' => array(
      'render element' => 'element',
    ),
  );
}

/**
 * Implements _webform_edit_component().
 */
function _webform_edit_node($component) {
  $form = array();
  $form['value'] = array(
    '#type' => 'textfield',
    '#size' => 5,
    '#title' => t('Node ID'),
    '#default_value' => $component['value'],
    '#description' => t('Enter a valid node id, whose body will be shown as markup in the webform.') . theme('webform_token_help'),
    '#weight' => -1,
    '#element_validate' => array('_webform_edit_validate_node'),
  );
  $form['extra']['view_mode'] = array(
    '#type' => 'radios',
    '#title' => t('Display Mode'),
    '#default_value' => $component['extra']['view_mode'],
    '#description' => t('Choose how you want the referenced node to be displayed.'),
    '#options' => array(
      'teaser' => t('Display the node teaser'),
      'full' => t('Display the full node body'),
    ),
  );
  $form['extra']['title'] = array(
    '#type' => 'checkbox',
    '#title' => t('Override Title'),
    '#default_value' => $component['extra']['title'],
    '#description' => t('If checked, the title of the referenced node will be used as the component label.'),
    '#options' => array(0, 1),
  );
  $form['extra']['description'] = array();
  $form['display'] = array('#type' => 'markup'); // Hide the display options.

  return $form;
}

/**
 * Element validation callback. Ensure a valid node ID was entered.
 */
function _webform_edit_validate_node($element, &$form_state) {
  if (!empty($element['#value'])) {
    $node = node_load($element['#value']);
    if (empty($node->nid)) {
      form_error($element, t('You must enter a valid node id.'));
    }
  }
  return TRUE;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_node($component, $value = NULL, $filter = TRUE) {
  $node = node_load($component['value']);
  $content = node_view($node, $component['extra']['view_mode']);

  // If there is a read-more link, default that to a new browser window.
  if (!empty($content['links']['node']['#links']['node-readmore'])) {
    $content['links']['node']['#links']['node-readmore']['attributes']['target'] = '_new';
  }

  $element = array(
    '#type' => 'markup',
    '#title' => $component['extra']['title'] ? check_plain($node->title) : $component['name'],
    '#weight' => $component['weight'],
    '#markup' => render($content['body']) . render($content['links']),
    '#input_format' => 'html',
    '#theme_wrappers' => array('webform_element'),
    '#webform_component' => $component,
  );

  $element['#value'] = $element['#markup'];
  return $element;
}

function theme_webform_display_node($variables) {
  $element = $variables['element'];
  return $element['#format'] == 'html' ? check_plain($element['#markup']) : $element['#markup'];
}
