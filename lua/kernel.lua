require "lua/tools"

header("Kernel")

local kel = Kernel(5,5)
print(pad("kel = Kernel(5,5)"), kel)
kel:SetElement(0,0, 1.3)
kel:SetElement(1,1, -0.75)
kel:SetElement(1,2, -0.25)
kel:SetElement(1,3, 0.8)
kel:SetElement(2,2, 0.111)
kel:SetElement(2,4, 0.25)
print("... after setting some elements.")
print(pad("kel"), kel)
local kel = kel:Invert()
print(pad("kel = kel:Invert()"), kel)
local kel = kel:Normalize(2.0)
print(pad("kel = kel:Normalize(2.0)"), kel)

local pix = kel:DisplayInPix(4, 2)
print(pad("pix = kel:DisplayInPix(4, 2)"), pix)
pix:View()

header()
