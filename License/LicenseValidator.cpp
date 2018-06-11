#include "StdAfx.h"
#include "LicenseValidator.h"
#include <Shlobj.h>
#include <fstream>
#include <sstream>
using namespace std;

///
LicenseValidator::LicenseValidator(void)
{
	// Have you called CoInitialize (NULL) during your app's initialization?
	// If not, then you need to do so in order to create a COM object.
	
	try
	{
		HRESULT hr = license.CreateInstance(__uuidof(QlmLicense));

		if (FAILED(hr))
		{
			// If the QlmLicense.dll is not registered, try to use com-free registration
			// The QlmLicenseLib.dll.manifest should be included with your application to use com-free registration.
			// Note that this DLL is version specific.

			//
			// For help in determining which binaries you need to copy to your target directory
			// check out the vc10 sample located in: C:\Users\Public\Documents\Quick License Manager\Samples\qlmpro\Windows\C++\QlmLicenseWizard\vc10
			// Take a look at the Project Properties / Build Events / Pre-Build Event
			// All the files copied to the binary folder are required to be shipped with your application
			// You should replicate the same project properties for your own project
			//

			ACTCTX actCtx;

			memset((void*)&actCtx, 0, sizeof(ACTCTX));

			actCtx.cbSize = sizeof(ACTCTX);

			// Load the manifest file
			CString manifestFile = GetThisModuleFolder();

			PathAppend(manifestFile.GetBuffer(MAX_PATH), _T("QlmLicenseLib.dll.manifest"));
			manifestFile.ReleaseBuffer();

			actCtx.lpSource = manifestFile;

			HANDLE hCtx = ::CreateActCtx(&actCtx);

			if (hCtx == INVALID_HANDLE_VALUE)
			{
				// Ensure that the QlmLicenseLib.dll.manifest exists in the same folder as QlmLicenseLib.dll
				throw "The QLM license DLL is not properly registered.";

			}

			ULONG_PTR cookie;
			if (::ActivateActCtx(hCtx, &cookie) == FALSE)
			{
				throw "The QLM license DLL is not properly registered.";

			}

			hr = license.CreateInstance(__uuidof(QlmLicense));

			if (FAILED(hr))
			{
				throw "The QLM license DLL is not properly registered.";
			}
			else
			{
				// Create the License Info object
				licenseInfo.CreateInstance(__uuidof(LicenseInfo));

				// Create the Hardware object
				hardware.CreateInstance(__uuidof(QlmHardware));

				// Create the Product Properties object
				productProperties.CreateInstance(__uuidof(QlmProductProperties));

				// Create the analytics object
				analytics.CreateInstance(__uuidof(QlmAnalytics));

				// Uncomment this line if you are using floating licenses
				//floatingLicense.CreateInstance(__uuidof(QlmFloatingLicenseMgr));

				::DeactivateActCtx(0, cookie);
			}

		}
		else
		{ 
			// if we get here, the QlmLicenseLib.dll is registered using regasm.exe and we are not using the free-registration approach
			// Create the License Info object
			licenseInfo.CreateInstance(__uuidof(LicenseInfo));

			// Create the Hardware object
			hardware.CreateInstance(__uuidof(QlmHardware));

			// Create the Product Properties object
			productProperties.CreateInstance(__uuidof(QlmProductProperties));

			// Uncomment this line if you are using floating licenses
			//floatingLicense.CreateInstance(__uuidof(QlmFloatingLicenseMgr));
		}

		if (SUCCEEDED(hr))
		{
			isEvaluation = false;
			evaluationExpired = false;
			evaluationRemainingDays = -1;

			wrongProductVersion = false;
			productPropertiesFileName = "qlmLicense.xml";

			// If you are using the QLM License Wizard, you can load the product definition from the settings.xml file generated
			// by the Protect Your App Wizard.
			// If you are using the QlmLicenseWizard, you must set the StoreKeysLocation to EStoreKeysTo_ERegistry
			// To load the settings from the XML file, call the license.LoadSettings function.
			char pdt[] = { '{','2','4','E','A','A','3','C','1','-','3','D','D','7','-','4','0','E','0','-','A','E','A','3','-','D','2','0','A','A','1','7','A','6','0','0','5','}', NULL };
			char pkey[] = { 'A','5','9','J','i','p','0','l','t','7','3','X','i','g','=','=',NULL };
			char enckey[] = { '{', '3','e','3','0','0','e','9','3','-','6','b','2','1','-','4','1','a','5','-','a','a','7','a','-','7','a','0','3','d','b','c','6','a','4','4','9' ,'}' ,NULL }; // ,
			char url[] = { 'h','t','t','p','s',':','/','/','q','u','i','c','k','l','i','c','e','n','s','e','m','a','n','a','g','e','r','.','c','o','m','/','f','i','d','e','l','i','t','y','h','e','i','g','h','t','/','q','l','m','l','i','c','e','n','s','e','s','e','r','v','e','r','/','q','l','m','s','e','r','v','i','c','e','.','a','s','m','x',NULL };
			char rsakey[] = { '<','R','S','A','K','e','y','V','a','l','u','e','>','<','M','o','d','u','l','u','s','>','u','Z','s','y','s','/','3','0','c','2','w','f','P','4','y','w','q','/','P','a','x','m','z','t','c','/','+','p','8','v','C','g','M','z','x','H','p','g','u','P','3','A','M','O','t','b','U','F','M','W','C','8','R','D','h','j','d','z','Q','E','5','A','u','N','X','C','h','V','n','p','9','I','A','s','4','M','N','K','W','M','o','V','w','f','V','L','y','D','X','V','l','V','P','a','F','K','T','P','I','b','D','m','U','M','t','z','/','8','t','0','C','4','T','O','X','c','O','T','M','M','E','P','m','v','K','v','g','8','n','o','l','H','W','f','d','x','J','e','F','5','j','f','O','d','D','l','/','3','T','N','t','g','A','T','k','S','U','D','i','T','1','l','t','L','R','6','M','a','G','9','s','=','<','/','M','o','d','u','l','u','s','>','<','E','x','p','o','n','e','n','t','>','A','Q','A','B','<','/','E','x','p','o','n','e','n','t','>','<','/','R','S','A','K','e','y','V','a','l','u','e','>', NULL };

			//license->DefineProduct (1, _bstr_t("Demo"), 1, 0, _bstr_t("DemoKey"), _bstr_t("{24EAA3C1-3DD7-40E0-AEA3-D20AA17A6005}"));
			license->DefineProduct(1, _bstr_t("Demo"), 1, 0, _bstr_t("DemoKey"), _bstr_t(pdt));
			license->LicenseEngineLibrary = ELicenseEngineLibrary_DotNet;
			//license->PublicKey = _bstr_t("A59Jip0lt73Xig==");
			license->PublicKey = _bstr_t(pkey);
			//license->CommunicationEncryptionKey = _bstr_t("{3e300e93-6b21-41a5-aa7a-7a03dbc6a449}");
			license->CommunicationEncryptionKey = _bstr_t(enckey);
			//license->DefaultWebServiceUrl = _bstr_t("https://quicklicensemanager.com/fidelityheight/qlmlicenseserver/qlmservice.asmx");
			license->DefaultWebServiceUrl = _bstr_t(url);
			//rsaPublicKey = CComBSTR ("<RSAKeyValue><Modulus>uZsys/30c2wfP4ywq/Paxmztc/+p8vCgMzxHpguP3AMOtbUFMWC8RDhjdzQE5AuNXChVnp9IAs4MNKWMoVwfVLyDXVlVPaFKTPIbDmUMtz/8t0C4TOXcOTMMEPmvKvg8nolHWfdxJeF5jfOdDl/3TNtgATkSUDiT1ltLR6MaG9s=</Modulus><Exponent>AQAB</Exponent></RSAKeyValue>");
			rsaPublicKey = CComBSTR(rsakey);
			license->StoreKeysLocation = EStoreKeysTo_ERegistry;
			checkIfLicenseIsRevoked= false;
			checkIfComputerIsRegistered= false;
			reactivateSubscription= false;
			publishAnalytics= false;
			
			license->LicenseEngineLibrary = ELicenseEngineLibrary_DotNet;
			license->LimitTerminalServerInstances = VARIANT_FALSE;
		}

	}
	catch (...)
	{
		throw;
	}
}

