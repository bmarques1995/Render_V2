param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)

if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    git clone --recursive https://github.com/libgit2/libgit2.git "$moduleDestination/modules/libgit2"
    cmake -S "$moduleDestination/modules/libgit2" -B "$moduleDestination/dependencies/windows/libgit2" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DBUILD_SHARED_LIBS=ON -DBUILD_STATIC_LIBS=OFF -DBUILD_OBJECT_LIBS=OFF
    cmake --build "$moduleDestination/dependencies/windows/libgit2" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}