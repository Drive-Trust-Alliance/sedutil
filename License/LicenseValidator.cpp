//#include "StdAfx.h"
#include "LicenseValidator.h"
using namespace System::Runtime::InteropServices;

LicenseValidator::LicenseValidator(void)
{
	try
	{
		// If you are using the QLM License Wizard, you can load the product definition from the settings.xml file generated
        // by the Protect Your App Wizard.
        // To load the settings from the XML file, call the license.LoadSettings function.

		license.DefineProduct (1, "Demo", 1, 0, "DemoKey", "{24EAA3C1-3DD7-40E0-AEA3-D20AA17A6005}");
		license.StoreKeysLocation = EStoreKeysTo::ERegistry;
		license.PublicKey = "A59Jip0lt73Xig==";
		license.CommunicationEncryptionKey = "{B6163D99-F46A-4580-BB42-BF276A507A14}";
		license.DefaultWebServiceUrl = "http://quicklicensemanager.com/qlmdemov10/qlm/qlmservice.asmx";
		defaultTrialKey = "E5H90-P0800-51EGE-N8M8T-1J1I9-3H2QQF";

		isEvaluation = true;
		evaluationExpired = true;
		evaluationRemainingDays = -1;

		// To embed a trial key, set the defaultTrialKey to a trial license key
		defaultTrialKey = System::String::Empty;
	}
	catch (...)
	{
	  throw;
   }
}


LicenseValidator::~LicenseValidator(void)
{
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
bool LicenseValidator::ValidateLicenseAtStartup(System::String^ computerID, bool* needsActivation, System::String^ returnMsg)
{
    returnMsg = System::String::Empty;            
    *needsActivation = false;

	System::String^ storedActivationKey = System::String::Empty;
    System::String^ storedComputerKey = System::String::Empty;

    license.ReadKeys (storedActivationKey, storedComputerKey);

    if (!System::String::IsNullOrEmpty(storedActivationKey))
    {
        activationKey = storedActivationKey;
    }

    if (!System::String::IsNullOrEmpty(storedComputerKey))
    {
        computerKey = storedComputerKey;
    }

	if (System::String::IsNullOrEmpty(activationKey) && System::String::IsNullOrEmpty(computerKey))
    {
        activationKey = defaultTrialKey;
    }

    return ValidateLicense(activationKey, computerKey, computerID, needsActivation, returnMsg);

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
/// <returns>true if the license is OK.</returns>
bool LicenseValidator::ValidateLicense(System::String^ activationKey, System::String^ computerKey, System::String^ computerID,
											bool *needsActivation, System::String^% returnMsg)
{
    bool ret = false;
    needsActivation = false;

    System::String ^licenseKey = computerKey;

	if (System::String::IsNullOrEmpty (licenseKey))
    {
        licenseKey = activationKey;

        if (System::String::IsNullOrEmpty(licenseKey))
        {
            return false;
        }
    }

    returnMsg = license.ValidateLicenseEx(licenseKey, computerID);

    int nStatus = (int)license.GetStatus();

    if (IsTrue(nStatus, (int) ELicenseStatus::EKeyInvalid) ||
        IsTrue(nStatus, (int)ELicenseStatus::EKeyProductInvalid) ||
        IsTrue(nStatus, (int)ELicenseStatus::EKeyVersionInvalid) ||
        IsTrue(nStatus, (int)ELicenseStatus::EKeyMachineInvalid) ||
        IsTrue(nStatus, (int)ELicenseStatus::EKeyTampered))
    {
        // the key is invalid
        ret = false;
    }
    else if (IsTrue(nStatus, (int)ELicenseStatus::EKeyDemo))
    {
        isEvaluation = true;

        if (IsTrue(nStatus, (int)ELicenseStatus::EKeyExpired))
        {
            // the key has expired
            ret = false;
            evaluationExpired = true;
        }
        else
        {
            // the demo key is still valid
            ret = true;
            evaluationRemainingDays = license.DaysLeft;
        }
    }
    else if (IsTrue(nStatus, (int)ELicenseStatus::EKeyPermanent))
    {
        // the key is OK                
        ret = true;
    }
            
    if (ret == true)
    {

        if (license.LicenseType == ELicenseType::Activation)
        {
            *needsActivation = true;
            ret = false;
        }       
    }

    return ret;

}



/// <summary>
/// Deletes the license keys stored on the computer. 
/// </summary>
void LicenseValidator::DeleteKeys()
{
	license.DeleteKeys();
}


System::String^ LicenseValidator::getfeaturestr()
{
	return license.GetFeaturesAsString();
}


/// <summary>
/// Returns the registered activation key
/// </summary>
System::String^ LicenseValidator::GetActivationKey ()
{
	return activationKey;
}

/// <summary>
/// Returns the registered computer key
/// </summary>
System::String^ LicenseValidator::GetComputerKey ()
{
	return computerKey;
}

/// <summary>
/// Returns whether the license is an eval license
/// </summary>
bool LicenseValidator::IsEvaluation ()
{
	return isEvaluation;
}

/// <summary>
/// Returns whether the license has expired
/// </summary>
bool LicenseValidator::HasEvaluationExpired()
{
	return evaluationExpired;
}

/// <summary>
/// Returns the number of remaining days for the evaluation
/// </summary>
int LicenseValidator::GetEvaluationRemainingDays()
{
	return evaluationRemainingDays;
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

