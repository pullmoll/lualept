require "lua/tools"

local image1 = images .. '/lobbyismus.jpg'

local pix = Pix(image1)

local pix2 = pix:Affine('white', 0.9, 0.9, 0.8, -0.5, 1.2, 0.3)
pix2:View()

local pixm = pix:MakeArbMaskFromRGB(0.8, 0.8, -1.2, 0.05)
pixm:View()
local pixs = pix:ConvertTo8()
pixs:View()

local ws = WShed(pixs, pixm)
print(pad("ws = WShed(pixs, pixm)"), ws)

local ok = ws:Apply()
print(pad("ws:Apply()"), ok)

local pix = ws:RenderFill()
pix:View()

local pix = ws:RenderColors()
pix:View()

header()
