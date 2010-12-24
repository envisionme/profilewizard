<?php
// $Id$

/**
 * @file
 * Administrative page callbacks for the profilewizard module.
 *
 * @ingroup profilewizard
 */
 
/**
 * Implementations of profile wizard settings form
 */ 
function profilewizard_settings_form() {
  $form = array();
  // get all content types
  $types = array(0 => 'none');
  foreach (node_get_types('types') as  $type) {
    $types[$type->type] = $type->name;
  }
  $form['profilewizard_type'] = array(
    '#type' => 'select',
    '#title' => t('Content-type for wizard'),
    '#default_value' => variable_get('profilewizard_type', null),
    '#options' => $types,
    '#description' => t('Content type to be used for the wizard process'),
  );
  
  // get all type fields
  $fields = array();
  $type_info = content_types(variable_get('profilewizard_type', null));
  foreach($type_info['fields'] as $field) {
    $fields[$field['field_name']] = $field['widget']['label'];
  }
  
  if (sizeof($fields) > 0)
  $form['profilewizard_fields'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Fields to use'),
    '#default_value' => variable_get('profilewizard_fields', null),
    '#options' => $fields,
    '#description' => t('Fields to use for the profile wizard'),
  );
  return system_settings_form($form);
}