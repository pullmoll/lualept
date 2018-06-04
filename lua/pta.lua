require "lua/tools"

function pta_test()
	header("Pta/Ptaa")
	-- Create a new Pta
	local pa = Pta(8)
	print(pad("pa = Pta(8)"), pa)

	-- Add some points
	pa:AddPt(2.71,3.14)
	pa:AddPt(10,12)
	pa:AddPt(12,16)
	pa:AddPt(14,20)
	pa:AddPt(16,24)
	print(pad("#pa"), #pa)
	print(pad("pa"), pa)

	local paa = Ptaa()
	paa:AddPta(pa)
	paa:AddPta(pa)
	paa:AddPta(pa)
	print(pad("#paa"), #paa)
	print(pad("paa"), paa)

	header()
end
