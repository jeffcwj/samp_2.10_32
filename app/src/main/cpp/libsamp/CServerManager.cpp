#include "CServerManager.h"

#include <stdint.h>

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("46.174.49.47", 1, 20, 7788, false),	// ������
	CServerInstance::create("46.174.49.47", 1, 20, 7825, false), 	// spb
	CServerInstance::create("46.174.49.47", 1, 20, 7786, false),	// ����
    CServerInstance::create("82.146.41.84", 1, 20, 7777, false),		// release
	CServerInstance::create("185.43.4.242", 1, 20, 7777, false)
};
