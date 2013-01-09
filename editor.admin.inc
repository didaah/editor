<?php
// $Id$

function _editor_admin_tabs() {
  dd_set_tabs(array(
    l(t('editor', '编辑器配置'), 'admin/editor'),
    l(t('editor', '全局设置'), 'admin/editor/settings'),
    l(t('editor', '添加编辑器配置'), 'admin/editor/add'),
  ));
}

/**
 * 编辑器配置列表
 */
function editor_admin_list() {
  _editor_admin_tabs();

  if (!empty($_GET['delete'])) {
    if (db_exec('DELETE FROM {editor} WHERE type = ?', array($_GET['type']))) {
      var_del('editor_instances');
      echo 'reload';
    }
    exit;
  }

  if ($fetch = db_query('SELECT * FROM {editor} ORDER BY created DESC')) {
    $table = array();

    foreach ($fetch as $o) {
      $data = unserialize($o->data);
      $rows = array(t(NULL, $o->name));
      $rows[] = $o->status ? t('editor', '启用') : t('editor', '禁用');
      $rows[] = empty($data['name']) ? t('editor', '全局') :  $data['name'];

      $links = array(l(t('editor', '编辑'), 'admin/editor/edit/' . $o->type));

      if (!empty($o->deleted)) {
        $name = t('editor', '删除');
      } else {
        $name = t('editor', '恢复默认值');
      }
      
      $links[] = l($name, 'admin/editor', array(
        'query' => 'delete=1&type=' . $o->type,
        'attributes' => array('class' => 'confirmajax'
      )));       

      $rows[] = dd_imp($links);

      $table[] = $rows;
    }

    $header = array(t('editor', '配置名称'), t('editor', '状态'), t('editor', '编辑器'), t('editor', '操作'));
    $output = theme('table', $header, $table, array('class' => 'center_table'));
  }

  return $output;
}

/**
 * 添加/更新编辑器配置
 * @param string $type
 *  编辑器实例名称，若指定，则更新该编辑器实例
 */
