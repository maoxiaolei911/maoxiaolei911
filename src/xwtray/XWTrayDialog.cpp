/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <QtGui>
#include <QDir>
#include <QStringList>
#include <QCoreApplication>
#include <QLocale>
#include <QFileInfo>
#include <QProcess>
#include <QProcessEnvironment>
#include <QXmlStreamReader>
#include <QTextStream>
#include <QNetworkReply>
#include "XWTrayDialog.h"


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

XWUpdateFile::XWUpdateFile()
{
	updated = false;
}

XWTrayDialog::XWTrayDialog(QWidget * parent)
	:QDialog(parent)
{
	setWindowTitle(tr("Xiuwen"));
  setWindowIcon(QIcon(":/images/xiuwen24.png"));
  
  logoLabel = new QLabel(this);
  logoLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  logoLabel->setPixmap(QPixmap(":/images/xiuwen64.png"));
  
  noteLabel = new QLabel(this);  
  noteLabel->setWordWrap(true);
    
  totalLabel = new QLabel(tr("No update now."));
  progressLabel = new QLabel(tr("Please close other xiuwen programs before updating."));
  
  progressBar = new QProgressBar;
  progressBar->setValue(0);
  progressBar->setMinimum(0);
  
  okButton = new QPushButton(tr("Update"));
  
  descriptionEdit = new QTextBrowser;
  
  trayIconMenu = new QMenu(this);
  trayIcon = new QSystemTrayIcon(this);
  trayIcon->setIcon(QIcon(":/images/xiuwen32.png"));
  trayIcon->setContextMenu(trayIconMenu);
  
  connect(okButton, SIGNAL(clicked()), this, SLOT(checkForUpdate()));
  
  programGrp = 0;
  
  output = 0;
  total = 0;
  remnant = 0;
  succ = 0;
  
#ifdef Q_OS_WIN	
	os = "Windows";
#elif defined(Q_OS_MAC)
	os = "MAC";
#elif defined(Q_OS_LINUX)
	os = "Linux";
#else
	os = "Unix";
#endif
	
	lang = QLocale::system().name();
	binDir = QCoreApplication::applicationDirPath();
	rootDir = binDir;
  int ti = rootDir.lastIndexOf(QChar('/'));
  if (ti > 0)
   	rootDir = rootDir.left(ti);
  qint64 pid = QCoreApplication::applicationPid();
  tmpDir = QString("%1/tmp/p%2").arg(rootDir).arg(pid);
  QDir tmpd(tmpDir);
  if (!tmpd.mkpath(tmpDir))
  	tmpDir = QDir::tempPath();
  
	QString driver = getDriver(binDir);
	QString serial;
	QString type;
	if (getDeviceInfo(driver, serial, type))
	{
		hardId = type;
    hardId += serial;
	}
	
	QString h = QString(tr("Update in a timely manner in order to obtain the latest feature."
	                       "The hardware ID is <h4>%1</h4>")).arg(hardId);
	noteLabel->setText(h);
	
	networkReply = 0;
	QString inifile = QString("%1/xwtray.ini").arg(binDir);
  setting = new QSettings(inifile, QSettings::IniFormat);
  	
  featureAction = new QAction(tr("Feature"), this); 
  connect(featureAction, SIGNAL(triggered()), this, SLOT(getFeature()));
  
  updateAction = new QAction(tr("&Update"), this); 
  connect(updateAction, SIGNAL(triggered()), this, SLOT(showNormal()));
  
  quitAction = new QAction(tr("&Quit"), this);
  connect(quitAction, SIGNAL(triggered()), this, SLOT(cleanup()));
  	
  createActions();
  
  QGridLayout *llayout = new QGridLayout();
  llayout->addWidget(logoLabel, 0, 0, 2, 1, Qt::AlignTop);
  llayout->addWidget(noteLabel, 0, 1, 1, 1);
  
  QHBoxLayout *playout = new QHBoxLayout();
  playout->addWidget(progressLabel);
  playout->addStretch();
  playout->addWidget(okButton);
  
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addLayout(llayout);
  mainLayout->addSpacing(10);
  mainLayout->addWidget(totalLabel);
  mainLayout->addSpacing(5);
  mainLayout->addLayout(playout);
  mainLayout->addSpacing(5);
  mainLayout->addWidget(progressBar);
  mainLayout->addSpacing(5);
  mainLayout->addWidget(descriptionEdit);
  setLayout(mainLayout);
  
  trayIcon->setToolTip(tr("Update now?"));
  
  trayIcon->show();
  resize(400, 300);
}