LicenseValidator::~LicenseValidator(void)
{
	if (license != NULL)
	{
		license.Release();
	}
}

bool LicenseValidator::ValidateLicenseAtStartup(LicenseBinding licenseBinding, bool &needsActivation, CString &returnMsg)
{
	return ValidateLicenseAtStartup(_T(""), licenseBinding, needsActivation, returnMsg);
}

bool LicenseValidator::ValidateLicenseAtStartup(CString computerID, bool &needsActivation, CString &returnMsg)
{
	return ValidateLicenseAtStartup(computerID, LicenseBinding_UserDefined, needsActivation, returnMsg);
}

/// <remarks>Call ValidateLicenseAtStartup when your application is launched. 
/// If this function returns false, exit your application.
/// </remarks>
/// 
/// <summary>
/// Validates the license when the application starts up. 
/// The first time a license key is validated successfully,
/// it is stored in a hidden file on the system. 
/// When the application is restarted, this code will load the license
/// key from the hidden file and attempt to validate it again. 
/// If it validates succesfully, the function returns true.
/// If the license key is invalid, expired, etc, the function returns false.
/// </summary>
/// <param name="computerID">Unique Computer identifier</param>
/// <param name="returnMsg">Error message returned, in case of an error</param>
/// <returns>true if the license is OK.</returns>
bool LicenseValidator::ValidateLicenseAtStartup(CString computerID, LicenseBinding licenseBinding, bool &needsActivation, CString &returnMsg)
{
	returnMsg = _T("");
	needsActivation = false;

	CComBSTR storedActivationKey("");
	CComBSTR storedComputerKey("");

	license->ReadKeys(&storedActivationKey, &storedComputerKey);

	if (storedActivationKey.Length() > 0)
	{
		activationKey = storedActivationKey;
	}

	if (storedComputerKey.Length() > 0)
	{
		computerKey = storedComputerKey;
	}

	if ((activationKey.Length() == 0) && (computerKey.Length() == 0))
	{
		if (defaultTrialKey.Length() > 0)
		{
			activationKey = defaultTrialKey;
		}
		else
		{
			return false;
		}
	}

	bool ret = ValidateLicense(CString(activationKey), CString(computerKey), computerID, licenseBinding, needsActivation, returnMsg);

	if (ret == true)
	{
		// If the local license is valid, check on the server if it's valid as well.
		CString serverMessage;
		if (ValidateOnServer(computerID, serverMessage) == false)
		{
			returnMsg += serverMessage;
			return false;
		}
	}

	//
	// If a license has expired but then renewed on the server, reactivating the key will extend the client
	// with the new subscription period.
	//
	if ((wrongProductVersion || evaluationExpired) && reactivateSubscription)
	{
		ret = ReactivateKey(computerID);
	}

	return ret;

}

