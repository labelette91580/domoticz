
function tt(name) {
    return name;
}


function SendCmd(devIdx,cmd,payload) {
	var result = {} ;
    $.ajax({
        url: "json.htm?type=enocean&hwid=" + $.hwid + "&cmd=" + cmd,
        data: payload,
        async: false,
        dataType: 'json',
        success: function (data) {
			if (data.status == 'OK') {
            	result = data.result ;
			}
			else {
				if (typeof data.message != 'undefined')
					ShowNotify($.t('Problem sending message '+ cmd + ':' + data.message ), 2500, true);
				else
					ShowNotify($.t('Problem sending message '+ cmd  ), 2500, true);
			}
		},
		error: function () {
			HideNotify();
			ShowNotify($.t('Problem sending message '+ cmd  ), 2500, true);
		}
    });
	return result ;
}
function EnOceanGetProfileCases(profil)
{
    var result= [] ;

    var Item = {};
    Item.Num = "1";
    Item.Title = "Case1";
    Item.Description = "Case1";
    result.push( Item ) ;

    var Item2 = {};
    Item2.Num = "2";
    Item2.Title = "Case2";
    Item2.Description = "Case2";
    result[1] = Item2;

    var Item3 = {};
    Item3.Num = "3";
    Item3.Title = "Case3";
    Item3.Description = "Case3";
    result[2] = Item3;

    //result.push(Item);
	var payload = {};
	payload["profil"] = profil ;

    result = SendCmd($.devIdx, "getCases", payload);

    return result;
}
//list : array[].Title/Value
function EnOceanRefreshDeviceCombo(ComboName, List, clear) {
		    //get list

		    var Combo = $(ComboName);

		    if (clear) Combo.find('option').remove().end();

		    $.each(List, function (i, item) {
		        var option = $('<option />');
			//		        option.attr('value', item.Num).text(item.Title);
		        option.attr('value', i ).text(item.Title);
			//		        option.attr('value', i ).text(item.Title);
		        Combo.append(option);
		    });

		    var option = $('<option />');
		    option.attr('value', '0').text('');
		    //    Combo.append(option);
}

function EnOceanRefreshDeviceComboWithList(ComboName, List, clear) {

		//create a variable that holds select tag with an id="combo", you can also put 'class' attribute instead of combo
		var Combo = $(ComboName);

		if (clear) Combo.find('option').remove().end();

        //iterate through each key/value in 'List' and create an option tag out of it
		Combo.find('option').remove().end();
        for(var val in List) {
            $('<option />', {value: val, text: List[val]}).appendTo(Combo);
        }
//        $('<option />', {value: "", text: ""}).appendTo(Combo);
//		var ComboObject = Combo.get( 0 );
		// Create a new 'change' event
//		var event = new Event('change');
		// Dispatch it.
//		ComboObject.dispatchEvent(event);
}

//enumerate  value:description;
//return a liste
function splitEnumerate(Enumerates)
{
		var enumList = {};
		var enumArray = [];
		var enums =  Enumerates.split(';');
		$.each(enums, function (i, item) {
			var enumvalue = item.split(':');
			if (enumvalue.length==2)
			{
				var Item = {};
				Item.Value = enumvalue[0];
				Item.Description = enumvalue[1];
				enumArray.push( Item ) ;
				enumList[enumvalue[0]] =  enumvalue[1];
			}
	    });
		return enumList ;
}
function EnOceanComboChange() {
    $.caseNb = $("#dialog-enoceansend  #CaseCombo").val();
    $("#dialog-enoceansend #description").val($.Cases[$.caseNb].Description);

    $.shortCuts = EnOceanGetProfileCase($.profil, $.caseNb);
    var i;
    for (i = 0; i < $.shortCuts.length ; i++) {
        $("#dialog-enoceansend  #Short" + i + " #LblParam" ).html($.shortCuts[i].Short);
        $("#dialog-enoceansend  #Short" + i + " #ValParam").val("0");
        $("#dialog-enoceansend  #Short" + i + " #DesParam").html($.shortCuts[i].Desc);
        $("#dialog-enoceansend  #Short" + i).show();

		enumList = splitEnumerate($.shortCuts[i].Enum);
		if (Object.keys(enumList).length>0){
			$("#dialog-enoceansend  #Short" + i + " #value  #ValParam").val(Object.keys(enumList)[0]);
			$("#dialog-enoceansend  #Short" + i + " #DesParam"+i).val(Object.values(enumList)[0]);

//			$("#dialog-enoceansend  #Short" + i + " #value  #ShortInput").val(Object.keys(enumList)[0]);
			}
			else{
		    $("#dialog-enoceansend  #Short" + i + " #value  #ValParam"  ).val("0");
//			$("#dialog-enoceansend  #Short" + i + " #value  #ShortInput").val("0");
			}
		EnOceanRefreshDeviceComboWithList( "#dialog-enoceansend  #Short" + i + " #value  #ShortCombo" , enumList, true) ;


    }
    for (i = $.shortCuts.length; i < 26; i++) {
        name = "#dialog-enoceansend  #Short" + i;
        $(name).hide();
    }
}

