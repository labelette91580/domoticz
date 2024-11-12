define(['app', 'timers/factories', 'timers/components','timers/planning'], function (app) {

    app.controller('DeviceTimersController', function ($scope, $routeParams, deviceApi, deviceLightApi, deviceRegularTimersApi, deviceSetpointTimersApi, deviceTimerOptions, deviceTimerConfigUtils, utils) {
        var vm = this;
        var deviceTimers;

        var deleteConfirmationMessage = $.t('Are you sure to delete this timers?\n\nThis action can not be undone...');
        var clearConfirmationMessage = $.t('Are you sure to delete ALL timers?\n\nThis action can not be undone!');

        vm.addTimer = addTimer;
        vm.updateTimer = updateTimer;
        vm.deleteTimer = utils.confirmDecorator(deleteTimer, deleteConfirmationMessage);
        vm.clearTimers = utils.confirmDecorator(clearTimers, clearConfirmationMessage);

        AddXmlDialog = function () {
            var obj = $("#dialog-copy");
            if (obj.length == 0)
                $(document.body).append(`
<div id="dialog-copy" title="Copy Timers" style="display:none;">
		<form>
				<label for="sensor"><span data-i18n="Sensor">Sensor</span>: </label>
				<select id="sensor" style="width:250px" class="combobox ui-corner-all">

				</select>
		</form>
</div>
        `);

        };

        RefreshDeviceCombo = function (ComboName, filter, clear) {
            //get list

            $.List = [];
            $.ajax({
                url: "json.htm?type=command&param=getdevices&filter=" + filter + "&used=true&order=Name",
                async: false,
                dataType: 'json',
                success: function (data) {
                    if (typeof data.result != 'undefined') {
                        $.each(data.result, function (i, item) {
                            console.log("idx:" + item.idx + "name:" + item.Name);
                            $.List.push({
                                idx: item.idx,
                                name: item.Name
                            });
                        });
                    }
                }
            });
            var Combo = $(ComboName);
            if (clear) Combo.find('option').remove().end();
            $.each($.List, function (i, item) {
                var option = $('<option />');
                option.attr('value', item.idx).text(item.name);
                Combo.append(option);
            });
            var option = $('<option />');
            option.attr('value', '0').text('');
            //    Combo.append(option);
        }

        vm.copySetPoints = function () {
            AddXmlDialog();
            $("#dialog-copy").dialog({
                autoOpen: false,
                width: 400,
                height: 160,
                modal: true,
                resizable: false,
                buttons: {
                    "OK": function () {
                        var bValid = true;
                        $(this).dialog("close");

                        var SensorIdx = $("#dialog-copy #sensor option:selected").val();
                        var SensorName = $("#dialog-copy #sensor option:selected").text();
                        if (typeof SensorName == 'undefined') {
                            bootbox.alert($.t('No Sensor Type Selected!'));
                            return;
                        }

                        vm.refreshTimersFromIdx(SensorIdx);

                        bootbox.alert($.t('Sensor Timer ' + SensorName + ' copied!'));

                    },
                    Cancel: function () {
                        $(this).dialog("close");
                    }
                },
                close: function () {
                    $(this).dialog("close");
                }
            });
            if (vm.device.isSetpointTimers)
                RefreshDeviceCombo("#dialog-copy #sensor", "utility", true);
            else
                RefreshDeviceCombo("#dialog-copy #sensor", "light", true);

            $("#dialog-copy").i18n();
            $("#dialog-copy").dialog("open");

        };


        init();

        function init() {
            vm.deviceIdx = $routeParams.id;
            vm.selectedTimerIdx = null;
//          $.myglobals.ismobile = true;
            vm.ismobile = ($.myglobals.ismobile == true);
			
            vm.typeOptions = deviceTimerOptions.timerTypes;
			vm.timerSettings = deviceTimerConfigUtils.getTimerDefaultConfig();

            deviceApi.getDeviceInfo(vm.deviceIdx).then(function (device) {
                vm.isLoaded = true;
                vm.itemName = device.Name;
                vm.device   = device     ;
                vm.colorSettingsType = device.SubType;
                vm.dimmerType = device.DimmerType;

                vm.isDimmer = device.isDimmer();
                vm.isSelector = device.isSelector();
                vm.isLED = device.isLED();
                vm.isCommandSelectionDisabled = vm.isSelector && device.LevelOffHidden;
				
				var type = device.TypeImg.toLowerCase();
				if ((device.CustomImage !== 0) && (typeof device.Image !== 'undefined')) {
					type = device.Image.toLowerCase();
				}
                vm.isSetpointTimers = (device.Type === 'Setpoint' && device.SubType == 'SetPoint') || (device.Type === 'Radiator 1') || (device.Type === 'Thermostat 6');
				vm.isBlind = (type == 'blinds');
                //vm.isBlind = [3, 13, 14, 15, 21].includes(device.SwitchTypeVal);
                vm.device.isSetpointTimers = vm.isSetpointTimers;
                vm.device.isBlind = vm.isBlind;
                vm.levelOptions = [];

                deviceTimers = vm.isSetpointTimers
                    ? deviceSetpointTimersApi
                    : deviceRegularTimersApi;

                 if (vm.isSelector) {
                    vm.levelOptions = device.getSelectorLevelOptions();
                }

                if (vm.isLED) {
                    $scope.$watch(function () {
                        return vm.timerSettings.color + vm.timerSettings.level;
                    }, setDeviceColor);
                }

                if (vm.isDimmer) {
                    vm.levelOptions = device.getDimmerLevelOptions(1);
                }

                if (vm.levelOptions.length > 0) {
                    vm.timerSettings.level = vm.levelOptions[0].value;
                }

                vm.refreshTimersFromIdx = refreshTimersFromIdx;

                deviceTimers = vm.isSetpointTimers
                    ? deviceSetpointTimersApi
                    : deviceRegularTimersApi;


                if (!vm.isLED) 
                    $(document).trigger("timersInitialized", [vm, refreshTimers]);//<===Update for Planning
                else
                    $('#GridTable').hide()

				if (typeof device.vunit !== 'undefined') {
					vm.timerSettings.vunit=device.vunit;
				}

                refreshTimers();
            });
        }

        function refreshTimers() {
            vm.selectedTimerIdx = null;

            deviceTimers.getTimers(vm.deviceIdx).then(function (items) {
                $( document ).trigger( "timersLoaded", [items] );//<===Update for Planning
                vm.timers = items;
            });


        }
        function refreshTimersFromIdx(Idx) {
            vm.selectedTimerIdx = null;

            deviceTimers.getTimers(Idx).then(function (items) {
                $(document).trigger("timersLoaded", [items]);//<===Update for Planning
                vm.timers = items;
            });


        }

        function setDeviceColor() {
            if (!vm.timerSettings.color || !vm.timerSettings.level) {
                return;
            }

            deviceLightApi.setColor(
                vm.deviceIdx,
                vm.timerSettings.color,
                vm.timerSettings.level
            );
        }

        function getTimerConfig() {
            var utils = deviceTimerConfigUtils;
            var config = Object.assign({}, vm.timerSettings);

            if (!utils.isDayScheduleApplicable(config.timertype)) {
                config.days = 0;
            }

            if ([6, 7, 10, 12].includes(config.timertype)) {
                config.days = 0x80;
            }

            if (utils.isOccurrenceApplicable(config.timertype)) {
                config.days = Math.pow(2, config.weekday);
            }

            config = Object.assign({}, config, {
                date: utils.isDateApplicable(config.timertype) ? config.date : undefined,
                mday: utils.isRDaysApplicable(config.timertype) ? config.mday : undefined,
                month: utils.isRMonthsApplicable(config.timertype) ? config.month : undefined,
                occurence: utils.isOccurrenceApplicable(config.timertype) ? config.occurence : undefined,
                weekday: undefined,
                color: vm.isLED ? config.color : undefined,
                tvalue: vm.isSetpointTimers ? config.tvalue : undefined,
                command: vm.isSetpointTimers ? undefined : config.command,
                randomness: vm.isSetpointTimers ? undefined : config.randomness,
                persistent: config.persistent
            });

            var error = deviceTimerConfigUtils.getTimerConfigErrors(config);

            if (error) {
                ShowNotify(error, 2500, true);
                return false;
            }

            var warning = deviceTimerConfigUtils.getTimerConfigWarnings(config);

            if (warning) {
                ShowNotify(error, 2500, true);
            }

            return config;
        }

        function addTimer() {
            const config = getTimerConfig();

            if (!config) {
                return false;
            }

            deviceTimers
                .addTimer(vm.deviceIdx, config)
                .then(refreshTimers)
                .catch(function () {
                    HideNotify();
                    ShowNotify($.t('Problem adding timer!'), 2500, true);
                });
        }

        function updateTimer(timerIdx) {
            const config = getTimerConfig();

            if (!config) {
                return false;
            }

            deviceTimers
                .updateTimer(timerIdx, config)
                .then(refreshTimers)
                .catch(function () {
                    HideNotify();
                    ShowNotify($.t('Problem updating timer!'), 2500, true);
                });
        }

        function deleteTimer(timerIdx) {
            return deviceTimers.deleteTimer(timerIdx)
                .then(refreshTimers)
                .catch(function () {
                    HideNotify();
                    ShowNotify($.t('Problem deleting timer!'), 2500, true);
                });
        }

        function clearTimers() {
            return deviceTimers
                .clearTimers(vm.deviceIdx)
                .then(refreshTimers)
                .catch(function () {
                    HideNotify();
                    ShowNotify($.t('Problem clearing timers!'), 2500, true);
                });
        }
    });
});
