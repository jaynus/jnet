private["_res"];
//diag_log text format["begin on_RscDisplayMultiplayerSetup"];
if(!(uiNamespace getVariable ["JNET_mpSetupInit", false])) then {
	_scriptId = [] spawn { while {true} do { uiSleep 1; }; };

	uiNamespace setVariable ["JNET_checkScriptId", _scriptId];
	uiNamespace setVariable ["JNET_mpSetupInit", true];
};

_res = "jnet" callExtension "setStage:on_RscDisplayMultiplayerSetup";
call compile preprocessfilelinenumbers "\x\jnet\addons\main\setClientInfo.sqf";

_res = ["onLoad",_this,"RscDisplayMultiplayerSetup",'GUI'] call compile preprocessfilelinenumbers "A3\ui_f\scripts\initDisplay.sqf";
if(isNil "_res") then { nil };

_res