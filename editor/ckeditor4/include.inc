<?php
// $Id$

/**
 * @file 编辑器配置文件。
 */

/**
 * 编辑器接口
 */
Class ckeditor4 extends EDITOR {

  // 定义编辑器基本信息
  static function info() {
    return array(
      'name' => t('editor', 'CKEditor4'),
      'description' => 'ckeditor 4.x',
    );
  }
   
   // 初始化，通常载入公用 js、css 等
  public function init() {
		dd_add_js($this->path . '/editor/ckeditor4/ckeditor.js');
		dd_add_js($this->path . '/editor/ckeditor4/include.js');
		dd_add_js('function updateTextArea(dom){ CKEDITOR.instances[dom].updateElement(); };', 'inline_nojq', 0, 'footer');
  }
    
  // 加载编辑器
  public function load($data) {

    if (!empty($data['plugins']) && empty($data['config']['extraPlugins'])) {
      $data['config']['extraPlugins'] = $data['plugins'];
    }

    if (strpos($data['config']['extraPlugins'], 'swfupload') !== false) {
      $data['config']['swfupload_config'] = array(
        'url' => url('editor/swfupload/html', array(
          'query' => 'name=editor&instances=' . $data['instances'] . '&form_token=' . $data['form_token']
        )),
      );
    }
   
    $data['config']['instanceName'] = $this->dom;
    
    if (empty($data['config']['hieght'])) $data['config']['height'] = 300;

    $js = 'CKEDITOR.replace(\'' . $this->dom . '\', ' . dd_to_js($data['config']) . ');';
    $js .= 'CKEDITOR.instances["' . $this->dom . '"].on("instanceReady", function(){';
    $js .= 'this.document.on("keyup", function(){updateTextArea(\'' . $this->dom . '\');});';
    $js .= 'this.document.on("paste", function(){updateTextArea(\'' . $this->dom . '\');});});';
    $js .= 'Dida.module.editor.instances.' . $data['instances'] . ' = CKEDITOR.instances["' . $this->dom . '"];';

    dd_add_js($js, 'inline_nojq', 0, 'footer');
  }

  // 文件上传处理
  public function upload($filename, $validate) {
    if ($file = file_save_upload($_FILES[$filename], $validate)) {
      // 生成缩略图
      if (!empty($_POST['thumb']) && preg_match('/^\d*?x\d*?$/', $_POST['thumb'])) {
        $path = f(image_get_thumb($file->fid, $file->filepath, $_POST['thumb']));
      } else {
        $path = f($file->filepath);
      }
      $data = 'function() {';
      $data .= 'window.parent.Dida.module.editor.ckeditor.fileInfo(this, ' . dd_to_js($file) . ');';
      $data .= '}';
    } else {
      $path = '';
      $data = 'alert("' . t('editor', '上传失败') . '");';
    }
    return _dd_js_print('window.parent.CKEDITOR.tools.callFunction(' . $_GET['CKEditorFuncNum'] . ', "' . $path . '",' . $data . ')');
  }

  // 文件管理，通常用于浏览已上传的文件，通过编辑器将文件插入编辑框
  static function files($fetch, $type) {

    $item = array();
    dd_add_js(dd_get_path('module', 'editor') . '/editor/ckeditor/include.js');

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
      array(
        'Source','-','AutoSave','Maximize','Preview', 'Print', '-',
        'Templates','Cut','Copy','Paste','PasteText', 'PasteFromWord','-',
        'Undo','Redo','-', 'SelectAll','RemoveFormat',
        'Bold','Italic','Underline','Strike','-',
      ),
      '/',
      array(
        'Styles','Format','Font','FontSize','TextColor',
        'BGColor','ShowBlocks','-', 'Find','Replace',// 'insertcode' // @TODO
      ),
      '/',
      array(
        'JustifyBlock','Link','Unlink','Anchor','Image','swfupload','Flash','Table', 'HorizontalRule',
        'Smiley','SpecialChar','PageBreak', '-',
        'Form','Checkbox','Radio','TextField',
        'Textarea','Select','Button','ImageButton','HiddenField',
        '-', 'about'
      )
    );
    
		$toolbar['small'] = array(array(
  		'Source', 'Maximize', '-', 'Strike', 'Underline', 'Bold',
  		'Italic','TextColor','BGColor', '-', 'NumberedList', 'BulletedList', '-',
  		'Link', 'Unlink', '-', 'insertcode', 'Smiley'
		));
		
		$toolbar['medium'] = array(
			array(
  			'Source', 'Maximize', 'AutoSave', '-', 'Strike',
        'Underline', 'Bold', 'Italic', 'TextColor','BGColor', '-',
        'Cut', 'Copy', 'Paste', 'PasteText','PasteFromWord', 'Undo', 'Redo', '-',
  			'NumberedList', 'BulletedList', 'BodyBreak'
			),
			'/',
			array(
  			'Format','Font','FontSize', '-', 'Link', 'Unlink',
  			'Image','Table','HorizontalRule', '-', 'RemoveFormat', 'SelectAll', '-',
  			'Blockquote', 'swfupload', 'insertcode', 'Smiley'
			)
		);
		
		$toolbar['mini'] = array(array('Strike', 'Underline', 'Bold'));
   
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


