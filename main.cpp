#include <iostream>
#include <fstream>

int main() {

    std::cout << "Please wait, this could take a while (if it's your first time running it on this machine)..." << std::endl;

    std::ofstream file;
    file.open("temp.ps1");

    std::string powershell;
    powershell = "Set-ExecutionPolicy Bypass -Force\n";
    powershell += "$nuGet = Get-PackageProvider | Where Name -match \"NuGet\"\n";
    powershell += "if (!$nuGet) { Install-PackageProvider -Name NuGet -Force }\n";
    powershell += "if (Get-Module -ListAvailable -Name NtObjectManager) {} else { Set-PSRepository PSGallery -InstallationPolicy Trusted; Install-Module -Name NtObjectManager -Confirm:$False -Force -RequiredVersion 1.1.32 }\n";
    powershell += "Import-Module NtObjectManager\n";
    powershell += "sc.exe stop TrustedInstaller\n";
    powershell += "sc.exe start TrustedInstaller\n";
    powershell += "$ti = Get-NtProcess TrustedInstaller.exe\n";
    powershell += "New-Win32Process cmd.exe -CreationFlags NewConsole -ParentProcess $ti\n";
    file << powershell << std::endl;
    file.close();

    std::cout << "Running script..." << std::endl;

    system("powershell.exe -ExecutionPolicy Bypass ./temp.ps1");

    std::cout << "Script ran!" << std::endl;

    system("TIMEOUT /T 3");

    remove("temp.ps1");

    return 0;
}
