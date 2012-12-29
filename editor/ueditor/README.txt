// $Id$

启用 ueditor 步骤：
1、至 http://ueditor.baidu.com/website/ipanel/panel.html，选择完整版；
2、下载 1.2.4(其它版本未测试) UTF-8 版本；
3、解压到本目录即可。

解压后的目录结构如：
  ueditor/README.txt
  ueditor/editor_all.js
  ueditor/editor_all_min.js
  ueditor/dialogs/
  ueditor/themes/
  ueditor/third-party/

百度编辑器使用 swfupload 进行附件上传，但其配置不完善，无法传递 seesion 到服务端
请使用 ueditor/attachment.html 覆盖 ueditor/dialogs/attachment/attachment.html 文件

