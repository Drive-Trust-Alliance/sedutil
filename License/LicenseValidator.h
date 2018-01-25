#pragma once

//*****************************************************************************
//
// You can either import the .NET 2.0/QlmLicenseLib.dll or .NET 4.0/QlmLicenseLib.dll
//
//*****************************************************************************
//#import "..\..\..\redistrib\.net 2.0\QlmLicenseLib.tlb" rename( "publicKey", "PublicKey" )
#import "C:\Program Files\Soraco\QuickLicenseMgr\redistrib\.net 4.0\QlmLicenseLib.tlb"

using namespace QlmLicenseLib;

class LicenseValidator
{
public:
	LicenseValidator(void);
	virtual ~LicenseValidator(void);

	virtual bool ValidateLicenseAtStartup(LicenseBinding licenseBinding, bool &needsActivation, CString &returnMsg);
	virtual bool ValidateLicenseAtStartup(CString computerID, bool &needsActivation, CString &returnMsg);

	virtual bool ValidateLicense(CString activationKey, CString computerKey, CString &computerID, LicenseBinding licenseBinding, bool &needsActivation, CString &errorMsg);
	virtual bool ReactivateKey(CString computerID);
	virtual bool ValidateOnServer(CString computerID, CString &returnMsg);
	virtual void DeleteAllKeys();

	CString GetComputerName();

	virtual bool IsEvaluation();
	virtual bool EvaluationExpired();
	virtual int	 EvaluationRemainingDays();

	bool WriteProductProperties(CString &returnMsg);
	IQlmProductPropertiesPtr ReadProductProperties(CString &returnMsg);

	//void PublishAnalyticsToServer(CString computerID, CString customData1, CString customData2, CString customData3);
	//void UnpublishAnalyticsToServer();

	CString GetCustomData1();
	CString GetCustomData2();
	CString GetCustomData3();

	void SetCustomData1(CString customData);
	void SetCustomData2(CString customData);
	void SetCustomData3(CString customData);

	CString GetThisModuleFolder();
	ELicenseModel getlicmodel();
	ELicenseType getlictype();
	long getdaylft();
	long getnlic();
	long getfeature();
	_bstr_t getf2s();
	DATE getexpire();
	// addition lic info
	long getmajorversion();
	long getminorversion();
	_bstr_t getversion();
	long getproductid();
	_bstr_t getproductname();
	_bstr_t getprivatekey();
	_bstr_t getpublickey();
	VARIANT_BOOL getbackwardcompatible();
	VARIANT_BOOL getevaluationperuser();



	// licenseInfo 
	DATE getreleasedate();  
	_bstr_t getcompany();
	_bstr_t getcomputerkey();
	_bstr_t getemail();
	_bstr_t getfullname();
	////////
	_bstr_t getlatestversion();
	_bstr_t geteligibleversion();
	_bstr_t getlatestversionurl();
	_bstr_t getlatestversionnotes();
	_bstr_t getserverdate();
	enum ReturnStatus getstatus();
	_bstr_t getregisteredcomputerkey();
	_bstr_t getregisteredcomputername();
	_bstr_t getregisteredcomputerid();
	DATE getactivationdate();
	_bstr_t getvendorid();
	_bstr_t getfeatures();
	
	//SAFEARRAY getfeature_2();

	IQlmLicensePtr license;
	ILicenseInfoPtr licenseInfo;
	IQlmHardwarePtr hardware;
	IQlmProductPropertiesPtr productProperties;
	IQlmAnalyticsPtr analytics;

	// Uncomment this line if you are using floating licenses
	// IQlmFloatingLicenseMgrPtr floatingLicense;

protected:
	CComBSTR activationKey;
	CComBSTR computerKey;
	CComBSTR defaultTrialKey;
	CComBSTR rsaPublicKey;

	CString productPropertiesFileName;
	CString computerID;

	bool isEvaluation;
	bool evaluationExpired;
	int evaluationRemainingDays;

	bool wrongProductVersion;

	bool checkIfLicenseIsRevoked;
	bool checkIfComputerIsRegistered;
	bool reactivateSubscription;

	bool publishAnalytics;
	CString customData1;
	CString customData2;
	CString customData3;

	void InitializeLicenseInfo();
	bool IsTrue(int nVal1, int nVal2);
	//CString GetProductVersion();

	CString Dirname(const CString &strFileName);
	//bool FileExists(const CString &strFileName);

	bool ValidateLicenseAtStartup(CString computerID, LicenseBinding licenseBinding, bool &needsActivation, CString &returnMsg);

};