function editor_admin_form($type = NULL) {
  _editor_admin_tabs();

  if ($type && $editor = db_query('SELECT * FROM {editor} WHERE type = ?', array($type), array('return' => 'one'))) {
    dd_set_tabs(array(l(t('editor', '更新编辑器配置'), $_GET['q'])));
    $editor->data = unserialize($editor->data);
  }

  $form = array(
    'fields' => array(),
    'settings' => array('#validate' => array('editor_admin_form_validate')),
  );

  $form['fields']['name'] = array(
    '#title' => t('editor', '配置名称'), 
    '#type' => 'textfield',
    '#default_value' => $editor->name,
    '#required' => 1
  );
  
  $form['fields']['type'] = array(
    '#default_value' => !empty($editor) ? $editor->type : '',
    '#type' => 'hidden',
    '#constant' => 1
  );  

  if (empty($editor) || !empty($editor->deleted)) {
    // 管理员手动添加的编辑器实例，允许修改 type，并统一前缀为 editor_
    $form['fields']['newtype'] = array(
      '#title' => t('editor', '配置索引'),
      '#default_value' => str_replace('editor_', '', $editor->type),
      '#field_prefix' => 'editor_',
      '#required' => 1,
      '#type' => 'textfield',
      '#description' => t('editor', '唯一标识。只能是字母和数字，不超过 12 个字符')
    );
  } else {
    // 通过 hook_editor_info() 添加的编辑器实例，不允许修改 type
    $form['fields']['_type'] = array(
      '#title' => t('editor', '配置索引'),
      '#default_value' => $editor->type,
      '#type' => 'value',
      '#description' => t('editor', '系统创建，无法修改')
    ); 

    // 通过 hook 添加的编辑器实例
    $form['fields']['hook'] = array(
      '#default_value' => 1,
      '#type' => 'hidden',
      '#constant' => 1
    );  
  }

  $form['fields']['status'] = array(
    '#title' => t('editor', '配置状态'),
    '#type' => 'radio',
    '#required' => 1,
    '#default_value' => $editor->status,
    '#options' => array(t('editor', '禁用'), t('editor', '启用')),
  );

  $form['fields']['data'] = array();

  if (!$data = editor_get_info()) return;

  $options = array(t('editor', '全局设置'));

  foreach ($data as $type => $info) {
    $options[$type] = $info['name'];
  }
  
  $form['fields']['data']['name'] = array(
    '#type' => 'radio',
    '#default_value' => !empty($editor->data) && !empty($editor->data['name']) ? $editor->data['name'] : 0,
    '#options' => $options,
    '#required' => 1,
    '#title' => t('editor', '选择编辑器'),
  );

  $form['fields']['data']['dom'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($editor->data) ? $editor->data['dom'] : '',
    '#required' => 1,
    '#title' => t('editor', '文本域 ID'),
    '#description' => '需要使用编辑器的 textarea 文本域 id',
  );

  $form['fields']['data']['path'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($editor->data) ? $editor->data['path'] : '',
    '#title' => t('editor', '显示页面'),
    '#description' => '允许加载编辑器的页面，留空则在所有页面寻找匹配的 ID。多个页面以 ; 号分隔，如：admin;user/login。// 视作正则模式，如：/^admin*/i',
  );

  $form['fields']['data']['form_id'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($editor->data) ? $editor->data['form_id'] : '',
    '#title' => t('editor', 'Form ID'),
    '#description' => '表单回调函数，若不明白这是什么，请保持现状',
  );

  $form['fields']['data']['form_field_name'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($editor->data) ? $editor->data['form_field_name'] : '',
    '#title' => t('editor', 'Field Name'),
    '#description' => '编辑器文本域字段系统名称，若不明白这是什么，请保持现状',
  );

  $form['fields']['data']['plugins'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($editor->data) ? $editor->data['plugins'] : '',
    '#title' => t('editor', '编辑器插件'),
    '#description' => '需加载的插件，多个以 , 号分隔，如：swfupload,smiley',
  );

  $form['fields']['data']['access'] = array(
    '#fieldset_prefix' => 'desc',
    '#fieldset_legend' => t('editor', '权限设置'),
    '#fieldset_suffix' => 1,
    '#fieldset' => true
  );
  
  $form['fields']['data']['access']['rids'] = array(
    '#title' => t('editor', '允许使用的角色'),
    '#default_value' => !empty($editor->data) ? $editor->data['access']['rids'] : array(),
    '#type' => 'checkbox',
    '#description' => t('editor', '留空则允许所有人'),
    '#options' => user_roles_opt()
  );

  $form['fields']['data']['access']['uids'] = array(
    '#title' => t('editor', '允许使用的用户'),
    '#default_value' => !empty($editor->data) ? $editor->data['access']['uids'] : '',
    '#type' => 'textfield',
    '#description' => t('editor', '请填写用户 uid，多个以逗号分隔。填写此项后，上一项角色权限设置则无效'),
  );

  $form['fields']['data']['swfupload'] = array(
    '#fieldset_prefix' => 'desc',
    '#fieldset_legend' => t('editor', 'swfupload 设置'),
    '#fieldset_suffix' => 1,
    '#fieldset' => true,
    '#description' => '可设置 swfupload 参数，该参数将直接传递给 swfupload。若不明白这是什么，请保持现状',
  );

  if (!empty($editor->data) && !empty($editor->data['swfupload'])) {
    $editor->data['swfupload'] += range(0, 5);
  } else {
    $editor->data['swfupload'] = range(0, 5);
  }

  $i = 0;
  
  foreach ($editor->data['swfupload'] as $id => $value) {
    $form['fields']['data']['swfupload'][$i]['key'] = array(
      '#type' => 'textfield',
      '#title' => t('editor', '参数名'),
      '#prefix' => '<div class="form_item">',
      '#default_value' => is_numeric($id) ? '' : $id,
    );
    $form['fields']['data']['swfupload'][$i]['value'] = array(
      '#type' => 'textfield',
      '#title' => t('editor', '参数值'),
      '#suffix' => '</div>',
      '#default_value' => is_numeric($id) ? '' : $value,
    );
    $i++;
  }

  $form['fields']['data']['files'] = array(
    '#fieldset_prefix' => 'desc',
    '#fieldset_legend' => t('editor', '文件上传设置'),
    '#fieldset_suffix' => 1,
    '#fieldset' => true,
    '#description' => '可设置编辑器的文件上传等功能，若不明白这是什么，请保持现状',
  );

  $fields = array(
    '#disabled_upload' => array(0, t('editor', '禁止从编辑器上传，0：允许，1、禁止')),
    '#disabled_view' => array(0, t('editor', '禁止在编辑器中浏览已上传文件，0：允许，1、禁止')),
    '#upload_image_url' => array('', t('editor', '处理图片上传文件的路径，建议留空')),
    '#upload_file_url' => array('', t('editor', '处理上传文件的路径，建议留空')),
    '#view_image_url' => array('', t('editor', '图片文件库浏览地址，建议留空')),
    '#view_file_url' => array('', t('editor', '文件库浏览地址，建议留空')),
    '#view_args' => array('fields' => array(
      'ext_type' => array('', t('editor', '浏览已有文件库的过滤条件，可留空')),
      'ext_id' => array('', t('editor', '浏览已有文件库的过滤条件，可留空')),
      'uid' => array(1, t('editor', '若为 1 则表示只能浏览自己的上传的文件，-1 则相反')),
    )),
    '#validate' => array('fields' => array(
      'ext_type' => array('', t('editor', '文件关联类别，根据需要设置，如：myupload，不建议留空')),
      'ext_id' => array(0, t('editor', '文件关联 ID，根据需要设置')),
      'ext_id2' => array(0, t('editor', '文件关联 ID，根据需要设置')),
      'savepath' => array('', t('editor', '上传文件保存位置，建议留空')),
      'no_insert' => array(0, t('editor', '是否将文件信息写入数据库，0、写入，1、不写入')),
      'title' => array('', t('editor', '文件标题，建议留空')),
      'filename' => array('', t('editor', '文件路径名称，建议留空')),
      'extension' => array('', t('editor', '文件扩展名，建议留空')),
      'status' => array(1, t('editor', '文件状态，1、正常，0、临时。临时状态下的文件会被清除')),
      'filesize' => array('', t('editor', '允许上传的最大文件，单位 KB')),
    )),
  );

  $i = 0;
  foreach ($fields as $key => $value) {
    if (empty($value['fields'])) {
      $default = !empty($v['_dida_values']->data['files']) ? $v['_dida_values']->data['files'][$key] : $value[0];
      $form['fields']['data']['files'][$i]['key'] = array(
        '#type' => 'textfield',
        '#title' => t('editor', '参数名'),
        '#prefix' => '<div class="form_item">',
        '#default_value' => $key
      );
      $form['fields']['data']['files'][$i]['value'] = array(
        '#type' => 'textfield',
        '#title' => t('editor', '参数值'),
        '#suffix' => '</div>',
        '#default_value' => $default,
        '#description' => $value[1]
      );
    } else {
      foreach ($value['fields'] as $_key => $_value) {
        $default = !empty($v['_dida_values']->data['files']) ? $v['_dida_values']->data['files'][$_key] : $_value[0];
        $form['fields']['data']['files'][$i]['key'] = array(
          '#type' => 'textfield',
          '#title' => t('editor', '参数名'),
          '#prefix' => '<div class="form_item">',
          '#default_value' => $_key
        );
        $form['fields']['data']['files'][$i]['value'] = array(
          '#type' => 'textfield',
          '#title' => t('editor', '参数值'),
          '#suffix' => '</div>',
          '#default_value' => $default,
          '#description' => $_value[1]
        );
        $i++;
      }
    }
    $i++;
  }

  $form['fields']['data']['config'] = array(
    '#fieldset_prefix' => 'desc',
    '#fieldset_legend' => t('editor', '编辑器参数'),
    '#fieldset_suffix' => 1,
    '#fieldset' => true,
    '#description' => '该参数将直接传递给编辑器，通常与 config.js 中的设置相同。若不明白这是什么，请保持现状',
  );

  if (!empty($editor->data) && !empty($editor->data['config'])) {
    $editor->data['config'] += range(0, 5);
  } else {
    $editor->data['config'] = range(0, 5);
  }

  $i = 0;

  foreach ($editor->data['config'] as $id => $value) {
    $form['fields']['data']['config'][$i]['key'] = array(
      '#type' => 'textfield',
      '#title' => t('editor', '参数名'),
      '#prefix' => '<div class="form_item">',
      '#default_value' => is_numeric($id) ? '' : $id,
    );
    $form['fields']['data']['config'][$i]['value'] = array(
      '#type' => 'textfield',
      '#title' => t('editor', '参数值'),
      '#suffix' => '</div>',
      '#default_value' => is_numeric($id) ? '' : $value,
    );
    if ($id === 'toolbar') {
      $form['fields']['data']['config'][$i]['value']['#description'] = t('editor', '编辑器按钮，可选值：full、medium、small、mini，或符合编辑器标准的 js 数组');
    }
    $i++;
  }

  $form['fields']['button'] = array(
    '#type' => 'submit',
  );

  return $form;
}

