tmpdir = "/tmp/lualept"
images = "images"

---
-- Print a header with title %str
-- \param str string to print in header, or nil for no title
--
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

function pad(str, width)
	width = width or 38
	str = str or "nil"
	local len = #str
	return str .. string.rep(' ', width - len)
end

function tbl(t)
	local str = "\n"
	for i,v in pairs(t) do
		str = str .. '    ' .. i .. ' = ' .. v .. '\n'
	end
	return str
end

function basename(s)
	return s:gsub(".*/([^/]+)$", "%1")
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
