GetThermostatBigText = function (item) {
	var bigtext='';
	if (typeof item.RoomTemp != 'undefined') {
		bigtext = item.RoomTemp + ' / ' ;
	}
	bigtext += item.Data;
	bigtext += ' '+ item.vunit;
	return bigtext;
}
GetThermostatStatus = function (item) {
	var status = "";
	if (typeof item.Power != 'undefined') {
		status = "Power:" + item.Power + '%';
	}
	return status;
}

getThermostatImage = function (item) {
	var image = (item.CustomImage == 0) ? '"images/override.png"' : '"images/' + item.Image + '48_On.png"';
	if (item.isVirtualThermostat) {
		if (item.Switch == 1)
			image = '"images/override.png"';
		else
			image = '"images/override_off.png"';
	}
	var undef;
	var xhtm = '<img src=' + image + ' class="lcursor" onclick="ShowSetpointPopup(event, ' + item.idx + ',' + item.Protected + ', ' + item.Data + ',false, ' + item.step + ', ' + item.min + ', ' + item.max + ',' + item.ConforTemp + ',' + item.EcoTemp + ');" height="48" width="48" ></td>\n';

	return xhtm;
}

function UpdateDeviceCombo(ComboName, list, clear) {
	var Combo = $(ComboName);

	if (clear) Combo.find('option').remove().end();

	$.each(list, function (i, item) {
		var option = $('<option />');
		option.attr('value', item.idx).text(item.name);
		Combo.append(option);
	});

	var option = $('<option />');
	option.attr('value', '0').text('');
	//    Combo.append(option);

}
function RefreshDeviceCombo(ComboName, filter, clear) {
	//get list

	$.List = [];
	$.ajax({
		url: "json.htm?type=devices&filter=" + filter + "&used=true&order=Name",
		async: false,
		dataType: 'json',
		success: function (data) {
			if (typeof data.result != 'undefined') {
				$.each(data.result, function (i, item) {
					$.List.push({
						idx: item.idx,
						name: item.Name,
						LevelNames: item.LevelNames

					});
				});
			}
		}
	});
	UpdateDeviceCombo(ComboName, $.List, clear)
}

function switchChange(selectObj) {
	switchNb=""
	if (this.selectedIndex < 0)
		return;
	var levelNames = $.List[this.selectedIndex].LevelNames;
	var options = [];
	if (typeof levelNames != 'undefined') {
		levelNames = b64DecodeUnicode(levelNames).split('|');
		$.each(levelNames, function (i, item) {
			options.push({ idx: item, name: item });
		});
	}
	else {
		option = "Off"; options.push({ idx: option, name: option });
		option = "On"; options.push({ idx: option, name: option });
	}
	UpdateDeviceCombo($("#dialog-virtualthermostatdevice  #OnCmd"  + switchNb), options, true);
	UpdateDeviceCombo($("#dialog-virtualthermostatdevice  #OffCmd" + switchNb), options, true);
	$("#dialog-virtualthermostatdevice  #OnCmd"  +switchNb).val("");
	$("#dialog-virtualthermostatdevice  #OffCmd" +switchNb).val("");
	//    var optionSelected = $("option:selected", this);
	//    var valueSelected = this.value;
	//    alert(valueSelected);
}

