//diag_log text format["begin on_RscDisplayRemoteMissions"];

_res = "jnet" callExtension "setStage:on_RscDisplayRemoteMissions";
call compile preprocessfilelinenumbers "\x\jnet\addons\main\setClientInfo.sqf";

["onLoad",_this,"RscDisplayRemoteMissions",'GUI'] call compile preprocessfilelinenumbers "A3\ui_f\scripts\initDisplay.sqf";