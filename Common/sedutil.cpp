/* C:B**************************************************************************
This software is Copyright 2014-2016 Bright Plaza Inc. <drivetrust@drivetrust.com>

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

#include <iostream>
#include "os.h"
#include "DtaHashPwd.h"
#include "DtaOptions.h"
#include "DtaLexicon.h"
#include "DtaDevGeneric.h"
#include "DtaDevOpal1.h"
#include "DtaDevOpal2.h"
#include "DtaDevEnterprise.h"
#include "Version.h"
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include "sedsize.h" 
#include "compressapi-8.1.h"
#include <iostream>
#include <fstream>
#include <Stdafx.h>
#include <LicenseValidator.h>
#include "DtaHexDump.h"
//#include <msclr\marshal_cppstd.h>
#endif

using namespace std;

int diskScan(char * devskip)
{
	char devname[25];
	int i = 0;
	int j = 0;
	#ifdef DEVICEMASKN
	bool f_sda_end = FALSE;
	bool f_nvme_first = FALSE;
	#endif
	DtaDev * d;
	LOG(D1) << "Creating diskList";
	printf("\nScanning for Opal compliant disks\n");
	
	while (TRUE) {
		DEVICEMASK;
		if (!strcasecmp(devname,devskip)) 
		{
			LOG(D1) << "Find skipped device " << devskip;
			i += 1; 
			DEVICEMASK;
		}
		#ifdef DEVICEMASKN
		if (f_sda_end )
			DEVICEMASKN;
		#endif
		//snprintf(devname,23,"/dev/nvme%i",i); //Linux nvme
		//snprintf(devname,23,"/dev/sd%c",(char) 0x61+i) Linux
		//sprintf_s(devname, 23, "\\\\.\\PhysicalDrive%i", i)  Windows
		d = new DtaDevGeneric(devname);
		if (d->isPresent()) {
			printf("%s", devname);
			if (d->isAnySSC())
				printf(" %s%s%s ", (d->isOpal1() ? "1" : " "),
				(d->isOpal2() ? "2" : " "), (d->isEprise() ? "E" : " "));
			else
				printf("%s", " No  ");
			cout << d->getModelNum() << ":" << d->getFirmwareRev() << ":" << d->getSerialNum() << std::endl;
			if (MAX_DISKS == (i+j)) {
				LOG(I) << MAX_DISKS << " disks, really?";
				delete d;
				return 1;
			}
		}
		else {
			#ifdef DEVICEMASKN
			if (!f_sda_end) { f_sda_end = TRUE; f_nvme_first = TRUE;}
			else 
			#endif
				break;
		}
		delete d;
		#ifdef DEVICEMASKN
		if (f_sda_end) {
			if (f_nvme_first) { j = 0; f_nvme_first = FALSE;}
			else	j += 1;
		}
		else 
		#endif
			i += 1;
	}
	delete d;
	printf("No more disks present ending scan\n");
	return 0;
}

int isValidSEDDisk(char *devname)
{
	DtaDev * d;
	d = new DtaDevGeneric(devname);
	if (d->isPresent()) {
		printf("%s", devname);
		if (d->isAnySSC())
			printf(" SED %s%s%s ", (d->isOpal1() ? "1" : "-"),
			(d->isOpal2() ? "2" : "-"), (d->isEprise() ? "E" : "-"));
		else
			printf("%s", " NO --- ");
		cout << d->getModelNum() << ":" << d->getFirmwareRev();
		cout << std::endl;
	}
	delete d;
	return 0;
}


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
int createvol(HANDLE &vol_handle, char * USBname)
{
	vol_handle = CreateFile(USBname, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING | FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_WRITE_THROUGH, // add write through
		NULL);

	if (vol_handle == INVALID_HANDLE_VALUE)
	{
		IFLOG(D1) printf("CreateFile error \n");
		return false;
	}
	else {
		IFLOG(D1) printf("CreateFile(%s,..) OK , vol_handle = %zd\n", USBname, (int64_t)vol_handle);
	}
	return true;
}

int lockvol( HANDLE &vol_handle)
{
	DWORD   n;
	if (!DeviceIoControl(vol_handle, FSCTL_DISMOUNT_VOLUME,
		NULL, 0, NULL, 0, &n, NULL))
	{
		DWORD err = GetLastError();
		IFLOG(D1) printf("FSCTL_DISMOUNT_VOLUME error %d\n", err);
		return false;
	}
	else {
		IFLOG(D1) printf("DeviceIoControl(vol_handle, FSCTL_DISMOUNT_VOLUME...) OK\n");
	}

	// lock volume
	DWORD status;
	if (!DeviceIoControl(vol_handle, FSCTL_LOCK_VOLUME,
		NULL, 0, NULL, 0, &status, NULL))
	{
		// error handling; not sure if retrying is useful
		// lock vol fail ; probably ok if there is no file system exist
		IFLOG(D1) printf("DeviceIoControl(vol_handle, FSCTL_LOCK_VOLUME,...) error\n");
		IFLOG(D1) printf("error = %d \n", GetLastError());
	}
	else {
		IFLOG(D1) printf("DeviceIoControl(vol_handle, FSCTL_LOCK_VOLUME,...) OK\n");
	}
	return true;
}

// unlock FSCTL_UNLOCK_VOLUME
// lock volume
int unlockvol(HANDLE &vol_handle)
{
	DWORD status;
	if (!DeviceIoControl(vol_handle, FSCTL_UNLOCK_VOLUME,
		NULL, 0, NULL, 0, &status, NULL))
	{
		// error handling; not sure if retrying is useful
		// lock vol fail ; probably ok if there is no file system exist
		IFLOG(D1) printf("DeviceIoControl(vol_handle, FSCTL_UNLOCK_VOLUME,...) error\n");
		IFLOG(D1) printf("error = %d \n", GetLastError());
	}
	else {
		IFLOG(D1) printf("DeviceIoControl(vol_handle, FSCTL_UNLOCK_VOLUME,...) OK\n");
	}
	return true;
}

int setfp(HANDLE &vol_handle,long sect)
{
	long    hipart = sect >> (32 - 9);
	long    lopart = sect << 9;
	long    err;

	SetLastError(0);       // needed before SetFilePointer post err detection
	lopart = SetFilePointer(vol_handle, lopart, &hipart, FILE_BEGIN);

	if (lopart == -1 && NO_ERROR != (err = GetLastError()))
	{
		//errormsg("HWWrite: error %d seeking drive %x sector %ld:  %s",
		//	err, drive, sect, w32errtxt(err));
		IFLOG(D1) printf("SetFilePointe error %d\n", err);
		return false;
	}
	else {
		IFLOG(D1) printf("SetFilePointer(vol_handle, lopart, &hipart, FILE_BEGIN) OK\n");
		return true;
	}

}
int reloadvol(HANDLE &vol_handle)
{
	DWORD n;
	DeviceIoControl(vol_handle, IOCTL_STORAGE_EJECT_MEDIA, NULL, 0, NULL, 0, &n, NULL);
	unlockvol(vol_handle);
	DeviceIoControl(vol_handle, IOCTL_STORAGE_LOAD_MEDIA, NULL, 0, NULL, 0, &n, NULL);
	FlushFileBuffers(vol_handle);
	return 0;
}


// retry zero out error
BOOL zeromem(uint64_t DecompressedBufferSize, char * USBname)
{
	HANDLE vol_handle;
	int status;
	DWORD   n;
	long sect;

	vol_handle = INVALID_HANDLE_VALUE;
	status = createvol(vol_handle, USBname);
	if (!status)
	{
		return false;
	}
	reloadvol(vol_handle);
	status = lockvol(vol_handle);

	sect = 0;
	status = setfp(vol_handle, sect);
	if (!status)
	{
		unlockvol(vol_handle);
		CloseHandle(vol_handle);
		return false;
	}

	PBYTE Bufferzero = (PBYTE)malloc(DecompressedBufferSize);
	if (!Bufferzero)
	{
		LOG(E) << "Cannot allocate memory for Bufferzero";
		unlockvol(vol_handle);
		CloseHandle(vol_handle);
		return false;
	}

	memset(Bufferzero, 0, DecompressedBufferSize);
	LOG(D1) << "zero out image area";
	if (!WriteFile(vol_handle, Bufferzero, (DWORD)DecompressedBufferSize, &n, NULL))
	{
		int err = GetLastError();
		IFLOG(D1) printf("zero out image error %d\n", err);
		unlockvol(vol_handle);
		CloseHandle(vol_handle);
		return false;
	}
	LOG(D1) << "zero out image area OK";
	IFLOG(D1) printf("close vol_handle %zd\n", (int64_t)vol_handle);
	reloadvol(vol_handle);
	unlockvol(vol_handle);
	CloseHandle(vol_handle);
	if (Bufferzero != NULL) free(Bufferzero);
	//Sleep(1000);
	return true;
}
#endif


int diskUSBwrite(char *devname, char * USBname)
{
#if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
	LOG(D1) << "createUSB() isn't supported in Linux";
	return 0;
#endif
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
	HANDLE vol_handle;
	int status;
	long sect;
	DWORD n;
	DtaDev * d, * u;
	d = new DtaDevGeneric(devname);
	if (d->isPresent() && d->isAnySSC()) 
		printf("Find Opal Drive %s\n", devname);
	else {
		printf("No Opal Drive %s\n", devname);
		delete d;
		return DTAERROR_CREATE_USB;
	}

	u = new DtaDevGeneric(USBname);
	if (u->isPresent())
		printf("Find USB drive %s\n", USBname);
	else {
		printf("No USB drive %s\n", USBname);
		delete u;
		return DTAERROR_CREATE_USB;
	}

	// write hashed series num to decompressed image buffer

	char * filename = "sedutil-cli.exe";
	LOG(D1) << "Entering createUSB()" << filename << " " << USBname;
	//uint8_t lastRC;
	uint64_t fivepercent = 0;
	uint64_t imgsize;
	int complete = 4;
	ifstream pbafile;
	// for decompression
	PBYTE DecompressedBuffer = NULL;
	uint64_t DecompressedBufferSize = NULL;
	PBYTE CompressedBuffer = NULL;
	uint64_t CompressedBufferSize = 0;
	DECOMPRESSOR_HANDLE Decompressor = NULL;
	DecompressedBuffer = NULL;
	BOOL Success;
	SIZE_T  DecompressedDataSize;
	void * somebuf;

	vector <uint8_t> buffer; // 0 buffer  (57344, 0x00),

		const char * fname[] = { "sedutil-cli.exe" }; // , "..\\rc\\sedutil.exe", "..\\rc\\UEFI.img"	};
		pbafile.open(fname[0], ios::in | ios::binary);
		pbafile.seekg(0, pbafile.end);
		imgsize = pbafile.tellg();
		pbafile.seekg(0, pbafile.beg);
		pbafile.seekg(sedsize);
		//LOG(I) << "read pointer=" << pbafile.tellg();
		int comprss = 1;

		if (comprss) {
			CompressedBufferSize = imgsize - sedsize;
			CompressedBuffer = (PBYTE)malloc(CompressedBufferSize);
			if (!CompressedBuffer)
			{
				LOG(E) << "Cannot allocate memory for compressed buffer.";
				return DTAERROR_OPEN_ERR;
			}
			pbafile.read((char *)CompressedBuffer, CompressedBufferSize); // read all img data

			Success = CreateDecompressor(
				COMPRESS_ALGORITHM_XPRESS_HUFF, //  Compression Algorithm
				NULL,                           //  Optional allocation routine
				&Decompressor);                 //  Handle
			if (!Success)
			{
				LOG(E) << "Cannot create a decompressor: " << GetLastError();
				goto done;
			}
			//  Query decompressed buffer size.
			Success = Decompress(
				Decompressor,                //  Compressor Handle
				CompressedBuffer,            //  Compressed data
				CompressedBufferSize,        //  Compressed data size
				NULL,                        //  Buffer set to NULL
				0,                           //  Buffer size set to 0
				&DecompressedBufferSize);    //  Decompressed Data size
											 // Allocate memory for decompressed buffer.
			if (!Success)
			{
				DWORD ErrorCode = GetLastError();
				// Note that the original size returned by the function is extracted 
				// from the buffer itself and should be treated as untrusted and tested
				// against reasonable limits.
				if (ErrorCode != ERROR_INSUFFICIENT_BUFFER)
				{
					LOG(E) << "Cannot query decompress data: " << ErrorCode;
					//printf("DecompressedBufferSize=%I64d \n", DecompressedBufferSize);
					goto done;
				}
				DecompressedBuffer = (PBYTE)malloc(DecompressedBufferSize);
				if (!DecompressedBuffer)
				{
					LOG(E) << "Cannot allocate memory for decompressed buffer";
					goto done;
				}
				somebuf = malloc(DecompressedBufferSize);
				if (!somebuf) {
					LOG(E) << "Cannot allocate memory for somebuf buffer";
					goto done;
				}
				memset(DecompressedBuffer, 0, DecompressedBufferSize);
			}
			//  Decompress data 
			Success = Decompress(
				Decompressor,               //  Decompressor handle
				CompressedBuffer,           //  Compressed data
				CompressedBufferSize,       //  Compressed data size
				DecompressedBuffer,         //  Decompressed buffer
				DecompressedBufferSize,     //  Decompressed buffer size
				&DecompressedDataSize);     //  Decompressed data size
			if (!Success)
			{
				LOG(E) << "Cannot really decompress data: " << GetLastError();
				//LOG(I) << ("DecompressedBufferSize=%I64d DecompressedDataSize=%I64d\n", DecompressedBufferSize, DecompressedDataSize);
				goto done;
			}

			/*
			printf("CompressedBuffer size: %I64d; DecompressedBufferSize:%I64d; DecompressedDataSize: %I64d\n",
			CompressedBufferSize,
			DecompressedBufferSize,
			DecompressedDataSize);
			printf("File decompressed.\n");
			*/
		done:
			// house keeping buffer and file handler
			if (Decompressor != NULL)
			{
				LOG(D1) << "free Decompressor" << endl;
				CloseDecompressor(Decompressor);
			}
			if (CompressedBuffer)
			{
				LOG(D1) << "free CompressBuffer" << endl;
				free(CompressedBuffer);
			}
		 // end cmprss
		}

		char * model = d->getModelNum();
		char * firmware = d->getFirmwareRev();
		char * sernum = d->getSerialNum();
		vector<uint8_t> hash;
		IFLOG(D1) printf("model : %s ", model);
		IFLOG(D1) printf("firmware : %s ", firmware);
		IFLOG(D1) printf("serial : %s\n", sernum);
		hash.clear();
		LOG(D1) << "start hashing";
		IFLOG(D4) DtaHashPwd(hash, sernum, d);
		LOG(D1) << "end hashing";
		IFLOG(D1) printf("hashed size = %zd\n", hash.size());
		IFLOG(D1) printf("hashed serial number is ");
		IFLOG(D1)
		for (int i = 0; i < hash.size(); i++)
		{
			printf("%02X", hash.at(i));
		}
		printf("\n");
	// try dump decompressed buffer of sector 0 , 1 
	IFLOG(D4) DtaHexDump(DecompressedBuffer+512,512);
	// write 32-byte date into buffer 
	for (int i = 2; i < hash.size(); i++)
	{
		DecompressedBuffer[512+64+i-2] = hash.at(i);
	}
	hash.clear();
	LOG(D1) << "start hashing usb";
	char usbstr[16] = "FidelityLockUSB";

	DtaHashPwd(hash,usbstr, d);
	for (int i = 2; i < hash.size(); i++)
	{
		DecompressedBuffer[512 + 96 + i - 2] = hash.at(i);
	}
	IFLOG(D4) DtaHexDump(DecompressedBuffer + 512, 512);
	// no zero write does it work ????  --> Nope, require to zero out first ,   why ?????
	vol_handle = INVALID_HANDLE_VALUE;
	status = createvol(vol_handle, USBname); 
	if (!status)
	{
		delete u;
		delete d;
		return DTAERROR_CREATE_USB;
	}
	reloadvol(vol_handle);
	CloseHandle(vol_handle);

	// zero out file system, retry once, seems to OK
	BOOL res;
	for (int cnt = 0; cnt < 2; cnt += 1)
	{
		if ((res = zeromem(DecompressedBufferSize,USBname))) //    DecompressedBufferSize))
			break;
	}
	if (!res) // zero twice fail 
	{
		delete u;
		delete d;
		return DTAERROR_CREATE_USB;
	}

	LOG(I) << "Writing PBA to USB " << USBname;
	vol_handle = INVALID_HANDLE_VALUE;
	status = createvol(vol_handle, USBname);
	if (!status)
	{
		delete u;
		delete d;
		return DTAERROR_CREATE_USB;
	}
	reloadvol(vol_handle);
	status = lockvol(vol_handle);
	sect = 0;
	status = setfp(vol_handle, sect);
	if (!status)
	{
		reloadvol(vol_handle);
		unlockvol(vol_handle);
		CloseHandle(vol_handle);
		if (DecompressedBuffer != NULL) free(DecompressedBuffer);
		delete u;
		delete d;
		return DTAERROR_CREATE_USB;
	}
	if (!WriteFile(vol_handle, DecompressedBuffer, (DWORD)DecompressedBufferSize, &n, NULL))
	{
		int err = GetLastError();
		//printf("write image data to USB error %d\n", err);
		unlockvol(vol_handle);
		CloseHandle(vol_handle);
		if (DecompressedBuffer != NULL) free(DecompressedBuffer);
		delete d;
		delete u;
		return DTAERROR_CREATE_USB;
	}
	//printf("write image data to USB %ld OK; close handle %zd\n", n, (int64_t)vol_handle);
	if (DecompressedBuffer != NULL) free(DecompressedBuffer);
	reloadvol(vol_handle);
	CloseHandle(vol_handle);
	delete d;
	delete u;
	return 0;
