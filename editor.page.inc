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

  $editor = new $name($config['dom'], $config);  

  echo $editor->upload($_GET['name'], $validate);

  exit;
}

/**
 * 编辑器文件管理
 */
function editor_page_files() {

}

/**
 * swfupload 文件上传管理
 */
function editor_page_swfupload() {
  if (!empty($_POST) && !empty($_GET['name']) && !empty($_FILES[$_GET['name']])) {
    echo "\n"; 

    $validate = !empty($_POST['validate']) ? unserialize(dd_decrypt($_POST['validate'])) : array();

    if (!empty($_POST['title'])) $validate['title'] = $_POST['title'];

    if (!empty($_POST['filebody'])) $validate['filebody'] = $_POST['filebody'];
         
    $file = file_save_upload($_FILES[$_GET['name']], $validate);
  
    if ($file->filepath) {
      echo f($file->filepath);
      exit;
    }
   
    header("HTTP/1.1 404 Internal Server Error");
  }

  exit;
}

