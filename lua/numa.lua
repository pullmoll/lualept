require "lua/tools"

local filename = tmpdir .. "/test.numa"
header("Numa/Numaa")

local PI = 3.14159265358979323846
local E = 2.7182818284590452354

-- Create a new Numa
local na = Numa()
print(pad("na = Numa()"), na)

-- Add some numbers
na:AddNumber(PI)
na:AddNumber(E)
na:InsertNumber(2, 10.99999)
na:AddNumber(PI*PI)
na:AddNumber(E^(1/PI))

print("... after adding some numbers")
print(pad("na"), na)
print(pad("#na"), #na)
print(pad("na:GetParameters()"), na:GetParameters())

-- As array of numbers
print("na:GetFArray()", tbl(na:GetFArray()))

-- As array of integers
print("na:GetIArray()", tbl(na:GetIArray()))

-- Create a new MUMAA
local naa = Numaa(1)
print(pad("naa = Numaa(1)"), naa)

local count = 4
for i = 1, count do naa:AddNuma(na) end
print("... after " .. count .." times naa:AddNuma(na)")

print(pad("#naa"), #naa)
print(pad("naa"), naa)
print(pad("naa:GetNumberCount()"), naa:GetNumberCount())
for i = 1, count do
	print(pad("naa:GetNumaCount("..i..")"), naa:GetNumaCount(i))
end

local ok = naa:Write(filename)
print(pad("naa:Write('" .. filename .. "')"), ok)

naa = Numaa().Read(filename)
print(pad("Numa().Read('" .. filename .. "')"), ok)

local na = naa:FlattenToNuma()
print("... after na = naa:FlattenToNuma()")
print(pad("#na"), #na)
print(pad("na"), na)
print("na:GetFArray():", tbl(na:GetFArray()))
print("na:GetIArray():", tbl(na:GetIArray()))

-- Create a Numa* from a table of floats
local na = na.FromArray({1.1, 2.22, 3.333, 4.4444, 5.55555, 6.66666})
print(pad("#na"), #na)
print(pad("na"), na)
