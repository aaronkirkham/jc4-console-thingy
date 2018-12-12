if (!(Test-Path ".\premake5.exe" -PathType Leaf)) {
    (New-Object Net.WebClient).DownloadFile("https://github.com/premake/premake-core/releases/download/v5.0.0-alpha13/premake-5.0.0-alpha13-windows.zip", ".\premake.zip")
    Expand-Archive -Path ".\premake.zip" -DestinationPath "." -Force
    Remove-Item ".\premake.zip"
}

./premake5.exe --file=premake.lua vs2017