/// <remarks>Call this function in the dialog where the user enters the license key to validate the license.</remarks>
/// <summary>
/// Validates a license key. If you provide a computer key, the computer key is validated. 
/// Otherwise, the activation key is validated. 
/// If you are using machine bound keys (UserDefined), you can provide the computer identifier, 
/// otherwise set the computerID to an empty string.
/// </summary>
/// <param name="activationKey">Activation Key</param>
/// <param name="computerKey">Computer Key</param>
/// <param name="computerID">Unique Computer identifier</param>
/// <param name="needsActivation">Return flag indicating whether the license needs activation</param>
/// <returns>true if the license is OK.</returns>
bool LicenseValidator::ValidateLicense(CString activationKey, CString computerKey, CString &computerID, LicenseBinding licenseBinding, bool &needsActivation, CString &errorMsg)
{
	USES_CONVERSION;
	bool bret = false;
	needsActivation = false;

	isEvaluation = false;
	evaluationExpired = false;
	evaluationRemainingDays = -1;

	CString licenseKey;

	if (computerKey.GetLength() > 0)
	{
		licenseKey = computerKey;
	}
	else
	{
		licenseKey = activationKey;
		if (licenseKey.GetLength() == 0)
		{
			return false;
		}
	}

	if (licenseBinding == LicenseBinding_UserDefined)
	{
		_bstr_t bstrstrErrorMsg = license->ValidateLicenseEx(_bstr_t(licenseKey), _bstr_t(computerID));
		errorMsg = (const char*)(bstrstrErrorMsg);
	}
	else
	{
		_bstr_t bstrstrErrorMsg = license->ValidateLicenseEx3(_bstr_t(licenseKey), licenseBinding, VARIANT_FALSE, VARIANT_FALSE);
		errorMsg = (const char*)(bstrstrErrorMsg);

		computerID = license->GetComputerID(licenseBinding).GetBSTR();

		
	}

	int nStatus = license->GetStatus();


	if (IsTrue(nStatus, (int)ELicenseStatus_EKeyInvalid) ||
		IsTrue(nStatus, (int)ELicenseStatus_EKeyProductInvalid) ||
		IsTrue(nStatus, (int)ELicenseStatus_EKeyMachineInvalid) ||
		IsTrue(nStatus, (int)ELicenseStatus_EKeyExceededAllowedInstances) ||
		IsTrue(nStatus, (int)ELicenseStatus_EKeyTampered))

	{
		// the key is invalid		
		bret = false;
	}
	else if (IsTrue(nStatus, (int)ELicenseStatus_EKeyVersionInvalid))
	{
		wrongProductVersion = true;
		bret = false;
	}
	else if (IsTrue(nStatus, (int)ELicenseStatus_EKeyDemo))
	{
		isEvaluation = true;

		if (IsTrue(nStatus, (int)ELicenseStatus_EKeyExpired))
		{
			// the key has expired
			bret = false;
			evaluationExpired = true;
		}
		else
		{
			// the demo key is still valid
			bret = true;
			evaluationRemainingDays = license->DaysLeft;
		}
	}
	else if (IsTrue(nStatus, (int)ELicenseStatus_EKeyPermanent))
	{
		// the key is OK                
		bret = true;
	}

	if (bret == true)
	{
		if (license->LicenseType == ELicenseType_Activation)
		{
			bret = false;
			needsActivation = true;
		}
	}

	return bret;
}