XWTrayDialog::~XWTrayDialog()
{
	clearUpdateItems();
	if (output)
	{
		output->close();
		delete output;
		output = 0;
	}
}

void XWTrayDialog::setVisible(bool visible)
{
	updateAction->setEnabled(!visible);
	QDialog::setVisible(visible);
}

void XWTrayDialog::closeEvent(QCloseEvent *event)
{
	if (trayIcon->isVisible())
	{
		hide();
		event->ignore();
	}
}

void XWTrayDialog::allFinished()
{
	QString path = QString("%1/tex/tex.lsr").arg(rootDir);
  if (QFile::exists(path))
  	QFile::remove(path);
  		
  path = QString("%1/fonts/fonts.lsr").arg(rootDir);
  if (QFile::exists(path))
  	QFile::remove(path);
  		
  path = QString("%1/xwtex/xwtex.lsr").arg(rootDir);
  if (QFile::exists(path))
  	QFile::remove(path);
  		
  path = QString("%1/help/help.lsr").arg(rootDir);
  if (QFile::exists(path))
  	QFile::remove(path);
  			
  path = QString("%1/makeindex/makeindex.lsr").arg(rootDir);
  if (QFile::exists(path))
  	QFile::remove(path);
  			
  path = QString("%1/bibtex/bibtex.lsr").arg(rootDir);
  if (QFile::exists(path))
  	QFile::remove(path);
  		
  path = QString("%1/dvips/dvips.lsr").arg(rootDir);
  if (QFile::exists(path))
  	QFile::remove(path);
  		
  path = QString("%1/Ghostscript/Ghostscript.lsr").arg(rootDir);
  if (QFile::exists(path))
  	QFile::remove(path);
  		
	okButton->setEnabled(true);
	
	if (networkReply->error())
	{
		networkReply->deleteLater();
		networkReply = 0;
		reply();
		return ;
	}
	else
	{
		networkReply->deleteLater();
		networkReply = 0;
	}
}

void XWTrayDialog::checkFinished()
{
	if (networkReply->error())
	{
		networkReply->deleteLater();
		networkReply = 0;
		progressLabel->setText(tr("Network error occur when checking updates."));
		okButton->setEnabled(true);
		return ;
	}
	
	networkReply->deleteLater();
	networkReply = 0;
	createUpdateItems();
	if (total > 0)
	{
		QString n = QString(tr("There %1 updates. Updates are starting, don't interrupt.")).arg(total);
		totalLabel->setText(n);
		download();
	}
}