function editor_admin_form_validate(&$form, &$v) {
  if (!empty($v['_dida_values']->hook) && empty($v['_dida_values']->type)) {
    dd_set_message(t('editor', '非法操作'), 'error');
    return true;
  }

  if (empty($v['_dida_values']->type)) {
    $args = NULL;
    $v['_dida_values']->created = time();
    $v['_dida_values']->deleted = 1;
  } else {
    $args = array('type');
  }

  if (empty($v['_dida_values']->hook)) {
    if (preg_match('/[^a-z0-9]/i', $v['_dida_values']->newtype)) {
      return array('newtype' => array('#error' => array(t('editor', '索引名称不合法，只能使用字母和数字'))));
    }

    $v['_dida_values']->newtype = 'editor_' . $v['_dida_values']->newtype;
    if (!empty($v['_dida_values']->type) && $v['_dida_values']->type != $v['_dida_values']->newtype) {
      if (db_query('SELECT type FROM {editor} WHERE type = ?',
      array($v['_dida_values']->newtype), array('return' => 'column'))) {
        return array('newtype' => array('#error' => array(t('editor', '索引名称已经存在'))));
      }
    }
    $v['_dida_values']->type = $v['_dida_values']->newtype;
  }

  $v['_dida_values']->data['swfupload'] = _editor_admin_form_values($v['_dida_values']->data['swfupload']);
  $v['_dida_values']->data['config'] = _editor_admin_form_values($v['_dida_values']->data['config']);
  $v['_dida_values']->data['files'] = _editor_admin_form_values($v['_dida_values']->data['files']);

  // 合并已有值
  if ($data = db_query('SELECT data FROM {editor} WHERE type = ?',
  array($v['_dida_values']->type), array('return' => 'column'))) {
    $data = unserialize($data);
    $v['_dida_values']->data = _editor_admin_values_merge($data, $v['_dida_values']->data);
  }

  if (db_write_record('editor', $v['_dida_values'], $args)) {
    dd_set_message(t('editor', '操作成功'));
    // 清除缓存
    var_del('editor_instances');
    $form['settings']['#redirect'] = url('admin/editor');
  }
}

