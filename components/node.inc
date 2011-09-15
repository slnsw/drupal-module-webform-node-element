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
      'nid' => 0,
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

  // Load the available view modes for 'node' entities, so users can also
  // choose custom ones if they have defined any.
  // Maybe RSS and search results shouldn't be listed. Maybe. Dunno.
  $view_modes = array();
  $data = entity_get_info('node');
  foreach ($data['view modes'] as $key => $value) {
    $view_modes[$key] = $value['label'];
  }

  $form['value'] = array(
    '#type' => 'textfield',
    '#size' => 5,
    '#title' => t('Node'),
    '#default_value' => $component['value'],
    '#description' => t('Enter a valid node id, whose body will be shown as markup in the webform.') . theme('webform_token_help'),
    '#weight' => -1,
    '#element_validate' => array('_webform_edit_validate_node'),
  );
  $form['extra']['nid'] = array(
    '#type' => 'hidden',
    '#default_value' => $component['extra']['nid'],
  );
  $form['extra']['view_mode'] = array(
    '#type' => 'radios',
    '#title' => t('Display Mode'),
    '#default_value' => $component['extra']['view_mode'],
    '#description' => t('Choose how you want the referenced node to be displayed.'),
    '#options' => $view_modes,
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

  // If ctools is present, use node title autocomplete.
  if (module_exists('ctools')) {
    $form['value']['#description'] = t('Enter a valid node id, or start typing to find the node whose body will be shown as markup in the webform.') . theme('webform_token_help');
    $form['value']['#autocomplete_path'] = 'ctools/autocomplete/node';
    unset($form['value']['#size']);
  }

  return $form;
}

/**
 * Element validation callback. Ensure a valid node ID was entered.
 */
function _webform_edit_validate_node($element, &$form_state) {
  if (!empty($element['#value'])) {

    // If ctools, the value might be in the form of "node title [nid: N]".
    if (module_exists('ctools')) {
      _webform_node_autocomplete_validate($element);
    }

    $node = node_load($element['#value']);
    if (empty($node->nid) || (empty($node->status) && !user_access('administer nodes'))) {
      form_error($element, t('Invalid node'));
    }

    // Store the node id that we'll use. #value is just for show :-)
    $form_state['values']['extra']['nid'] = $node->nid;
  }
  return TRUE;
}

/**
 * Implements _webform_render_component().
 */
function _webform_render_node($component, $value = NULL, $filter = TRUE) {
  $node = node_load($component['extra']['nid']);

  // Add a bit of error checking; the target node may have been deleted.
  if (empty($node->nid)) {
    drupal_set_message(t('Unable to load referenced node id !nid', array('!nid' => $component['extra']['nid'])), 'error');
    return array();
  }

  // All good, render the node body in the chosen format.
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

/**
 * Overridable theme function.
 */
function theme_webform_display_node($variables) {
  $element = $variables['element'];
  return $element['#format'] == 'html' ? check_plain($element['#markup']) : $element['#markup'];
}

/**
 * Helper to pull apart a possible ctools node autocomplete submission.
 *
 * This is lifted verbatim from the ctools_node_content_type_edit_form_validate()
 * function at http://drupalcontrib.org/api/drupal/contributions--ctools--plugins--content_types--node--node.inc/7/source
 *
 * @param &$element
 *   A reference to the form element submitted by the user.
 */
function _webform_node_autocomplete_validate(&$element) {
  $nid = $element['#value'];
  $preg_matches = array();

  $match = preg_match('/\[id: (\d+)\]/', $nid, $preg_matches);
  if (!$match) {
    $match = preg_match('/^id: (\d+)/', $nid, $preg_matches);
  }

  if ($match) {
    $nid = $preg_matches[1];
  }
  if (is_numeric($nid)) {
    $node = db_query('SELECT nid FROM {node} WHERE nid = :nid', array(':nid' => $nid))->fetchObject();
  }
  else {
    $node = db_query('SELECT nid FROM {node} WHERE LOWER(title) = LOWER(:title)', array(':title' => $nid))->fetchObject();
  }
  if ($node) {
    $element['#value'] = $node->nid;
  }

  return;
}
