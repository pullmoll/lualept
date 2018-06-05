require "lua/tools"

header("Bmf")
local dir = "fonts";
local bmf = Bmf(dir, 8);
print(pad("bmf"), bmf)
local chr = string.byte('x')
print(pad("bmf:GetWidth("..chr..")"), bmf:GetWidth(chr))
local l = {}
local h, t = bmf:GetLineStrings("This is just a simple test to see how bmf:GetLineStrings() works out.", 240, 0)
print(pad("bmf:GetLineStrings(...)"), h, #t, t, tbl(t))

local str = "English text here: Name."
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
	pixa:AddPix(pix)
end

local pix = Pix(w, h + d)
for i = 1, #pixa do
	local pixs = pixa:GetPix(i)
	local sx,sy,sw,sh = boxa:GetBoxGeometry(i)
	pix:Rasterop(sx, sy, sw, sh, "src&!dst", pixs, 0, 0)
end
pix:View()

header()