function EnOceanGetProfileCase(profil,caseNb) {
    var result = [];

    result.push({
        Short: "     Sh1" ,
        Desc : "Des  sh1", 
        Enum : "Enum sh1" 
    });

    if (caseNb >= 1) {
        result.push({
            Short: "     Sh2",
            Desc: "Des  sh2",
            Enum: "Enum sh2"
        });

    }
    if (caseNb >= 2) 

    result.push({
        Short: "     Sh3",
        Desc: "Des  sh3",
        Enum: "Enum sh3"
    });


    var payload = {};
	payload["profil"] = profil ;
    payload["casenb"] = caseNb;
    result = SendCmd($.devIdx, "getCaseShortCut", payload);
    
    return result;


}
EnOceanDeviceSendDialogOpen = function (hwid, profil, deviceId , baseAddr) {
	$.t = tt;
	$.profil = profil;
	$.devIdx = deviceId;
	$.hwid  = hwid;
	$.baseAddr  = baseAddr;

	//creation boutton et dialog enoceansend
	EnOceanDeviceSendAddXmlDialog();
	EnOceanDeviceSendCreateDialog(profil, deviceId);

	var Cases = EnOceanGetProfileCases(profil);

	$.Cases = Cases;

	$("#dialog-enoceansend #name").val(profil);
	$("#dialog-enoceansend #description").val(Cases[0].Description);

	EnOceanRefreshDeviceCombo("#dialog-enoceansend #CaseCombo", Cases, true);

	$("#dialog-enoceansend #CaseCombo").change( EnOceanComboChange ) ;

	//					$("#dialog-enoceansend #CaseCombo").change();
	EnOceanComboChange();
				    
	//				$("#dialog-enoceansend").i18n();
	$("#dialog-enoceansend").dialog("open");
};

EnOceanSendActuatorSetMeasurement = function (profil, deviceId,  Resetmeasurement, measurementType , IOchannel , MeasurementDelta, MeasurementUnit , MaxSendMessageTimeInSec , MinSendMessageTimeInSec ) 
{
	    var payload = {};
	    var i;
		payload[0] = 5;                          // "Command ID",{{ 5 , "ID 05" },}},
		payload[1] = 1;                          // "Report measurement",{{ 0 , "Report measurement: query only" },{ 1 , "Report measurement: query / auto reporting" },}},
		payload[2] = Resetmeasurement;           // "Reset measurement",{{ 0 , "Reset measurement: not active" },{ 1 , "Reset measurement: trigger signal" },}},
		payload[3] = measurementType;            // "Measurement mode",{{ 0 , "Energy measurement" },{ 1 , "Power measurement" },}},
		payload[4] = IOchannel;                  // "I/O channel",{}},
		payload[5] = MeasurementDelta & 0xF;     // "Measurement delta to be reported (LSB)"},
		payload[6] = MeasurementDelta >> 4;     // "Measurement delta to be reported (MSB)"},
		payload[7] = MeasurementUnit;            // "Unit",{{ 0 , "Energy [Ws]" },{ 1 , "Energy [Wh]" },{ 2 , "Energy [KWh]" },{ 3 , "Power [W]" },{ 4 , "Power [KW]" },}},
		payload[8] = MaxSendMessageTimeInSec / 10; // "Maximum time between two subsequent actuator messages",{}},
		payload[9] = MinSendMessageTimeInSec; // "Minimum time between two subsequent actuator messages",{}},

        payload["profil"]= profil;
        payload["casenb"]= 4 ; //SetMeasurement  cmd=5
        payload["devidx"]= deviceId ;
        payload["baseAddr"]= $.baseAddr ;
			
        result = SendCmd($.devIdx, "sendvld",payload );
}
EnOceanSendEnergyPowerMeasurement = function (profil, deviceId ) 
{
	EnOceanSendActuatorSetMeasurement($.profil, $.devIdx, 0, 0, 0, 1, 1, 70, 75);
//	EnOceanSendActuatorSetMeasurement($.profil, $.devIdx, 0, 1, 0, 1, 3, 60, 65);

}
EnOceanButtonActionOk = function (profil, deviceId) 
{
    var caseNb =  $("#dialog-enoceansend  #CaseCombo").val();

	var bValid = true;
	if (bValid) {

//	    $("#dialog-enoceansend").dialog("close");

	    var payload = {};
	    var i;
	    for (i = 0; i < $.shortCuts.length ; i++) {
	        var name = "#dialog-enoceansend  #Short" + i + " #ValParam" ;
	        payload[i] = $(name).val();
	        if ( isNaN(payload[i]) )
              	ShowNotify($.t('Invalid value '+ payload[i] ), 2500, true);
	        if ( isNaN(parseInt(payload[i]))) 
              	ShowNotify($.t('Invalid value '+ payload[i] ), 2500, true);
	    }
	    {
            payload["profil"]=profil;
            payload["casenb"]= $.caseNb ;
            payload["devidx"]= $.devIdx ;
            payload["baseAddr"]= $.baseAddr ;
			
            result = SendCmd($.devIdx, "sendvld",payload );
	    }
	}
}

