param (
    [string]$texturePath,
    [string]$prefixPath,
    [bool]$genMipMap = $false,
    [bool]$isSRGB = $false
)
if (($texturePath -ne "") -and ($prefixPath -ne ""))
{
    $d3dMipArg = ""
    $d3dFormat = "R8G8B8A8_UNORM"
    $vkMipArg = ""
    $vkSrgb = "linear"
    if($genMipMap -eq $true)
    {
		$d3dMipArg = "-m 0"
		$vkMipArg = "--genmipmap --filter box"
        Write-Output "Generating mipmaps"
    }

	if($isSRGB -eq $true)
    {
        $d3dFormat = "R8G8B8A8_UNORM_SRGB"
        $vkSrgb = "srgb"
    }

    $basename = (Get-Item $texturePath ).Basename
    Write-Output $basename
    $dir = Split-Path $texturePath
    Write-Output $dir
    Start-Process -FilePath "$prefixPath/bin/texconv.exe" -ArgumentList "-f $d3dFormat $d3dMipArg $texturePath -o $dir -y"
    Start-Process -FilePath "$prefixPath/bin/toktx.exe" -ArgumentList "--encode uastc $vkMipArg $dir/$basename.ktx2 $texturePath"
}
else
{
    Write-Output "Invalid texture path or prefix path"
}