// <summary>
/// Check on the License Server if the license key has been revoked and/or if the license is illegal, i.e. not registered on the server
/// </summary>
/// <param name="computerID"></param>
/// <param name="returnMsg"></param>
/// <returns></returns>
bool LicenseValidator::ValidateOnServer(CString computerID, CString &returnMsg)
{
	bool ret = true;

	if (!this->checkIfLicenseIsRevoked && !this->checkIfComputerIsRegistered && !publishAnalytics)
	{
		return true;
	}

	// First let's determine if the license key is expected to be found on the server
	// This is the case for keys that require activation.
	// So we will first check if the ActivationKey variable actually holds a key that requires activation
	// If we have an ActivationKey and a ComputerKey, we already called ValidateLicenseEx with the ComputerKey
	// Now we need to call it with the Activation Key

	ELicenseType licenseType = license->LicenseType;

	if ((activationKey.Length() > 0) && (computerKey.Length() > 0))
	{
		LicenseValidator *lv = new LicenseValidator();
		_bstr_t _returnMsg = lv->license->ValidateLicense(_bstr_t(activationKey));
		licenseType = lv->license->LicenseType;
	}


	if ((license->DefaultWebServiceUrl.length() == 0) ||
		(activationKey.Length() == 0) ||
		(licenseType != QlmLicenseLib::ELicenseType_Activation))

	{
		// If no URL to the web service was defined, we cannot do any validation with the server
		// If we do not have an Activation Key, we cannot check anything on the server
		return true;
	}

	DATE serverDate;
	CComBSTR bstrResponse;
	if (license->Ping(_T(""), &bstrResponse, &serverDate) == false)
	{
		// we cannot connect to the server so we cannot do any validation with the server
		return true;
	}
	/*
	if (this->publishAnalytics)
	{
		PublishAnalyticsToServer(computerID, customData1, customData2, customData3);
	}
	*/
	if (this->checkIfLicenseIsRevoked && license->IsLicenseKeyRevoked(_T(""), _bstr_t(activationKey)))
	{
		DeleteAllKeys();
		return false;
	}

	if (this->checkIfComputerIsRegistered && license->IsIllegalComputer(_T(""), _bstr_t(activationKey),
		_bstr_t(computerKey), _bstr_t(computerID), _bstr_t(GetComputerName()), "5.0.00", &bstrResponse))
	{
		ret = false;

		CComBSTR returnMsg;

		InitializeLicenseInfo();

		if (license->ParseResultsEx(_bstr_t(bstrResponse), licenseInfo, &returnMsg) == VARIANT_TRUE)
		{

		}

		// If the system is detected as illegal, it means that we found license keys on the local system but the keys are not registerd on the server.
		DeleteAllKeys();

		return false;

	}

	return ret;
}

/// <summary>
/// Delete all license keys stored in the registry or on the file system
/// </summary>

void LicenseValidator::DeleteAllKeys()
{
	// the license was revoked, we need to remove the keys on this system.
	EStoreKeysTo saveLocation = license->StoreKeysLocation;

	try
	{
		// Remove keys stored on the file system
		license->StoreKeysLocation = QlmLicenseLib::EStoreKeysTo_EFile;
		license->DeleteKeys();

		// Remove keys stored in the registry
		license->StoreKeysLocation = QlmLicenseLib::EStoreKeysTo_ERegistry;
		license->DeleteKeys();
	}
	catch (...)
	{
	}

	// Restore the previous setting
	license->StoreKeysLocation = saveLocation;
}

