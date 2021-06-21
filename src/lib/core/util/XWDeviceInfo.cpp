/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/

#include <QDir>
#include <QStringList>

#include "XWDeviceInfo.h"

#ifdef Q_WS_WIN
//#include <windows.h>
//#include <winioctl.h>
//#include <TCHAR.H>
//#include <initguid.h>
//#include <setupapi.h>
//DEFINE_GUID(UsbClassGuid, 0xa5dcbf10L, 0x6530, 0x11d2, 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed);
//DEFINE_GUID(DiskClassGuid, 0x53f56307L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);
//DEFINE_GUID(FloppyClassGuid, 0x53f56311L, 0xb6bf, 0x11d0, 0x94, 0xf2, 0x00, 0xa0, 0xc9, 0x1e, 0xfb, 0x8b);

#define _WIN32_DCOM
using namespace std;
#include <comdef.h>
#include <Wbemidl.h>
# pragma comment(lib, "wbemuuid.lib")
#endif

static QString getDriver(const QString & devPath);

static bool getDeviceInfo(const QString drv, QString & sn, QString & t);

#ifdef Q_WS_WIN

static long    getDeviceNumber(wchar_t *  lpDevicePath);
static bool getDeviceSerialNumber(long number, LPGUID lpGuid, QString & sn, QString & t);

static QString getDriver(const QString & devPath)
{
    QDir dir(devPath);
    if (!dir.exists())
        return QString();
        
    QString absolute = dir.absolutePath();
    
    absolute = absolute.left(2);
	absolute = absolute.toUpper();
	return absolute;
}

bool getDeviceInfo(const QString drv, QString & sn, QString & t)
{
	HRESULT hres;
	hres =  CoInitialize(0); 
	if (FAILED(hres))
		return false;
		
	 hres =  CoInitializeSecurity(
        NULL, 
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities 
        NULL                         // Reserved
        );
	if (FAILED(hres))
	{
		CoUninitialize();
		return false;
	}
		
	IWbemLocator *pLoc = NULL;
	hres = CoCreateInstance(
        CLSID_WbemLocator,             
        0, 
        CLSCTX_INPROC_SERVER, 
        IID_IWbemLocator, (LPVOID *) &pLoc);
	if (FAILED(hres))
	{
		CoUninitialize();
		return false;
	}
	
	IWbemServices *pSvc = NULL;
	hres = pLoc->ConnectServer(
         _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
         NULL,                    // User name. NULL = current user
         NULL,                    // User password. NULL = current
         0,                       // Locale. NULL indicates current
         NULL,                    // Security flags.
         0,                       // Authority (e.g. Kerberos)
         0,                       // Context object 
         &pSvc                    // pointer to IWbemServices proxy
         );
	if (FAILED(hres))
	{
		pLoc->Release();
		CoUninitialize();
		return false;
	}
	
	hres = CoSetProxyBlanket(
       pSvc,                        // Indicates the proxy to set
       RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
       RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
       NULL,                        // Server principal name 
       RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
       RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
       NULL,                        // client identity
       EOAC_NONE                    // proxy capabilities 
    );
	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return false;
	}
	
	QString sql = QString("ASSOCIATORS OF {Win32_LogicalDisk.DeviceID=\"%1\"}").arg(drv);
	BSTR bsql = SysAllocString(sql.utf16());
	
	IEnumWbemClassObject* pEnumerator = NULL;
	hres = pSvc->ExecQuery(
        bstr_t("WQL"), 
        bstr_t(bsql),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);
	SysFreeString(bsql);
  if (FAILED(hres))
 	{
 		pSvc->Release();
 		pLoc->Release();
 		CoUninitialize();
 		return false;
  }
  
  IWbemClassObject *pclsObj;
  ULONG uReturn = 0;
  QString partitionID;
  while (pEnumerator)
  {
  	HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
  	if(0 == uReturn)
  		break;
  	
  	VARIANT vtProp;
  	hr = pclsObj->Get(L"DeviceID", 0, &vtProp, 0, 0);
  	partitionID = QString::fromUtf16(vtProp.bstrVal);
  	VariantClear(&vtProp);
  	if (partitionID.startsWith("Disk"))
  		break;
  }
  
  if (partitionID.isEmpty())
  {
  	pSvc->Release();
 		pLoc->Release();
 		pclsObj->Release();
 		CoUninitialize();
 		return false;
  }
  
  hres = pSvc->ExecQuery(
        bstr_t("WQL"), 
        bstr_t("SELECT * FROM Win32_DiskPartition"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);
  if (FAILED(hres))
 	{
 		pSvc->Release();
 		pLoc->Release();
 		pclsObj->Release();
 		CoUninitialize();
 		return false;
  }
  
  uint index = 0;
  bool founded = false;
  while (pEnumerator)
  {
  	HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
  	if(0 == uReturn)
  		break;
  	
  	VARIANT vtProp;
  	hr = pclsObj->Get(L"DeviceID", 0, &vtProp, 0, 0);
  	QString id = QString::fromUtf16(vtProp.bstrVal);  	
  	if (id == partitionID)
  	{
  		founded = true;
  		hr = pclsObj->Get(L"DiskIndex", 0, &vtProp, 0, 0);
  		index = vtProp.uintVal;
  	}
  	
  	VariantClear(&vtProp);
  	if (founded)
  		break;
  }
  
  if (!founded)
  {
  	pSvc->Release();
 		pLoc->Release();
 		pclsObj->Release();
 		CoUninitialize();
 		return false;
  }
  
  hres = pSvc->ExecQuery(
        bstr_t("WQL"), 
        bstr_t("SELECT * FROM Win32_DiskDrive"),
        WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 
        NULL,
        &pEnumerator);
  if (FAILED(hres))
 	{
 		pSvc->Release();
 		pLoc->Release();
 		CoUninitialize();
 		return false;
  }
  
  founded = false;
  while (pEnumerator)
  {
  	HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
  	if(0 == uReturn)
  		break;
  	
  	VARIANT vtProp;
  	hr = pclsObj->Get(L"Index", 0, &vtProp, 0, 0);
  	if (vtProp.uintVal == index)
  	{
  		founded = true;
  		hr = pclsObj->Get(L"InterfaceType", 0, &vtProp, 0, 0);
  		t = QString::fromUtf16(vtProp.bstrVal); 
  		VariantClear(&vtProp);
  		hr = pclsObj->Get(L"Model", 0, &vtProp, 0, 0);
  		QString m = QString::fromUtf16(vtProp.bstrVal); 
  		VariantClear(&vtProp);
  		for (int i = 0; i < m.length(); i++)
  		{
  			if (!m[i].isSpace())
  				t.append(m[i]);
  		}
  		
  		hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);
  		sn = QString::fromUtf16(vtProp.bstrVal); 
  		VariantClear(&vtProp);
  	}
  	else
  		VariantClear(&vtProp);
  		
  	if (founded)
  		break;
  }
  
  pSvc->Release();
 	pLoc->Release();
 	pclsObj->Release();
 	CoUninitialize();
 	return founded;
}