Editvirtualthermostatdevice = function (idx, isprotected, refresh, TempSign, HwIdx) {
	HandleProtection(isprotected, function () {
		//creation boutton et dialog virtualthermostatdevice
		CreatevirtualthermostatXmlDialog();
		CreatevirtualthermostatDialog(HwIdx, idx, refresh);

		$.devIdx = idx;
		var Item = {};
		if (idx != 0) {

			$.ajax({
				url: "json.htm?type=devices&filter=utility&used=true&rid=" + idx,
				async: false,
				dataType: 'json',
				success: function (data) {
					if (typeof data.result != 'undefined') {
						if (data.result.length >= 1)
							Item = data.result[0];
					}
				}
			});
		}
		else {
			//error
			return;
		}
		$.Item = Item;


		$("#dialog-virtualthermostatdevice #devicename").val(Item.Name);
		$("#dialog-virtualthermostatdevice #devicedescription").val(Item.Description);
		$('#dialog-virtualthermostatdevice #protected').prop('checked', (Item.Protected == true));
		$("#dialog-virtualthermostatdevice #setpoint").val(Item.SetPoint);
		$("#dialog-virtualthermostatdevice #tempunit").html(TempSign);

		RefreshDeviceCombo("#dialog-virtualthermostatdevice #comboTemperature", 'temp', true);

		$("#dialog-virtualthermostatdevice  #comboTemperature").val(Item.TempIdx);

		RefreshDeviceCombo("#dialog-virtualthermostatdevice #SwitchIdx", 'light', true);
		$("#dialog-virtualthermostatdevice  #SwitchIdx").val(Item.SwitchIdx);
		$("#dialog-virtualthermostatdevice  #SwitchIdx").on('change', switchChange);
		$("#dialog-virtualthermostatdevice  #SwitchIdx").change();
		$("#dialog-virtualthermostatdevice  #OnCmd").val(Item.OnCmd);
		$("#dialog-virtualthermostatdevice  #OffCmd").val(Item.OffCmd);

		$("#dialog-virtualthermostatdevice  #CoefProp").val(Item.CoefProp);
		$("#dialog-virtualthermostatdevice  #CoefInteg").val(Item.CoefInteg);
		$("#dialog-virtualthermostatdevice  #Eco").val(Item.EcoTemp);
		$("#dialog-virtualthermostatdevice  #Confor").val(Item.ConforTemp);
		$("#dialog-virtualthermostatdevice  #virtualThermostat").show();

		$("#dialog-virtualthermostatdevice").i18n();
		$("#dialog-virtualthermostatdevice").dialog("open");
	});
}