bool LicenseValidator::ReactivateKey(CString computerID)
{
	bool ret = false;

	CComBSTR xmlresponse;

	license->ActivateLicense(license->DefaultWebServiceUrl, _bstr_t(activationKey), _bstr_t(computerID),
		_bstr_t(computerID), _bstr_t("5.0.00"), _bstr_t(""), &xmlresponse);


	CComBSTR bstrMessage;

	InitializeLicenseInfo();

	if (license->ParseResultsEx(_bstr_t(xmlresponse), licenseInfo, &bstrMessage) == VARIANT_TRUE)
	{
		CString ckey(computerKey);
		CString liComputerKey(licenseInfo->ComputerKey.GetBSTR());

		if (ckey.Compare(liComputerKey) != 0)
		{
			_bstr_t newComputerKey = licenseInfo->ComputerKey;
			bool needsActivation = false;
			CString returnMsg;;
			ret = ValidateLicense(CString(activationKey), CString(newComputerKey.GetBSTR()), computerID, LicenseBinding_UserDefined, needsActivation, returnMsg);
			if (ret == true)
			{
				// The Computer Key has changed, update the local one
				license->StoreKeys(_bstr_t(activationKey), newComputerKey);
			}
		}
	}

	return ret;
}

/// <summary>
/// returns whether a license is an evaluation
/// </summary>
/// <returns></returns>
bool LicenseValidator::IsEvaluation()
{
	return isEvaluation;
}

/// <summary>
/// returns whether the license has expired
/// </summary>
/// <returns></returns>
bool LicenseValidator::EvaluationExpired()
{
	return evaluationExpired;
}

/// <summary>
/// returns the number of days remaining for the evaluation
/// </summary>
/// <returns></returns>
int LicenseValidator::EvaluationRemainingDays()
{
	return evaluationRemainingDays;
}

CString LicenseValidator::GetCustomData1()
{
	return customData1;
}
CString LicenseValidator::GetCustomData2()
{
	return customData2;
}
CString LicenseValidator::GetCustomData3()
{
	return customData3;
}

void LicenseValidator::SetCustomData1(CString customData)
{
	customData1 = customData;
}
void LicenseValidator::SetCustomData2(CString customData)
{
	customData2 = customData;
}
void LicenseValidator::SetCustomData3(CString customData)
{
	customData3 = customData;
}

/// <summary>
/// Compares flags
/// </summary>
/// <param name="nVal1">Value 1</param>
/// <param name="nVal2">Value 2</param>
/// <returns></returns>
bool LicenseValidator::IsTrue(int nVal1, int nVal2)
{
	if (((nVal1 & nVal2) == nVal1) || ((nVal1 & nVal2) == nVal2))
	{
		return true;
	}
	return false;
}

CString LicenseValidator::GetComputerName()
{
	DWORD dwSize = MAX_PATH;
	CString computerName;

	BOOL bOK = ::GetComputerName(computerName.GetBuffer(MAX_PATH), &dwSize);
	if (bOK) {
		computerName.ReleaseBuffer();
		computerName = computerName.MakeUpper();
	}

	return computerName;
}

void LicenseValidator::InitializeLicenseInfo()
{
	if (licenseInfo == NULL)
	{
		HRESULT hr = licenseInfo.CreateInstance(__uuidof(LicenseInfo));
		if (FAILED(hr))
		{
			throw "The QLM license DLL is not properly registered.";
		}
	}
}


