#include "Services.h"

CHAR* SrvStatesName[] = {
	"Stopped",
	"Start Pending",
	"Stop Pending",
	"Running",
	"Continue Pending",
	"Pause Pending",
	"Paused"
};

CHAR* SrvStartTypeName[] = {
	"BootStart",
	"SystemStart",
	"AutoStart",
	"DemandStart",
	"Disabled"
};

BOOL EnumServices(PARM* Parm)
{
	DWORD dwServerCount = 0;
	DWORD i = 0;
	LPENUM_SERVICE_STATUS lpscmBuf = NULL;
	DWORD dwNeedSize = 0;
	// �򿪷��������
	SC_HANDLE  hSCM = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if (NULL == hSCM) return FALSE;

	// ��ȡ����
	EnumServicesStatus(hSCM, SERVICE_WIN32|SERVICE_DRIVER, SERVICE_STATE_ALL, NULL, 0, &dwNeedSize, &dwServerCount, NULL);
	lpscmBuf = (LPENUM_SERVICE_STATUS)malloc(dwNeedSize);
	memset(lpscmBuf, 0, dwNeedSize);
	if (!EnumServicesStatus(hSCM, SERVICE_WIN32|SERVICE_DRIVER, SERVICE_STATE_ALL, lpscmBuf, dwNeedSize, &dwNeedSize, &dwServerCount, NULL))
	{
		free(lpscmBuf);
		return FALSE;
	}

	// ������ȡ��Ϣ
	for(i = 0; i < dwServerCount; i++)
	{
		QUERY_SERVICE_CONFIG* pSrvConf;
		SC_HANDLE hSrv = OpenService(hSCM, lpscmBuf[i].lpServiceName, SERVICE_ALL_ACCESS);
		PCHAR pStartType = NULL;
		// ��ѯ��������״̬��·����
		if(hSrv)
		{
			DWORD dwSrvNeedSize = 0;
			QueryServiceConfig(hSrv, 0, 0, &dwSrvNeedSize);
			pSrvConf = (LPQUERY_SERVICE_CONFIG)malloc(dwSrvNeedSize);
			if(QueryServiceConfig(hSrv, pSrvConf, dwSrvNeedSize, &dwSrvNeedSize))
			{
				pStartType = SrvStartTypeName[pSrvConf->dwStartType];
				free(pSrvConf);
			}
		} 
		printf("%s %s %s\r\n", lpscmBuf[i].lpServiceName, lpscmBuf[i].lpDisplayName, pStartType);
		CloseServiceHandle(hSrv);
	}

	CloseServiceHandle(hSCM);
	free(lpscmBuf);
	return TRUE;
}


