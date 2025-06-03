# Load System.Drawing assembly
Add-Type -AssemblyName System.Drawing

# Load the PNG image
$pngPath = "icon.png"
$icoPath = "icon.ico"

# Create a new bitmap from the PNG file
$bitmap = New-Object System.Drawing.Bitmap($pngPath)

# Create an icon from the bitmap
$icon = [System.Drawing.Icon]::FromHandle($bitmap.GetHicon())

# Save the icon
$fileStream = New-Object System.IO.FileStream($icoPath, [System.IO.FileMode]::Create)
$icon.Save($fileStream)
$fileStream.Close()

# Clean up
$icon.Dispose()
$bitmap.Dispose()

Write-Host "Icon converted successfully to $icoPath" 