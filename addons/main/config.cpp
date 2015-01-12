#include "script_loaders.hpp"

class CfgPatches
{
	class jnet_main
	{
		units[] = {};
		requiredVersion = 0.01;
		requiredAddons[] = { "A3_UI_F" };
		version = "1.2.3";
		author[] = {"Jaynus", "Nou"};
		authorUrl = "http://www.idi-systems.com";
	};
};

class Extended_PostInit_EventHandlers
{
	JNET_init = "call compile preprocessFileLineNumbers  ""x\jnet\addons\main\on_gameStart.sqf"" ";
};


// This value is loaded by A3 at the start of the game and according to the source
// it is never used anywhere in the actual engine and the value is always 7503 on
// all machines. Probably a safe place to put our initialization!
TrackIR_Developer_ID = " call compile preprocessFileLineNumbers  ""x\jnet\addons\main\on_processStart.sqf"" ";


class RscDisplayEmpty;
class RscStandardDisplay;
class RscChatListDefault;
#include "RscDisplayMultiplayerSetup.hpp"
#include "RscDisplayRemoteMissions.hpp"


class JNET_Events {
	class RscDisplayRemoteMissions {
		jnet = "call compile preprocessFileLineNumbers 'x\jnet\addons\main\on_RscDisplayRemoteMissions.sqf'";
	};
	
	class RscDisplayMultiplayerSetup {
		jnet = "call compile preprocessFileLineNumbers 'x\jnet\addons\main\on_RscDisplayMultiplayerSetup.sqf'";
	};
};