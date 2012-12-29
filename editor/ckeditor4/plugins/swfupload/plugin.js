// $Id$

CKEDITOR.plugins.add('swfupload',{
  init: function(edit) {
    edit.addCommand('swfupload', new CKEDITOR.dialogCommand('swfupload'));
    edit.ui.addButton( 'swfupload',{
      label : Dida.t('editor', '批量上传'),
      command : 'swfupload',
      icon : this.path + 'swfupload.gif'
    });

    CKEDITOR.dialog.add('swfupload', function(edit) {
      var v = edit.config.swfupload_config;
      return {
        title: Dida.t('editor', '批量上传'),
        minWidth: 780,
        minHeight: 350,
        contents: [{
          id: 'iframe',
          label: '',
          expand: true,
          elements: [{
            type: 'html',
            html: '<iframe width="100%" height="100%" src="' + v.url + '" class="cke_dialog_ui_iframe" frameborder="0"></iframe>',
            style: 'width:100%;min-height:350px;padding:0;margin:0;',
          }]
        }],
        buttons: {disabled: true}
      }
    });   
       
  }
});