ButtonActionvirtualthermostatDialog = function (hwidx, devIdx) {
	var bValid = true;
	bValid = bValid && checkLength($("#dialog-virtualthermostatdevice #devicename"), 2, 100);
	if (bValid) {

		if ($("#dialog-virtualthermostatdevice  #devicename").val() == null) {
			ShowNotify($.t('Please enter a Name!'), 2500, true);
			return;
		}

		if ($("#dialog-virtualthermostatdevice  #comboTemperature").val() == null) {
			ShowNotify($.t('Please select a temperature device!'), 2500, true);
			return;
		}

		if ($("#dialog-virtualthermostatdevice  #SwitchIdx").val() == null) {
			ShowNotify($.t('Please select a switch device!'), 2500, true);
			return;
		}
		if ($("#dialog-virtualthermostatdevice  #OnCmd").val() == null) {
			ShowNotify($.t('Please select the switch On Command!'), 2500, true);
			return;
		}
		if ($("#dialog-virtualthermostatdevice  #OffCmd").val() == null) {
			ShowNotify($.t('Please select the switch OffCmd Command!'), 2500, true);
			return;
		}
		
		if ($("#dialog-virtualthermostatdevice  #Eco").val() == "") {
			ShowNotify($.t('Please enter Eco Temperature!'), 2500, true);
			return;
		}
		if ($("#dialog-virtualthermostatdevice  #Confor").val() == "") {
			ShowNotify($.t('Please enter Comfort Temperature!'), 2500, true);
			return;
		}
		if ($("#dialog-virtualthermostatdevice  #CoefProp").val() == "") {
			ShowNotify($.t('Please enter proprotional Coeficient !'), 2500, true);
			return;
		}
		if ($("#dialog-virtualthermostatdevice  #CoefInteg").val() == "") {
			ShowNotify($.t('Please enter integral Coeficient !'), 2500, true);
			return;
		}
		

		$("#dialog-virtualthermostatdevice").dialog("close");

		var option = [];
		{
			option.push("Power" + ':' + $.Item.Power);
			option.push("RoomTemp" + ':' + $.Item.RoomTemp);
			option.push("TempIdx" + ':' + $("#dialog-virtualthermostatdevice  #comboTemperature").val());
			option.push("EcoTemp" + ':' + $("#dialog-virtualthermostatdevice  #Eco").val());
			option.push("CoefProp" + ':' + $("#dialog-virtualthermostatdevice  #CoefProp").val());
			option.push("ConforTemp" + ':' + $("#dialog-virtualthermostatdevice  #Confor").val());
			option.push("CoefInteg" + ':' + $("#dialog-virtualthermostatdevice  #CoefInteg").val());
			option.push("SwitchIdx" + ':' + $("#dialog-virtualthermostatdevice  #SwitchIdx").val());
			option.push("OnCmd" + ':' + $("#dialog-virtualthermostatdevice  #OnCmd").val());
			option.push("OffCmd" + ':' + $("#dialog-virtualthermostatdevice  #OffCmd").val());
		}
		//if update
		if (devIdx) {
			$.ajax({
				url: "json.htm?type=setused&idx=" + devIdx +
					'&name=' + encodeURIComponent($("#dialog-virtualthermostatdevice #devicename").val()) +
					'&description=' + encodeURIComponent($("#dialog-virtualthermostatdevice #devicedescription").val()) +
					'&setpoint=' + $("#dialog-virtualthermostatdevice #setpoint").val() +
					'&protected=' + $('#dialog-virtualthermostatdevice #protected').is(":checked") +
					'&devoptions=' + (option.join(';')) +

					'&used=true',
				async: false,
				dataType: 'json',
				success: function (data) {
					//ShowUtilities();
				}
			});
		}
		else {
			$.ajax({
				url: "json.htm?type=createdevice&idx=" + hwidx +
					"&sensorname=" + encodeURIComponent($("#dialog-virtualthermostatdevice #devicename").val()) +
					"&devicetype=" + "242" +
					"&devicesubtype=" + "01" +
					'&sensoroptions=' + (option.join(';')),

				async: false,
				dataType: 'json',
				success: function (data) {
					if (data.status == 'OK') {
						ShowNotify($.t('Virtual Thermostat Sensor Created, and can be found in the devices tab!'), 2500);
					}
					else {
						ShowNotify($.t('Problem creating Sensor!'), 2500, true);
					}
				},
				error: function () {
					HideNotify();
					ShowNotify($.t('Problem creating Sensor!'), 2500, true);
				}
			});

		}

	}

}
CreatevirtualthermostatDialog = function (hwidx, devIdx, refresh) {
	var dialog_editvirtualthermostatdevice_buttons = {};
	if (hwidx == 0) {

		dialog_editvirtualthermostatdevice_buttons[$.t("Update")] = function () {
			ButtonActionvirtualthermostatDialog(hwidx, devIdx);
		};
		dialog_editvirtualthermostatdevice_buttons[$.t("Remove Device")] = function () {
			$(this).dialog("close");
			bootbox.confirm($.t("Are you sure to remove this Device?"), function (result) {
				if (result == true) {
					$.ajax({
						url: "json.htm?type=setused&idx=" + devIdx +
							'&name=' + encodeURIComponent($("#dialog-editsetpointdevice #devicename").val()) +
							'&description=' + encodeURIComponent($("#dialog-editsetpointdevice #devicedescription").val()) +
							'&used=false',
						async: false,
						dataType: 'json',
						success: function (data) {
							refresh();
						}
					});
				}
			});
		};
		dialog_editvirtualthermostatdevice_buttons[$.t("Cancel")] = function () {
			$(this).dialog("close");
		};
	}
	else {
		dialog_editvirtualthermostatdevice_buttons[$.t("Create")] = function () {

			ButtonActionvirtualthermostatDialog(hwidx, devIdx);

		};
	}


	$("#dialog-virtualthermostatdevice").dialog({
		autoOpen: false,
		width: 'auto',
		height: 'auto',
		modal: true,
		resizable: false,
		title: $.t("Edit Device"),
		buttons: dialog_editvirtualthermostatdevice_buttons,
		close: function () {
			$(this).dialog("close");
		}
	});


}

