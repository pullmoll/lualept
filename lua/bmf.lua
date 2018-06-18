require "lua/tools"

header("Bmf")
local bmf = Bmf("fonts", 10);
print(pad("bmf"), bmf)

local pix = Pix(640, 96, 8)
pix:AddGrayColormap8()
pix:SetAll()
local str = "Some funny name is: Mississippi."
local overflow = pix:AddSingleTextblock(bmf, str, 0, "add-above")
print(pad("pix:AddSingleTextblock()"), overflow)
pix:View()

local chr = string.byte('x')
print(pad("bmf:GetWidth("..chr..")"), bmf:GetWidth(chr))
local l = {}
local h, sa = bmf:GetLineStrings("This is just a simple test to see how bmf:GetLineStrings() works out.", 240, 0)
print(pad("h, sa = bmf:GetLineStrings(...)"), h, #sa, sa)
local t = sa:TableArray()
print(pad("t = sa:TableArray()"), h, #t, t, tbl(t))

header()
