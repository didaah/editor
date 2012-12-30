<?php
// $Id$

/**
 * @file 编辑器配置文件。
 */

/**
 * 编辑器接口
 */
Class markitup extends EDITOR {
  
   // 初始化，通常载入公用 js、css 等
  public function init() {
    dd_add_js($this->path . '/editor/markitup/include.js');
		dd_add_js($this->path . '/editor/markitup/jquery.markitup.js');
    dd_add_js($this->path . '/editor/markitup/sets/dida/set.js');
    dd_add_css($this->path . '/editor/markitup/skins/dida/style.css');
    dd_add_js('misc/jquery.form.js');
    dd_jqui();
  }
    
  // 加载编辑器
  public function load($data) {
 
    $data['config']['root'] = f($this->path) . '/editor/markitup/';

    $data['config']['instanceName'] = $this->dom;
       
    $js = 'Dida.module.editor.instances.' . $data['instances'] . ' = ';
    $js .= '$("#' . $this->dom . '").markItUp(Dida.markItUp.html(' . dd_to_js($data['config']) . '));';

    dd_add_js($js, 'inline', 0, 'footer');
  }

  // 文件上传处理
  public function upload($filename, $validate) {
    if ($file = file_save_upload($_FILES[$filename], $validate)) {
      // 生成缩略图
      if (!empty($_POST['width']) && !empty($_POST['height']) && is_numeric($_POST['width']) && !empty($_POST['height'])) {
        $file->thumb = image_get_thumb($file->fid, $file->filepath, $_POST['width'] . 'x' . $_POST['height']);
      }
      $file->error = '';
      unset($file->tmp_name);
    } else {
      $file = array('error' => t('editor', '上传失败'));
    }
   
    $js = 'window.parent.Dida.markItUp.call.success(';
    $js .= 'window.parent.Dida.module.editor.instances.' . $_GET['instances'] . ',';
    $js .= dd_to_js($file) . ', "' . $_GET['type'] . '");';

    return _dd_js_print($js);
  }

  // 文件管理，通常用于浏览已上传的文件，通过编辑器将文件插入编辑框
  static function files($fetch, $type) {

    $item = array();
    dd_add_js(dd_get_path('module', 'editor') . '/editor/markitup/include.js');

    if ($type == 'image') {
      foreach ($fetch as $o) {
        $thumb = image_get_thumb($o->fid, $o->filepath, '85x85');
        $data = '<span class="files_view_editor_image">' . img($thumb) . '</span>';
        $data .= '<span class="files_view_file_insert">';
        $data .= l(t('editor', '插入小图'), $thumb, array('file' => 1, 'attributes' => array(
          'title' => $o->filename, 'target' => '_blank', 'fid' => $o->fid
        )));
        $data .= l(t('editor', '插入大图'), $o->filepath, array('file' => 1, 'attributes' => array(
          'title' => $o->filename, 'target' => '_blank', 'fid' => $o->fid
        )));
        $data .= '</span>';
        $item[] = $data;
      }
    } else {
      foreach ($fetch as $o) {
        $data = '<span class="files_view_file_insert">';
        $data .= dd_get_strong(t('editor', '上传时间')) . format_date($o->timestamp);
        $data .= dd_get_strong(t('editor', '文件名')) . $o->filename . '(' . round($o->filesize/1024, 2) . 'KB)';
        $data .= l(t('editor', '插入文件'), $o->filepath, array('file' => 1, 'attributes' => array(
          'title' => $o->filename, 'target' => '_blank', 'fid' => $o->fid
        )));
        $data .= '</span>';
        $item[] = $data;
      }
    }
    
    $output = theme('item_list', $item, NULL, 'ul', array('id' => 'files_view_editor_' . $type));
    $output .= pager_view();
   
    return $output;  
  }

  // 定义编辑器按钮栏，各编辑器必需预设四种工具栏
  static function toolbar() {
     
    $toolbar = array();

    $toolbar['full'] = array(
      'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'p', 'strong', 'u', 'em', 'del',
      'ul', 'ol', 'img', 'a', 'blockquote', 'code', 'clean', 'smiley', 'save', 'perview'
    );
    
		$toolbar['small'] = array(
      'strong', 'u', 'em', 'del', 'ul', 'ol', 'img', 'a', 'blockquote', 'save', 'smiley'
		);
		
		$toolbar['medium'] = array(
      'h1', 'h2', 'p', 'strong', 'u', 'em', 'del',
      'ul', 'ol', 'img', 'a', 'blockquote', 'code', 'clean', 'smiley', 'save', 'perview'
		);
		
		$toolbar['mini'] = array(array('string', 'u', 'em', 'del'));
   
    return $toolbar;
  }

  // 将 editor 模块定义的参数映射到编辑器配置
  static function map() {
    return array(
      // 服务器文件浏览地址
      'view_file_url' => 'filebrowserBrowseUrl',
      // 服务器图片文件浏览地址
      'view_image_url' => 'filebrowserImageBrowseUrl',
      // 文件上传地址
      'upload_file_url' => 'filebrowserUploadUrl',
      // 文件上传时 name 值
      'upload_file_name' => 'upload',
      // 图片上传地址
      'upload_image_url' => 'filebrowserImageUploadUrl',
      // 图片上传时 name 值
      'upload_image_name' => 'upload',
    );
  }

  static function swfupload($type, $v) {
		dd_add_js(dd_get_path('module', 'editor') . '/editor/ckeditor/include.js');
		$v['swfupload']['custom_settings']['dida_insertCall'] = 'Dida.module.editor.ckeditor.swfuploadInsert';
    return $v;
  }

}


