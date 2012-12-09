// $Id$

Dida.swfupload = {
  endFile: {}, // 实例中所选取的最后一个文件
  stopUpload: {}, // 实例状态，启动或停止
  loaded: {}, // 实例初始化状态
  uploaded: {} // 已上传成功的文件数据，该数据为服务端所返回
};

(function($) {

	var defaultHandlers = [
    'swfupload_loaded_handler', 'file_queued_handler', 'file_queue_error_handler',
    'file_dialog_start_handler', 'file_dialog_complete_handler', 'upload_start_handler',
    'upload_progress_handler', 'upload_error_handler', 'upload_success_handler',
    'upload_complete_handler', 'queue_complete_handler'
  ];

	var additionalHandlers = [];
	
	$.fn.swfupload = function() {
		var args = $.makeArray(arguments);
		return this.each(function() {
			var swfu;
			if (args.length == 1 && typeof(args[0]) == 'object') {
				swfu = $(this).data('__swfu');
				if (!swfu) {
					var settings = args[0];
					
					var $magicUploadControl = $(this);
					var handlers = [];
					$.merge(handlers, defaultHandlers);
					$.merge(handlers, additionalHandlers);
					$(handlers).each(function(i, v) {
						settings[v] = eval(settings[v]);
					})
					$(this).data('__swfu', new SWFUpload(settings));
				}
			} else if (args.length > 0 && typeof(args[0]) == 'string') {
				var methodName = args.shift();
				swfu = $(this).data('__swfu');
				if (swfu && swfu[methodName]) {
					swfu[methodName].apply(swfu, args);
				}
			}
		});
	};
	
	$.swfupload = {
		additionalHandlers: function() {
			if (arguments.length === 0) {
				return additionalHandlers.slice();
			} else {
				$(arguments).each(function(i, v) {
					$.merge(additionalHandlers, $.makeArray(v));
				});
			}
		},
		defaultHandlers: function() {
			return defaultHandlers.slice();
		},
		getInstance: function(el) {
			return $(el).data('__swfu');
		}
	};
	
	Dida.swfupload.cancelUpload = function(movieName, fileID) {
	  var self = SWFUpload.instances[movieName];
	  self.cancelUpload(fileID, false);
	  var d = '#' + fileID;
	  $(d + ' .dida_swfupload_file_status_queued').text(Dida.t('editor', '已取消'));
	  $(d + ' .dida_swfupload_file_op_queued').html(Dida.swfupload.getDel(d));
	  Dida.swfupload.setCount(self);
	}
	
	// 初始化完成
	Dida.swfupload.swfLoaded = function() {
	  var s = this.customSettings;
	  if (Dida.swfupload.loaded[this.movieName] != 1) {
	  	var html = '<div class="dida_swfupload_files_table fieldset-wrapper"><table><thead><tr>';
	  	if (!s.not_filename) {
	  		html += '<th>' + Dida.t('editor', '文件名称') + '</th>';
	  	}
	  	if (!s.not_filebody) {
	  		html += '<th>' + Dida.t('editor', '文件描述') + '</th>';
	  	}
	  	//if (!s.not_filetype) {
	  	//	html += '<th width="40">' + Dida.t('editor', '类型') + '</th>';
	  	//}
	  	if (!s.not_filesize) {
	  		html += '<th width="60">' + Dida.t('editor', '大小') + '</th>';
	  	}
	  	if (!s.not_status) {
	  		html += '<th width="100">' + Dida.t('editor', '状态') + '</th>';
	  	}
	  	if (!s.not_op) {
	  		html += '<th width="60">' + Dida.t('editor', '操作') + '</th>';
	  	}
	  	html += '</tr></thead><tbody></tbody></table></div>';
	    html += '<div class="dida_swfupload_status">' + Dida.t('editor', '请选择文件') + '</div>';
      html += '<div class="dida_swfupload_loading"></div><div class="dida_swfupload_loading_mark"></div>';
	  	
	  	$(s.fileLists).append(html);

		  var opt = {disabled: true, alt: this.movieName};
		  
      $(s.wrapper + ' .dida_editor_swfupload_start_upload').attr(opt).click(function() {
		  	Dida.swfupload.startUpload($(this).attr('alt'));
		  });

		  $(s.wrapper + ' .dida_editor_swfupload_stop_upload').attr(opt).click(function() {
		  	Dida.swfupload.stopUpload($(this).attr('alt'));
		  });
		}
	  Dida.swfupload.stopUpload[this.movieName] = 0;
	  Dida.swfupload.loaded[this.movieName] = 1;
	}

	// 选择框打开前
	Dida.swfupload.dialogStart = function() {
	}

	// 选择框关闭，成功文件
	Dida.swfupload.fileQueued = function(file) {
		Dida.swfupload.endFile[this.movieName] = file;
	  var s = this.customSettings;
	  var r = 0;
	  $(s.fileLists + ' .dida_swfupload_file_name_queued').each(function() {
	    if (file.name == $(this).text()) {
	      r = 1
	      return false;
	    }
	  });
	  var c = '';
	  if (r != 0) {
	  	c = ' dida_swfupload_file_list_cancel';
	  }
	  if (file.index%2 == 0) {
	  	c += ' even';
	  } else {
	  	c += ' odd';
	  }
	  var html = '<tr class="dida_swfupload_file_list dida_swfupload_file_list_queued' + c + '" id="' + file.id + '">';
	  if (!s.not_filename) {
	  	html += '<td class="dida_swfupload_file_name dida_swfupload_file_name_queued">';
      html += '<p>' + file.name + '</p>';
      html += '<input type="text" name="filename_' + file.id + '" id="filename_' + file.id + '" value="' + file.name.replace(file.type, '') + '" />';
      html += '</td>';
	  }
	  if (!s.not_filebody) {
	  	html += '<td class="dida_swfupload_file_body dida_swfupload_file_name_queued">';
      html += '<textarea name="filebody_' + file.id + '" id="filebody_' + file.id + '"></textarea>';
      html += '</td>';
	  }
	  //if (!s.not_filetype) {
	  //	html += '<td class="dida_swfupload_file_type dida_swfupload_file_type_queued">' + file.type + '</td>';
	  //}
    
    if (!s.not_filesize) {
      html += '<td class="dida_swfupload_file_size dida_swfupload_file_size_queued">' + (file.size ? (file.size/1024/1024).toFixed(2) : 0)+' M</td>';
    }

	  if (file.size) {
	    if (r == 0) {
	      html += '<td class="dida_swfupload_file_status dida_swfupload_file_status_queued">' + Dida.t('editor', '等待上传') + '</td>';
	      html += '<td class="dida_swfupload_file_op dida_swfupload_file_op_queued">';
        html +='<a href="javascript:void(0);" class="dida_swfupload_cancel_upload" alt="';
        html += file.id + '" title="' + Dida.t('editor', '取消');
        html += '" onclick="Dida.swfupload.cancelUpload(\'' + this.movieName + '\', \'' + file.id + '\');">';
        html += Dida.t('editor', '取消') + '</a></td>';
	    } else {
	      html += '<td class="dida_swfupload_file_status_error">' + Dida.t('editor', '已取消');
        html += '(<span class="dida_swfupload_error_msg">' + Dida.t('editor', '已在队列中') + '</span>)</td>';
	      html += '<td class="dida_swfupload_file_op_queued">' + Dida.swfupload.getDel('#' + file.id) + '</td>';
	      this.cancelUpload(file.id, false);
	    }
	  } else {
	    this.cancelUpload(file.id, false);
	    html += '<td class="dida_swfupload_file_status_error">' + Dida.t('editor', '已取消');
      html += '(<span class="dida_swfupload_error_msg">' + Dida.t('editor', '空文件') + '</span>)</td>';
	    html += '<td class="dida_swfupload_file_op_queued">' + Dida.swfupload.getDel('#' + file.id) + '</td>';
	  }

	  html += '</tr>';
	  $(s.fileLists + ' tbody').append(html);
	}

	// 选择框关闭，失败文件
	Dida.swfupload.fileQueueError = function(file, error, message) {
		Dida.swfupload.endFile[this.movieName] = file;
	  if (file !== null) {
	    var s = this.customSettings;
	    var html = '<tr class="dida_swfupload_file_list dida_swfupload_file_list_queued" id="' + file.id + '">';
	    if (!s.not_filename) {
	    	html += '<td class="dida_swfupload_file_name swfupload_file_name_queued">' + file.name + '</td>';
	    }

      if (!s.not_filebody) {
	    	html += '<td class="dida_swfupload_file_body swfupload_file_body_queued"></td>';
	    }

	   // if (!s.not_filetype) {
	   // 	html += '<td class="dida_swfupload_file_type dida_swfupload_file_type_queued">' + file.type + '</td>';
	   // }
	    
	    if (!s.not_filesize) {
		    html += '<td class="dida_swfupload_file_size dida_swfupload_file_size_queued">' + (file.size ? (file.size/1024/1024).toFixed(2) : 0)+' M</td>';
	    }

	    if (!s.not_filestatus) {
	    	html += '<td class="dida_swfupload_file_status dida.swfupload_file_status_queued" colspan="2">';
        html += Dida.t('editor', '加入失败') + '(<span class="dida_swfupload_error_msg">' + Dida.swfupload.error(this, error) + '</span>)</td>';
	    }
	    html += '</tr>';
	    $(s.fileLists + ' tbody').append(html);
	  } else {
	    alert(Dida.swfupload.error(this, error));
	  }
	}

	// 选择框关闭，文件总数
	Dida.swfupload.dialogComplete = function(sum, queued) {
	  if (sum > 0) {
	    Dida.swfupload.setCount(this);
	    if (this.getStats().files_queued > 0) {
	    	$(this.customSettings.wrapper + ' .dida_editor_swfupload_start_upload').attr('disabled', false);
	    }
	  }
	}

	// 上传启动前
	Dida.swfupload.uploadStart = function(file) {
		var self = SWFUpload.instances[this.movieName];
    // 文件标题，手动填写的标题，非文件名称
    self.addFileParam(file.id, 'title', $('#filename_' + file.id).val());
    // 文件详细描述
    self.addFileParam(file.id, 'filebody', $('#filebody_' + file.id).val());
	}

	// 开始上传
	Dida.swfupload.startUpload = function(movieName) {
		var self = SWFUpload.instances[movieName];
		Dida.swfupload.stopUpload[movieName] = 0;
	  self.startUpload();
	  $(self.customSettings.wrapper + ' .dida_swfupload_start_upload').attr('disabled', true);
	  $(self.customSettings.wrapper + ' .dida_swfupload_stop_upload').attr('disabled', false);
	  $(self.customSettings.fileLists + ' .dida_swfupload_loading').show();
	}

	// 停止上传
	Dida.swfupload.stopUpload = function(movieName) {
		var self = SWFUpload.instances[movieName];
	  Dida.swfupload.stopUpload[movieName] = 1;
	  $(self.customSettings.wrapper + ' .dida_swfupload_stop_upload').attr('disabled', true);
	  $(self.customSettings.wrapper + ' .dida_swfupload_start_upload').attr('disabled', false);
	}

	// 上传中
	Dida.swfupload.uploadProgress = function(file, bytes, total) {
		var w = parseInt(bytes/total*100) + '%';
		var html = Dida.t('editor', '正在上传 !name', {'!name':  file.name});
    html += '<span class="dida_swfupload_loading_percentage">' + w +'</span>';
    html += '<span class="dida_swfupload_loading_bytes">' + bytes + '</span>';
    html += '/<span class="dida_swfupload_loading_total">' + total + '</span>';
		$(this.customSettings.fileLists + ' .dida_swfupload_loading').html(html);
		$(this.customSettings.fileLists + ' .dida_swfupload_loading_mark').css('width', w);
		$('#' + file.id).addClass('dida_swfupload_uploading');
	}

	// 上传被终止或失败
	Dida.swfupload.uploadError = function(file, error, message) {
		var d = '#' + file.id;
	  $(d + ' .dida_swfupload_file_status_queued').html('<span class="dida_swfupload_error_msg">'+Dida.swfupload.error(this, message)+'</span>');
	  $(d + ' .dida_swfupload_file_op_queued').html(Dida.swfupload.getDel(d));
	  $(d).removeClass('dida_swfupload_uploading').addClass('dida_swfupload_upload_error');
  	if (this.customSettings.errorCall && this.customSettings.errorCall != 'undefined') {
  		Dida.callFunc(this.customSettings.errorCall, file, error, message);
  	}
	  Dida.swfupload.setCount(this);
	}

	// 上传成功
	Dida.swfupload.uploadSuccess = function(file, data) {
    Dida.swfupload.uploaded[file.id] = data; // 保存服务端返回的数据
		var d = '#' + file.id;
	  $(d + ' .dida_swfupload_file_status_queued').text(Dida.t('editor', '上传成功'));

    var html = Dida.swfupload.getDel(d);

    /**
     * 如果设置了插入回调函数，则生成一个链接，通过该链接 onClick 事件回调函数
     * 传递 file.id 参数，可通过该参数，获取对应的文件数据，例如：
     *  function test(id) { console.log(Dida.swfupload.uploaded[id]); }
     */
  	if (this.customSettings.dida_insertCall) {
		  html += ' | <a href="javascript: void(0);" onClick="' + this.customSettings.dida_insertCall
      html += '(this, \'' + file.id + '\')" class="dida_swfupload_file_insert">' + Dida.t('ediot', '插入') + '</a>';
    }

	  $(d + ' .dida_swfupload_file_op_queued').html(html);

	  $(d).removeClass('dida_swfupload_uploading').addClass('dida_swfupload_upload_ok');
	  Dida.swfupload.setCount(this);

    // 如果设置了回调函数
  	if (this.customSettings.dida_successCall && this.customSettings.dida_successCall != 'undefined') {
  		Dida.callFunc(this.customSettings.dida_successCall, file, data);
  	}
	}

	// 一个周期完成，即单个文件操作完全结束
	Dida.swfupload.uploadComplete = function(file) {
		var html;
	  if (file.id != Dida.swfupload.endFile[this.movieName].id) {
	  	html = Dida.t('editor', '!name 上传成功！', {'!name': file.name});
	  } else {
	  	html = Dida.t('editor', '上传完毕！');
	  	$(this.customSettings.wrapper +' .dida_swfupload_start_upload').attr('disabled', true);
	  	$(this.customSettings.wrapper +' .dida_swfupload_stop_upload').attr('disabled', true);
	  	if (this.customSettings.endCall && this.customSettings.endCall != 'undefined') {
	  		Dida.callFunc(this.customSettings.endCall, this);
	  	}
	  }
	  $(this.customSettings.fileLists + ' .dida_swfupload_loading_mark').css('width', '0px');
	  $(this.customSettings.fileLists + ' .dida_swfupload_loading').html(html);
	  if (Dida.swfupload.stopUpload[this.movieName] == 0) {
	  	this.startUpload();
	  }
	  
	  $('.dida_swfupload_files_table').scrollTop($('.dida_swfupload_files_table').scrollTop()+70)
	}

  // 上传全部完成
	Dida.swfupload.uploadQueueComplete = function(obj) {
	}
	
	// debug
	Dida.swfupload.debug = function(message) {
	}

  // 错误信息解析
	Dida.swfupload.error = function(self, code) {
	  var m = '';
	  switch (code) {
	    case SWFUpload.QUEUE_ERROR.QUEUE_LIMIT_EXCEEDED:
	      m = Dida.t('editor', '文件过多，最多允许 !count 个', {'!count': self.settings.file_upload_limit});
	    break;
	    case '405': case SWFUpload.QUEUE_ERROR.FILE_EXCEEDS_SIZE_LIMIT:
	      m = Dida.t('editor', '最大 !size', {'!size': self.settings.file_size_limit});
	    break;
	    case '404':
	    	m = Dida.t('editor', '类型不符，允许 !type', {'!type': self.settings.file_types});
	    break;
	    case '406':
	    	m = Dida.t('editor', '图片尺寸过小');
	    break;
	    case '403':
	    	m = Dida.t('editor', '没有权限');
	    break;
	    default: 
	      m = Dida.t('editor', '上传错误');
	  }
	  return m;
	}

  // 数字包裹
	Dida.swfupload.wrapperNum = function(str) {
	  return '<span class="dida_swfupload_num">' + str +'</span>';
	}

  // 上传统计
	Dida.swfupload.setCount = function(self) {
	  var s = self.getStats();
	  var stats = [Dida.t('editor', '等待上传(!count)', {'!count': Dida.swfupload.wrapperNum(s.files_queued)})];
	  
    if (s.queue_errors) {
	    stats[stats.length] = Dida.t('editor', '加入失败(!count)', {'!count': Dida.swfupload.wrapperNum(s.queue_errors)});
	  }

	  if (s.upload_cancelled) {
	    stats[stats.length] = Dida.t('editor', '已取消(!count)', {'!count': Dida.swfupload.wrapperNum(s.upload_cancelled)});
	  }
    
    if (s.successful_uploads) {
	    stats[stats.length] = Dida.t('editor', '上传成功(!count)', {'!count': Dida.swfupload.wrapperNum(s.successful_uploads)});
	  }
	  
    if (s.upload_errors) {
	    stats[stats.length] = Dida.t('editor', '上传失败(!count)', {'!count': Dida.swfupload.wrapperNum(s.upload_errors)});
	  }

	  $(self.customSettings.fileLists + ' .dida_swfupload_status').html(stats.join(' | '));
	}

  // 获取隐藏链接
	Dida.swfupload.getDel = function(c) {
		return '<a href="javascript: void(0);" onclick="Dida.swfupload.deleteTr(\'' + c + '\');">' + Dida.t('ediot', '隐藏') + '</a>';
	}

  // 隐藏一行记录
	Dida.swfupload.deleteTr = function(c) {
		$(c).hide();
	}

})(jQuery);
