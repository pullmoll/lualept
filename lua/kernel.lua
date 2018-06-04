function header(str)
	local len = str ~= nil and #str or 0
	local nstar = (78 - len) // 2
	io.write(string.rep('*',nstar))
	if (len > 0) then
		io.write('[' .. str .. ']')
	end
	io.write(string.rep('*',nstar))
	io.write("\n")
end

function pad(str)
	local len = #str
	local nblank = 38 - len
	return str .. string.rep(' ', nblank)
end

function tbl(t)
	local str = "\n"
	for i,v in pairs(t) do
		str = str .. '    ' .. i .. ' = ' .. v .. '\n'
	end
	return str
end


header("Main program")
header()

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
print("That's all, folks!")
