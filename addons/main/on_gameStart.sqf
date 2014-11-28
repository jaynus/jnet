diag_log text format["JNET Initialized"];

if(!isDedicated) then {
	_res = "jnet" callExtension "init:gameStart";
	call compile preprocessfilelinenumbers "\x\jnet\addons\main\setClientInfo.sqf";
} else { 
	_res = "jnet" callExtension "init:gameStart";
};

