require "lua/tools"

header("Bmf")
local dir = "fonts";
local bmf = Bmf(dir, 10);
print(pad("bmf"), bmf)
local chr = string.byte('x')
print(pad("bmf:GetWidth("..chr..")"), bmf:GetWidth(chr))
local l = {}
local h, t = bmf:GetLineStrings("This is just a simple test to see how bmf:GetLineStrings() works out.", 240, 0)
print(pad("bmf:GetLineStrings(...)"), h, #t, t, tbl(t))

header()
