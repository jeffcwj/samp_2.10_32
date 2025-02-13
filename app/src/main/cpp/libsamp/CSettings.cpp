#include "main.h"
#include "CSettings.h"
#include "game/game.h"
#include "vendor/ini/config.h"
#include "java_systems/CHUD.h"
#include "util/patch.h"
#include "CDebugInfo.h"

extern CGame *pGame;
stSettings CSettings::m_Settings;

static void ClearBackslashN(char *pStr, size_t size) {
	for (size_t i = 0; i < size; i++) {
		if (pStr[i] == '\n' || pStr[i] == 13)
		{
			pStr[i] = 0;
		}
	}
}

void CSettings::toDefaults(int iCategory)
{
	char buff[0x7F];
	sprintf(buff, "%sSAMP/settings.ini", g_pszStorage);

	FILE *pFile = fopen(buff, "w");

	fwrite("[gui]", 1, 6, pFile);

	fclose(pFile);

	save(iCategory);
	LoadSettings();

	CHUD::ChangeChatHeight(m_Settings.iChatMaxMessages);
	CHUD::ChangeChatTextSize(m_Settings.iChatFontSize);

}

void CSettings::save(int iIgnoreCategory)
{
	char buff[0x7F];
	sprintf(buff, "%sSAMP/settings.ini", g_pszStorage);
	remove(buff);

	ini_table_s *config = ini_table_create();

	ini_table_create_entry_as_int(config, "gui", "hparmourtext", m_Settings.iHPArmourText);
	ini_table_create_entry_as_int(config, "gui", "damageinformer", m_Settings.iIsEnableDamageInformer);
	ini_table_create_entry_as_int(config, "gui", "text3dinveh", m_Settings.iIsEnable3dTextInVehicle);

	ini_table_create_entry_as_int(config, "client", "debug", m_Settings.szDebug);
	ini_table_create_entry_as_int(config, "client", "headmove", m_Settings.szHeadMove);
	ini_table_create_entry_as_int(config, "client", "dl", m_Settings.szDL);
	ini_table_create_entry_as_int(config, "client", "timestamp", m_Settings.szTimeStamp);
	ini_table_create_entry_as_int(config, "client", "test", m_Settings.isTestMode);

	ini_table_create_entry(config, "gui", "Font", m_Settings.szFont);

	ini_table_create_entry_as_float(config, "gui", "FontSize", m_Settings.fFontSize);
	ini_table_create_entry_as_int(config, "gui", "FontOutline", m_Settings.iFontOutline);

	ini_table_create_entry_as_int(config, "gui", "fps", m_Settings.iFPS);

	if (iIgnoreCategory != 1)
	{
		ini_table_create_entry_as_int(config, "gui", "ChatFontSize", m_Settings.iChatFontSize);
		ini_table_create_entry_as_int(config, "gui", "ChatMaxMessages", m_Settings.iChatMaxMessages);

		ini_table_create_entry_as_int(config, "gui", "androidKeyboard", m_Settings.iAndroidKeyboard);
		ini_table_create_entry_as_int(config, "gui", "outfit", m_Settings.iOutfitGuns);

		// ini_table_create_entry_as_int(config, "gui", "hparmourtext", m_Settings.iHPArmourText);
		// ini_table_create_entry_as_int(config, "gui", "pcmoney", m_Settings.iPCMoney);
	}

	ini_table_write_to_file(config, buff);
	ini_table_destroy(config);
}

extern bool g_bIsTestMode;
extern void ApplyFPSPatch(uint8_t fps);
void CSettings::LoadSettings(int iChatLines)
{

	Log("Loading settings..");

	char buff[0x7F];
	sprintf(buff, "%sSAMP/settings.ini", g_pszStorage);

	ini_table_s *config = ini_table_create();
	Log("Opening settings: %s", buff);
	if (!ini_table_read_from_file(config, buff))
	{
		Log("Cannot load settings, exiting...");
		pGame->exitGame();
		return;
	}

	snprintf(m_Settings.szFont, sizeof(m_Settings.szFont), "visby-round-cf-extra-bold.ttf");

	std::string szName = ini_table_get_entry(config, "client", "name");

	m_Settings.szDebug = ini_table_get_entry_as_int(config, "client", "debug", 0);
	CDebugInfo::SetDrawFPS(CSettings::m_Settings.szDebug);

	m_Settings.szHeadMove = ini_table_get_entry_as_int(config, "client", "headmove", 0);
	m_Settings.szDL = ini_table_get_entry_as_int(config, "client", "dl", 0);
	m_Settings.szTimeStamp = ini_table_get_entry_as_int(config, "client", "timestamp", 0);
	m_Settings.isTestMode = ini_table_get_entry_as_int(config, "client", "test", 0);
	g_bIsTestMode = (bool)m_Settings.isTestMode;

	std::string szFontName = ini_table_get_entry(config, "gui", "Font");

	if ( !szFontName.empty() )
	{
		strcpy(m_Settings.szFont, szFontName.c_str());
	}

	ClearBackslashN(m_Settings.szFont, sizeof(m_Settings.szFont));

	m_Settings.fFontSize = ini_table_get_entry_as_float(config, "gui", "FontSize", 30.0f);
	m_Settings.iChatFontSize = ini_table_get_entry_as_int(config, "gui", "ChatFontSize", -1);
	m_Settings.iFontOutline = ini_table_get_entry_as_int(config, "gui", "FontOutline", 2);

	m_Settings.iChatMaxMessages = ini_table_get_entry_as_int(config, "gui", "ChatMaxMessages", -1);

	m_Settings.iFPS = ini_table_get_entry_as_int(config, "gui", "fps", 60);
	if( m_Settings.iFPS < 20 ) m_Settings.iFPS = 60;
	ApplyFPSPatch(m_Settings.iFPS);

	m_Settings.iAndroidKeyboard = ini_table_get_entry_as_int(config, "gui", "androidKeyboard", 0);

	m_Settings.iOutfitGuns = ini_table_get_entry_as_int(config, "gui", "outfit", 1);
	CWeaponsOutFit::SetEnabled(CSettings::m_Settings.iOutfitGuns);

	m_Settings.iIsEnableDamageInformer = ini_table_get_entry_as_int(config, "gui", "damageinformer", 1);
	m_Settings.iIsEnable3dTextInVehicle = ini_table_get_entry_as_int(config, "gui", "text3dinveh", 1);

	m_Settings.iHPArmourText = ini_table_get_entry_as_int(config, "gui", "hparmourtext", 0);

	ini_table_destroy(config);
}