CString LicenseValidator::Dirname(const CString &strFileName)
{
	CString strPath(strFileName);
	CString strBasename;

	int len = strFileName.GetLength();

	int nPos = strFileName.ReverseFind('\\');

	if (nPos != -1)
	{
		strPath = strFileName.Left(nPos);
	}

	return strPath;
}
/*
bool LicenseValidator::FileExists(const CString &strFileName)
{

// Check the return value of a function e.g. _Check_return_ ErrorCode Foo();
#define _Check_return_           _SAL2_Source_(_Check_return_, (), _Check_return_impl_)
#define _Must_inspect_result_    _SAL2_Source_(_Must_inspect_result_, (), _Must_inspect_impl_ _Check_return_impl_)
#define _ACRTIMP __declspec(dllimport)

#define _SAL2_Source_(Name, args, annotes) _SA_annotes3(SAL_name, #Name, "", "2") _Group_(annotes _SAL_nop_impl_)
The comments in this file are intended to give basic understanding of
the usage of SAL, the Microsoft Source Code Annotation Language.
For more details, please see https://go.microsoft.com/fwlink/?LinkID=242134

	_Check_return_
		_ACRTIMP int __cdecl _waccess(
			_In_z_ wchar_t const* _FileName,
			_In_   int            _AccessMode
		);

	_Check_return_
		_ACRTIMP int __cdecl _waccess(
			_In_z_ const CString* _FileName,
			_In_   int            _AccessMode
		);

	return _waccess(strFileName, 4) ? false : true;
}
*/
bool LicenseValidator::WriteProductProperties(CString &returnMsg)
{

	bool ret = false;

	try
	{
		// every time we activate a license, get the product properties frm the server
		CComBSTR bstrResponse;

		string props = license->GetProductProperties(_bstr_t(""), _bstr_t(this->activationKey), &bstrResponse);

		if (!props.empty())
		{

			// Before deserializing the data, check its signature
			// The Public Key can be found in Define Products / Encryption Keys / Non-Windows Encryption Keys

			if (productProperties->ValidateSignature(props.c_str(), _bstr_t(rsaPublicKey), &bstrResponse) == VARIANT_TRUE)
			{
				// store the license file - you may want to customize the destination folder
				TCHAR docsFolder[MAX_PATH];
				SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, 0, docsFolder);

				CString licenseFile = docsFolder + CString("\\") + productPropertiesFileName;

				ofstream fout(licenseFile);
				fout << props << endl;
				fout.close();

				ret = true;
			}

			returnMsg = bstrResponse;
		}

	}
	catch (...)
	{
		throw;
	}
	return ret;
}

IQlmProductPropertiesPtr LicenseValidator::ReadProductProperties(CString &returnMsg)
{
	try
	{
		// store the license file - you may want to customize the destination folder
		TCHAR docsFolder[MAX_PATH];
		SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, 0, docsFolder);

		CString licenseFile = docsFolder + CString("\\") + productPropertiesFileName;

		std::ifstream inFile(licenseFile); //open the input file
		if (inFile.good())
		{
			std::stringstream strStream;

			strStream << inFile.rdbuf();//read the file
			string str = strStream.str();//str holds the content of the file

			CComBSTR bstrError;
			if (productProperties->ValidateSignature(str.c_str(), _bstr_t(rsaPublicKey), &bstrError) == VARIANT_FALSE)
			{
				returnMsg = bstrError;
				return NULL;
			}
			else
			{
				returnMsg = bstrError;

				productProperties->Deserialize(str.c_str());

				// Example of retrieving a property
				//IQlmProductPropertyPtr pp = productProperties->GetProperty(_bstr_t("engineering"), _bstr_t("module_eng"));
				//OutputDebugString((LPCWSTR) (pp->PropValue.pbstrVal));

			}
		}
	}
	catch (...)
	{

	}

	return productProperties;
}

CString LicenseValidator::GetThisModuleFolder()
{
	CString thisPath = _T("");
	WCHAR path[MAX_PATH];
	HMODULE hm;
	if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		(LPWSTR)&thisPath, &hm))
	{
		GetModuleFileNameW(hm, path, sizeof(path));
		PathRemoveFileSpecW(path);
		thisPath = CStringW(path);
		if (!thisPath.IsEmpty() && thisPath.GetAt(thisPath.GetLength() - 1) != '\\')
		{
			thisPath += L"\\";
		}
	}
#if _AFXDLL || _MFC_VER
	else
	{
		GetModuleFileNameW(AfxGetApp()->m_hInstance, path, sizeof(path));
		PathRemoveFileSpecW(path);
		thisPath = CStringW(path);
		if (!thisPath.IsEmpty() && thisPath.GetAt(thisPath.GetLength() - 1) != '\\')
		{
			thisPath += L"\\";
		}
	}
#endif

	if (thisPath.GetLength() == 0)
	{
		// Get the full path of current exe file.
		TCHAR FilePath[MAX_PATH] = { 0 };
		GetModuleFileName(0, FilePath, MAX_PATH);

		// Strip the exe filename from path and get folder name.
		PathRemoveFileSpec(FilePath);

		thisPath = FilePath;

	}

	return thisPath;
}
/*
long GetFeatures ( );
long GetDaysLeft ( );
long GetNumberOfLicenses ( );



enum ELicenseModel GetLicenseModel ( );
ELicenseModel
{
ELicenseModel_none = 0,
ELicenseModel_permanent = 1,
ELicenseModel_trial = 2,
ELicenseModel_subscription = 3
};
*/


long LicenseValidator::getdaylft()
{
	return license->GetDaysLeft ( ); 
}

