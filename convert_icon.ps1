# Load System.Drawing assembly
Add-Type -AssemblyName System.Drawing

# Load the PNG image
$pngPath = "icon.png"
$icoPath = "icon.ico"

# Create a new bitmap from the PNG file
$bitmap = New-Object System.Drawing.Bitmap($pngPath)

# Create an array of icon sizes - using larger sizes for better quality
$sizes = @(32, 48, 64, 96, 128, 256)

# Create a list to store the icon images
$iconImages = New-Object System.Collections.ArrayList

foreach ($size in $sizes) {
    # Create a new bitmap with the desired size
    $resizedBitmap = New-Object System.Drawing.Bitmap($bitmap, $size, $size)
    
    # Create an icon from the resized bitmap
    $icon = [System.Drawing.Icon]::FromHandle($resizedBitmap.GetHicon())
    
    # Add to our list
    [void]$iconImages.Add($icon)
    
    # Clean up the bitmap
    $resizedBitmap.Dispose()
}

# Create a new icon file
$fileStream = New-Object System.IO.FileStream($icoPath, [System.IO.FileMode]::Create)

# Save the first icon (this will include all sizes)
$iconImages[0].Save($fileStream)
$fileStream.Close()

# Clean up
foreach ($icon in $iconImages) {
    $icon.Dispose()
}
$bitmap.Dispose()

Write-Host "Icon converted successfully to $icoPath with multiple sizes" 