void XWTrayDialog::checkForUpdate()
{
	if (output)
	{
		output->close();
		delete output;
		output = 0;
	}
	
	okButton->setEnabled(false);
	progressLabel->setText(tr("Checking updates in http://www.xwtex.com."));
	
	QString path = QString("%1/updating.xml").arg(tmpDir);
	output = new QFile(path);
	output->open(QIODevice::WriteOnly | QIODevice::Text);
		
	QUrl url("http://www.xwtex.com/update/check.php");
	url.addQueryItem("HardID", hardId);
	url.addQueryItem("Local", lang);
	url.addQueryItem("OS", os);
	QNetworkRequest request(url);
  networkReply = manager.get(request);
  connect(networkReply, SIGNAL(finished()), this, SLOT(checkFinished()));
  connect(networkReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
}

void XWTrayDialog::cleanup()
{
	QDir tmpdir(tmpDir);
	if (tmpdir.exists())
	{
		QStringList files = tmpdir.entryList(QDir::Files);
   	for (int i = 0; i < files.size(); i++)
   	{
       	QString name = files.at(i);
       	QString filename = QString("%1/%2").arg(tmpDir).arg(name);
       	QFile::remove(filename);
   	}
   	
   	tmpdir.rmdir(tmpDir);
  }
  
  qApp->quit();
}

void XWTrayDialog::createUpdateItems()
{
	if (!output)
		return ;
		
	output->close();
	delete output;
	output = 0;
	clearUpdateItems();
	
	QString path = QString("%1/updating.xml").arg(tmpDir);
	
	QFile ufile(path);
	ufile.open(QIODevice::ReadOnly | QIODevice::Text);
	QXmlStreamReader xml(&ufile);
	bool menuchang = false;
	if (xml.readNextStartElement())
	{
		if (xml.name() == "xiuwen" && xml.attributes().value("version") == "1.0")
		{
			QString key;
			QString chk;
			while (xml.readNextStartElement())
			{
				if (xml.name() == "license")
				{
					while (xml.readNextStartElement())
					{
						if (xml.name() == "orderid")
							orderID = xml.readElementText();
						else if (xml.name() == "filename")
							key = xml.readElementText();
						else if (xml.name() == "filechk")
							chk = xml.readElementText();
						else
							xml.skipCurrentElement();	
					}
				}
				else if (xml.name() == "software")
				{
					QString id;
					QString package;
					QString directory;
					QString description;
					QString bin;
					QString menu;
					while (xml.readNextStartElement())
					{
						if (xml.name() == "id")
							id = xml.readElementText();
						else if (xml.name() == "package")
							package = xml.readElementText();
						else if (xml.name() == "directory")
							directory = xml.readElementText();
						else if (xml.name() == "description")
							description = xml.readElementText();
						else if (xml.name() == "bin")
							bin = xml.readElementText();
						else if (xml.name() == "menu")
							menu = xml.readElementText();
						else
							xml.skipCurrentElement();
					}
					
					if (!id.isEmpty() && !package.isEmpty())
					{
						if (!updateItems.contains(id))
						{
							XWUpdateFile * f = new XWUpdateFile;
							if (f)
							{
								f->id = id;
								f->package = package;
								f->directory = directory;
								if (directory.isEmpty() || directory == "/")
									f->dest = rootDir;
								else if (directory.startsWith("/"))
									f->dest = QString("%1%2").arg(rootDir).arg(directory);
								else
									f->dest = QString("%1/%2").arg(rootDir).arg(directory);
								f->description = description;
								updateItems[id] = f;
							}
						}
					}
					
					if (!id.isEmpty() && !bin.isEmpty() && !menu.isEmpty())
					{
						setting->setValue(bin, menu);
						menuchang = true;
					}
				}
				else
					xml.skipCurrentElement();
			}
			
			if (!key.isEmpty() && !chk.isEmpty())
			{
				QString filename = QString("%1/xwtex.dat").arg(binDir);
				QFile file(filename);
				if (file.open(QIODevice::WriteOnly))
				{
					QByteArray ba = key.toAscii();
					ba += '\n';
					file.write(ba);
					ba = chk.toAscii();
					ba += '\n';
					file.write(ba);
					file.close();
				}
			}
		}
	}
	
	total = updateItems.size();	
	remnant = total;
	succ = 0;
	ufile.close();
	ufile.remove();
	if (menuchang)
		createActions();
}

void XWTrayDialog::download()
{
	QHash<QString, XWUpdateFile*>::iterator i = updateItems.begin();
  if (i != updateItems.end())
  {
  	if (output)
  	{
  		output->close();
			delete output;
			output = 0;
  	}
  	
  	XWUpdateFile * f = i.value();  	
  	QString p = QString(tr("Dowloading %1...")).arg(f->package);
  	progressLabel->setText(p);
  	descriptionEdit->setText(f->description);
  	curFile = f->id;
  	
  	QUrl url(f->package);
  	QString path = url.path();
  	QFileInfo finfo(path);
  	QString n = finfo.fileName();
  	f->fileName = QString("%1/%2").arg(tmpDir).arg(n);
  	output = new QFile(f->fileName);
  	output->open(QIODevice::WriteOnly);
  	
  	QNetworkRequest request(url);
  	networkReply = manager.get(request);
  	connect(networkReply, SIGNAL(finished()), this, SLOT(downloadFinished()));
  	connect(networkReply, SIGNAL(readyRead()), this, SLOT(readyRead()));
  	connect(networkReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgress(qint64,qint64)));
  }
}

