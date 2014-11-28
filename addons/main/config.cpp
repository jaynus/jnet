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

class PhysXParams
{
	ragdollHitDmgLimit = " call compile preprocessFileLineNumbers  ""x\jnet\addons\main\on_processStart.sqf""  ";
};

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