#endif
	return 0;
}

int main(int argc, char * argv[])
{
	string st1;
	DTA_OPTIONS opts;
	DtaDev *tempDev = NULL, *d = NULL;
	if (DtaOptions(argc, argv, &opts)) {
		return DTAERROR_COMMAND_ERROR;
	}
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
	//m_strFirst = _T("This sample simulates your application. Since no license key was detected, the application launched the License Activation Form at startup.\r\n\r\n1. To generate an Activation Key, launch the QLM Console.\r\n2. Click on the Manage Keys tab.\r\n3. Click on the Create button in the License Keys group\r\n4. Select the Demo product and the settings of your choice and click OK.\r\n5. Copy/Paste the generated Activation Key in the License Activation field.\r\n6. Click on the Activate button\r\n\r\nOnce the license is activated, a computer bound key is generated.\r\n\r\n8. Go back to the QLM Console and click the Search button to refresh the page.\r\n9. The Computer Key is now recorded in the DB and on the end user system. The computer key is the license key that enables the application.");
	//m_strSecond = _T("This sample uses the QLM License Wizard Control to activate a license over the internet.\r\n\r\n When the user enters an activation key and clicks the Activate button, the QLM Control validates and activates the key.\r\n\r\nIf the activation succeeds, the activation key and a new license key that is valid only on this computer are stored in a file or in the registry.\r\n\r\nThe next time the sample is launched, the keys are retrieved and validated. \r\n\r\nUse the Open License Form button to activate and view license information. An activation key can be generated using the QLM console's Manage Keys - Create button.\r\n\r\nUse the Clear License button to release a license and start over.\r\n\r\nIf you need help, please contact us at support@soraco.co.");
	LicenseValidator *m_lv;
	//Initialize COM
	CoInitialize(NULL);
	CString computerName;
	try
	{
		m_lv = new LicenseValidator();

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
			//char url[250] = "https://fidelityl.test.onfastspring.com/"; // old 
			char url[250] = "https://fidelityheight.test.onfastspring.com/"; // new 12/1/2017
			printf("No valid license of Fidelity Lock found, please register to get demo license or buy basic/premium license\n");
			ShellExecute(0, 0, url, 0, 0, SW_SHOWNORMAL);
			return 0;
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
			printf("Valid Fidelity Lock License found %s \n", (char*) m_lv->getf2s());
			// printf("License will expire  in %f \n", m_lv->getexpire());

			long licmodel = m_lv->getlicmodel();
			long lictype = m_lv->getlictype();
			long feat = m_lv->getfeature();
			long nlic = m_lv->getnlic();
			long nday = m_lv->getdaylft();
			printf("License Model = %ld \n", licmodel);
			printf("License Type = %ld \n",lictype);
			printf("Features = %ld \n", feat);
			printf("Number of License = %ld \n",nlic );
			printf("Number of Day Left = %ld \n", nday);
			
			bool eval = m_lv->IsEvaluation();
			bool licexpired =  m_lv->EvaluationExpired();
			int rem = m_lv->EvaluationRemainingDays();
			printf("License is evaluation = %ld \n",eval );
			printf("License Evaluation Remaining Days = %d \n", rem);
			printf("License is expired = %d \n", licexpired);

			SYSTEMTIME lt;
			VariantTimeToSystemTime(m_lv->getexpire(), &lt);
			printf("License Expire date : %d/%d/%d %d:%d:%d\n", lt.wYear, lt.wMonth,lt.wDay,lt.wHour,lt.wMinute,lt.wSecond);
			// JERRY additional lic info 
			long majver = m_lv->getmajorversion(); 
			long minver = m_lv->getminorversion(); 
			_bstr_t ver = m_lv->getversion(); 
			long prodid = m_lv->getproductid();
			DATE relsdate = m_lv->getreleasedate();
			printf("Product Release Date = %f \n", relsdate);
			printf("Product ID = %ld \n", prodid);
			printf("License Major Version = %ld \n", majver);
			printf("License Minor Version = %ld \n", minver);
			printf("License Version = %s \n", (char *)ver);

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

			// JERRY 
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
		ExitProcess(0);
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

	if ((opts.action != sedutiloption::scan) && 
		(opts.action != sedutiloption::validatePBKDF2) &&
		(opts.action != sedutiloption::version) &&
		(opts.action != sedutiloption::createUSB) &&
		(opts.action != sedutiloption::isValidSED)) {
		if (opts.device > (argc - 1)) opts.device = 0;
		tempDev = new DtaDevGeneric(argv[opts.device]);
		if (NULL == tempDev) {
			LOG(E) << "Create device object failed";
			return DTAERROR_OBJECT_CREATE_FAILED;
		}
		if ((!tempDev->isPresent()) || (!tempDev->isAnySSC())) {
			LOG(E) << "Invalid or unsupported disk " << argv[opts.device];
			delete tempDev;
			return DTAERROR_COMMAND_ERROR;
		}
		if (tempDev->isOpal2())
			d = new DtaDevOpal2(argv[opts.device]);
		else
			if (tempDev->isOpal1())
				d = new DtaDevOpal1(argv[opts.device]);
			else
				if (tempDev->isEprise())
					d = new DtaDevEnterprise(argv[opts.device]);
				else
				// isOpalite() isPyrite()
				{
					LOG(E) << "Unknown OPAL SSC ";
					return DTAERROR_INVALID_COMMAND;
				}
		delete tempDev;
		if (NULL == d) {
			LOG(E) << "Create device object failed";
			return DTAERROR_OBJECT_CREATE_FAILED;
		}
		// make sure DtaDev::no_hash_passwords is initialized
		d->no_hash_passwords = opts.no_hash_passwords;
		d->usermodeON = opts.usermode;
		d->translate_req = opts.translate_req;
	}
    switch (opts.action) {
 	case sedutiloption::initialSetup:
		LOG(D) << "Performing initial setup to use sedutil on drive " << argv[opts.device];
        return (d->initialSetup(argv[opts.password]));
	case sedutiloption::setup_SUM:
		LOG(D) << "Performing SUM setup on drive " << argv[opts.device];
		return (d->setup_SUM(opts.lockingrange, atoll(argv[opts.lrstart]),
			atoll(argv[opts.lrlength]), argv[opts.password], argv[opts.newpassword]));
		break;
	case sedutiloption::setSIDPassword:
        LOG(D) << "Performing setSIDPassword ";
        return d->setSIDPassword(argv[opts.password], argv[opts.newpassword]);
		break;
	case sedutiloption::setAdmin1Pwd:
        LOG(D) << "Performing setPAdmin1Pwd ";
        return d->setPassword(argv[opts.password], (char *) "Admin1",
                            argv[opts.newpassword]);
		break;
	case sedutiloption::auditWrite:
		LOG(D) << "audit log write";
		printf("argv[opts.eventid]=%s\n", argv[opts.eventid]);
		return d->auditWrite(argv[opts.password], argv[opts.eventid], argv[opts.userid]);
		break;
	case sedutiloption::auditRead:
		LOG(D) << "audit log read";
		return d->auditRead(argv[opts.password], argv[opts.userid]);
		break;
	case sedutiloption::auditErase:
		LOG(D) << "audit log erase ";
		return d->auditErase(argv[opts.password], argv[opts.userid]);
		break;
	case sedutiloption::getmfgstate:
		LOG(D) << "get manufacture life cycle state";
		return d->getmfgstate();
		break;
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
	case sedutiloption::pbaValid:
		LOG(D) << "PBA image validation";
		return d->pbaValid(argv[opts.password]);
		break;
	case sedutiloption::activate:
		LOG(D) << "activate LockingSP with MSID";
		return d->activate(argv[opts.password]);
		break;
	case sedutiloption::DataStoreWrite:
		LOG(D) << "Write to Data Store";
		return d->DataStoreWrite(argv[opts.password], argv[opts.pbafile], (uint8_t)atoi(argv[opts.dsnum]), atol(argv[opts.startpos]), atol(argv[opts.len]));
		break;
	case sedutiloption::DataStoreRead:
		LOG(D) << "Read Data Store to file";
		return d->DataStoreRead(argv[opts.password], argv[opts.pbafile], (uint8_t)atoi(argv[opts.dsnum]), atol(argv[opts.startpos]), atol(argv[opts.len]));
		break;
	case sedutiloption::MBRRead:
		LOG(D) << "Read shadow MBR to file";
		return d->MBRRead(argv[opts.password], argv[opts.pbafile], atol(argv[opts.startpos]), atol(argv[opts.len]));
		break;

	case sedutiloption::createUSB:
		LOG(D) << "create bootable USB drive " << argv[opts.pbafile] << " to " << opts.device;
		//return d->createUSB(argv[opts.pbafile]);
		return diskUSBwrite(argv[opts.device], argv[opts.devusb]);
		break;
    #endif

	case sedutiloption::getMBRsize:
		LOG(D) << "get shadow MBR table size ";
		return d->getMBRsize(argv[opts.password]);
		break;
	case sedutiloption::loadPBAimage:
        LOG(D) << "Loading PBA image " << argv[opts.pbafile] << " to " << opts.device;
        return d->loadPBA(argv[opts.password], argv[opts.pbafile]);
		break;
	case sedutiloption::setLockingRange:
        LOG(D) << "Setting Locking Range " << (uint16_t) opts.lockingrange << " " << (uint16_t) opts.lockingstate;
        return d->setLockingRange(opts.lockingrange, opts.lockingstate, argv[opts.password]);
		break;
	case sedutiloption::setLockingRange_SUM:
		LOG(D) << "Setting Locking Range " << (uint16_t)opts.lockingrange << " " << (uint16_t)opts.lockingstate << " in Single User Mode";
		return d->setLockingRange_SUM(opts.lockingrange, opts.lockingstate, argv[opts.password]);
		break;
	case sedutiloption::enableLockingRange:
        LOG(D) << "Enabling Locking Range " << (uint16_t) opts.lockingrange;
        return (d->configureLockingRange(opts.lockingrange,
			(DTA_READLOCKINGENABLED | DTA_WRITELOCKINGENABLED), argv[opts.password]));
        break;
	case sedutiloption::disableLockingRange:
		LOG(D) << "Disabling Locking Range " << (uint16_t) opts.lockingrange;
		return (d->configureLockingRange(opts.lockingrange, DTA_DISABLELOCKING,
			argv[opts.password]));
		break;
	case sedutiloption::readonlyLockingRange:
		LOG(D) << "Enabling Locking Range " << (uint16_t)opts.lockingrange;
		return (d->configureLockingRange(opts.lockingrange,
			DTA_WRITELOCKINGENABLED, argv[opts.password]));
		break;
	case sedutiloption::setupLockingRange:
		LOG(D) << "Setup Locking Range " << (uint16_t)opts.lockingrange;
		return (d->setupLockingRange(opts.lockingrange, atoll(argv[opts.lrstart]),
			atoll(argv[opts.lrlength]), argv[opts.password]));
		break;
	case sedutiloption::setupLockingRange_SUM:
		LOG(D) << "Setup Locking Range " << (uint16_t)opts.lockingrange << " in Single User Mode";
		return (d->setupLockingRange_SUM(opts.lockingrange, atoll(argv[opts.lrstart]),
			atoll(argv[opts.lrlength]), argv[opts.password]));
		break;
	case sedutiloption::listLockingRanges:
		LOG(D) << "List Locking Ranges ";
		return (d->listLockingRanges(argv[opts.password], -1));
		break;
	case sedutiloption::listLockingRange:
		LOG(D) << "List Locking Range[" << opts.lockingrange << "]";
		return (d->listLockingRanges(argv[opts.password], opts.lockingrange));
		break;
    case sedutiloption::rekeyLockingRange:
		LOG(D) << "Rekey Locking Range[" << opts.lockingrange << "]";
		return (d->rekeyLockingRange(opts.lockingrange, argv[opts.password]));
        break;
    case sedutiloption::setBandsEnabled:
        LOG(D) << "Set bands Enabled";
        return (d->setBandsEnabled(-1, argv[opts.password]));
        break;
    case sedutiloption::setBandEnabled:
        LOG(D) << "Set band[" << opts.lockingrange << "] enabled";
        return (d->setBandsEnabled(opts.lockingrange, argv[opts.password]));
        break;
	case sedutiloption::setMBRDone:
		LOG(D) << "Setting MBRDone " << (uint16_t)opts.mbrstate;
		return (d->setMBRDone(opts.mbrstate, argv[opts.password]));
		break;
	case sedutiloption::setMBREnable:
		LOG(D) << "Setting MBREnable " << (uint16_t)opts.mbrstate;
		return (d->setMBREnable(opts.mbrstate, argv[opts.password]));
		break;
	case sedutiloption::enableuser:
        LOG(D) << "Performing enable user for user " << argv[opts.userid];
        return d->enableUser(opts.mbrstate, argv[opts.password], argv[opts.userid]);
        break;
	case sedutiloption::enableuserread:
		LOG(D) << "Performing enable user for user " << argv[opts.userid];
		return d->enableUserRead(opts.mbrstate, argv[opts.password], argv[opts.userid]);
		break;
	case sedutiloption::activateLockingSP:
		LOG(D) << "Activating the LockingSP on" << argv[opts.device];
        return d->activateLockingSP(argv[opts.password]);
        break;
	case sedutiloption::activateLockingSP_SUM:
		LOG(D) << "Activating the LockingSP on" << argv[opts.device];
		return d->activateLockingSP_SUM(opts.lockingrange, argv[opts.password]);
		break;
	case sedutiloption::eraseLockingRange_SUM:
		LOG(D) << "Erasing LockingRange " << opts.lockingrange << " on" << argv[opts.device];
		return d->eraseLockingRange_SUM(opts.lockingrange, argv[opts.password]);
		break;
    case sedutiloption::query:
		LOG(D) << "Performing diskquery() on " << argv[opts.device];
        d->puke();
        return 0;
        break;
	case sedutiloption::scan:
        LOG(D) << "Performing diskScan(skipdevice) ";
        diskScan(argv[opts.device]);
        break;
	case sedutiloption::isValidSED:
		LOG(D) << "Verify whether " << argv[opts.device] << "is valid SED or not";
        return isValidSEDDisk(argv[opts.device]);
        break;
	case sedutiloption::takeOwnership:
		LOG(D) << "Taking Ownership of the drive at" << argv[opts.device];
        return d->takeOwnership(argv[opts.password]);
        break;
 	case sedutiloption::revertLockingSP:
		LOG(D) << "Performing revertLockingSP on " << argv[opts.device];
        return d->revertLockingSP(argv[opts.password], 0);
        break;
	case sedutiloption::setPassword:
        LOG(D) << "Performing setPassword for user " << argv[opts.userid];
        return d->setPassword(argv[opts.password], argv[opts.userid],
                              argv[opts.newpassword]);
        break;
	case sedutiloption::setPassword_SUM:
		LOG(D) << "Performing setPassword in SUM mode for user " << argv[opts.userid];
		return d->setNewPassword_SUM(argv[opts.password], argv[opts.userid],
			argv[opts.newpassword]);
		break;
	case sedutiloption::revertTPer:
		LOG(D) << "Performing revertTPer on " << argv[opts.device];
        return d->revertTPer(argv[opts.password], 0, 0);
        break;
	case sedutiloption::revertNoErase:
		LOG(D) << "Performing revertLockingSP  keep global locking range on " << argv[opts.device];
		return d->revertLockingSP(argv[opts.password], 1);
		break;
	case sedutiloption::validatePBKDF2:
        LOG(D) << "Performing PBKDF2 validation ";
        TestPBKDF2();
        break;
	case sedutiloption::yesIreallywanttoERASEALLmydatausingthePSID:
	case sedutiloption::PSIDrevert:
		LOG(D) << "Performing a PSID Revert on " << argv[opts.device] << " with password " << argv[opts.password];
        return d->revertTPer(argv[opts.password], 1, 0);
        break;
	case sedutiloption::PSIDrevertAdminSP:
		LOG(D) << "Performing a PSID RevertAdminSP on " << argv[opts.device] << " with password " << argv[opts.password];
        return d->revertTPer(argv[opts.password], 1, 1);
        break;
	case sedutiloption::eraseLockingRange:
		LOG(D) << "Erase Locking Range " << (uint16_t)opts.lockingrange;
		return (d->eraseLockingRange(opts.lockingrange, argv[opts.password]));
		break;
	case sedutiloption::objDump:
		LOG(D) << "Performing objDump " ;
		return d->objDump(argv[argc - 5], argv[argc - 4], argv[argc - 3], argv[argc - 2]);
		break;
    case sedutiloption::printDefaultPassword:
		LOG(D) << "print default password";
        d->printDefaultPassword();
        return 0;
        break;
	case sedutiloption::rawCmd:
		LOG(D) << "Performing cmdDump ";
		return d->rawCmd(argv[argc - 7], argv[argc - 6], argv[argc - 5], argv[argc - 4], argv[argc - 3], argv[argc - 2]);
		break;
	case sedutiloption::version:
		LOG(D) << "print version number ";
		st1 = "unknownOS";
        #if defined(__unix__) || defined(linux) || defined(__linux__) || defined(__gnu_linux__)
		st1 = "linux";
        #endif
        #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
		st1 = "window";
        #endif

        #if defined(APPLE) || defined(_APPLE) || defined(__APPLE__)
		st1 = "macOS";
        #endif
		
        printf("Fidelity Lock Version : 0.2.3.%s.%s 20180124-A001\n", st1.c_str(),GIT_VERSION);
		return 0;
		break;
    default:
        LOG(E) << "Unable to determine what you want to do ";
        usage();
    }
	return DTAERROR_INVALID_COMMAND;
}
