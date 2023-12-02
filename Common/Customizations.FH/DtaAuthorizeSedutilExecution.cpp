/* C:B**************************************************************************
This software is  Copyright 2023 Fidelity Height LLC  <fidelityheight.com>

This file is part of sedutil.

sedutil is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sedutil is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sedutil.  If not, see <http://www.gnu.org/licenses/>.

* C:E********************************************************************** */

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#pragma warning(disable: 4224) //C2224: conversion from int to char , possible loss of data
#pragma warning(disable: 4244) //C4244: 'argument' : conversion from 'uint16_t' to 'uint8_t', possible loss of data
#pragma comment(lib, "rpcrt4.lib")  // UuidCreate - Minimum supported OS Win 2000
#endif // Windows
// this resolve error of uuid

#include <iostream>
#include "log.h"
#include "DtaHashPwd.h"
#include "DtaOptions.h"
#include "DtaLexicon.h"
#include "DtaDevGeneric.h"
#include "DtaDevOpal1.h"
#include "DtaDevOpal2.h"
#include "DtaDevEnterprise.h"
#include "Version.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include "regtry.h"
#include "uuid.h"
#define WRITE_RETRIES 3
#include "sedsize.h"
#if (!WINDOWS7)
#include "compressapi-8.1.h"
#endif
#include <fstream>
#include <..\License\Stdafx.h>
#include <..\License\LicenseValidator.h>
#include "DtaHexDump.h"
//#include <msclr\marshal_cppstd.h>
#endif
#include "DtaAuthorize.h"

using namespace std;


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

inline void logc(int argc, char * argv[])
{
	LSTATUS ls;
	regMgr m;
	ls = m.registry_readex(HKEY_LOCAL_MACHINE, "SOFTWARE\\Fidelity Height LLC\\SEDUTIL LOG", "LOGGING", REG_SZ);
	if (ls) {
		//std::cout << "registry_writeex error" << std::endl;
		return ;
	}
	if (m.get_logging()) { //logging ON
		char filename[] = "sedutil.log";
		fstream uidlFile(filename, std::fstream::in | std::fstream::out | std::fstream::app);
		string s;
		s = "- "; s.append(NowTime()); s.append(" LOG: ");
		for (int i = 0; i < argc; i++) {
			//uidlFile << argv[i];
			s.append(argv[i]); s.append(" ");
		}
		s.append("\n");
		if (uidlFile.is_open()) {
			//uidlFile << "- " << NowTime() << " LOG: " ;
			uidlFile << s.c_str();
			uidlFile.close();
		}
	}
}

#endif  // Windows-only