extern "C"
{

JNIEXPORT void JNICALL
Java_com_sasamp_cr_core_DialogClientSettingsCommonFragment_ChatLineChanged(JNIEnv *env,
																			   jobject thiz,
																			   jint newcount) {
	CSettings::m_Settings.iChatMaxMessages = newcount;
	CSettings::save();
}

JNIEXPORT void JNICALL
Java_com_sasamp_cr_core_DialogClientSettingsCommonFragment_ChatFontSizeChanged(JNIEnv *env,
																				   jobject thiz,
																				   jint size) {
	CSettings::m_Settings.iChatFontSize = size;
	CSettings::save();
	CHUD::ChangeChatTextSize(size);
}

JNIEXPORT jboolean JNICALL
Java_com_sasamp_cr_core_DialogClientSettingsCommonFragment_getNativeDamageInformer(JNIEnv *env,
																					   jobject thiz) {
	return CSettings::m_Settings.iIsEnableDamageInformer;
}

JNIEXPORT void JNICALL
Java_com_sasamp_cr_core_DialogClientSettingsCommonFragment_setNativeDamageInformer(JNIEnv *env,
																					   jobject thiz,
																					   jboolean state) {
	CSettings::m_Settings.iIsEnableDamageInformer = state;
	CSettings::save();
}
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_core_DialogClientSettingsCommonFragment_setNativeShow3dText(JNIEnv *env,
                                                                                   jobject thiz,
                                                                                   jboolean state) {
	CSettings::m_Settings.iIsEnable3dTextInVehicle = state;
	CSettings::save();
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_sasamp_cr_core_DialogClientSettingsCommonFragment_getNativeShow3dText(JNIEnv *env,
																				   jobject thiz) {
	return CSettings::m_Settings.iIsEnable3dTextInVehicle;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_sasamp_cr_core_DialogClientSettingsCommonFragment_getNativeFpsLimit(JNIEnv *env,
                                                                                 jobject thiz) {
	return CSettings::m_Settings.iFPS;
}

extern void ApplyFPSPatch(uint8_t fps);

extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_core_DialogClientSettingsCommonFragment_setNativeFpsCount(JNIEnv *env,
																				 jobject thiz,
																				 jint fps) {
	CSettings::m_Settings.iFPS = fps;
	CSettings::save();
	ApplyFPSPatch(fps);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_core_DialogClientSettingsCommonFragment_setNativeFpsCounterSettings(
        JNIEnv *env, jobject thiz, jboolean b) {
	CSettings::m_Settings.szDebug = b;

	CDebugInfo::SetDrawFPS(b);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_core_DialogClientSettingsCommonFragment_setNativeOutfitGunsSettings(
		JNIEnv *env, jobject thiz, jboolean b) {
	CSettings::m_Settings.iOutfitGuns = b;

	CWeaponsOutFit::SetEnabled(b);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_core_DialogClientSettings_onSettingsWindowDefaults(JNIEnv *env, jobject thiz,
																		  jint category) {
	CSettings::toDefaults(category);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_core_DialogClientSettings_onSettingsWindowSave(JNIEnv *env, jobject thiz) {
	CSettings::save();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_core_DialogClientSettingsCommonFragment_setNativeHpArmourText(JNIEnv *env,
																					 jobject thiz,
																					 jboolean b) {
	CSettings::m_Settings.iHPArmourText = b;
	CHUD::ToggleHpText(b);
	CSettings::save();
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_sasamp_cr_core_DialogClientSettingsCommonFragment_getNativeFpsCounterSettings(
		JNIEnv *env, jobject thiz) {
	return CSettings::m_Settings.szDebug;
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_sasamp_cr_core_DialogClientSettingsCommonFragment_getNativeOutfitGunsSettings(
		JNIEnv *env, jobject thiz) {
	return CSettings::m_Settings.iOutfitGuns;
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_sasamp_cr_core_DialogClientSettingsCommonFragment_getNativeHpArmourText(JNIEnv *env,
																					 jobject thiz) {
	CSettings::m_Settings.iHPArmourText;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_sasamp_cr_core_DialogClientSettingsCommonFragment_getAndroidKeyboard(JNIEnv *env,
																				  jobject thiz) {
	return CSettings::m_Settings.iAndroidKeyboard;
}
extern "C"
JNIEXPORT void JNICALL
Java_com_sasamp_cr_core_DialogClientSettingsCommonFragment_setAndroidKeyboard(JNIEnv *env,
																				  jobject thiz,
																				  jboolean b) {
	CSettings::m_Settings.iAndroidKeyboard = b;
	CSettings::save();
}