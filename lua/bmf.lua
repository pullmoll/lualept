require "lua/tools"

header("Bmf")
local bmf = Bmf(".", 6);
print(pad("bmf"), bmf)

local pix = Pix(440, 220, 32)
pix:SetAllArbitrary("yellow")
local pix = pix:AddSingleTextblock(bmf, "Some funny name is: Mississippi.", "dark red", "add-at-top")
local pix = pix:AddSingleTextblock(bmf, "Here is bottom!", "violet", "add-at-bot")
pix:View()

local chr = string.byte('x')
print(pad("bmf:GetWidth("..chr..")"), bmf:GetWidth(chr))
local l = {}
local h, sa = bmf:GetLineStrings("This is just a simple test to see how bmf:GetLineStrings() works out.", 240, 0)
print(pad("h, sa = bmf:GetLineStrings(...)"), h, #sa, sa)
local t = sa:TableArray()
print(pad("t = sa:TableArray()"), h, #t, t, tbl(t))

header()
