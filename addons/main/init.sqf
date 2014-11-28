diag_log text format["JNET Initialized"];

_res = "jnet" callExtension "init:base";

if(!isServer) then {
	_res = "jnet" callExtension "init:client";
} else {
	if(isDedicated) then {
		_res = "jnet" callExtension "init:server";
	};
};

