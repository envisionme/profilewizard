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
  $profilewizard_type = arg(3);
  $profilewizard_step_data = variable_get('profilewizard_step_data', array());

  if (empty($profilewizard_type)) {
    $content_profile_types = content_profile_get_types();

    //Error message if no content profile types detected.
    if (sizeof($content_profile_types) <= 0) {
      drupal_set_message('You don\'t currently have any content profiles selected. Please select a content profile type to continue.', 'error');
      return $form;
    }

    // Print links to profile wizard settings pages.
    $links = array();
    foreach ($content_profile_types as $type => $obj) {
      $links[] = l($obj->name, 'admin/settings/profilewizard/' . $type);
    }
    $content = 'Please configure one of the following profile wizard types:';

    $form['markup'] = array(
      '#value' => $content . theme('item_list', $links),
    );
    return $form;
  }
  $form['profilewizard_type'] = array(
    '#type' => 'value',
    '#value' => $profilewizard_type,
  );
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

  $form['profilewizard_step_data'] = array(
    '#type' => 'fieldset',
    '#title' => t('Steps'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  // Render step form elements.
  foreach ($profilewizard_step_data[$profilewizard_type] as $delta => $step) {
    $form['profilewizard_step_data'][$profilewizard_type][$delta] = _profilewizard_form_combo($profilewizard_type, $delta, $profilewizard_step_data[$profilewizard_type][$delta]['label'], $profilewizard_step_data[$profilewizard_type][$delta]['fields'], $fields);
  }

  //  Add another button
  $form['profilewizard_step_data']['add_step'] = array(
    '#type' => 'submit',
    '#value' => t('Add a step'),
    '#submit' => array('profilewizard_settings_add_step'),
  );
  // Use custom submit handler.
  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
    '#submit' => array('profilewizard_settings_form_submit'),
  );

  return $form;
}

/**
 * Submit handler for add another step functionality.
 */
function profilewizard_settings_add_step($form, &$form_state) {
  $type = $form_state['values']['profilewizard_type'];
  $profilewizard_step_data = variable_get('profilewizard_step_data', array());
  if (!empty($profilewizard_step_data[$type]))
    $profilewizard_step_data[$type][] = array('label' => '', 'fields' => array());
  else
    $profilewizard_step_data[$type][1] = array('label' => '', 'fields' => array());
  variable_set('profilewizard_step_data', $profilewizard_step_data);
  drupal_set_message(t('New wizard step added.'));
}

/**
 * Submit handler for settings form.
 */
function profilewizard_settings_form_submit($form, &$form_state) {
  $type = $form_state['values']['profilewizard_type'];
  $profilewizard_step_data = variable_get('profilewizard_step_data', array());
  $profilewizard_step_data_new = $form_state['values']['profilewizard_step_data'];
  foreach ($profilewizard_step_data_new[$type] as $delta => $step) {
    if (empty($step['label'])) {
      unset($profilewizard_step_data_new[$type][$delta]);
    }
  }
  // Reset correct step keys for ease of use.
  $var = array();
  $i = 1;
  foreach ($profilewizard_step_data_new[$form_state['values']['profilewizard_type']] as $step) {
    $var[$i] = $step;
    $i++;
  }
  // Save steps for current type without loosing other type data.
  $profilewizard_step_data[$type] = $var;
  variable_set('profilewizard_step_data', $profilewizard_step_data);
  drupal_set_message(t('Your changes have been saved.'));
}

/**
 * Profilewizard combo boxes for admin settings form.
 */
function _profilewizard_form_combo($type, $delta, $label, $fields, $all_fields) {
  // Print step labels as for better readability.
  if (!empty($label)) {
    $step_label = t('Step @step - @label', array(
      '@step' => $delta,
      '@label' => $label
    ));
  } else {
    $step_label = t('Step @step', array(
      '@step' => $delta
    ));
  }

  // Render step fieldgroup.
  $form = array(
    '#tree' => TRUE,
  );
  $form[$delta] = array(
    '#type' => 'fieldset',
    '#title' => $step_label,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#parents' => array('profilewizard_step_data', $type, $delta),
  );
  $form[$delta]['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Step Label'),
    '#default_value' => $label,
    '#description' => t('Label name of the step'),
    '#parents' => array('profilewizard_step_data', $type, $delta, 'label'),
  );
  $form[$delta]['fields'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Fields to use'),
    '#default_value' => $fields,
    '#options' => $all_fields,
    '#description' => t('Fields to use for the profile wizard'),
    '#parents' => array('profilewizard_step_data', $type, $delta, 'fields'),
  );
  return $form;
}
