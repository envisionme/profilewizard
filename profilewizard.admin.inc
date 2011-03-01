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
  foreach (content_profile_get_types() as  $type) {
    $types[$type->type] = $type->name;
  }
	// return if no content profile type specified
	if (count($types) <= 1) {
		$form['error'] = array('#value' => 'No content profile types detected. Please mark a content type as a content profile first.');
		return $form;
	}
  $form['profilewizard_type'] = array(
    '#type' => 'select',
    '#title' => t('Content-type for wizard'),
    '#default_value' => variable_get('profilewizard_type', null),
    '#options' => $types,
    '#description' => t('Content type to be used for the wizard process'),
  );
  
	// if no other type selected return basic form
  if (variable_get('profilewizard_type', null) == null)
		return system_settings_form($form);
		
	// get all type fields
	$fields = array();
  $type_info = content_types(variable_get('profilewizard_type', null));

	// add cck3 multigroup compatability
	$field_groups = array();
	foreach(fieldgroup_groups(variable_get('profilewizard_type', null)) as $group_name => $group) {
		if ($group['group_type'] == 'multigroup')
			$field_groups[$group_name] = $group['label'].' (multigroup)';
	}

	foreach($type_info['fields'] as $field) {
		// only print fields not part of a multigroup
		if (!empty($field_groups)) {
		if (!array_key_exists(fieldgroup_get_group(variable_get('profilewizard_type', null), $field['field_name']), $field_groups))
			$fields[$field['field_name']] = $field['widget']['label'];
		} else
			$fields[$field['field_name']] = $field['widget']['label'];
  }
	
	// add multigroup fields to selection list
	$fields = array_merge($field_groups, $fields);
	// sorting field list
	asort($fields);
  
	if (sizeof($fields) > 0)
	$form['profilewizard_steps'] = array(
    '#type' => 'select',
    '#title' => t('Amount of steps needed'),
    '#default_value' => variable_get('profilewizard_steps', null),
    '#options' => array(1 => 1, 2 => 2, 3 => 3, 4 => 4, 5 => 5),
    '#description' => t('Amount of steps to be used for wizard'),
  );
	
  if (sizeof($fields) > 0 && variable_get('profilewizard_steps', null) == 1) {
		$form['profilewizard_fields'] = array(
			'#type' => 'checkboxes',
			'#title' => t('Fields to use'),
			'#default_value' => variable_get('profilewizard_fields', null),
			'#options' => $fields,
			'#description' => t('Fields to use for the profile wizard'),
		);
	} elseif (sizeof($fields) > 0 && variable_get('profilewizard_steps', null) > 0) {
		// iterate through and give each step a it's own fieldset
		for ($step = 1; $step <= variable_get('profilewizard_steps', null); $step++) {
			$form['step'.$step] = array(
				'#type' => 'fieldset',
				'#title' => t('Step '.$step),
				// '#weight' => 5,
				'#collapsible' => TRUE, 
				'#collapsed' => TRUE,
			);
			$form['step'.$step]['profilewizard_label_step'.$step] = array(
				'#type' => 'textfield',
				'#title' => t('Step Label'),
				'#default_value' => variable_get('profilewizard_label_step'.$step, null),
				'#description' => t('Label name of the step'),
			);
			$form['step'.$step]['profilewizard_fields_step'.$step] = array(
				'#type' => 'checkboxes',
				'#title' => t('Fields to use'),
				'#default_value' => variable_get('profilewizard_fields_step'.$step, null),
				'#options' => $fields,
				'#description' => t('Fields to use for the profile wizard'),
			);
		}
	}
  return system_settings_form($form);
}