long LicenseValidator::getnlic()
{
	return license->GetNumberOfLicenses ( ); 
}

/*
inline enum ELicenseModel ILicenseInfo::GetLicenseModel ( ) {
enum ELicenseModel _result;
HRESULT _hr = get_LicenseModel(&_result);
if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
return _result;
}

IQlmLicensePtr license;
ILicenseInfoPtr licenseInfo;
IQlmHardwarePtr hardware;
IQlmProductPropertiesPtr productProperties;
IQlmAnalyticsPtr analytics;
*/
enum ELicenseModel LicenseValidator::getlicmodel()
{
	return licenseInfo->GetLicenseModel ( );
}

/*
enum ELicenseType GetLicenseType ( );
ELicenseType
{
ELicenseType_PermanentGeneric = 71,
ELicenseType_Generic = 71,
ELicenseType_Evaluation = 69,
ELicenseType_Activation = 65,
ELicenseType_ComputerName = 67,
ELicenseType_UserDefined = 85
};

inline enum ELicenseType IQlmLicense::GetLicenseType ( ) {
enum ELicenseType _result;
HRESULT _hr = get_LicenseType(&_result);
if (FAILED(_hr)) _com_issue_errorex(_hr, this, __uuidof(this));
return _result;
}
*/
enum ELicenseType LicenseValidator::getlictype()
{
	return license->GetLicenseType ( );
}

long LicenseValidator::getfeature()
{
	return license->GetFeatures(); //long GetFeatures ( );
}

// JERRY additional lic info

long LicenseValidator::getmajorversion()
{
	return license->GetmajorVersion(); 
}

long LicenseValidator::getminorversion()
{
	return license->GetminorVersion();
}

_bstr_t LicenseValidator::getversion()
{
	return license->GetVersion();
}

long LicenseValidator::getproductid()
{
	return license->GetProductID();
}

//_bstr_tGetProxyUser ( )
//_bstr_t GetProxyDomain ( )
//_bstr_t GetProxyHost()
//long GetProxyPort()
//VARIANT_BOOL GetUseProxyServer
//VARIANT_BOOL GetEnableRedirectUrl ( )
//VARIANT_BOOL GetEnableDRSite ( )
//_bstr_t GetRedirectorUrl()
//VARIANT_BOOL  GetEnableClientLanguageDetection ( )
//_bstr_t GetPrivateKey ( )
//_bstr_t GetPublicKey ( )
//VARIANT_BOOL GetBackwardCompatible ( )
//VARIANT_BOOL_bstr_t GetEvaluationPerUser ( ) 



//GetproductName
_bstr_t LicenseValidator::getproductname()
{
	return (_bstr_t)""; 
	return license->GetproductName(); // be careful GetproductName NOT GetProductName, lower case p NOT upper case P
}

_bstr_t LicenseValidator::getprivatekey()
{
	return license->GetPrivateKey();
}

_bstr_t  LicenseValidator::getpublickey()
{
	return license->GetPublicKey();
}
VARIANT_BOOL LicenseValidator::getbackwardcompatible()
{
	return license->GetBackwardCompatible();
}
VARIANT_BOOL LicenseValidator::getevaluationperuser()
{
	return license->GetEvaluationPerUser();
}

// licenseInfo all blank if not set, unlike license is from QLM server ?????
DATE LicenseValidator::getreleasedate()  // always return 0
{
	return licenseInfo->GetReleaseDate();
}

_bstr_t LicenseValidator::getcompany()
{
	return licenseInfo->GetCompany();
}

_bstr_t LicenseValidator::getcomputerkey() // NG
{
	return licenseInfo->GetComputerKey();
}

_bstr_t LicenseValidator::getemail()
{
	return licenseInfo->GetEmail();
}

_bstr_t LicenseValidator::getfullname() 
{
	return licenseInfo->GetFullName();
}
////////////////////////////////////////////
_bstr_t LicenseValidator::getlatestversion()
{
	return licenseInfo->GetLatestVersion();
}
_bstr_t LicenseValidator::geteligibleversion()
{
	return licenseInfo->GetEligibleVersion();
}

_bstr_t LicenseValidator::getlatestversionurl()
{
	return licenseInfo->GetLatestVersionUrl();
}
_bstr_t LicenseValidator::getlatestversionnotes()
{
	return licenseInfo->GetLatestVersionNotes();
}

_bstr_t LicenseValidator::getserverdate()
{
	return licenseInfo->GetServerDate();
}

