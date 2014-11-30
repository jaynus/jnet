class RscDisplayRemoteMissions: RscStandardDisplay
{
	onLoad = QUOTE(DEF_RscDisplayRemoteMissionsLoader);
	onMouseMoving = "if((uiNamespace getVariable ['JNET_rmInit', false]) && {scriptDone (uiNamespace getVariable 'JNET_checkScriptId')}) then { ['RscDisplayRemoteMissions'] call (uiNamespace getVariable 'jnet_fnc_eventProcessor'); uiNamespace setVariable ['JNET_rmInit', false]; };";
};