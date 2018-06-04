require "lua/tools"

function pix_test()
	local filename = tmpdir .. "/test.png"
	header("Pix")
	local width, height, depth = 320, 240, 2
	local pix = Pix(width,height,depth)
	pix:SetText("Created with LuaLept-" .. LuaLept:Version())
	print(pad("pix = Pix("..width..","..height..","..depth..")"), pix)

	local cmap = PixColormap(2)
	print(pad("cmap = PixColormap(2)"), cmap)
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
	pix:View()

	-- Draw some border rings
	for dist = 4,13 do pix:SetBorderRingVal(dist, bluish-1) end

	-- Draw some horizontal lines around the vertical center
	for y = height/2-3,height/2+3 do
		for x = 5, width-6 do pix:SetPixel(x, y, black) end
	end
	pix:SetSpecial(10+9)	-- maximum compression

	local ok = pix:Write(filename, "png")
	print(pad("pix:Write('" .. filename .."')"), ok)
	pix:View()

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

	local ok = pix:DestroyColormap()
	print(pad("ok = pix:DestroyColormap()"), ok);

	local pix2 = Pix(filename)
	print(pad("pix2 = Pix('" .. filename .. "')"), pix2);
	pix2:View()

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
	local pix3 = Pix()
	print(pad("pix3 = Pix()"), pix3)
	local pix3 = pix3.DisplayColorArray(240, 4, 6, carray)
	print(pad("pix3 = pix3.DisplayColorArray(240, 4, 6, carray)"), pix3)
	local pix3 = pix3:AddBorder(20, LuaLept.RGB(255,255,255))
	print(pad("pix3 = pix3:AddBorder(20, LuaLept.RGB(255,255,255))"), pix3)
	pix3:View()

	print(pad("pix3"), pix3)
	local ok = pix3:Write(tmpdir .. "/carray.tiff", "lzw")

	local data, pdf = pix3:ConvertToPdf("jpg", 75, tmpdir .. "/carray.pdf", 0, 0, 300, "A PixColormap array")
	print(pad("pix3:ConvertToPdf(...)"), pdf)
end
