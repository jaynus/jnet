//diag_log text format["begin on_RscDisplayMultiplayerSetup"];
if(!(uiNamespace getVariable ["JNET_mpSetupInit", false])) then {
	_scriptId = [] spawn { while {true} do { uiSleep 1; }; };

	uiNamespace setVariable ["JNET_checkScriptId", _scriptId];
	uiNamespace setVariable ["JNET_mpSetupInit", true];
	
	["onLoad",_this,"RscDisplayMultiplayerSetup",'GUI'] call compile preprocessfilelinenumbers "A3\ui_f\scripts\initDisplay.sqf";
};
diag_log text "HI MP SETUP SCREEN!";
_res = "jnet" callExtension "setStage:on_RscDisplayMultiplayerSetup";
call compile preprocessfilelinenumbers "\x\jnet\addons\main\setClientInfo.sqf";

