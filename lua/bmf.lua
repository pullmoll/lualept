require "lua/tools"

header("Bmf")
local bmf = Bmf("fonts", 6);
print(pad("bmf"), bmf)
local chr = string.byte('x')
print(pad("bmf:GetWidth("..chr..")"), bmf:GetWidth(chr))
local l = {}
local h, sa = bmf:GetLineStrings("This is just a simple test to see how bmf:GetLineStrings() works out.", 240, 0)
print(pad("h, sa = bmf:GetLineStrings(...)"), h, #sa, sa)
local t = sa:TableArray()
print(pad("t = sa:TableArray()"), h, #t, t, tbl(t))

local str = "Some funny name is: Mississippi."
local pixa = Pixa(#str)
local boxa = Boxa(#str)
local d = 2
local x = 1
local y = 1
local w = 4
local h = 0
for i = 1, #str do
	local pix = bmf:GetPix(str:byte(i))
	local pw = pix:GetWidth()
	local ph = pix:GetHeight()
	local box = Box(x,y,pw,ph)
	boxa:AddBox(box)
	x = x + pw + d
	w = w + pw + d
	if ph > h then
		h = ph
	end
	pixa:AddPix(pix:ConvertTo8())
end

local pix = Pix(w, h + d, 8)
pix:SetAll()
for i = 1, #pixa do
	local pixs = pixa:GetPix(i)
	local sx,sy,sw,sh = boxa:GetBoxGeometry(i)
	pix:Rasterop(sx, sy, sw, sh, "mask", pixs, 0, 0)
end
pix = pix:AddBorder(4,0xc0)
pix:View()

header()
