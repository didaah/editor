// $Id$

CKEDITOR.plugins.add('swfupload',{
  requires: ['iframedialog'],
  init: function(edit) {
      edit._.filebrowserFn = CKEDITOR.tools.addFunction( edit.config.swfupload_config.url, edit );
            edit.on( 'destroy', function () { CKEDITOR.tools.removeFunction( this._.filebrowserFn ); } );
    edit.addCommand('swfupload', new CKEDITOR.dialogCommand('swfupload'));
    edit.ui.addButton( 'swfupload',{
      label : Dida.t('editor', '批量上传'),
      command : 'swfupload',
      icon : this.path + 'swfupload.gif'
    });
  }
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
      filebrowser : 'info:url',
      elements: [{
        type: 'iframe',
        src: v.url,
        style: 'width:100%;height:100%;padding:0;margin:0;',
        onContentLoad: function() {
        }
      }]
    }],
    buttons: {disabled: true}
  }
});
