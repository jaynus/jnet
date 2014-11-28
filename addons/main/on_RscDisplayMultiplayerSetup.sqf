//diag_log text format["begin on_RscDisplayMultiplayerSetup"];

_res = "jnet" callExtension "setStage:on_RscDisplayMultiplayerSetup";
call compile preprocessfilelinenumbers "\x\jnet\addons\main\setClientInfo.sqf";

["onLoad",_this,"RscDisplayMultiplayerSetup",'GUI'] call compile preprocessfilelinenumbers "A3\ui_f\scripts\initDisplay.sqf";