<?php
// $Id$

/**
 * 编辑器文件上传处理
 */
function editor_page_upload($name) {
  if (empty($_GET['instances']) || !$config = editor_get_instances($_GET['instances'])) exit;
  
  if (empty($_GET['name']) || empty($_FILES[$_GET['name']])) exit;

  $file = DIDA_ROOT . '/' . dd_get_path('module', 'editor') . '/editor/' . $name . '/include.inc';

  // 指定的编辑器不存在
  if (!is_file($file)) exit;

  require_once $file;
 
  global $user;
  
  $validate = !empty($config['files']) && is_array($config['files']['#validate']) ? $config['files']['#validate'] : array();
  
  if (!empty($_GET['type']) && $_GET['type'] == 'image' && empty($validate['image'])) {
    $validate['image'] = true;
  }

  if (empty($validate['ext_type']) && !empty($_GET['form_token'])) {
    $validate['ext_type'] = $_GET['form_token'];
  }

  if (!empty($_POST['filename']) && empty($validate['title'])) $validate['title'] = $_POST['filename'];

  if (!empty($_POST['filebody']) && empty($validate['filebody'])) $validate['filebody'] = $_POST['filebody'];

  $editor = new $name($config['dom']);  

  echo $editor->upload($_GET['name'], $validate);

  exit;
}

/**
 * 编辑器文件管理
 */
function editor_page_files($name) {

  if (empty($_GET['type'])) exit;
  
  if (empty($_GET['instances']) || !$config = editor_get_instances($_GET['instances'])) exit;

  $file = DIDA_ROOT . '/' . dd_get_path('module', 'editor') . '/editor/' . $name . '/include.inc';

  // 指定的编辑器不存在
  if (!is_file($file)) exit;

  require_once $file;
 
  global $user;

  $args = $where = array();

  $args[':uid'] = $user->uid;
  $where['uid'] = 'uid = :uid';
 
  if (!empty($config['files']) && !empty($config['files']['#view_args'])) {
    if (!empty($config['files']['#view_args']['uid']) && $config['files']['#view_args']['uid'] == -1) {
      unset($args[':uid'], $config['files']['#view_args']['uid'], $where['uid']);
    }

    if (!empty($config['files']['#view_args'])) {
      foreach ($config['files']['#view_args'] as $key => $value) {
        $args[':' . $key] = $value;
        $where[$key] = $key . ' = :' . $value;
      }
    }
  }

  if ($_GET['type'] == 'image') {
    $where['filemime'] = 'filemime LIKE :filemime';
    $args[':filemime'] = 'image/%';
  }

  $query = 'SELECT * FROM {files} ';

  if (!empty($where)) $query .= 'WHERE ' . implode(' AND ', $where);

  $query .= ' ORDER BY fid DESC';
  
  if ($fetch = pager_query($query, 20, 0, NULL, $args)) {
    $output = $name::files($fetch, $_GET['type']);
  } else {
    $output = system_no_content();
  }

  echo theme('print', $output);

  exit;
}

/**
 * swfupload 文件上传
 */
function editor_page_swfupload() {
  if (!empty($_POST) && !empty($_GET['name']) && !empty($_FILES[$_GET['name']])) {
    echo "\n"; 

    $validate = !empty($_POST['validate']) ? unserialize(dd_decrypt($_POST['validate'])) : array();

    if (!empty($_POST['title'])) $validate['title'] = $_POST['title'];

    if (!empty($_POST['filebody'])) $validate['filebody'] = $_POST['filebody'];
         
    $file = file_save_upload($_FILES[$_GET['name']], $validate);
  
    if ($file->filepath) {
      echo json_encode($file);
      exit;
    }
   
    header("HTTP/1.1 404 Internal Server Error");
  }

  exit;
}

/**
 * swfupload 上传 html 界面，可用于 iframe
 * 必须先通过 hook_editor_swfupload_html() 定义，才能够调用该界面
 */
function editor_swfupload_html() {
  if (empty($_GET['name'])) exit;

  $info = module_invoke_all('editor_swfupload_html', 'info');

  if (empty($info[$_GET['name']])) exit;

  $settings = module_invoke_all('editor_swfupload_html', 'settings', $_GET['name'], $_GET);

  // 若没有配置信息，则不允许调用，通常可设置文件上传验证
  if (!empty($settings)) {
    // 只允许图片，GET 中传递的参数，优先级最高
    if (!empty($_GET['isImage'])) $settings['swfupload']['file_types'] = '*.jpg;*.png;*.gif';
    echo theme('print', editor_get_swfupload($settings));
  }

  exit;
}

/**
 * @Implement of hook_editor_swfupload_html()
 */
function editor_editor_swfupload_html($type, $name = NULL, $v = NULL) {
  switch ($type) {
    case 'info':
    return array(
      'editor' => array(
        'name' => t('editor', '编辑器批量上传'),
        'description' => t('editor', '可以 iframe 模式整合在编辑器中，编辑器必须定义 swfupload 接口')
      )
    );
    case 'settings':
      if ($name == 'editor') {
        // 读取编辑器配置
        if (empty($v['instances']) || !$config = editor_get_instances($v['instances'])) return false;
        
        if (empty($config['name'])) $config['name'] = var_get('editor_default', 'ckeditor');

        // 指定了 form_id，但 GET 参数中没有 form_token，视作不合法请求
        if (!empty($config['form_id']) && empty($v['form_token'])) return false;
        
        $validate = !empty($config['files']) && is_array($config['files']['#validate']) ? $config['files']['#validate'] : array();
        
        if (!empty($v['type']) && $v['type'] == 'image' && empty($validate['image'])) {
          $validate['image'] = true;
        }

        if (empty($validate['ext_type']) && !empty($v['form_token'])) {
          $validate['ext_type'] = $v['form_token'];
        }
        
        $settings = array('validate' => $validate, 'swfupload' => (!empty($config['swfupload']) ? $config['swfupload'] : array()));

        $file = DIDA_ROOT . '/' . dd_get_path('module', 'editor') . '/editor/' . $config['name'] . '/include.inc';

        if (!is_file($file)) exit;

        require_once $file;

        return $config['name']::swfupload('settings', $settings);
      }
    break;
  }
}
