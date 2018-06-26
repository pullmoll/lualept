local pixa = Pixa(32)
for i = 2, #arg do
	local pix = Pix(arg[i])
	pixa:AddPix(pix)
end
pixa:ConvertToPdf(300, 1.0, "jpeg", 80, "Pictures", "/tmp/pictures.pdf")

