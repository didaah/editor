// $Id$

$(function() {
  $('#files_view_editor_image .files_view_file_insert a').click(function() {
    if (window.parent) {
      var path = Dida.settings.base_path + 'files/' + $(this).attr('fid') + '/view';
      window.parent.Dida.markItUp.tools.filebrowserInsert($(this).attr('href'), $(this).attr('title'), $(this).attr('title'), path);
      window.parent.Dida.dialog_close();
    }
    return false;
  });
  $('#files_view_editor_file .files_view_file_insert a').click(function() {
    if (window.parent) {
      var path = Dida.settings.base_path + 'files/' + $(this).attr('fid') + '/view';
      window.parent.Dida.markItUp.tools.filebrowserInsert(path, $(this).attr('title'), $(this).attr('title'), path);
      window.parent.Dida.dialog_close();
    }
    return false;
  }); 
});
 
