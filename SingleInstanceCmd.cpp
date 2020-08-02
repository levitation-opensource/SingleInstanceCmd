//
// Copyright (c) Roland Pihlakas 2017 - 2020
// roland@simplify.ee
//
// Roland Pihlakas licenses this file to you under the Mozilla Public License, ver 2.0.
// See the LICENSE file for more information.
//

#include "stdafx.h"

const TCHAR *SkipArg(const TCHAR *p)
{
	if (*p == '"' && *(++p))
	{
		while (*p && *p != '"')
			++p;

		if (*p)
			++p;
	}
	else
	{
		while (*p && *p != ' ')
			++p;
	}

	while (*p == ' ') ++p;

	return p;

}	//TCHAR *SkipArg(TCHAR *p)


int _tmain(int argc, const _TCHAR* argv[])
{
	if (argc < 3)
	{
		wprintf(L"Copyright (c) Roland Pihlakas 2017-2020, roland@simplify.ee");
		wprintf(L"");
		wprintf(L"\nUsage:\n");
		wprintf(L"SingleInstanceCmd.exe mutexname [-] cmd [args]\n");
		wprintf(L"- indicates that no job object must be created");
		return 1;
	}

	const _TCHAR* mutexName = argv[1];
	PROCESS_INFORMATION processInformation;
	STARTUPINFO startupInfo = { sizeof(STARTUPINFO) };

	HANDLE mutex = CreateMutex(NULL, true, mutexName);
	DWORD lastError = GetLastError();
	if (!mutex || lastError == ERROR_ALREADY_EXISTS)
	{
		wprintf(L"\nMutex exist:\n");
		CloseHandle(mutex);
		return lastError;
	}




	const TCHAR *p = GetCommandLine();

	p = SkipArg(p);		//SingleInstanceCmd.exe
	p = SkipArg(p);		//mutexname

	if (p[0] == '-' && (p[1] == ' ' || p[1] == '\t'))	//do not create job
	{
		p = SkipArg(p);
	}
	else
	{
		//assign a job object so that child processes will be killed when parent (SingleInstanceCmd.exe) is killed

		JOBOBJECT_EXTENDED_LIMIT_INFORMATION job_info;
		DWORD out_size;
		BOOL success;


		// https://msdn.microsoft.com/en-us/library/windows/desktop/ms682409(v=vs.85).aspx
		// If the object existed before the function call, the function returns a handle to the existing job object and GetLastError returns ERROR_ALREADY_EXISTS.
		HANDLE job = CreateJobObject(NULL, NULL);

		success = QueryInformationJobObject(
			job,
			JobObjectExtendedLimitInformation,
			&job_info,
			sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION),
			&out_size
		);


		job_info.BasicLimitInformation.LimitFlags |= JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

		// job_info.MaximumWorkingSetSize = size


		success = SetInformationJobObject(
			job,
			JobObjectExtendedLimitInformation,
			&job_info,
			sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION)
		);

		//A handle to the process to associate with the job object. The handle must have the PROCESS_SET_QUOTA and PROCESS_TERMINATE access rights. 
		HANDLE process = OpenProcess
		(
			/*dwDesiredAccess = */PROCESS_SET_QUOTA | PROCESS_TERMINATE,
			/*bInheritHandle = */FALSE,
			GetCurrentProcessId()
		);
		BOOL success_assign = AssignProcessToJobObject(job, process);
		success = CloseHandle(process);

	}	// if (p[0] == '-' && (p[1] == ' ' || p[1] == '\t'))	//do not create job


	if (CreateProcess(0, (TCHAR *)p, 0, 0, false, 0, 0, 0, &startupInfo, &processInformation))
	{
		DWORD exitCode;

		WaitForSingleObject(processInformation.hProcess, INFINITE);
		GetExitCodeProcess(processInformation.hProcess, &exitCode);
		CloseHandle(processInformation.hThread);
		CloseHandle(processInformation.hProcess);

		return exitCode;
	}
	else
	{
		lastError = GetLastError();

		wprintf(L"\nCreateProcess failed: error %d\n", lastError);

		CloseHandle(mutex);

		return lastError;
	}
}

