diag_log text format["JNET Game Initialized"];

_started = uiNamespace getVariable ["jnet_procstart_ran", false];
if(!_started) then {
	[] call compile preprocessFileLineNumbers  "x\jnet\addons\main\on_processStart.sqf";
};

if(!isDedicated) then {
	_res = "jnet" callExtension "init:gameStart";
	call compile preprocessfilelinenumbers "\x\jnet\addons\main\setClientInfo.sqf";
} else { 
	_res = "jnet" callExtension "init:gameStart";
};

