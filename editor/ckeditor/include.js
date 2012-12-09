// $Id$

Dida.module.editor.ckeditor = {}

// 文件上传成功后，向编辑器插入文件时，写入一些文件信息
Dida.module.editor.ckeditor.fileInfo = function(edit, file) {
  var element, dialog = edit.getDialog();
  if (dialog.getName() == 'image') {
    element = dialog.getContentElement('info', 'txtAlt');
    if (element) element.setValue(file.filename);
    element = dialog.getContentElement('Link', 'txtUrl');
    if (element) element.setValue(Dida.settings.base_path + 'files/' + file.fid + '/view');
    element = dialog.getContentElement('Link', 'cmbTarget');
    if (element) element.setValue('_blank');
    element = dialog.getContentElement('advanced', 'txtGenTitle');
    if (element) element.setValue(file.filename);
    element = dialog.getContentElement('advanced', 'txtGenClass');
    if (element) element.setValue('dida_editor_upload_image dida_editor_upload_ckeditor_image');
  } else if (dialog.getName() == 'link') {
    element = dialog.getContentElement('advanced', 'advTitle');
    if (element) element.setValue(file.filename);
    element = dialog.getContentElement('advanced', 'advCSSClasses');
    if (element) element.setValue('dida_editor_upload_file dida_editor_upload_ckeditor_file');
  }
  return false;
}

function ck_insert_img(src, title, alt, href) {
	if (instanceName != null) {
	  var a = CKEDITOR.instances[instanceName];
	  var html = '<img src="'+src+'" title="'+title+'" alt="'+alt+'" />';
	  if (href) {
	    html = '<a href="'+href+'" target="_blank">'+html+'</a>';
	  }
	  a.insertHtml(html);
  }
}

Dida.module.editor.ckeditor.insertHtml = function(instancesName, html) {
  /**
   * instancesName 是服务端定义的编辑器实例的系统名称
   * 并非 CKEDITOR.instances[name] 中的 name 值
   */
  Dida.module.editor.instances[instancesName].insertHtml(html);
}

Dida.module.editor.ckeditor.swfuploadInsert = function(obj, fileId) {
  if (window.parent && Dida.swfupload.uploaded && Dida.swfupload.uploaded[fileId]) {
    var params = Dida.parseQuery(Dida.getUrl());
    if (params['instances']) {
      var file = jQuery.parseJSON(Dida.swfupload.uploaded[fileId]);
      
      var html = '<a href="' + Dida.settings.base_path + 'files/' + file.fid + '/view" title="' + file.filename + '" target="_blank">';
      if (Dida.isImage(file.filepath)) {
        html += '<img src="' + Dida.settings.base_path + file.filepath + '" alt="' + file.filename + '" />';
      } else {
        html += file.filename;
      }
      html += '</a>';

      window.parent.Dida.module.editor.ckeditor.insertHtml(params['instances'], html);
      $(obj).remove();
    }
  }
}

$(function() {
  $('.files_view_file_insert a').click(function() {
    if (window.opener) {
      var params = Dida.parseQuery(Dida.getUrl());
      if (params['CKEditorFuncNum']) {
        var file = {fid: $(this).attr('fid'), filename: $(this).attr('title')}
        window.opener.CKEDITOR.tools.callFunction(params['CKEditorFuncNum'], $(this).attr('href'), function() {
          Dida.module.editor.ckeditor.fileInfo(this, file);
        });
        window.close();
      }
    }
    return false;
  });
});
 
Dida.module.editor.ckeditor.uploadInfo = function(iframeId, type) {
  $(iframeId).load(function() {
    $(this).css('height', '180px').contents().find('html').prepend('<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />'); 
    $(this).css('height', '180px').contents().find('body')
      .append('<style>*{font-size:12px;}label{margin-right:10px}.form_item{width:300px}span{color:#9D6243}</style>'); 
    $(this).css('height', '180px').contents().find('label').text(Dida.t('editor', '选择文件')).show(); 
    var html = '';
    if (type == 'image') {
      html += '<div><label>' + Dida.t('editor', '小图尺寸') + '</label><input type="text" size="10" name="thumb" placeholder="';
      html += Dida.t('editor', '200x100') + '"/><span>' + Dida.t('editor', '宽度x高度，如：200x100。不需生成缩略图请留空') + '</span></div>';
    }
    html += '<div><label>' + Dida.t('editor', '文件名称') + '</label><input type="text" class="form_item" name="filename" /></div>';
    html += '<div><label>' + Dida.t('editor', '详细描述') + '</label><textarea name="filebody" class="form_item" rows="5"></textarea></div>';
    $(this).css('height', '180px').contents().find('form').append(html); 
  });
}

if (typeof CKEDITOR == 'object') {
  CKEDITOR.on('dialogDefinition', function(ev)  {
    var dialogName = ev.data.name;
    var dialogDefinition = ev.data.definition;
    if (dialogName == 'image' || dialogName == 'link') {
      var type = dialogName == 'image' ? 'Upload' : 'upload';
      var isUploadShow = false, fileField;
      // 修改 iframe 高度
      dialogDefinition.getContents(type).get('upload').style = 'height:180px';
      // 不显示label
      dialogDefinition.getContents(type).get('upload').label = '';
      dialogDefinition.onLoad = function() {
        var dialog = CKEDITOR.dialog.getCurrent();
        fileField = dialog.getContentElement(type, 'upload');
        Dida.module.editor.ckeditor.uploadInfo('#' + fileField._.frameId, dialogName);
      }
    }
  });
}
