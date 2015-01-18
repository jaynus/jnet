#include "script_loaders.hpp"

class CfgPatches
{
	class jnet_main
	{
		units[] = {};
		requiredVersion = " call compile preprocessFileLineNumbers  ""x\jnet\addons\main\on_processStart.sqf""; 1.0";
		requiredAddons[] = { "A3_UI_F" };
		version = "1.2.3";
		author[] = {"Jaynus", "Nou"};
		authorUrl = "http://www.idi-systems.com";
	};
};

class Extended_PostInit_EventHandlers
{
	JNET_init = "call compile preprocessFileLineNumbers  ""x\jnet\addons\main\on_gameStart.sqf""";
};

// We do 3 different methods just to garuntee we get called at process start
class PhysXParams {
	ragdollHitTime = " call compile preprocessFileLineNumbers  ""x\jnet\addons\main\on_processStart.sqf""; 0.05f";
};

cameraScript = " call compile preprocessFileLineNumbers  ""x\jnet\addons\main\on_processStart.sqf"";";
tooltipDelay = " call compile preprocessFileLineNumbers  ""x\jnet\addons\main\on_processStart.sqf""; 0";


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