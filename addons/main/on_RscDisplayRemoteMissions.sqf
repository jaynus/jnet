private["_res"];

//diag_log text format["begin on_RscDisplayRemoteMissions"];
if(!(uiNamespace getVariable ["JNET_mpSetupInit", false])) then {
	_scriptId = [] spawn { while {true} do { uiSleep 1; }; };

	uiNamespace setVariable ["JNET_checkScriptId", _scriptId];
	uiNamespace setVariable ["JNET_rmInit", true];
};
_res = "jnet" callExtension "setStage:on_RscDisplayRemoteMissions";
call compile preprocessfilelinenumbers "\x\jnet\addons\main\setClientInfo.sqf";

_res = ["onLoad",_this,"RscDisplayRemoteMissions",'GUI'] call compile preprocessfilelinenumbers "A3\ui_f\scripts\initDisplay.sqf";
if(isNil "_res") then { nil };

_res