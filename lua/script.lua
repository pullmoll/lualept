function aset_test()
	print("************** ASET")
	local aset = Lept.ASET("int")
	print("aset            ", '#'..#aset, aset)
	aset:Insert(2)
	aset[17] = true
	aset[4] = true
	aset[-1000] = true
	aset[99] = true
	aset[1] = true
	print("aset            ", '#'..#aset, aset)
	print("aset:Find(5)    ", aset:Find(5))
	print("aset:Find(99)   ", aset:Find(99))
	print("aset:Delete(17) ", aset:Delete(17))
	print("aset            ", '#'..#aset, aset)
end

function amap_test()
	print("************** AMAP")
	local amap = Lept.AMAP("int")
	print("amap            ", '#'..#amap, amap)
	amap[2] = 22
	amap[17] = 100
	amap[4] = 33
	amap[-1000] = 512
	amap[99] = 999
	amap[1] = 0
	print("amap            ", '#'..#amap, amap)
	print("amap:Find(5)    ", amap:Find(5))
	print("amap:Find(99)   ", amap:Find(99))
	print("amap:Delete(17) ", amap:Delete(17))
	print("amap            ", '#'..#amap, amap)
end

function pta_test()
	print("************** PTA/PTAA")
	-- Create a new PTA
	local pa = Lept.PTA(8)
	print("pa = Lept.PTA(8)", pa)

	-- Add some points
	pa:AddPt(2.71,3.14)
	pa:AddPt(10,12)
	pa:AddPt(12,16)
	pa:AddPt(14,20)
	pa:AddPt(16,24)
	print("#pa", #pa)
	print("pa", pa)

	local paa = Lept.PTAA()
	paa:AddPta(pa)
	paa:AddPta(pa)
	paa:AddPta(pa)
	print("#paa", #paa)
	print("paa", paa)
end

function boxa_test()
	print("************** BOX/BOXA/BOXAA")
	-- Create a new BOX
	local b = Lept.BOX(10,10,40,80)
	print("b = BOX(10,10,40,80)", b)

	-- Create a new BOXA
	local ba = Lept.BOXA(1)
	print("ba = Lept.BOXA(1)", ba)
	print("#ba", #ba)

	-- Add box b using default, clone, copy
	ba:AddBox(b)
	ba:AddBox(b, "clone")
	ba:AddBox(b)
	ba:AddBox(b, "copy")
	ba:AddBox(b)
	print("... after 5x ba:AddBox()")

	-- Print ba:GetCount() or short #ba
	print("#ba", #ba)
	print("ba", ba)

	-- Insert before the third BOX in BOXA
	ba:InsertBox(3, Lept.BOX(3,4,19,18))
	print("... after ba:InsertBox(3, Lept.BOX(3,4,19,18))")
	-- print("#ba", #ba)
	-- print("ba", ba)

	-- Get the inserted BOX
	b = ba:GetBox(3)
	print("b = ba:GetBox(3)", b)

	-- Get the BOX geometry
	b = ba:GetBox(4)
	print("b = ba:GetBox(4)", b)
end

function numa_test()
	local filename = "test.log"
	print("************** NUMA/NUMAA")

	local PI = 3.14159265358979323846
	local E = 2.7182818284590452354

	-- Create a new NUMA
	local na = Lept.NUMA()
	print("na = Lept.NUMA()", na)

	-- Add some numbers
	na:AddNumber(PI)
	na:AddNumber(E)
	na:InsertNumber(2, 10.99999)
	na:AddNumber(PI*PI)
	na:AddNumber(E^(1/PI))

	print("... after adding some numbers")
	print("na", na)
	print("#na", #na)
	print("na:GetParameters()", na:GetParameters())

	-- As array of numbers
	print("na:GetFArray()", na:GetFArray())

	-- As array of integers
	print("na:GetIArray()", na:GetIArray())

	-- Create a new MUMAA
	local naa = Lept.NUMAA(1)
	print("naa = Lept.NUMAA(1)", naa)

	local count = 10
	for i = 1, count do
		naa:AddNuma(na)
	end
	print("... after 16 x naa:AddNuma(na)")

	print("#naa",	#naa)
	for i = 1, count do
		print("naa:GetNumaCount("..i..")", naa:GetNumaCount(i))
	end
	print("naa:GetNumberCount()", naa:GetNumberCount())

	local na = naa:FlattenToNuma()
	print("... after na = naa:FlattenToNuma()")
	print("#na",	#na)
	print("na",	na)
	print("na:GetFArray()", na:GetFArray())

	local ok = naa:Write(filename)
	print("naa:Write('"..filename.."')", ok)

	naa = naa.Read(filename)
	print("naa.Read('"..filename.."')", ok)
end

function dna_test()
	print("************** DNA/DNAA")

	local PI = 3.14159265358979323846
	local E = 2.7182818284590452354

	-- Create a new DNA
	local da = Lept.DNA()
	print("da = Lept.DNA()", da)

	-- Add some numbers
	da:AddNumber(PI)
	da:AddNumber(E)
	da:InsertNumber(2, 10.999999999999)
	da:AddNumber(PI*PI)
	da:AddNumber(E^(1/PI))

	print("... after adding some numbers")
	print("da", da)
	print("#da", #da)
	print("da:GetParameters()", da:GetParameters())

	-- As array of numbers
	print("da:GetDArray()", da:GetDArray())

	-- Create a new DNAA
	local daa = Lept.DNAA(1)
	print("daa = Lept.DNAA(1)", daa)

	local count = 10
	for i = 1, count do
		daa:AddDna(da)
	end
	print("... after 16 x daa:AddDna(da)")

	print("#daa",	#daa)
	for i = 1, count do
		print("daa:GetDnaCount("..i..")", daa:GetDnaCount(i))
	end
	print("daa:GetNumberCount()", daa:GetNumberCount())

	local da = daa:FlattenToDna()
	print("... after da = daa:FlattenToDna()")
	print("#da",	#da)
	print("da",	da)
	print("da:GetDArray()", da:GetDArray())

	local filename = "test.log"
	local ok = daa:Write(filename)
	print("daa:Write('"..filename.."')", ok)

	daa = daa.Read(filename)
	print("daa.Read('"..filename.."')", ok)
end

function pix_test()
	local filename = "/tmp/test.png"
	print("************** PIX")
	local width, height, depth = 640, 480, 2
	local pix = Lept.PIX(width,height,depth)
	print("pix = Lept.PIX("..width..","..height..","..depth..")", pix)

	local cmap = Lept.PIXCMAP(2)
	print("cmap = Lept.PIXCMAP(2)", cmap)
	print("#cmap", #cmap)

	print("cmap:GetFreeCount()", cmap:GetFreeCount())
	cmap:AddColor(0x00,0x00,0x00)
	cmap:AddColor(0x7f,0x7f,0x7f)
	cmap:AddColor(0x40,0xe0,0xf0)
	cmap:AddColor(0xff,0xff,0xff)

	local color = 3
	print("cmap:GetWhiteVal() index", color)
	print("cmap:GetFreeCount()", cmap:GetFreeCount())

	local ok = pix:SetColormap(cmap)
	print("pix:SetColormap()", ok)
	print("pix", pix);

	-- Clear all pixels
	pix:SetAll()

	for dist = 4, 11 do
		local ok = pix:SetBorderRingVal(dist,0)
		print("pix:SetBorderRingVal("..dist..",2)", ok);
	end

	for x = 0, width-1 do
		pix:SetPixel(x, height/2, color)
	end

	pix:SetSpecial(10+9)	-- maximum compression
	local ok = pix:Write(filename, "png")
	print("pix:Write('" .. filename .."')", ok)

	local pix2 = Lept.PIX(filename)
	print("pix = Lept.PIX('" .. filename .. "')", pix2);

	local w = pix2:GetWidth()
	print("w", w)

	local h = pix2:GetHeight()
	print("h", h)

	local d = pix2:GetDepth()
	print("d", d)

	local spp = pix2:GetSpp()
	print("spp", spp)

	local wpl = pix2:GetWpl()
	print("wpl", wpl)

	local bits = h * wpl * 32
	print("bits", bits)
end

function pix2_test()
	local pix = Lept.PIX('lualept.jpg')
	print ("pix", pix)

	local ok = pix:Write('/tmp/lualept.png', 'png')
	print ("pix:Write('/tmp/lualept.png', 'png')", ok)

	local w, h, d = pix:GetDimensions()
	print ("dimensions      :", w, h, d)
	print ("area            :", w * h)
	print ("RGB colors      :", pix:CountRGBColors())
	-- print ("color histogram :", pix:GetColorAmapHistogram(8))

	local pixm = pix:MakeArbMaskFromRGB(0.0, 0.5, -0.5, 0.05)
	pixm:SetInputFormat('tiff-g4')
	print ("pixm            :", pixm)
	print ("dimensions      :", pixm:GetDimensions())

	local box = Lept.BOX(12,12,140,140)
	print ("pixels          :", pixm:CountPixels())
	print ("pixels in rect  :", box, pixm:CountPixelsInRect(box))
	print ("foreground fract:", pixm:ForegroundFraction())

	local ok = pixm:Write('/tmp/lualept-mask.tif', 'g3')
	pix:PaintThroughMask(pixm, 0, 0, Lept.RGB(255,0,128))

	local ok = pix:Write('/tmp/lualept-masked.jpg', 'jpg')
end

function hex_dump(buf)
	for i=1, math.ceil(#buf/16) * 16 do
		if (i-1) % 16 == 0 then
			io.write(string.format('%08X  ', i-1))
			end
		io.write(i > #buf and '   ' or string.format('%02X ', buf:byte(i)))
		if i %  8 == 0 then
			io.write(' ')
		end
		if i % 16 == 0 then
			io.write('\n')
		end
	end
end

print("************** MAIN")
print("Hello, world!")
print("**************")
print("Lept:Version()", Lept:Version())

aset_test()
amap_test()
-- pta_test()
-- boxa_test()
-- numa_test()
-- dna_test()
-- pix_test()
-- pix2_test()

print("**************")
print("That's all, folks!")