enum ReturnStatus LicenseValidator::getstatus()
{
	return licenseInfo->GetStatus();
}
_bstr_t LicenseValidator::getregisteredcomputerkey()
{
	return licenseInfo->GetRegisteredComputerKey();
}

_bstr_t LicenseValidator::getregisteredcomputername()
{
	return licenseInfo->GetRegisteredComputerName();
}

_bstr_t LicenseValidator::getregisteredcomputerid()
{
	return licenseInfo->GetRegisteredComputerID();
}

DATE LicenseValidator::getactivationdate()
{
	return licenseInfo->GetActivationDate();
}

_bstr_t LicenseValidator::getvendorid()
{
	return licenseInfo->GetVendorID();
}

//
_bstr_t LicenseValidator::getfeatures()
{
	return licenseInfo->GetFeatures();
}
/*
_bstr_t LicenseValidator::getvendorid()
{
	return licenseInfo->GetVendorID();
}
_bstr_t LicenseValidator::getvendorid()
{
	return licenseInfo->GetVendorID();
}
_bstr_t LicenseValidator::getvendorid()
{
	return licenseInfo->GetVendorID();
}
_bstr_t LicenseValidator::getvendorid()
{
	return licenseInfo->GetVendorID();
}
*/

//  JERRY 

_bstr_t LicenseValidator::getf2s()
{
	return license->ConvertFeaturesToString(license->GetFeatures_2());
}

DATE LicenseValidator::getexpire()
{
	return license->GetExpiryDate();
}

/*
void LicenseValidator::PublishAnalyticsToServer(CString computerID, CString customData1, CString customData2, CString customData3)
{
	if (this->publishAnalytics)
	{
		try
		{
			analytics->QlmLicenseObject = license;

			CComBSTR errorMsg;

			_bstr_t installID = analytics->ReadInstallID(&errorMsg);
			_bstr_t swVersion = ""; // GetProductVersion();

			if (installID.length() == 0)
			{

				CComBSTR bstrInstallID;

				if (analytics->AddInstallEx(swVersion, analytics->GetOperatingSystem(), _bstr_t(GetComputerName()), _bstr_t(computerID),
					_bstr_t(activationKey), _bstr_t(computerKey), IsEvaluation(), license->productName, license->majorVersion, license->minorVersion,
					_bstr_t(customData1), _bstr_t(customData2), _bstr_t(customData3), &bstrInstallID) == VARIANT_TRUE)
				{
					analytics->WriteInstallID(_bstr_t(bstrInstallID), &errorMsg);
				}
			}
			else
			{
				analytics->UpdateInstallEx(installID, swVersion, analytics->GetOperatingSystem(), _bstr_t(GetComputerName()), _bstr_t(computerID),
					_bstr_t(activationKey), _bstr_t(computerKey), IsEvaluation(), license->productName, license->majorVersion, license->minorVersion,
					_bstr_t(customData1), _bstr_t(customData2), _bstr_t(customData3));
			}

		}
		catch (...)
		{

		}
	}
}
*/

/*
void LicenseValidator::UnpublishAnalyticsToServer()
{
	if (this->publishAnalytics)
	{
		try
		{
			analytics->QlmLicenseObject = license;

			CComBSTR errorMsg;

			_bstr_t installID = analytics->ReadInstallID(&errorMsg);

			if (installID.length() != 0)
			{
				if (analytics->RemoveInstall(installID, &errorMsg))
				{
				}
			}
		}
		catch (...)
		{

		}
	}
}
*/
/*
CString LicenseValidator::GetProductVersion()
{
	CString version;

	try
	{
		_TCHAR szModPath[MAX_PATH];
		szModPath[0] = '\0';
		GetModuleFileName(NULL, szModPath, sizeof(szModPath));
		DWORD dwHandle;
		DWORD dwSize = GetFileVersionInfoSize(szModPath, &dwHandle);

		if (dwSize > 0)
		{
			BYTE* pVersionInfo = new BYTE[dwSize];
			if (GetFileVersionInfo(szModPath, dwHandle, dwSize, pVersionInfo))
			{
				UINT uiSize;
				VS_FIXEDFILEINFO* pFileInfo;
				if (VerQueryValue(pVersionInfo, _T("\\"), (LPVOID*)&pFileInfo, &uiSize))
				{
					int major = (pFileInfo->dwFileVersionMS >> 16) & 0xffff;
					int minor = (pFileInfo->dwFileVersionMS) & 0xffff;

					version.Format(_T("%d.%d"), major, minor);
				}
			}

			delete[] pVersionInfo;
		}
	}
	catch (...)
	{
	}

	return version;
}
*/