bool authorize_sedutil_execution(int argc, char * argv[])
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    // Log command here
    logc(argc, argv);

    //m_strFirst = _T("This sample simulates your application. Since no license key was detected, the application launched the License Activation Form at startup.\r\n\r\n1. To generate an Activation Key, launch the QLM Console.\r\n2. Click on the Manage Keys tab.\r\n3. Click on the Create button in the License Keys group\r\n4. Select the Demo product and the settings of your choice and click OK.\r\n5. Copy/Paste the generated Activation Key in the License Activation field.\r\n6. Click on the Activate button\r\n\r\nOnce the license is activated, a computer bound key is generated.\r\n\r\n8. Go back to the QLM Console and click the Search button to refresh the page.\r\n9. The Computer Key is now recorded in the DB and on the end user system. The computer key is the license key that enables the application.");
	//m_strSecond = _T("This sample uses the QLM License Wizard Control to activate a license over the internet.\r\n\r\n When the user enters an activation key and clicks the Activate button, the QLM Control validates and activates the key.\r\n\r\nIf the activation succeeds, the activation key and a new license key that is valid only on this computer are stored in a file or in the registry.\r\n\r\nThe next time the sample is launched, the keys are retrieved and validated. \r\n\r\nUse the Open License Form button to activate and view license information. An activation key can be generated using the QLM console's Manage Keys - Create button.\r\n\r\nUse the Clear License button to release a license and start over.\r\n\r\nIf you need help, please contact us at support@soraco.co.");
	LicenseValidator *m_lv;
	//Initialize COM
	CoInitialize(NULL);
	CString computerName;
	try
	{
		m_lv = new LicenseValidator();

		//long timeout = m_lv->license->GetTimeout();
		//long graceperiod = m_lv->license->GetDateTamperingGracePeriod();
		//printf("Debug : timeout=%ld graceperiod=%ld\n", timeout, graceperiod);

		m_lv->SetCustomData1(_T("C++"));
		m_lv->SetCustomData2(_T("Desktop"));
		m_lv->SetCustomData3(_T("QlmLicenseWizardVC_NoMFC"));

		computerName = m_lv->GetComputerName();
		LicenseBinding licenseBinding = LicenseBinding_ComputerName;

		bool needsActivation = false;
		CString returnMsg("");
		// bool ValidateLicenseAtStartup(CString computerID, LicenseBinding licenseBinding, bool &needsActivation, CString &returnMsg);
		if (m_lv->ValidateLicenseAtStartup(licenseBinding, needsActivation, returnMsg) == FALSE)
		{
			// no valid license

			//char url[250] = "https://fidelityheight.test.onfastspring.com/"; // new 12/1/2017
			printf("No valid license of Fidelity Lock found, please register to get demo license or buy basic/premium license\n");
			//ShellExecute(0, 0, url, 0, 0, SW_SHOWNORMAL);
			return LICENSE_NOT_ACIVATED;
			//this->LaunchLicenseWizard();
			//if (m_lv->ValidateLicenseAtStartup(licenseBinding, needsActivation, returnMsg) == FALSE)
			//{
				//FreeResources();
			//	ExitProcess(0);
			//}
			//else
			//{
			//	m_lv->WriteProductProperties(returnMsg);
			//}

		}
		else
		{
			printf("Valid Fidelity Lock License found %s %s \n", (char*) m_lv->getf2s(), argv[opts.device]);
			// printf("License will expire  in %f \n", m_lv->getexpire());

			long licmodel = m_lv->getlicmodel();
			long lictype = m_lv->getlictype();
			long feat = m_lv->getfeature();
			long nlic = m_lv->getnlic();
			long nday = m_lv->getdaylft();
			/*
			printf("License Model = %ld \n", licmodel);
			printf("License Type = %ld \n",lictype);
			printf("Features = %ld \n", feat);
			printf("Number of License = %ld \n",nlic );
			printf("Number of Day Left = %ld \n", nday);
			*/
			bool eval = m_lv->IsEvaluation();
			bool licexpired =  m_lv->EvaluationExpired();
			int rem = m_lv->EvaluationRemainingDays();
			/*
			printf("License is evaluation = %ld \n",eval );
			printf("License Evaluation Remaining Days = %d \n", rem);
			printf("License is expired = %d \n", licexpired);
			*/
			SYSTEMTIME lt;
			VariantTimeToSystemTime(m_lv->getexpire(), &lt);
			//printf("License Expire date : %d/%d/%d %d:%d:%d\n", lt.wYear, lt.wMonth,lt.wDay,lt.wHour,lt.wMinute,lt.wSecond);
			// additional lic info
			long majver = m_lv->getmajorversion();
			long minver = m_lv->getminorversion();
			_bstr_t ver = m_lv->getversion();
			long prodid = m_lv->getproductid();
			DATE relsdate = m_lv->getreleasedate();
			/*
			printf("Product Release Date = %f \n", relsdate);
			printf("Product ID = %ld \n", prodid);
			printf("License Major Version = %ld \n", majver);
			printf("License Minor Version = %ld \n", minver);
			printf("License Version = %s \n", (char *)ver);
			*/
			if (0) { // some how m_lv return empty string for the following
				_bstr_t company = m_lv->getcompany();
				printf("Company = %s\n", (char *)company);
				_bstr_t ckey = m_lv->getcomputerkey();
				printf("ComputerKey = %s\n", (char *)ckey);
				_bstr_t email = m_lv->getemail();
				printf("email = %s\n", (char *)email);
				_bstr_t fullname = m_lv->getfullname();
				printf("fullname = %s\n", (char *)fullname);

				_bstr_t latestversion = m_lv->getlatestversion();
				printf("Lateset Version = %s\n", (char *)latestversion);
				_bstr_t eligibleversion = m_lv->getemail();
				printf("Eligible Version = %s\n", (char *)eligibleversion);

				printf("Email = %s\n", (char *)m_lv->getemail());
				printf("GetFeatures = %s\n", (char *)m_lv->getfeatures());
			}

			// found valid license
			//std::string lic = msclr::interop::marshal_as<std::string>(m_lv->getfeaturestr());
			//printf("Valid Fidelity Lock License found %s \n", lic.c_str());
			//ReadProductProperties();
			//ReadProductProperty(_bstr_t("engineering"), _bstr_t("module_eng"));

		}
	}
	catch (char *error)
	{
		//MessageBox(QlmLicenseWizardDlg::s_hwndDialog, CString(error), _T("QlmLicenseWizard"), MB_OK);
		//FreeResources();
		printf("License Error : %s\n",error);
		return false;
	}

	//UpdateData(FALSE);

	/*
	LicenseValidator ^lv = gcnew  LicenseValidator;
	System::String^ computerID = "";
	bool needsActivation;
	System::String^ returnMsg;

	if (lv->ValidateLicenseAtStartup(System::Environment::MachineName, &needsActivation, returnMsg) == false)
	{
		char url[250] = "https://fidelityl.test.onfastspring.com/";
		printf("No valid license of Fidelity Lock found, please register to get demo license or buy basic/premium license\n");
		ShellExecute(0, 0, url, 0, 0, SW_SHOWNORMAL);
		return 0;
	}
	else {
		//#include <msclr\marshal_cppstd.h> // got error to have include here, move to top
		// std::string unmanaged = msclr::interop::marshal_as<std::string>(managed);
		std::string lic = msclr::interop::marshal_as<std::string>(lv->getfeaturestr());

		printf("Valid Fidelity Lock License found %s \n",lic.c_str());
		//property System::String^ Features { get; set; }
	}
	*/
#endif


	
	return true;
}
