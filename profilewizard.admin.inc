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
function profilewizard_settings() {
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
  return $form;
}