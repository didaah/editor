<?php
// $Id$

/**
 * @file 编辑器配置文件。
 */

/**
 * @file 编辑器配置文件。
 */

/**
 * 编辑器接口
 */
Class ueditor extends EDITOR {
 
  /**
   * 编辑器基本信息
   */
  static function info() {
    return array(
      'name' => t('editor', '百度编辑器'), // 必须
      'description' => t('editor', 'UEditor是由百度web前端研发部开发所见即所得富文本web编辑器'),
      'link' => 'http://ueditor.baidu.com/website/'
    );
  }
 
   // 初始化，通常载入公用 js、css 等
  public function init() {
		dd_add_js($this->path . '/editor/ueditor/editor_all_min.js');
		//dd_add_js($this->path . '/editor/ueditor/include.js');
  }
    
  // 加载编辑器
  public function load($data) {

    if (!empty($data['plugins']) && empty($data['config']['extraPlugins'])) {
      $data['config']['extraPlugins'] = $data['plugins'];
    }

    if (empty($data['config']['swfupload'])) {
      $data['config']['swfupload'] = array(
        'params' => array(
          '__SETPHPSESSID' => $GLOBALS['user']->session,
          '__SETUID' => $GLOBALS['user']->uid,
        ),
        'url' => $data['config']['fileUrl'] . '&swfupload=1'
      );
    }
   
    if (empty($data['config']['toolbars']) && !empty($data['config']['toolbar'])) {
      $data['config']['toolbars'] = $data['config']['toolbar'];
      unset($data['config']['toolbar']);
    }
    
    if (empty($data['config']['scrawlUrl'])) {
      $data['config']['scrawlUrl'] = url('editor/custom/ueditor', array('query' => 'name=scrawl&instances=' . $data['instances']));
    }

    $data['config']['instanceName'] = $this->dom;

    $config = array(
      'wordCount' => false, // 不开启字数统计
      'height' => 300,
      'imagePath' => '', // 修正地址不需要
      'imageManagerPath' => '', // 同上
      'filePath' => '',
      'scrawlPath' => '',
      'initialFrameWidth' => '100%', // 默认宽度
      'autoHeightEnabled' => false,
      'scaleEnabled' => true,
      'initialContent' => '',
      'catchRemoteImageEnable' => false, // 禁用远程图片抓取
      'pageBreakTag' => '_dida_page_break_tag_',
      'UEDITOR_HOME_URL' =>  $GLOBALS['base_path'] . $this->path . '/editor/ueditor/',
    );

    $config = array_merge($config, $data['config']);

    $js = 'Dida.module.editor.instances.' . $data['instances'] . ' = UE.getEditor(\'' . $this->dom . '\', ' . dd_to_js($config) . ');';

    dd_add_js($js, 'inline_nojq', 0, 'footer');
  }

  /**
   * 涂鸦数据处理
   */
  static function custom_scrawl($g, $p) {

    if (!user_access('file upload')) {
      dd_get_access();
      exit;
    }

    $validate = array(
      'image' => true,
      'savepath' => 'ueditor/scrawl', // 涂鸦
      'ext_type' => 'ueditor',
      'status' => 0, // 临时保存
    );

    if ( $_GET['action'] == 'tmpImg') { // 背景上传
      if ($file = file_save_upload($_FILES['upfile'], $validate)) {
        echo "<script>parent.ue_callback('" . f($file->filepath) . "','SUCCESS')</script>";
      } else {
        echo "<script>parent.ue_callback('','" . t('editor', '上传失败') . "')</script>";
      }
    } else {
      $validate['status'] = 1; // 涂鸦永久保存
      $validate['filename'] = md5($_POST['content'] . time()) . '.png';
      if ($file = file_save_data(base64_decode($_POST['content']), $validate)) {
        echo "{'url':'" . f($file->filepath) . "',state:'SUCCESS'}";
      } else {
        echo "<script>parent.ue_callback('','" . t('editor', '上传失败') . "')</script>";
      }
    }

    exit;
  }

  // 文件上传处理
  public function upload($filename, $validate) {
    if (!empty($_POST['pictitle'])) $validate['title'] = $_POST['pictitle'];

    if ($file = file_save_upload($_FILES[$filename], $validate)) {
      // 生成缩略图
      if (!empty($_POST['thumb']) && preg_match('/^\d*?x\d*?$/', $_POST['thumb'])) {
        $path = f(image_get_thumb($file->fid, $file->filepath, $_POST['thumb']));
      } else {
        $path = f($file->filepath);
      }
      $state = 'SUCCESS';
      if (!empty($_GET['swfupload'])) $path = url('files/' . $file->fid . '/view');
    } else {
      $path = '';
      $state = t('editor', '上传失败');
    }

    return "{'url':'" . $path . "','title':'" . $file->filename . "','original':'" . $file->filename . "','state':'" . $state . "'}";
  }

  // 文件管理，通常用于浏览已上传的文件，通过编辑器将文件插入编辑框
  static function files($fetch, $type) {
    $item = array();

    $output = '';
    foreach ($fetch as $o) {
      $output .= f($o->filepath) . 'ue_separate_ue';
    }
    
    echo $output;
    exit;
  }

  // 定义编辑器按钮栏，各编辑器必需预设四种工具栏
  static function toolbar() {
     
    $toolbar = array();

    $toolbar['full'] = array(array(
      'fullscreen', 'source', '|', 'undo', 'redo', '|',
      'bold', 'italic', 'underline', 'strikethrough', 'superscript', 'subscript', 'removeformat', 'formatmatch','autotypeset','blockquote', 'pasteplain', '|', 'forecolor', 'backcolor', 'insertorderedlist', 'insertunorderedlist','selectall', 'cleardoc', '|',
      'rowspacingtop', 'rowspacingbottom','lineheight','|',
      'customstyle', 'paragraph', 'fontfamily', 'fontsize', '|',
      'directionalityltr', 'directionalityrtl', 'indent', '|',
      'justifyleft', 'justifycenter', 'justifyright', 'justifyjustify', '|','touppercase','tolowercase','|',
      'link', 'unlink', 'anchor', '|', 'imagenone', 'imageleft', 'imageright','imagecenter', '|',
      'insertimage', 'emotion','scrawl', 'insertvideo','music','attachment', 'map', 'gmap', 'insertframe','highlightcode','webapp','pagebreak','template','background', '|',
      'horizontal', 'date', 'time', 'spechars','snapscreen', 'wordimage', '|',
      'inserttable', 'deletetable', 'insertparagraphbeforetable', 'insertrow', 'deleterow', 'insertcol', 'deletecol', 'mergecells', 'mergeright', 'mergedown', 'splittocells', 'splittorows', 'splittocols', '|',
      'print', 'preview', 'searchreplace','help'
    ));
    
		$toolbar['medium'] = array(array(
      'fullscreen', 'source', '|', 'undo', 'redo', '|',
      'bold', 'italic', 'underline', 'strikethrough', 'superscript', 'subscript', 'removeformat', 'formatmatch','autotypeset','blockquote', 'pasteplain', '|', 'forecolor', 'backcolor', 'insertorderedlist', 'insertunorderedlist','selectall', 'cleardoc', '|',
      'rowspacingtop', 'rowspacingbottom','lineheight','|',
      'customstyle', 'paragraph', 'fontfamily', 'fontsize', '|',
      'directionalityltr', 'directionalityrtl', 'indent', '|',
      'justifyleft', 'justifycenter', 'justifyright', 'justifyjustify', '|', 'link', 'unlink', 'anchor', '|',
      'insertimage', 'emotion','scrawl', 'insertvideo','music','attachment', 'map', 'gmap', 'insertframe','highlightcode', 'pagebreak','background', '|',
      'horizontal', 'date', 'time', 'spechars','snapscreen', 'wordimage', '|',
      'inserttable', 'deletetable', '|',
      'preview', 'searchreplace','help'
		));
		
		$toolbar['small'] = array(array(
      'fullscreen', 'source', '|',
      'bold', 'italic', 'underline', 'strikethrough', 'blockquote', 'pasteplain', '|', 'forecolor', 'backcolor', 'insertorderedlist', 'insertunorderedlist','|',
      'justifyleft', 'justifycenter', 'justifyright', 'justifyjustify', '|', 'link', 'unlink', '|',
      'insertimage', 'emotion', 'attachment','map', 'gmap', 'highlightcode',
    ));
		
    $toolbar['mini'] = array(array(
      'bold', 'italic', 'underline', 'strikethrough', 'blockquote', 
    ));
   
    return $toolbar;
  }

  // 将 editor 模块定义的参数映射到编辑器配置
  static function map() {
    return array(
      // 服务器文件浏览地址
//      'view_file_url' => 'filebrowserBrowseUrl',
      // 服务器图片文件浏览地址
      'view_image_url' => 'imageManagerUrl',
      // 文件上传地址
      'upload_file_url' => 'fileUrl',
      // 文件上传时 name 值
      'upload_file_name' => 'upfile',
      // 图片上传地址
      'upload_image_url' => 'imageUrl',
      // 图片上传时 name 值
      'upload_image_name' => 'upfile',
    );
  }

  static function swfupload($type, $v) {
		dd_add_js(dd_get_path('module', 'editor') . '/editor/ckeditor/include.js');
		$v['swfupload']['custom_settings']['dida_insertCall'] = 'Dida.module.editor.ckeditor.swfuploadInsert';
    return $v;
  }

}


