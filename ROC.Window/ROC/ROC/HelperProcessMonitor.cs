using System;
using System.Management;
using System.Diagnostics;

namespace ROC
{
	public static class HelperProcessMonitor
	{
		public static int ProcessID
		{
			get
			{
				return Process.GetCurrentProcess().Id;
			}
		}

		public static void KillOpenProcess(string name)
		{
			if (GLOBAL.ShownDM)
			{
				foreach (Process proc in Process.GetProcesses())
				{
					if (proc.ProcessName == name && proc.Id != ProcessID)
					{
						proc.Kill();
					}
				}
			}
		}

		public static bool AlreadyRunning(string name)
		{
			if (GLOBAL.ShownDM)
			{
				foreach (Process proc in Process.GetProcesses())
				{
					if (proc.ProcessName == name && proc.Id != ProcessID)
					{
						return true;
					}
				}
			}

			return false;
		}

		#region - Property Name -

		//BootDevice
		//BuildNumber
		//BuildType
		//Caption
		//CodeSet
		//CountryCode
		//CreationClassName
		//CSCreationClassName
		//CSDVersion
		//CSName
		//CurrentTimeZone
		//DataExecutionPrevention_32BitApplications
		//DataExecutionPrevention_Available
		//DataExecutionPrevention_Drivers
		//DataExecutionPrevention_SupportPolicy
		//Debug
		//Description
		//Distributed
		//EncryptionLevel
		//ForegroundApplicationBoost
		//FreePhysicalMemory
		//FreeSpaceInPagingFiles
		//FreeVirtualMemory
		//InstallDate
		//LargeSystemCache
		//LastBootUpTime
		//LocalDateTime
		//Locale
		//Manufacturer
		//MaxNumberOfProcesses
		//MaxProcessMemorySize
		//Name
		//NumberOfLicensedUsers
		//NumberOfProcesses
		//NumberOfUsers
		//Organization
		//OSLanguage
		//OSProductSuite
		//OSType
		//OtherTypeDescription
		//PAEEnabled
		//PlusProductID
		//PlusVersionNumber
		//Primary
		//ProductType
		//QuantumLength
		//QuantumType
		//RegisteredUser
		//SerialNumber
		//ServicePackMajorVersion
		//ServicePackMinorVersion
		//SizeStoredInPagingFiles
		//Status
		//SuiteMask
		//SystemDevice
		//SystemDirectory
		//SystemDrive
		//TotalSwapSpaceSize
		//TotalVirtualMemorySize
		//TotalVisibleMemorySize
		//Version
		//WindowsDirectory

		#endregion

		public static bool CheckRemoteProcess(string processName)
		{
			string subFilter = string.Format("SELECT * FROM Win32_PerfRawData_PerfProc_Process Where Name = '{0}'", processName);
			ManagementObjectCollection subProcess = GetRemoteProcess(subFilter, "pdrt-frb-mds1.ronin.roncap.com", "romserver", "RomManager!");

			if (subProcess != null && subProcess.Count > 0)
			{
				return true;
				//foreach (ManagementObject proc in subProcess)
				//{
				//    foreach (PropertyData pd in proc.Properties)
				//    {
				//        Debug.Print(string.Format("{0} - {1}", pd.Name, pd.Value));
				//    }
				//}
			}

			return false;
		}

		private static ManagementObjectCollection GetRemoteProcess(string wmiFilter, string remoteComputerName, string username, string password)
		{
			try
			{
				ConnectionOptions conn = new ConnectionOptions();
				conn.Username = username;
				conn.Password = password;

				ManagementScope scope = new ManagementScope(string.Format("\\\\{0}\\root\\cimv2", remoteComputerName), conn);
				scope.Connect();

				ObjectQuery query = new ObjectQuery(wmiFilter);
				ManagementObjectSearcher searcher = new ManagementObjectSearcher(scope, query);

				return searcher.Get();
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}

			return null;
		}
	}
}
