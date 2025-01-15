param (
    [string]$buildMode = "Debug",
    [string]$installPrefix,
    [string]$moduleDestination
)

if (($buildMode -eq "Debug" -or $buildMode -eq "Release") -and ($installPrefix -ne "") -and ($moduleDestination -ne ""))
{
    git clone --recursive https://github.com/bmarques1995/imgui_custom_builder.git "$moduleDestination/modules/imgui_custom_builder"
    git clone --recursive https://github.com/ocornut/imgui.git "$moduleDestination/modules/imgui_custom_builder/imgui" -b docking
    cmake -S "$moduleDestination/modules/imgui_custom_builder" -B "$moduleDestination/dependencies/windows/imgui" -DCMAKE_INSTALL_PREFIX="$installPrefix" -DBUILD_SHARED_LIBS=ON
    cmake --build "$moduleDestination/dependencies/windows/imgui" --config "$buildMode" --target install
}
else
{
    Write-Output "Invalid build type or install path. Please provide either 'Debug' or 'Release', a valid prefix path and a valid Module Destination"
}