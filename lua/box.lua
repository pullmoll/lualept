require "lua/tools"

header("Box/Boxa/Boxaa")
-- Create a new Box
local b = Box(10,10,40,80)
print(pad("b = Box(10,10,40,80)"), b)

-- Create a new Boxa
local ba = Boxa(1)
print(pad("ba = Boxa(1)"), ba)
print(pad("#ba"), #ba)

-- Add box b using default, clone, copy
ba:AddBox(b)
ba:AddBox(b, "clone")
ba:AddBox(b)
ba:AddBox(b, 'clone')
ba:AddBox(b)
print("... after 5x ba:AddBox()")
print(pad("#ba"), #ba)
print(pad("ba"), ba)

-- Insert before the third Box in Boxa
ba:InsertBox(3, Box(3,4,49,38))
print("... after ba:InsertBox(3, Box(3,4,49,38))")
print(pad("#ba"), #ba)
print(pad("ba"), ba)

-- Get the inserted Box
local b = ba:GetBox(3)
print(pad("b = ba:GetBox(3)"), b)
local b = b:Transform(4,4)
print(pad("b = b:Transform(4,4)"), b)
local b = b:RelocateOneSide(5,'top')
print(pad("b = b:RelocateOneSide(5,'top')"), b)
local b = b:Transform(0,0,2.5,2.5)
print(pad("b = b:Transform(0,0,2.5,2.5)"), b)
local b = b & ba:GetBox(4)
print(pad("b = b & ba:GetBox(4)"), b)
local b = ba:GetBox(3) | ba:GetBox(4)
print(pad("b = ba:GetBox(3) | ba:GetBox(4)"), b)

-- Get the Box at index 4
b = ba:GetBox(4)
print(pad("b = ba:GetBox(4)"), b)

baa = Boxaa()
print(pad("baa = Boxaa()"), baa)
print(pad("#baa"), #baa)
local count = 7
for i = 1,count do baa:AddBoxa(ba) end
print("... after " .. count .. " times baa:AddBoxa(ba)")
print(pad("#baa"), #baa)
print(pad("baa"), baa)
print(pad("baa:GetBoxCount()"), baa:GetBoxCount())

local ba2 = baa:FlattenToBoxa()
print(pad("baa::FlattenToBoxa()"), ba2)

local mat1d = { 0.2, 0.0, 0.3,
		0.1, 0.8, 0.3,
		0.0, 0.5, 1.0
	}

local mat2d = { { 0.2, 0.0, 0.3 },
		{ 0.1, 0.8, 0.3 },
		{ 0.0, 0.5, 1.0 }
	}

local at1 = ba:AffineTransform(0.2, 0.0, 0.3, 0.1, 0.8, 0.3, 0.0, 0.5, 1.0)
print(pad("ba:AffineTransform(9 x float)"), at1)

local at2 = ba:AffineTransform(mat1d)
print(pad("ba:AffineTransform(mat1d)"), at2)

local at3 = ba:AffineTransform(mat2d)
print(pad("ba:AffineTransform(mat2d)"), at3)

header()
