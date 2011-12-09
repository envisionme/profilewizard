<?php
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
  $profilewizard_type = variable_get('profilewizard_type', null);
  $profilewizard_steps = variable_get('profilewizard_steps', 0);
  $profilewizard_step_data = variable_get('profilewizard_step_data', array());

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
    '#default_value' => $profilewizard_type,
    '#options' => $types,
    '#description' => t('Content type to be used for the wizard process'),
  );

	// If no content-type selected, return basic form.
  if ($profilewizard_type == null) {
		return system_settings_form($form);
  }

  // get all type fields
  $fields = array();
  $type_info = content_types($profilewizard_type);

  // add cck3 multigroup compatibility
  $field_groups = array();
  foreach(fieldgroup_groups($profilewizard_type) as $group_name => $group) {
    if ($group['group_type'] == 'multigroup')
      $field_groups[$group_name] = $group['label'].' (multigroup)';
  }

  foreach($type_info['fields'] as $field) {
    // only print fields not part of a multigroup
    if (!empty($field_groups)) {
      if (!array_key_exists(fieldgroup_get_group($profilewizard_type, $field['field_name']), $field_groups))
      $fields[$field['field_name']] = $field['widget']['label'];
    } else
      $fields[$field['field_name']] = $field['widget']['label'];
  }

  // Notify the user if there is no fields in the profile content-type.
  if (empty($fields)) {
    $node_manage_fields_link = l('here', 'admin/content/node-type/' . $profilewizard_type . '/fields', array('query' => drupal_get_destination()));
    drupal_set_message(t('There are currently no fields in your profile
    content-type. You can add a fields !here.',
      array('!here' => $node_manage_fields_link)), 'error');
  }

  // add multigroup fields to selection list
  $fields = array_merge($field_groups, $fields);
  // sorting field list
  asort($fields);

  if (sizeof($fields) > 0)
  $form['profilewizard_steps'] = array(
    '#type' => 'select',
    '#title' => t('Amount of steps needed'),
    '#default_value' => $profilewizard_steps,
    '#options' => array(1 => 1, 2 => 2, 3 => 3, 4 => 4, 5 => 5, 6 => 6,
      7 => 7),
    '#description' => t('Amount of steps to be used for wizard'),
  );

  if (sizeof($fields) > 0 && $profilewizard_steps > 0) {
    $form['profilewizard_step_data'] = array(
      '#type' => 'fieldset',
      '#title' => t('Steps'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
    );

    $delta = 1;
    for ($delta = 1; $delta <= $profilewizard_steps; $delta++) {
      $form['profilewizard_step_data'][$delta] = _profilewizard_form_combo($delta, $profilewizard_step_data[$delta]['label'], $profilewizard_step_data[$delta]['fields'], $fields);
    }
  }

  // Use custom submit handler.
  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
    '#submit' => array('profilewizard_settings_form_submit'),
  );

  return $form;
}

/**
 * Submit handler for settings form.
 */
function profilewizard_settings_form_submit($form, &$form_state) {
  variable_set('profilewizard_type', $form_state['values']['profilewizard_type']);
  variable_set('profilewizard_steps', $form_state['values']['profilewizard_steps']);
  $profilewizard_step_data = $form_state['values']['profilewizard_step_data'];
  /*foreach ($profilewizard_step_data as $delta => $step) {
    if (empty($step['label'])) {
      unset($profilewizard_step_data[$key]);
    }
  }*/
  variable_set('profilewizard_step_data', $profilewizard_step_data);
  drupal_set_message(t('Your changes have been saved.'));
}

/**
 * Profilewizard combo boxes for admin settings form.
 */
function _profilewizard_form_combo($delta, $label, $fields, $all_fields) {
  $form = array(
    '#tree' => TRUE,
  );
  $form[$delta] = array(
    '#type' => 'fieldset',
    '#title' => t('Step ' . $delta),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#parents' => array('profilewizard_step_data', $delta),
  );
  $form[$delta]['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Step Label'),
    '#default_value' => $label,
    '#description' => t('Label name of the step'),
    '#parents' => array('profilewizard_step_data', $delta, 'label'),
  );
  $form[$delta]['fields'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Fields to use'),
    '#default_value' => $fields,
    '#options' => $all_fields,
    '#description' => t('Fields to use for the profile wizard'),
    '#parents' => array('profilewizard_step_data', $delta, 'fields'),
  );
  return $form;
}
