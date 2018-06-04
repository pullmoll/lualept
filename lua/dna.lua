require "lua/tools"

header("Dna/Dnaa")

local PI = 3.14159265358979323846
local E = 2.7182818284590452354

-- Create a new Dna
local da = Dna()
print(pad("da = Dna()"), da)

-- Add some numbers
da:AddNumber(PI)
da:AddNumber(E)
da:InsertNumber(2, 10.999999999999)
da:AddNumber(PI*PI)
da:AddNumber(E^(1/PI))

print("... after adding some numbers")
print(pad("#da"), #da)
print(pad("da"), da)
print(pad("da:GetParameters()"), da:GetParameters())

-- As array of numbers
print("da:GetDArray()")
tbl(da:GetDArray())

-- Create a new Dnaa
local daa = Dnaa(1)
print(pad("daa = Dnaa(1)"), daa)

local count = 5
for i = 1, count do daa:AddDna(da) end
print("... after " .. count .. " times daa:AddDna(da)")

print(pad("#daa"), #daa)
print(pad("daa"), daa)
for i = 1, count do print(pad("daa:GetDnaCount(" .. i .. ")"), daa:GetDnaCount(i)) end
print(pad("daa:GetNumberCount()"), daa:GetNumberCount())

local da = daa:FlattenToDna()
print("... after da = daa:FlattenToDna()")
print(pad("#da"), #da)
print(pad("da"), da)
local t = da:GetDArray()
print("da:GetDArray():", t)
tbl(t)

local filename = tmpdir .. "/test.daa"
local ok = daa:Write(filename)
print(pad("daa:Write('"..filename.."')"), ok)

daa = Dnaa(filename)
print(pad("daa = Dnaa('"..filename.."')"), daa)

local da = daa:FlattenToDna()
print("... after da = daa:FlattenToDna()")
print(pad("#da"), #da)
print(pad("da"), da)

local t = da:GetIArray()
print("da:GetIArray()", #t, t, tbl(t))
