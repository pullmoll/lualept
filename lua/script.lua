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

function bmf_test()
	local dir = "fonts";
	local bmf = LuaLept.Bmf(dir, 10);
	print(pad("bmf"), bmf)
	local chr = string.byte('x')
	print(pad("bmf:GetWidth("..chr..")"), bmf:GetWidth(chr))
	local l = {}
	local h, t = bmf:GetLineStrings("This is just a simple test to see how bmf:GetLineStrings() works out.", 240, 0)
	print(pad("bmf:GetLineStrings(...)"), h, #t, t, tbl(t))

end

function aset_test()
	header("Aset")
	local aset = LuaLept.Aset("int")
	print(pad("aset"), '#'..#aset, aset)
	aset:Insert(2)
	aset[17] = true
	aset[4] = true
	aset[-1000] = true
	aset[99] = true
	aset[1] = true
	print(pad("aset"), '#'..#aset, aset)
	print(pad("aset:Find(5)"), aset:Find(5))
	print(pad("aset:Find(99)"), aset:Find(99))
	print(pad("aset:Find(17)"), aset:Find(17))
	print(pad("aset:Delete(17)"), aset:Delete(17))
	print(pad("aset:Find(17)"), aset:Find(17))
	print(pad("aset"), '#'..#aset, aset)
end

function amap_test()
	header("Amap")
	local amap = LuaLept.Amap("int")
	print(pad("amap"), '#'..#amap, amap)
	amap[2] = 22
	amap[17] = 100
	amap[4] = 33
	amap[-1000] = 512
	amap[99] = 999
	amap[1] = 0
	print(pad("amap"), '#'..#amap, amap)
	print(pad("amap:Find(5)"), amap:Find(5))
	print(pad("amap:Find(99)"), amap:Find(99))
	print(pad("amap:Find(17)"), amap:Find(17))
	print(pad("amap:Delete(17)"), amap:Delete(17))
	print(pad("amap:Find(17)"), amap:Find(17))
	print(pad("amap"), '#'..#amap, amap)
end

function pta_test()
	header("Pta/Ptaa")
	-- Create a new Pta
	local pa = LuaLept.Pta(8)
	print(pad("pa = LuaLept.Pta(8)"), pa)

	-- Add some points
	pa:AddPt(2.71,3.14)
	pa:AddPt(10,12)
	pa:AddPt(12,16)
	pa:AddPt(14,20)
	pa:AddPt(16,24)
	print(pad("#pa"), #pa)
	print(pad("pa"), pa)

	local paa = LuaLept.Ptaa()
	paa:AddPta(pa)
	paa:AddPta(pa)
	paa:AddPta(pa)
	print(pad("#paa"), #paa)
	print(pad("paa"), paa)
end

function box_test()
	header("Box/Boxa/Boxaa")
	-- Create a new Box
	local b = LuaLept.Box(10,10,40,80)
	print(pad("b = Box(10,10,40,80)"), b)

	-- Create a new Boxa
	local ba = LuaLept.Boxa(1)
	print(pad("ba = LuaLept.Boxa(1)"), ba)
	print(pad("#ba"), #ba)

	-- Add box b using default, clone, copy
	ba:AddBox(b)
	ba:AddBox(b, "clone")
	ba:AddBox(b)
	ba:AddBox(b, "copy")
	ba:AddBox(b)
	print("... after 5x ba:AddBox()")

	-- Print ba:GetCount() or short #ba
	print(pad("#ba"), #ba)
	print(pad("ba"), ba)

	-- Insert before the third Box in Boxa
	ba:InsertBox(3, LuaLept.Box(3,4,19,18))
	print("... after ba:InsertBox(3, LuaLept.Box(3,4,19,18))")
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

	-- Get the Box at index 4
	b = ba:GetBox(4)
	print(pad("b = ba:GetBox(4)"), b)

	baa = LuaLept.Boxaa()
	print(pad("baa = LuaLept.Boxaa()"), baa)
	print(pad("#baa"), #baa)
	local count = 7
	for i = 1,count do baa:AddBoxa(ba) end
	print("... after " .. count .. " times baa:AddBoxa(ba)")
	print(pad("#baa"), #baa)
	print(pad("baa"), baa)
	print(pad("baa:GetBoxCount()"), baa:GetBoxCount())

	local ba2 = baa:FlattenToBoxa()
	print(pad("baa::FlattenToBoxa()"), ba2)
end

function numa_test()
	local filename = "/tmp/test.numa"
	header("Numa/Numaa")

	local PI = 3.14159265358979323846
	local E = 2.7182818284590452354

	-- Create a new Numa
	local na = LuaLept.Numa()
	print(pad("na = LuaLept.Numa()"), na)

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
	local naa = LuaLept.Numaa(1)
	print(pad("naa = LuaLept.Numaa(1)"), naa)

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

	naa = LuaLept.Numaa().Read(filename)
	print(pad("naa.Read('" .. filename .. "')"), ok)

	local na = naa:FlattenToNuma()
	print("... after na = naa:FlattenToNuma()")
	print(pad("#na"), #na)
	print(pad("na"), na)
	print("na:GetFArray():", tbl(na:GetFArray()))
	print("na:GetIArray():", tbl(na:GetIArray()))
end

function dna_test()
	header("Dna/Dnaa")

	local PI = 3.14159265358979323846
	local E = 2.7182818284590452354

	-- Create a new Dna
	local da = LuaLept.Dna()
	print(pad("da = LuaLept.Dna()"), da)

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
	local daa = LuaLept.Dnaa(1)
	print(pad("daa = LuaLept.Dnaa(1)"), daa)

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

	local filename = "/tmp/test.daa"
	local ok = daa:Write(filename)
	print(pad("daa:Write('"..filename.."')"), ok)

	daa = daa.Read(filename)
	print(pad("daa.Read('"..filename.."')"), ok)
	local da = daa:FlattenToDna()
	print("... after da = daa:FlattenToDna()")
	print(pad("#da"), #da)
	print(pad("da"), da)
	local t = da:GetIArray()
	print("da:GetIArray()", #t, t, tbl(t))
end

function pix_test()
	local filename = "/tmp/test.png"
	header("Pix")
	local width, height, depth = 320, 240, 2
	local pix = LuaLept.Pix(width,height,depth)
	pix:SetText("Created with LuaLept-" .. LuaLept:Version())
	print(pad("pix = LuaLept.Pix("..width..","..height..","..depth..")"), pix)

	local cmap = LuaLept.PixColormap(2)
	print(pad("cmap = LuaLept.PixColormap(2)"), cmap)
	print(pad("#cmap"), #cmap)

	print(pad("cmap:GetFreeCount()"), cmap:GetFreeCount())
	local black = cmap:AddNearestColor(0x00,0x00,0x00)
	local bluish = cmap:AddNewColor(0x40,0xe0,0xf0)
	cmap:AddColor(0x7f,0x7f,0x7f)
	cmap:AddRGBA(0xff,0xff,0xff,0xff)
	print(pad("cmap:AddNearestColor(0x00,0x00,0x00)"), black)
	print(pad("cmap:AddNewColor(0x40,0xe0,0xf0)"), bluish)

	print(pad("cmap:GetFreeCount()"), cmap:GetFreeCount())

	print("cmap", cmap)

	local ok = pix:SetColormap(cmap)
	print(pad("pix:SetColormap()"), ok)
	print(pad("pix"), pix);

	-- Set all pixels (to 2^depth - 1)
	pix:SetAllArbitrary(3)

	-- Draw some border rings
	for dist = 4,13 do pix:SetBorderRingVal(dist, bluish-1) end

	-- Draw some horizontal lines around the vertical center
	for y = height/2-2,height/2+2 do
		for x = 4, width-4 do pix:SetPixel(x, y, black) end
	end

	pix:SetSpecial(10+9)	-- maximum compression
	local ok = pix:Write(filename, "png")
	print(pad("pix:Write('" .. filename .."')"), ok)

	local cmap = pix:GetColormap()
	print(pad("cmap"), cmap)
	local r,g,b,a = cmap:ToArrays()
	print(pad("r,g,b,a = cmap:ToArrays()"), r, g, b, a)
	print(pad("r"), tbl(r))
	print(pad("g"), tbl(g))
	print(pad("b"), tbl(b))
	print(pad("a"), tbl(a))
	local rgb = cmap:ToRGBTable()
	print(pad("rgb = cmap:ToRGBTable()"), rgb, #rgb, tbl(rgb))
	local data = cmap:SerializeToMemory(4)
	print(pad("data = cmap:SerializeToMemory(4)"), data)
	hex_dump(data)

	local pix2 = LuaLept.Pix(filename)
	print(pad("pix2 = LuaLept.Pix('" .. filename .. "')"), pix2);


	-- get the pixel data of pix2 as two dimensional array of integers
	local data = pix2:GetData();
	print(pad("pix2:GetData()"), data)
	for y,words in pairs(data) do
		io.write(string.format("%d = {", y))
		for x,val in pairs(words) do
			io.write(string.format(" %08x", val))
		end
		io.write(" }\n")
	end

	local ok = pix2:SetData(data)
	print(pad("pix2:SetData(data)"), ok)

	local w, h, d = pix2:GetDimensions()
	print(pad("width"), w)
	print(pad("height"), h)
	print(pad("depth"), d)

	local spp = pix2:GetSpp()
	print(pad("spp"), spp)

	local wpl = pix2:GetWpl()
	print(pad("wpl"), wpl)

	local bits = h * wpl * 32
	print(pad("bits"), bits)

	local carray = {
		0x00000000, 0x11111100, 0x22222200, 0x33333300,
		0x44444400, 0x55555500, 0x66666600, 0x77777700,
		0x88888800, 0x99999900, 0xaaaaaa00, 0xbbbbbb00,
		0xcccccc00, 0xdddddd00, 0xeeeeee00, 0xffffff00,
		0x32847100, 0xc8411200, 0x01928200, 0x12479a00,
		0x81ff2a00, 0x19f9fa00, 0x8a818b00, 0x586a9400}
	--[[
	      FIXME: how to access functions of LuaLept.Pix()
	      Right now I need to create a dummy pix3 to have
	      access to pix3.DisplayColorArray(...).
	      Should I be using global function names.
	--]]
	local pix3 = LuaLept.Pix(1,1)
	local pix3 = pix3.DisplayColorArray(240, 4, 6, carray)
	local pix3 = pix3:AddBorder(20, LuaLept.RGB(255,255,255))
	print(pad("pix3"), pix3)
	local ok = pix3:Write("/tmp/carray.tiff", "lzw")
end

function pix2_test()
	local filename = "/tmp/lualept.png"
	local filename2 = "/tmp/lualept-mask.tif"
	local filename3 = "/tmp/lualept-masked.png"
	header("Pix2")
	local pix = LuaLept.Pix('lualept.jpg')
	print (pad("pix"), pix)

	local ok = pix:Write(filename, 'png')
	print (pad("pix:Write('" .. filename .. "', 'png')"), ok)

	local w, h, d = pix:GetDimensions()
	print (pad("dimensions"), w, h, d)
	print (pad("area"), w * h)
	print (pad("RGB colors"), pix:CountRGBColors())
	local hist = pix:GetColorAmapHistogram(1)
	print (pad("color histogram"), hist)

	local pixm = pix:MakeArbMaskFromRGB(-0.5, -0.5, 0.75, 0.05)
	-- pixm:SetInputFormat('tiff-g4')
	print (pad("pixm"), pixm)
	print (pad("dimensions"), pixm:GetDimensions())

	print (pad("pixels"), pixm:CountPixels())
	local box = LuaLept.Box(10,w-20,h-30,30)
	print (pad("rect"), box)
	print (pad("pixels in rect"), pixm:CountPixelsInRect(box))
	print (pad("foreground fract"), pixm:ForegroundFraction())

	local ok = pixm:Write(filename2,'g4')
	print (pad("pixm:Write('" .. filename2 .. "','g4')"), ok)
	pix:PaintThroughMask(pixm, 0, 0, LuaLept.RGB(255,128,128))
	local ok = pix:Write(filename3,'png')
	print (pad("pix:Write('" .. filename3 .. "','png')"), ok)
end

function hex_dump(buf)
	for i = 1, math.ceil(#buf/16) * 16 do
		if (i-1) % 16 == 0 then
			io.write(string.format('%08X  ', i-1))
		end
		io.write(i > #buf and '   ' or string.format('%02X ', buf:byte(i)))
		if i %  8 == 0 then
			io.write(' ')
		end
		if i % 16 == 0 then
			io.write('|')
			for j = 1,16 do
				if i-16+j > #buf then
					io.write(' ')
				else
					local c = buf:byte(i-16+j)
					io.write(c > 31 and c < 127 and string.char(c) or '.')
				end
			end
			io.write('|\n')
		end
	end
end

header("Main program")
print("Hello, world!")
header()
print(pad("LuaLept:Version()"), LuaLept:Version())
print(pad("LuaLept:LuaVersion()"), LuaLept:LuaVersion())
print(pad("LuaLept:LeptVersion()"), LuaLept:LeptVersion())

-- bmf_test()
-- aset_test()
-- amap_test()
-- pta_test()
-- box_test()
numa_test()
-- dna_test()
pix_test()
-- pix2_test()

header()
print("That's all, folks!")