void XWTrayDialog::downloadFinished()
{
	remnant--;
	XWUpdateFile * f = updateItems.take(curFile);
	updatedItems << f;	
	
	if (!output)
		return ;
		
	output->close();
	delete output;
	output = 0;	
	if (!networkReply)
		return ;
		
	if (networkReply->error())
	{
		QFile::remove(f->fileName);
		networkReply->deleteLater();
		networkReply = 0;
		QString p = QString(tr("Network error occur when downloading %1.")).arg(f->package);
		progressLabel->setText(p);
	}
	else
	{
		networkReply->deleteLater();
		networkReply = 0;
	
		if (updateFile(f->fileName, f->dest))
		{
			succ++;
			f->updated = true;
		}
		else
		{
			QString p = QString(tr("Error occur when installing %1.")).arg(f->package);
			progressLabel->setText(p);
		}
		QFile::remove(f->fileName);
	}
	
	QString msg = QString(tr("%1 of %2 updates are finished.")).arg(succ).arg(total);
	totalLabel->setText(msg);
	
	if (remnant > 0)
		download();
	else
		reply();
}

void XWTrayDialog::getFeature()
{
	QString path = QString("http://www.xwtex.com/feature%1.html").arg(lang);
	QUrl url(path);
	QDesktopServices::openUrl(url);
}

void XWTrayDialog::readyRead()
{
	if (output)
     output->write(networkReply->readAll());
}

void XWTrayDialog::startProgram(QAction * a)
{
	if (!a)
		return ;
		
	QString k = a->data().toString();	
	if (k.isEmpty())
		return ;
				
	QString pp = QString("%2").arg(k);
	QStringList arglist;
	QProcess::startDetached(pp, arglist);
}

void XWTrayDialog::updateProgress(qint64 bytesReceived,qint64 bytesTotal)
{
	progressBar->setMaximum(bytesTotal);
	progressBar->setValue(bytesReceived);
}

void XWTrayDialog::clearUpdateItems()
{
	QHash<QString, XWUpdateFile*>::iterator i = updateItems.begin();
  while (i != updateItems.end())
	{
		XWUpdateFile * f = i.value();
		if (f)
			delete f;
		i = updateItems.erase(i);
	}
	
	while (!updatedItems.isEmpty())
		delete updatedItems.takeFirst();
}

void XWTrayDialog::createActions()
{
	trayIconMenu->clear();
	if (programGrp)
		delete programGrp;
		
	programGrp = new QActionGroup(this);
	connect(programGrp, SIGNAL(triggered(QAction*)), this, SLOT(startProgram(QAction*)));
	QStringList keys = setting->allKeys();
	for (int i = 0; i < keys.size(); i++)
	{
		QString k = keys[i];
		if (!k.isEmpty())
		{
			QString v = setting->value(k).toString();
			if (!v.isEmpty())
			{
				QAction * a = new QAction(v, trayIconMenu);
				a->setData(k);
				programGrp->addAction(a);
				trayIconMenu->addAction(a);
			}
		}
	}
	
	if (keys.size() > 0)
		trayIconMenu->addSeparator();
		
	trayIconMenu->addAction(featureAction);
	trayIconMenu->addAction(updateAction);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(quitAction);
}

void XWTrayDialog::reply()
{
	QString list;
  for (int i = 0; i < updatedItems.size(); i++)
  {
  	XWUpdateFile * f = updatedItems.at(i);
  	if (f->updated)
  	{
  		list += f->id;
  		list += "A";
  	}
  }
  	
  if (!list.isEmpty())
  {
  	if (list[list.length() - 1] == QChar('A'))
  		list.remove(list.length() - 1, 1);
  			
  	QString path = QString("http://www.xwtex.com/update/updated.php");
  	QUrl url(path);
  	url.addQueryItem("HardID", hardId);
  	url.addQueryItem("IDs", list);
  	url.addQueryItem("OrderID", orderID);
  	if (total > 0 && total == succ)
  		url.addQueryItem("Finished", "1");
  	else
  		url.addQueryItem("Finished", "0");
  	QNetworkRequest request(url);
  	networkReply = manager.get(request);
  	connect(networkReply, SIGNAL(finished()), this, SLOT(allFinished()));
  }
}

bool XWTrayDialog::updateFile(const QString & p, const QString & d)
{
	QString msg = QString(tr("Install %1 to %2...")).arg(p).arg(d);
	progressLabel->setText(msg);
	
	QDir dir(d);
	if (!dir.exists(d))
	{
		if (!dir.mkpath(d))
			return false;
	}
	
	QString program = QString("7z x %1 -o%2 -r -aoa").arg(p).arg(d);
	int bb = QProcess::execute(program);
	bool b = bb < 0 ? false : true;
	return b;
}
