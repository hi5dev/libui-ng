<#
.SYNOPSIS
Sets up a build environment for Windows.

.DESCRIPTION
This script ensures all the required packages, build tools, and libraries are
installed. It will download and install everything as necessary.
#>

# directory to download temporary files
$tempdir = 'tmp'

<#
.SYNOPSIS
Checks if a command by the given name exists.

.EXAMPLE
Confirm-Command -Name 'Get-Help'

.PARAMETER Name
Name of the command to search for.

.OUTPUTS
boolean - Confirm-Command returns $true if a command with the given name is found.
#>
function Confirm-Command
{
    param (
        [String] [Parameter (Mandatory = $true)]
        $Name
    )

    Get-Command "$Name" -ErrorAction SilentlyContinue | Out-Null

    return $?
}

<#
.SYNOPSIS
Downloads a file from the internet to the temp directory, and installs it using Add-AppxPackage.

.PARAMETER Name
Name of the command as known to the user.

# .PARAMETER Uri
URL to the app's installer.

.PARAMETER OutFile
Path to save the file - defaults to "$tempdir/$Name" if not specified.

.EXAMPLE
Add-AppxFromWeb -Name 'Hello World' -Uri 'https://example.com/hello-world' -OutFile 'tmp\HelloWorld.msix'
#>
function Add-AppxFromWeb
{
    param (
        [String] [Parameter (Mandatory = $true, Position = 0)]
        $Name,

        [String] [Parameter (Mandatory = $true, Position = 1)]
        $Uri,

        [String] [Parameter (Mandatory = $false)]
        $OutFile
    )

    if ("" -eq "$Outfile") { $Outfile = $Name }

    $OutFile = "$tempdir/$OutFile"

    $ProgressPreference = 'SilentlyContinue'

    Write-Host ">> Downloading $Name..."
    Invoke-WebRequest -Uri "$Uri" -OutFile "$tempdir/$Name"
    if (-not $?)
    {
        Write-Host ">> Failed to download $Name"
        return $false
    }

    Write-Host ">> Installing $Name..."
    Add-AppxPackage "$tempdir/$Name"
    if ($?)
    {
        Write-Host ">> Successfully installed $Name"
        return $true
    }

    Write-Host ">> Failed to install $Name"
    return $false
}

<#
.SYNOPSIS
Installs an app using winget if the given command and/or path doesn't already exist.

.PARAMETER Name
Name of the application familiar to the user (e.g. "Microsoft Paint").

.PARAMETER ID
ID of the command known to winget (e.g. "Neovim.Neovim"). This command always uses an exact match.

.PARAMETER Command
Name of a command that is installed with the app to check if it exists first (e.g. "nvim").

.PARAMETER Path
Path to a directory created by the app to check if it exists before installing (e.g. "C:\msys64").

.EXAMPLE
Add-AppxFromWinget -Name "Neovim" -ID "Neovim.Neovim" -Command "nvim"

.OUTPUTS
boolean - $true if the app was successfully installed, or if it was already installed, $false if there's an error.
#>
function Add-AppxFromWinget
{
    param (
        [String] [Parameter (Mandatory = $true, Position = 0)]
        $Name,

        [String] [Parameter (Mandatory = $true, Position = 1)]
        $ID,

        [String] [Parameter (Mandatory = $false)]
        $Command,

        [String] [Parameter (Mandatory = $false)]
        $Path
    )

    if (("" -ne "$Command") -and (Confirm-Command -Name "$Command")) { return $true }
    if (("" -ne "$Path") -and (Test-Path -PathType Container -Path "$Path")) { return $true }

    Write-Host ">> Installing $Name..."
    winget install --source 'winget' --exact --id "$ID"

    if ($?)
    {
        Write-Host ">> Successfully installed $Name"
        return $true
    }

    Write-Host ">> Failed to install $Name"
    return $false
}

<#
.SYNOPSIS
Installs winget if it isn't already installed.

.OUTPUTS
boolean - $true if winget was or was already installed, $false on error.
#>
function Install-Winget
{
    if (Confirm-Command -Name 'winget') { return $true }

    $vclibs_uri = 'https://aka.ms/Microsoft.VCLibs.x64.14.00.Desktop.appx'
    $xaml_uri = 'https://github.com/microsoft/microsoft-ui-xaml/releases/download/v2.8.6/Microsoft.UI.Xaml.2.8.x64.appx'
    $winget_uri = 'https://aka.ms/getwinget'
    $winget_out = 'Microsoft.DesktopAppInstaller_8wekyb3d8bbwe.msixbundle'

    if (-not (Add-AppxFromWeb -Name 'Microsoft.VCLibs' -Uri "$vclibs_uri")) { return $false }
    if (-not (Add-AppxFromWeb -Name 'Microsoft.UI.Xaml' -Uri "$xaml_uri")) { return $false }
    if (-not (Add-AppxFromWeb -Name 'WinGet' -Uri "$winget_uri"  -OutFile "$winget_out")) { return $false }

    return $true
}

# ensure temporary directory exists
New-Item -ItemType Directory -Force -Path $tempdir | Out-Null

if (-not (Install-Winget)) { exit(1) }

if (-not (Add-AppxFromWinget -Name 'MSYS2' -ID 'MSYS2.MSYS2' -Path 'C:\msys64')) { exit(1) }

if (-not (Add-AppxFromWinget -Name 'Doxygen' -ID 'DimitriVanHeesch.Doxygen')) { exit(1) }

C:\msys64\msys2_shell.cmd -defterm -here -no-start -mingw64 -shell bash -l -c 'pacman --noconfirm --needed -Suy'
if (-not $?) { exit(1) }

C:\msys64\msys2_shell.cmd -defterm -here -no-start -mingw64 -shell bash -l -c './setup_msys2.sh'
if (-not $?) { exit(1) }

Write-Host "Build environment ready to use."
Write-Host ""
Write-Host "Here's the command to open a new MinGW shell (you can add this to the Terminal app):"
Write-Host "  C:\msys64\msys2_shell.cmd -defterm -here -no-start -mingw64 -shell bash -l"
Write-Host ""

exit(0)