EnOceanDeviceSendCreateDialog = function (profil, deviceId) {
    var dialog_buttons = {};

    dialog_buttons[$.t("Send")] = function () {
        EnOceanButtonActionOk(profil, deviceId);
    };
    dialog_buttons[$.t("Cancel")] = function () {
        $(this).dialog("close");
	};
    dialog_buttons[$.t("SetMeasurement")] = function () {
        EnOceanSendEnergyPowerMeasurement(profil, deviceId);
    };

    $("#dialog-enoceansend").dialog({
        autoOpen: false,
        width: 'auto',
        height: 'auto',
        modal: true,
        resizable: false,
        title: $.t("Send VLD"),
        buttons: dialog_buttons,
        close: function () {
            $(this).dialog("close");
        }
    });
}

		EnOceanDeviceSendAddXmlDialog = function () {
		    var obj = $("#dialog-enoceansend");
            if (obj.length == 0 )
	        $(document.body).append(`
<div id="dialog-enoceansend" title="Edit Device" style="display:none;">
    <form>
        <table class ="display" id="metertable" border="0" cellpadding="0" cellspacing="0">
        <tr>
          <td align="right" style="width:60px"><label ><span data-i18n="Name">Name</span>: </label></td>
          <td><input type="text" id="name" style="width: 250px; padding: .4em;" class ="text ui-widget-content ui-corner-all" /></td>
        </tr>
		<tr>
			<td align="right" style="width:120px"><label ><span data-i18n="Description"></span>: </label></td>
			<td><textarea id="description" name="description" rows="4" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea></td>
		</tr>
		<tr >
			<td align="right" style="width:120px"><label ><span data-i18n="Case">Case</span>: </label></td>
			<td>
				<select id="CaseCombo" style="width:400px" class ="combobox ui-corner-all">
					<option value="one">one</option>
				</select>
			</td>
		</tr>

		</table>
        <table class ="display" id="EnOceanSendVld" border="0" cellpadding="0" cellspacing="0">
        <tr id="Short0" >
          <td align="right" style="width:60px"><label id="LblParam" >Param1 </label></td>
		  <td>
			<div id="value" class ="text ui-widget-content ui-corner-all"   style="position:relative; border:none;  width:150px;  background-color:white; "  >
				<select id="ShortCombo" class ="combobox ui-corner-all" onchange="this.nextElementSibling.value=this.value;$(\'#DesParam0\').val(this.options[this.selectedIndex].innerHTML);"  style="position:absolute; top:0px; left:0px; border:none; width:150px; margin:0; padding: .2em;"   >
					<option value="one">one</option>
				</select>
				<input id="ValParam" class ="text ui-widget-content ui-corner-all" type="text" name="format" value="" style="position:absolute; top:0px; left:0px; width:130px; padding:0px;  border:none; padding: .2em;"  />
			</div>
		</td>
		<td>
			<textarea id="DesParam" name="DesParam" rows="3" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
			<br/>
			<textarea id="DesParam0" name="DesParam0" rows="1" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
		</td>
        </tr>
        <tr id="Short1">
          <td align="right" style="width:60px"><label id="LblParam" >Param1 </label></td>
		  <td>
			<div id="value" class ="text ui-widget-content ui-corner-all"   style="position:relative; border:none;  width:150px;  background-color:white; "  >
				<select id="ShortCombo" class ="combobox ui-corner-all" onchange="this.nextElementSibling.value=this.value;$(\'#DesParam1\').val(this.options[this.selectedIndex].innerHTML);"  style="position:absolute; top:0px; left:0px; border:none; width:150px; margin:0; padding: .2em;"   >
					<option value="one">one</option>
				</select>
				<input id="ValParam" class ="text ui-widget-content ui-corner-all" type="text" name="format" value="" style="position:absolute; top:0px; left:0px; width:130px; padding:0px;  border:none; padding: .2em;"  />
			</div>
		</td>
		<td>
			<textarea id="DesParam" name="DesParam" rows="3" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
			<br/>
			<textarea id="DesParam1" name="DesParam0" rows="1" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
		</td>
        </tr>
        <tr id="Short2">
          <td align="right" style="width:60px"><label id="LblParam" >Param1 </label></td>
		  <td>
			<div id="value" class ="text ui-widget-content ui-corner-all"   style="position:relative; border:none;  width:150px;  background-color:white; "  >
				<select id="ShortCombo" class ="combobox ui-corner-all" onchange="this.nextElementSibling.value=this.value;$(\'#DesParam2\').val(this.options[this.selectedIndex].innerHTML);"  style="position:absolute; top:0px; left:0px; border:none; width:150px; margin:0;padding: .2em; "   >
					<option value="one">one</option>
				</select>
				<input id="ValParam" class ="text ui-widget-content ui-corner-all" type="text" name="format" value="" style="position:absolute; top:0px; left:0px; width:130px; padding:0px;  border:none;padding: .2em;"  />
			</div>
		</td>
		<td>
			<textarea id="DesParam" name="DesParam" rows="3" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
			<br/>
			<textarea id="DesParam2" name="DesParam2" rows="1" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
		</td>
        </tr>
        <tr id="Short3">
          <td align="right" style="width:60px"><label id="LblParam" >Param1 </label></td>
		  <td>
			<div id="value" class ="text ui-widget-content ui-corner-all"   style="position:relative; border:none;  width:150px;  background-color:white; "  >
				<select id="ShortCombo" class ="combobox ui-corner-all" onchange="this.nextElementSibling.value=this.value;$(\'#DesParam3\').val(this.options[this.selectedIndex].innerHTML);"  style="position:absolute; top:0px; left:0px; border:none; width:150px; margin:0;padding: .2em; "   >
					<option value="one">one</option>
				</select>
				<input id="ValParam" class ="text ui-widget-content ui-corner-all" type="text" name="format" value="" style="position:absolute; top:0px; left:0px; width:130px; padding:0px;  border:none;padding: .2em;"  />
			</div>
		</td>
		<td>
			<textarea id="DesParam" name="DesParam" rows="3" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
			<br/>
			<textarea id="DesParam3" name="DesParam0" rows="1" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
		</td>
        </tr>
        <tr id="Short4">
          <td align="right" style="width:60px"><label id="LblParam" >Param1 </label></td>
		  <td>
			<div id="value" class ="text ui-widget-content ui-corner-all"   style="position:relative; border:none;  width:150px;  background-color:white; "  >
				<select id="ShortCombo" class ="combobox ui-corner-all" onchange="this.nextElementSibling.value=this.value;$(\'#DesParam4\').val(this.options[this.selectedIndex].innerHTML);"  style="position:absolute; top:0px; left:0px; border:none; width:150px; margin:0; padding: .2em;"   >
					<option value="one">one</option>
				</select>
				<input id="ValParam" class ="text ui-widget-content ui-corner-all" type="text" name="format" value="" style="position:absolute; top:0px; left:0px; width:130px; padding:0px;  border:none;padding: .2em;"  />
			</div>
		</td>
		<td>
			<textarea id="DesParam" name="DesParam" rows="3" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
			<br/>
			<textarea id="DesParam4" name="DesParam" rows="1" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
		</td>
        </tr>
        <tr id="Short5">
          <td align="right" style="width:60px"><label id="LblParam" >Param1 </label></td>
		  <td>
			<div id="value" class ="text ui-widget-content ui-corner-all"   style="position:relative; border:none;  width:150px;  background-color:white; "  >
				<select id="ShortCombo" class ="combobox ui-corner-all" onchange="this.nextElementSibling.value=this.value;$(\'#DesParam5\').val(this.options[this.selectedIndex].innerHTML);"  style="position:absolute; top:0px; left:0px; border:none; width:150px; margin:0; padding: .2em;"   >
					<option value="one">one</option>
				</select>
				<input id="ValParam" class ="text ui-widget-content ui-corner-all" type="text" name="format" value="" style="position:absolute; top:0px; left:0px; width:130px; padding:0px;  border:none;padding: .2em;"  />
			</div>
		</td>
		<td>
			<textarea id="DesParam" name="DesParam" rows="3" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
			<br/>
			<textarea id="DesParam5" name="DesParam5" rows="1" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
		</td>
        </tr>
        <tr id="Short6">
          <td align="right" style="width:60px"><label id="LblParam" >Param1 </label></td>
		  <td>
			<div id="value" class ="text ui-widget-content ui-corner-all"   style="position:relative; border:none;  width:150px;  background-color:white; "  >
				<select id="ShortCombo" class ="combobox ui-corner-all" onchange="this.nextElementSibling.value=this.value;$(\'#DesParam6\').val(this.options[this.selectedIndex].innerHTML);"  style="position:absolute; top:0px; left:0px; border:none; width:150px; margin:0; padding: .2em;"   >
					<option value="one">one</option>
				</select>
				<input id="ValParam" class ="text ui-widget-content ui-corner-all" type="text" name="format" value="" style="position:absolute; top:0px; left:0px; width:130px; padding:0px;  border:none;padding: .2em;"  />
			</div>
		</td>
		<td>
			<textarea id="DesParam" name="DesParam" rows="3" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
			<br/>
			<textarea id="DesParam6" name="DesParam6" rows="1" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
		</td>
        </tr>
        <tr id="Short7">
          <td align="right" style="width:60px"><label id="LblParam" >Param1 </label></td>
		  <td>
			<div id="value" class ="text ui-widget-content ui-corner-all"   style="position:relative; border:none;  width:150px;  background-color:white; "  >
				<select id="ShortCombo" class ="combobox ui-corner-all" onchange="this.nextElementSibling.value=this.value;$(\'#DesParam7\').val(this.options[this.selectedIndex].innerHTML);"  style="position:absolute; top:0px; left:0px; border:none; width:150px; margin:0; padding: .2em;"   >
					<option value="one">one</option>
				</select>
				<input id="ValParam" class ="text ui-widget-content ui-corner-all" type="text" name="format" value="" style="position:absolute; top:0px; left:0px; width:130px; padding:0px;  border:none;padding: .2em;"  />
			</div>
		</td>
		<td>
			<textarea id="DesParam" name="DesParam" rows="3" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
			<br/>
			<textarea id="DesParam7" name="DesParam7" rows="1" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
		</td>
        </tr>
        <tr id="Short8">
          <td align="right" style="width:60px"><label id="LblParam" >Param1 </label></td>
		  <td>
			<div id="value" class ="text ui-widget-content ui-corner-all"   style="position:relative; border:none;  width:150px;  background-color:white; "  >
				<select id="ShortCombo" class ="combobox ui-corner-all" onchange="this.nextElementSibling.value=this.value;$(\'#DesParam8\').val(this.options[this.selectedIndex].innerHTML);"  style="position:absolute; top:0px; left:0px; border:none; width:150px; margin:0; padding: .2em;"   >
					<option value="one">one</option>
				</select>
				<input id="ValParam" class ="text ui-widget-content ui-corner-all" type="text" name="format" value="" style="position:absolute; top:0px; left:0px; width:130px; padding:0px;  border:none;padding: .2em;"  />
			</div>
		</td>
		<td>
			<textarea id="DesParam" name="DesParam" rows="3" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
			<br/>
			<textarea id="DesParam8" name="DesParam8" rows="1" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
		</td>
        </tr>
        <tr id="Short9">
          <td align="right" style="width:60px"><label id="LblParam" >Param1 </label></td>
		  <td>
			<div id="value" class ="text ui-widget-content ui-corner-all"   style="position:relative; border:none;  width:150px;  background-color:white; "  >
				<select id="ShortCombo" class ="combobox ui-corner-all" onchange="this.nextElementSibling.value=this.value;$(\'#DesParam9\').val(this.options[this.selectedIndex].innerHTML);"  style="position:absolute; top:0px; left:0px; border:none; width:150px; margin:0; padding: .2em;"   >
					<option value="one">one</option>
				</select>
				<input id="ValParam" class ="text ui-widget-content ui-corner-all" type="text" name="format" value="" style="position:absolute; top:0px; left:0px; width:130px; padding:0px;  border:none;padding: .2em;"  />
			</div>
		</td>
		<td>
			<textarea id="DesParam" name="DesParam" rows="3" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
			<br/>
			<textarea id="DesParam9" name="DesParam9" rows="1" style="width: 356px; padding: .2em;" class ="text ui-widget-content ui-corner-all"></textarea>
		</td>
        </tr>
		</table>
    </form>
</div>
        `);

	    };


		