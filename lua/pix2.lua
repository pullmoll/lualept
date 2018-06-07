require "lua/tools"

local filename = tmpdir .. "/lualept.png"
local filename2 = tmpdir .. "/lualept-mask.tif"
local filename3 = tmpdir .. "/lualept-masked.png"
local image1 = images .. '/lualept.jpg'
header("Pix2")
local pix = Pix(image1)
print (pad("pix"), pix)
pix:View()

local ok = pix:Write(filename, 'png')
print (pad("pix:Write('" .. filename .. "', 'png')"), ok)

local w, h, d = pix:GetDimensions()
print (pad("dimensions"), w, h, d)
print (pad("area"), w * h)
print (pad("RGB colors"), pix:CountRGBColors())
local hist = pix:GetColorAmapHistogram(1)
print (pad("color histogram #"), #hist)

local pixm = pix:MakeArbMaskFromRGB(-0.5, -0.5, 0.75, 0.05)
pixm:View()
-- pixm:SetInputFormat('tiff-g4')
print (pad("pixm"), pixm)
print (pad("dimensions"), pixm:GetDimensions())

print (pad("pixels"), pixm:CountPixels())
local box = Box(10,w-20,h-100,100)
print (pad("rect"), box)
print (pad("pixels in rect"), pixm:CountPixelsInRect(box))
print (pad("foreground fract"), pixm:ForegroundFraction())

local ok = pixm:Write(filename2,'g4')
print (pad("pixm:Write('" .. filename2 .. "','g4')"), ok)
pix:PaintThroughMask(pixm, 0, 0, LuaLept.RGB(20,192,96))
pix:View()

local ok = pix:Write(filename3,'png')
print (pad("pix:Write('" .. filename3 .. "','png')"), ok)

header()
