//#pragma once

using namespace QlmLicenseLib;

ref class LicenseValidator
{
public:
	LicenseValidator(void);
	virtual ~LicenseValidator(void);

	bool ValidateLicenseAtStartup(System::String^ computerID, bool* needsActivation, System::String^ returnMsg);

	bool ValidateLicense(System::String^ activationKey, System::String^ computerKey, System::String^ computerID,
							bool *needsActivation, System::String^% returnMsg);

	System::String^ GetActivationKey ();
	System::String^ GetComputerKey ();
	void DeleteKeys();

	System::String ^ getfeaturestr();

	bool IsEvaluation ();
	bool HasEvaluationExpired ();
	int GetEvaluationRemainingDays ();

	QlmLicense license;

private:
	
	System::String ^ activationKey;
	System::String^ computerKey;
	System::String^ defaultTrialKey;
	bool isEvaluation;
	bool evaluationExpired;
	int evaluationRemainingDays;

	

	bool IsTrue(int nVal1, int nVal2);
};