// protected _editor_admin_form_values($values) {{{ 
/**
 * _editor_admin_form_values
 * 解析编辑器参数，将表单中提交的参数，解析为与 hook_editor_info() 格式相匹配
 * 
 * @param array $values 
 * @access protected
 * @return array
 */
function _editor_admin_form_values($values) {
  $config = array();
  foreach ($values as $id => $data) {
    if (!empty($data['key'])) {
      $config[$data['key']] = $data['value'];
    } else if (is_array($data) && $array = _editor_admin_form_values($data)) {
      $config[$data['key']] = $array;
    }
  }
  return $config;
}
// }}}

// protected _editor_admin_values_merge($data, $values) {{{ 
/**
 * _editor_admin_values_merge
 * 递归合并两个数组，与 array_merge_recursive 的区别在于，相同键名将覆盖
 * @param array $data 
 *  数组 1
 * @param array $values 
 *  数组 2
 * @access protected
 * @return array
 */
function _editor_admin_values_merge($data, $values) {
  $array = $data;

  foreach ($values as $key => $value) {
    if (empty($key)) continue;
    if (is_array($value)) {
      if (!empty($data[$key])) {
        $array[$key] = _editor_admin_values_merge($data[$key], $value);
      } else {
        $array[$key] = $value;
      }
    } else {
      $array[$key] = $value;
    }
  }

  return $array;
}
// }}}

/**
 * 编辑器全局设置
 */
function editor_admin_settings() {
  _editor_admin_tabs();
  
  $form = array(
    'fields' => array(),
    'settings' => array('#validate' => array('editor_admin_settings_validate')),
  );

  if (!$data = editor_get_info()) return;

  $options = array();

  foreach ($data as $type => $info) {
    $options[$type] = $info['name'];
  }

  $form['fields']['editor_default'] = array(
    '#title' => t('editor', '默认编辑器'),
    '#type' => 'radio',
    '#default_value' => var_get('editor_default', 'markitup'),
    '#options' => $options,
  );

  $form['fields']['button'] = array(
    '#type' => 'submit',
  );

  return $form;  
}

function editor_admin_settings_validate(&$form, $v) {
  var_set('editor_default', $v['_dida_values']->editor_default);
}