/*
static long getDeviceNumber(wchar_t *  lpDevicePath)
{
    HANDLE hDeviceHandle = CreateFileW(lpDevicePath, GENERIC_READ,
		                               FILE_SHARE_READ | FILE_SHARE_WRITE,
		                               NULL, OPEN_EXISTING, 0, NULL);
	if (hDeviceHandle == INVALID_HANDLE_VALUE)
		return -1;
	
	STORAGE_DEVICE_NUMBER dn;
	DWORD dwBytesReturned;
	BOOL bResult = ::DeviceIoControl(hDeviceHandle, 
		                             IOCTL_STORAGE_GET_DEVICE_NUMBER,
						             NULL, 0, (void*)&dn, sizeof(dn), 
						             &dwBytesReturned, (LPOVERLAPPED)NULL);
	CloseHandle(hDeviceHandle);

	if (bResult)
		return dn.DeviceNumber;

	return -1;
}

static bool getDeviceSerialNumber(long number, LPGUID lpGuid, QString & sn, QString & t)
{
    const int interfaceDetailSize = 1024;
    HDEVINFO hDevInfoSet;
    
    SP_DEVICE_INTERFACE_DATA ifdata;
    
    PSP_DEVICE_INTERFACE_DETAIL_DATA pDetail;
    
    int  nCount;
    BOOL bResult;
    
    hDevInfoSet = ::SetupDiGetClassDevs(lpGuid,  
                                        NULL, NULL, 
                                        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfoSet == INVALID_HANDLE_VALUE)
        return false;
        
    pDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)::GlobalAlloc(LMEM_ZEROINIT, interfaceDetailSize);
    pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    
    nCount = 0;
    bResult = TRUE;
    
    QString ret;
    QStringList list;
    // 设备序号=0,1,2... 逐一测试设备接口，到失败为止
    while (bResult)
    {
        ifdata.cbSize = sizeof(ifdata);
        
        // 枚举符合该GUID的设备接口
        bResult = ::SetupDiEnumDeviceInterfaces(
                                                hDevInfoSet,     // 设备信息集句柄
                                                NULL,            // 不需额外的设备描述
                                                lpGuid,          // GUID
                                                (ULONG)nCount,   // 设备信息集里的设备序号
                                                &ifdata);        // 设备接口信息
                                                
        if (bResult)
        {
            // 取得该设备接口的细节(设备路径)
            bResult = SetupDiGetInterfaceDeviceDetail(
                                                      hDevInfoSet,    // 设备信息集句柄
                                                      &ifdata,        // 设备接口信息
                                                      pDetail,        // 设备接口细节(设备路径)
                                                      interfaceDetailSize,   // 输出缓冲区大小
                                                      NULL,           // 不需计算输出缓冲区大小(直接用设定值)
                                                      NULL);          // 不需额外的设备描述
            if (bResult)
            {
                nCount++;
                if (number == getDeviceNumber(pDetail->DevicePath))
                {
                    ret = QString::fromWCharArray(pDetail->DevicePath, -1);
                    list = ret.split("#");
                    ret = list.at(1);
                    int i = ret.indexOf("ven_", Qt::CaseInsensitive);
                    if (i != -1)
                    	i += 4;
                    else
                    	i = 0;
                    	
                    while (!ret[i].isNull() && 
                           (ret[i].isLetterOrNumber() || 
                           ret[i] == QChar('_')))
                    {
                    	if (ret[i].isLetter())
                    		t.append(ret[i].toUpper());
                    	else if (ret[i] != QChar('_'))
                    		t.append(ret[i]);
                    	i++;
                    }
                    
                    int j = ret.indexOf("prod_", Qt::CaseInsensitive);
                    if (j != -1)
                    	i = j + 5;
                    while (!ret[i].isNull() && 
                          (ret[i].isLetterOrNumber() || 
                           ret[i] == QChar('_')))
                    {
                    	if (ret[i].isLetter())
                    		t.append(ret[i].toUpper());
                    	else if (ret[i] != QChar('_'))
                    		t.append(ret[i]);
                    	i++;
                    }
                    
                    j = ret.indexOf("rev_", Qt::CaseInsensitive);
                    if (j != -1)
                    	i = j + 4;
                    	
                    while (!ret[i].isNull() && 
                           (ret[i].isLetterOrNumber() || 
                           ret[i] == QChar('_')))
                    {
                    	if (ret[i].isLetter())
                    		t.append(ret[i].toUpper());
                    	else if (ret[i] != QChar('_'))
                    		t.append(ret[i]);
                    	i++;
                    }
                    
                    ret = list.at(2); 
                    i = 0;
                    while (!ret[i].isNull())
                    {
                    	if (ret[i].isLetter())
                    		sn.append(ret[i].toUpper());
                    	else if (ret[i].isDigit())
                    		sn.append(ret[i]);
                    	i++;
                    }
                                            
                    ::GlobalFree(pDetail);
                        
                    return true;
                }
            }
        }
    }
    
    ::GlobalFree(pDetail);
    ::SetupDiDestroyDeviceInfoList(hDevInfoSet);
    
    return false;
}

bool getDeviceInfo(const QString drv, QString & sn, QString & t)
{
    QString driver = QString("\\\\.\\%1").arg(drv);    
    int len = driver.length() + 1;
    
    wchar_t * d = (wchar_t *)malloc(len * sizeof(wchar_t));
    if (d == NULL)
        return false;
        
    len = driver.toWCharArray(d);
    d[len] = 0;
    
    long number = getDeviceNumber(d);
    
    free(d);
    
    if (!getDeviceSerialNumber(number, (LPGUID)&DiskClassGuid, sn, t))
    {
        if (!getDeviceSerialNumber(number, (LPGUID)&UsbClassGuid, sn, t))
        {
            if (!getDeviceSerialNumber(number, (LPGUID)&FloppyClassGuid, sn, t))
                return false;
        }
    }
    
    return true;
}
*/
#endif

XWDeviceInfo::XWDeviceInfo(const QString & devPath, QObject * parent)
    :QObject(parent),
     m_devPath(devPath)
{
    m_driver = getDriver(devPath);
    if (!m_driver.isEmpty())
    {
        if (!getDeviceInfo(m_driver, m_serialNumber, m_type))
        {
            m_serialNumber.clear();
            m_type.clear();
        }
    }
}

XWDeviceInfo::XWDeviceInfo(QObject * parent)
    :QObject(parent)
{
}

void XWDeviceInfo::setDevPath(const QString & devPath)
{
    m_devPath = devPath;
    m_driver = getDriver(devPath);
    if (m_driver.isEmpty())
    {
        m_serialNumber.clear();
        m_type.clear();
        return ;
    }
    
    if (!getDeviceInfo(m_driver, m_serialNumber, m_type))
    {
        m_serialNumber.clear();
        m_type.clear();
    }
}