CreatevirtualthermostatXmlDialog = function () {
	$(document.body).append(`
<div id="dialog-virtualthermostatdevice" title="Edit Device" style="display:none;">
    <form>
        <table class ="display" id="metertable" border="0" cellpadding="0" cellspacing="0">
        <tr>
          <td align="right" style="width:60px"><label for="devicename"><span data-i18n="Name">Name</span>: </label></td>
          <td><input type="text" id="devicename" style="width: 250px; padding: .4em;" class ="text ui-widget-content ui-corner-all" /></td>
        </tr>
        <tr>
            <td align="right" style="width:120px"><label for="devicedescription"><span data-i18n="Description"></span>: </label></td>
            <td><textarea id="devicedescription" name="devicedescription" rows="4" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea></td>
        </tr>
        <tr>
            <td align="right"><span data-i18n="Protected">Protected</span>:</td>
            <td><input type="checkbox" id="protected"><label for="protected"/></td>
        </tr>
        <tr>
          <td align="right"><label><span data-i18n="Value">Value</span>: </label></td>
          <td><input type="text" id="setpoint" style="width: 50px; padding: .4em;" class ="text ui-widget-content ui-corner-all" /> &deg; <span id="tempunit">C</span></td>
        </tr>
        </table>
        <table class ="display" id="virtualThermostat" border="0" cellpadding="0" cellspacing="0">
        <tr id="TemperatureDiv" >
            <td align="right" style="width:120px"><label for="comboTemperature"><span data-i18n="Temperature">Temperature</span>: </label></td>
            <td><select id="comboTemperature" style="width:150px" class ="combobox ui-corner-all">
                <!--#embed-->
            </select></td>
        </tr>
        <tr id="SwitchDiv" >
            <td align="right" style="width:60px"><label for="Switch">Switch: </label></td>
            <td><select id="SwitchIdx" " style="width:150px" class ="combobox ui-corner-all">
            </select></td>
          <td align="right" style="width:60px"><label>On :</label></td>
          <td><select type="text" id="OnCmd" style="width: 100px; padding: .4em;" class ="combobox ui-corner-all" >
              <option value="On"          >On     </option>
              <option value="Off"         >Off    </option>
          </select></td>

          <td align="right" style="width:60px"><label>Off:</label></td>
          <td><select type="text" id="OffCmd" style="width: 100px; padding: .4em;" class ="combobox ui-corner-all" >
              <option value="On"          >On     </option>
              <option value="Off"         >Off    </option>
          </select></td>

        <tr>
        <tr id="CoefPropDiv" >
          <td align="right" style="width:60px"><label>Kp: </label></td>
          <td><input type="text" id="CoefProp" style="width: 50px; padding: .4em;" class ="text ui-widget-content ui-corner-all" /></td>
        </tr>
        <tr id="CoefIntegDiv" >
          <td align="right" style="width:60px"><label>Ki: </label></td>
          <td><input type="text" id="CoefInteg" style="width: 50px; padding: .4em;" class ="text ui-widget-content ui-corner-all" /></td>
        </tr>
        <tr id="ConforDiv" >
          <td align="right" style="width:60px"><label data-i18n="Confor">Confor: </label></td>
          <td><input type="text" id="Confor" style="width: 50px; padding: .4em;" class ="text ui-widget-content ui-corner-all" /> &deg; <span id="Span1">C</span></td>
        </tr>
        <tr id="EcoDiv" >
          <td align="right" style="width:60px"><label data-i18n="Eco">Eco: </label></td>
          <td><input type="text" id="Eco" style="width: 50px; padding: .4em;" class ="text ui-widget-content ui-corner-all" /> &deg; <span id="Span2">C</span></td>
        </tr>
        </table>
    </form>
</div>
        `);

};


