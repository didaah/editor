/*********************************************************************************************************/
/**
 * insertcode plugin for CKEditor 3.x
 */
/*********************************************************************************************************/

CKEDITOR.plugins.add('insertcode',
{
	requires:['dialog'],lang:['zh-cn'],init:function (a)
	{
		var b="insertcode";
		var c=a.addCommand(b,new CKEDITOR.dialogCommand(b));
		c.modes=
		{
			wysiwyg:1,source:0
		};
		c.canUndo=false;
		a.ui.addButton("insertcode",
		{
			label:a.lang.insertcode.title,command:b,icon:this.path+"images/insertcode.gif"
		});
		CKEDITOR.dialog.add(b,this.path+"dialogs/insertcode.js")
	}
});