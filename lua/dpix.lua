require "lua/tools"

local filename = tmpdir .. "/dpix-test.dpix"
local filename2 = tmpdir .. "/dpix-test.png"
local filename3 = tmpdir .. "/dpix-test.pdf"
header("DPix")
local dpix = DPix(160,100)
dpix:SetResolution(150,150)
print(pad("dpix = DPix(160,100)"), dpix)
dpix:SetAllArbitrary(160.95)
-- draw a diagonal line with random pixel values between 0 and 255.
math.randomseed(12345)
for x=0,159 do
	local y = 100*x//160
	local val = math.random() * 255
	dpix:SetPixel(x-1, y, val)
	val = math.random() * 255
	dpix:SetPixel(x, y, val)
	val = math.random() * 255
	dpix:SetPixel(x+1, y, val)
end
local ok = dpix:Write(filename)
print(pad("dpix:Write('" .. filename .."')"), ok)

local dpix2 = DPix(filename)
print(pad("dpix2 = DPix('" .. filename .. "')"), dpix2)
local dpix3 = DPix(dpix2)
print(pad("dpix3 = DPix(dpix2)"), dpix3)

local data = dpix2:GetData()
print(pad("data = dpix2:GetData()"), data)

local negvals = "clip-to-zero"
local pix = dpix2:ConvertToPix(0, negvals, true)
print(pad("pix = dpix2:ConvertToPix(0,'" .. negvals .. "', true)"), ok)
pix:Write(filename2)
print(pad("pix:Write('" .. filename2 .."')"), ok)

local data, pdf = pix:ConvertToPdf("png", 75, filename3, 0, 0, 150, "A DPix converted to Pix, then to PDF")
print(pad("pix:ConvertToPdf(...)"), data)

header()
