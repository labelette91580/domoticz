define(['app'], function (app) {

    app.component('esp3Hardware', {
		bindings: {
			hardware: '<'
		},
		templateUrl: 'app/hardware/setup/EnOcean.html',
		controller: EnOceanHardwareController
	});

	function EnOceanHardwareController($scope, $interval, $http, dataTableDefaultSettings) {
		var $ctrl = this;
		
		const confirmAction          = ['Delete', 'DeleteEntrys'];
		const refreshLinkTableAction = ['GetLinkTable', 'Link','DeleteEntrys','GetLinkConfig'];
		const refreshNodeTableAction = ['Delete', 'GetProductId', 'LearnIn', 'ClearTeachInStatus'];
		const broadcastCmd           = ['SendCode', 'GetProductId', 'Lock', 'UnLock'];

		getNodeName = function (node) {
			return node ? node.nodeName : '';
		};
		getGroupName = function (group) {
			return group ? group.groupName : '';
		};
		$ctrl.$onInit = function () {
			$.esp3hwdid = $ctrl.hardware.idx;

			EditEnOcean($ctrl.hardware.idx, $ctrl.hardware.Name, 1, 2, 3, 4, 5, 6);
		};
		EnOceanVldSend = function () {
		    var oTable = $('#nodestable').dataTable();
		    var anSelected = fnGetSelected(oTable);
		    var data = oTable.fnGetData(anSelected[0]);
		    EnOceanDeviceSendDialogOpen($.hwid, data[5], data[0], data[7]);
		}
		function ShowWaiting(txt, iserror) {
				$("#notification").html('<p>' + txt + '</p>');

				if (typeof iserror != 'undefined') {
					$("#notification").css("background-color", "red");
				} else {
					$("#notification").css("background-color", "#204060");
				}
				$("#notification").center();
				$("#notification").show();
	}
		function HideWaiting() {
		$("#notification").hide();
	}
		EnOceanSendCmd = function (cmd, payload) {
			$.ajax({
				beforeSend: function () {
					ShowWaiting($.t('Waiting response ' + cmd), true);
				},
				//		        type: 'POST',
				url: "json.htm?type=enocean&hwid=" + $.hwid + "&cmd=" + cmd,
				data: payload,
				async: true,
				dataType: 'json',
				success: function (data) {
					HideNotify();
					if (data.status == "ERR") {
						ShowNotify($.t('Error function ' + cmd + ' : ' + data.message), 2500, true);
						refreshStatus($.deviceIdSelected[0], "failed");
					}
					else
						if (data.message != "Undefined") {
							bootbox.confirm(data.message, function (result) { });
							refreshStatus($.deviceIdSelected[0], "ok");
						}
						else if (data.status == "OK") {
							ShowNotify($.t('Function ' + cmd + ' =  OK '), 2500, true);
							//bootbox.alert($.t('Function '+cmd + ' =  OK '  ));
							refreshStatus($.deviceIdSelected[0], "ok");
						}
					if (refreshLinkTableAction.includes(cmd))
						refreshLinkTable($.deviceIdSelected[0]);

					if (refreshNodeTableAction.includes(cmd))
						RefreshOpenEnOceanNodeTable();

					if (typeof data.result != 'undefined') {

					}
				},
				error: function (response) {
					HideNotify();
					ShowNotify($.t('Error waiting function ' + cmd), 2500, true);
				}
			});
		}
		EnOceanConfirmAndSendCmd = function(cmd,payload)
		{
			if ( confirmAction.includes(cmd) )
			{
				bootbox.confirm("Do you want to " + cmd ,	function (result) {
					if (result==true)
						EnOceanSendCmd (cmd,payload) ;
				});
			
			}
			else
				EnOceanSendCmd  (cmd,payload) ;

		}
		refreshStatus = function(deviceId , status ){
				    
			$('#statusImg'+deviceId).attr("src", "images/"+status+".png" );
		
		}
		EnOceanCmd = function (cmd, MinIdSelected, MaxIdSelected , MinEntrySelected,MaxEntrySelected) {

		    //		    $.ajax({ type: 'POST', url: url + "api/" + topic, data: payload, async: true });
		    var oTable = $('#nodestable').dataTable();
		    var anSelected = fnGetSelected(oTable);

		    var oLinkTable = $('#inboundlinktable').dataTable();
		    var linkSelected = fnGetSelected(oLinkTable);

			var payload = {};
			//broadcast default if no selected
			if (broadcastCmd.includes(cmd))
				payload[0] = "FFFFFFFF;FFFFFFFF" ;
//		    for (i = 0; i < anSelected.length ; i++) {
//		        var data = oTable.fnGetData(anSelected[i]);
//		        payload[i] = data[0];
//		    }

			$.deviceIdSelected = {} ;
		    
		    var totalselected = $('#nodestable input:checkbox:checked').length;

		    if (totalselected == 0) {
				if (MinIdSelected>0)  {
					if(anSelected.length > 0 ){
						var data = oTable.fnGetData(anSelected[0]);
						payload[0] = data[0] + ";" + data[1];
						$.deviceIdSelected[0] = data[0];
						totalselected=1;
					}
				}
		    }
		    if (totalselected < MinIdSelected)  {
		        bootbox.alert($.t('No Devices selected !'));
		        return;
		    }
		    if (totalselected > MaxIdSelected)  {
		        bootbox.alert($.t('Maximum ' + MaxIdSelected + ' Devices selected !'));
		        return;
		    }

            //number entry selected
		    totalLinkSelected = $('#inboundlinktable input:checkbox:checked').length;

		    if ( (totalLinkSelected < MinEntrySelected) && (linkSelected.length <MinEntrySelected) ) {
		        bootbox.alert($.t('No Entry selected !'));
		        return;
		    }
		    if (totalLinkSelected > MaxEntrySelected) {
		        bootbox.alert($.t('Maximum ' + MaxEntrySelected + ' ; selected !'));
		        return;
		    }


		    var cnt = 0;
		    $('#nodestable input:checkbox:checked').each(function () {
		        lineNo = $(this).val();
		        var data = oTable.fnGetData(lineNo);
		        payload[cnt] = data[0] + ";" + data[1];
		        $.deviceIdSelected[cnt] = data[0];
		        cnt++;
		    });



		    var selectedEntry ="" ;
		    $('#inboundlinktable input:checkbox:checked').each(function () {
		        lineNo = $(this).val();
		        var entry = oLinkTable.fnGetData(lineNo);
		        if (selectedEntry == "")
		            selectedEntry = entry["entry"] + ';';
		        selectedEntry = selectedEntry + entry[0] + ';';
		    });

			//si pas de checkbox sur link
			if (totalLinkSelected==0 )  {
				if(linkSelected.length > 0 ){
					var entry = oLinkTable.fnGetData(linkSelected[0]);
					if (selectedEntry == "")
						selectedEntry = entry["entry"] + ';';
					selectedEntry = selectedEntry + entry[0] + ';';
				}
			}

		    payload["entry"] = selectedEntry;
			EnOceanConfirmAndSendCmd (cmd,payload);

		}
		dislayLinkTable = function (DeviceID, result) {
						var oTableLink = $('#inboundlinktable').dataTable();
						oTableLink.fnClearTable();
		                $.each(result, function (i, item) {
		                    var itemChecker = '<input type="checkbox" class="noscheck" name="Check-' + i + ' id="Check-' + i + '" value="' + i + '" />';



		                    var n = "" + i; if (i <= 9) n = "0" + n;
		                    var healButton = '<img src="images/delete.png" onclick="DeviceCmd(\'DeleteEntrys\',\''+ DeviceID + '\',1,' + n + '  )" class="lcursor" title="' + $.t("Delete Link Entry") + '" />';
		                    var addId = oTableLink.fnAddData({
		                        //		                        "Name": item.Name,
		                        "entry": DeviceID,
		                        "0": n,
		                        "1": item.Profile,
		                        "2": item.Name,
		                        "3": item.SenderId,
		                        "4": item.Config,
		                        "5": item.Channel,
		                        //		                                    "4": statusImg + '&nbsp;&nbsp;' + healButton,
								// "5": itemChecker,
		                        "6": healButton,

		                    });
						});
						$("#inboundlinktable tbody").off();
						$("#inboundlinktable tbody").on('click', 'tr', function () {
							if ($(this).hasClass('row_selected')) {
								$(this).removeClass('row_selected');
							}
							else {
								var oTable = $('#inboundlinktable').dataTable();
								oTable.$('tr.row_selected').removeClass('row_selected');
								$(this).addClass('row_selected');

							}
						});


		}
		refreshLinkTable = function (DeviceID) {
		    var statusImg = '<img src="images/' + status + '.png" />';
            var payload = {};
            payload[0] = DeviceID + ";1" ;

		    $.ajax({
		        url: "json.htm?type=enocean&hwid=" + $.hwid + "&cmd=GetLinkTableList" ,
                data: payload,
                async: false,
		        dataType: 'json',
		        success: function (data) {
		            if (typeof data.result != 'undefined') {
						$.linkTable[DeviceID]=data.result;
						dislayLinkTable (DeviceID, data.result);
		            }
		        }
		    });


		}
		DeviceCmd = function( cmd,DeviceID ,UnitId , linkEntry )
		{
		    var payload = {};
	        payload[0] = DeviceID + ";" + UnitId ;
			var selectedEntry = "" ;
			if(linkEntry!="Undefined")
				selectedEntry = DeviceID + ";" + linkEntry ;
			payload["entry"] = selectedEntry;
			$.deviceIdSelected = {} ;
			$.deviceIdSelected[0] = DeviceID ;
			EnOceanConfirmAndSendCmd(cmd,payload);
			
		}
		RefreshOpenEnOceanNodeTable = function () {
		    $('#modal').show();

		    $('#updelclr #nodeupdate').attr("class", "btnstyle3-dis");
		    $('#updelclr #nodedelete').attr("class", "btnstyle3-dis");
		    //		    $("#hardwarecontent #configuration").html("");
		    $("#hardwarecontent #nodeparamstable #nodename").val("");

		    var oTable = $('#nodestable').dataTable();
		    oTable.fnClearTable();

		    $.ajax({
		        url: "json.htm?type=enocean&hwid=" + $.hwid + "&cmd=GetNodeList",
		        async: false,
		        dataType: 'json',
		        success: function (data) {
		            if (typeof data.result != 'undefined') {

		                $("#ownNodeId").val(String(data.ownNodeId));

		                $.each(data.result, function (i, item) {
		                    var status = "ok";
		                    var statusImg = '<img id="statusImg'+item.DeviceID+'" src="images/' + status + '.png" />';
		                    var healButton = '<img src="images/delete.png" onclick="DeviceCmd(\'Delete\',  \''+ item.DeviceID + '\',\'' +  item.Unit +  '\')" class="lcursor" title="' + $.t("Delete node") + '" />';
		                    //var itemChecker = '<input type="checkbox" class="noscheck" name="Check-' + item.DeviceID + ' id="Check-' + item.DeviceID + '" value="' + item.DeviceID + '" />';
		                    var itemChecker = '<input type="checkbox" class="noscheck" name="Check-' + item.DeviceID + ' id="Check-' + item.DeviceID + '" value="' + i + '" />';

		                    //		                    var Description = item.Description;
		                    //		                    var nodeStr = addLeadingZeros(item.NodeID, 3) + " (0x" + addLeadingZeros(item.NodeID.toString(16), 2) + ")";
		                    var addId = oTable.fnAddData({
		                        //		                        "Name": item.Name,
		                        //"State": item.State,
		                        //"NodeID"  : item.ID,
		                        "DeviceID": item.DeviceID,
		                        "0": item.DeviceID,
		                        "1": item.Unit,
		                        "2": item.Name,
		                        "3": item.Description,
		                        "4": item.Manufacturer_name,
		                        "5": item.Profile,
		                        "6": item.TypeName,
		                        "7": item.TeachInStatus,
		                        "8": item.EnoTypeName,
		                        //		                        "8": statusImg + '&nbsp;&nbsp;' + healButton,
		                        "9": statusImg  + '&nbsp;&nbsp;' + healButton + '&nbsp;&nbsp;' + itemChecker,
		                    });
		                });

		            }
		        }
		    });

		    /* Add a click handler to the rows - this could be used as a callback */
		    $("#nodestable tbody").off();
		    $("#nodestable tbody").on('click', 'tr', function () {
		        $('#updelclr #nodedelete').attr("class", "btnstyle3-dis");
		        if ($(this).hasClass('row_selected')) {
		            //$(this).removeClass('row_selected');

		            /*		            $('#updelclr #nodeupdate').attr("class", "btnstyle3-dis");
                                        $("#hardwarecontent #configuration").html("");
                                        $("#hardwarecontent #nodeparamstable #nodename").val("");
                                        $('#hardwarecontent #usercodegrp').hide();
                    */
		        }
		        else {
		            var oTable = $('#nodestable').dataTable();
		            oTable.$('tr.row_selected').removeClass('row_selected');
		            $(this).addClass('row_selected');

		        }
				
		        {
		            var iOwnNodeId = parseInt($("#ownNodeId").val());
		            var oTable = $('#nodestable').dataTable();
		            //		            oTable.$('tr.row_selected').removeClass('row_selected');

		            $('#updelclr #nodeupdate').attr("class", "btnstyle3");
		            //		            var anSelected = fnGetSelected(oTable);
		            //		            if (anSelected.length !== 0) 
		            {
		                //		                var data = oTable.fnGetData(anSelected[0]);
		                var data = oTable.fnGetData($(this));
		                var DeviceID = data["DeviceID"];

						dislayLinkTable (DeviceID,  $.linkTable[DeviceID]);

		                //		                var szConfig = "";
		                //		                $("#hardwarecontent #configuration").html(szConfig);
		                //$("#hardwarecontent #configuration").i18n();
		                //refreshLinkTable(DeviceID);

		            }
		        }
		    });

		    $('#modal').hide();
		}
		EditEnOcean = function (idx, name, Mode1, Mode2, Mode3, Mode4, Mode5, Mode6) {
		    $.hwid = idx;
		    $.devName = name;
			$.deviceIdSelected = {} ;
			$.linkTable={}

		    cursordefault();
		    var htmlcontent = '';
		    //htmlcontent = '<p><center><h2><span data-i18n="Device"></span>: ' + name + '</h2></center></p>\n';
		    htmlcontent += $('#enocean').html();
		    //$('#hardwarecontent').html(GetBackbuttonHTMLTable('ShowHardware') + htmlcontent);
		    $('#hardwarecontent').html( htmlcontent);
		    $('#hardwarecontent').i18n();

		    var oTable = $('#nodestable').dataTable({
		        "sDom": '<"H"lfrC>t<"F"ip>',
		        "oTableTools": {
		            "sRowSelect": "single",
		        },
		        "aaSorting": [[0, "desc"]],
		        "bSortClasses": false,
		        "bProcessing": true,
		        "bStateSave": true,
		        "bJQueryUI": true,
		        "aLengthMenu": [[25, 50, 100, -1], [25, 50, 100, "All"]],
		        "iDisplayLength": 25,
		        "sPaginationType": "full_numbers",
		        language: $.DataTableLanguage
		    });

		    $('#hardwarecontent #idx').val(idx);

			//get code 
			$.ajax({
				url: "json.htm?type=enocean&hwid=" + $.hwid + "&cmd=" + "GetCode",
				async: false,
				dataType: 'json',
				success: function (data) {
					if (data.status == "KO")
						EnOceanSetCode();
				},
				error: function (response) {
				}
			});


		    RefreshOpenEnOceanNodeTable();
		}
		EnOceanSetCode = function () {
		    bootbox.dialog({
		        message: $.t("Please enter the code protection:") + "<input type='text' id='add_node' data-toggle='tooltip' title='NodeId or NodeId.Instance'></input><br>",
		        title: 'Set Code  ' ,
		        buttons: {
		            main: {
		                label: $.t("Save"),
		                className: "btn-primary",
		                callback: function () {
		                    var addnode = $("#add_node").val();
		                    $http({
		                        url: "json.htm?type=enocean&hwid=" + $.hwid + "&cmd=SetCode" + "&code=" + addnode ,
		                        async: true,
		                        dataType: 'json'
		                    }).then(function successCallback(response) {
		                        //RefreshGroupTable();
		                    }, function errorCallback(response) {
		                    });
		                }
		            },
		            nothanks: {
		                label: $.t("Cancel"),
		                className: "btn-cancel",
		                callback: function () {
		                }
		            }
		        }
		    });
		};
		EnOceanCreateSensor = function () {
		    bootbox.dialog({
		        message: $.t("Please enter sensor ID in hexa 0x12345678 / EEP: D20101 :<br>") 
				+ "<input type='text' id='add_node' data-toggle='tooltip' title='NodeId or NodeId.Instance'></input> : deviceId<br>"
				+ "<input type='text' id='eep' data-toggle='tooltip' title='EEP'></input> : EEP <br>",
		        title: 'Create Sensor  ' ,
		        buttons: {
		            main: {
		                label: $.t("Create"),
		                className: "btn-primary",
		                callback: function () {
		                    var addnode = $("#add_node").val();
		                    var eepnode = $("#eep").val();
		                    $http({
		                        url: "json.htm?type=enocean&hwid=" + $.hwid + "&cmd=CreateSensor" + "&id=" + addnode + "&eep=" + eepnode,
		                        async: true,
		                        dataType: 'json'
		                    }).then(function successCallback(response) {
		                        //RefreshGroupTable();
								
								if (response.data.status == "OK"){
									ShowNotify($.t('Sensor '+addnode + ' created '  ), 2500, true);
									RefreshOpenEnOceanNodeTable();
								}
								else
									ShowNotify($.t('Error creating sensor :'+response.data.message), 2500, true);

		                    }, function errorCallback(response) {
							 var addnode = $("#add_node").val();
		                    });
		                }
		            },
		            nothanks: {
		                label: $.t("Cancel"),
		                className: "btn-cancel",
		                callback: function () {
		                }
		            }
		        }
		    });
		};
		EnableLearnMode = function () {
			if (typeof $scope.mytimer !== "undefined") {
				$interval.cancel($scope.mytimer);
				$scope.mytimer = undefined;
			}
			$.ajax({
				url: "json.htm?type=command&param=esp3enablelearnmode&hwdid=" + $.esp3hwdid + "&minutes=1",
				async: true,
				dataType: "json",
				success: function (data, status) {
					$("#esp3lmdwaiting").show();
					$("#esp3lmdteachedin").hide();
					$("#esp3lmdtimedout").hide();
					$("#esp3learnmodedialog").modal("show");

					$scope.mytimer = $interval(function () { $scope.IsNodeTeachedIn(); }, 1000);
				},
				error: function (result, status, error) {
					ShowNotify($.t("Problem enabling learn mode!"), 2500, true);
				},
			});
		};

		$scope.IsNodeTeachedIn = function () {
			if (typeof $scope.mytimer !== "undefined") {
				$interval.cancel($scope.mytimer);
				$scope.mytimer = undefined;
			}
			$.ajax({
				url: "json.htm?type=command&param=esp3isnodeteachedin&hwdid=" + $.esp3hwdid,
				async: true,
				dataType: "json",
				success: function (data, status) {
					if (data.result === 1) { // An EnOcean node has been teached-in
						$scope.esp3_nodeid = addLeadingZeros(parseInt(data.nodeid).toString(16).toUpperCase(), 8);
						$scope.esp3_manufacturername = data.manufacturername;
						$scope.esp3_eep = data.eep;
						$scope.esp3_description = data.description;
						$("#esp3lmdwaiting").hide();
						$("#esp3lmdteachedin").show();
						$scope.$apply();
						return;
					}
					if (data.result === 2) { // Learn mode timed out
						$("#esp3lmdwaiting").hide();
						$("#esp3lmdtimedout").show();
						return;
					}
					// Keep waiting
					$scope.mytimer = $interval(function () { $scope.IsNodeTeachedIn(); }, 1000);
				},
				error: function (result, status, error) {
					ShowNotify($.t("Problem teachin-in node!"), 2500, true);
				},
			});
		};

		OnCancelTeachIn = function () {
			$.ajax({
				url: "json.htm?type=command&param=esp3cancelteachin&hwdid=" + $.esp3hwdid,
				async: true,
				dataType: "json",
			});
			$interval.cancel($scope.mytimer);
			$scope.mytimer = undefined;
			$("#esp3learnmodedialog").modal("hide");
		};

		OnCloseLearnMode = function () {
			$("#esp3learnmodedialog").modal("hide");
			RefreshNodesTable();
		};

	}
});
