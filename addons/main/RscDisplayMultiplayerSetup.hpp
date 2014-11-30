class RscDisplayMultiplayerSetup : RscStandardDisplay
{
	onLoad = QUOTE(DEF_RscDisplayMultiplayerSetupLoader);
	onMouseMoving = "if((uiNamespace getVariable ['JNET_mpSetupInit', false]) && {scriptDone (uiNamespace getVariable 'JNET_checkScriptId')}) then { ['RscDisplayMultiplayerSetup'] call (uiNamespace getVariable 'jnet_fnc_eventProcessor'); uiNamespace setVariable ['JNET_mpSetupInit', false]; };";
};