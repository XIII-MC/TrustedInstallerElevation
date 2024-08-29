Set-ExecutionPolicy Bypass -Force

# Check if NuGet is installed
$nuGet = Get-PackageProvider | Where Name -match "NuGet"

if (!$nuGet) {
    Install-PackageProvider -Name NuGet -Force
}

# Check if NtObjectManager is installed
if (Get-Module -ListAvailable -Name NtObjectManager) {
} else {
    Set-PSRepository PSGallery -InstallationPolicy Trusted
    Install-Module -Name NtObjectManager -Confirm:$False -Force -RequiredVersion 1.1.32
}
Import-Module NtObjectManager

sc.exe stop TrustedInstaller
sc.exe start TrustedInstaller

$ti = Get-NtProcess TrustedInstaller.exe
New-Win32Process cmd.exe -CreationFlags NewConsole -ParentProcess $ti