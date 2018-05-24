--[[
	Sort a lualept *.cpp file's functions by the order in which
	they appear in the ll_register_<Class>() function.
--]]

---
-- copy a file src to dst
-- \param src source filename
-- \param dst destination filename
-- \param blocksize block size for copying (default 1M)
-- \return true on success; nil,error on error
function copyfile(src, dst, blocksize)
	blocksize = blocksize or 1024*1024
	local fs, fd, err
	-- return after closing the file descriptors
	local function ret(...)
		if fs then fs:close() end
		if fd then fd:close() end
		return ...
	end
	fs, err = io.open(src)
	if not fs then return ret(nil, err) end
	fd, err = io.open(dst, "wb")
	if not fd then return ret(nil, err) end
	while true do
		local ok, data
		data = fs:read(blocksize)
		if not data then break end
		ok, err = fd:write(data)
		if not ok then return ret(nil, err) end
	end
	return ret(true)
end

---
-- List the array table (t) contents to fd.
-- \param fd io file descriptor
-- \param t array table to list
function list(fd, t)
	if t ~= nil then
		for _,l in ipairs(t) do
			fd:write(l .. '\n')
		end
	end
end

---
--- Sort the file fs and write the result to fd.
-- \param fs source file stream
-- \param fd destination file stream
-- \return true on success
function sortfile(fs, fd)
	local index = {}		-- table array of function names in sequence [1 ... #funcs]
	local funcs = {}		-- table of extracted function bodies
	local heads = {}		-- table of extracted headings before functions
	local found = {}		-- array of names already found when inserting to sort
	local name = "head"		-- current function/heading name (start with head before 1st function)
	local sorted = { name }		-- table for sorting function names
	local func = {}			-- currently collected function body
	local head = {}			-- currently collected heading

	local f = 0			-- parsing state

	for i = 1,99999 do
		line = fs:read()
		if line == nil then
			-- end of file
			break
		end

		-- print('[' .. f .. ',#' .. #func .. ']', line)

		if line == '/**' then
			-- start of Doxygen comment
			-- introduces a new function
			heads[name] = head
			f = 1
		end

		if f == 3 then
			-- 2nd line after a comment
			-- contains the name of the function
			name = line:match('^[%a%d_]+')
			f = 4
		end

		if f == 2 then
			-- 1st line after a comment
			-- return type
			f = 3
		end

		if line == ' */' then
			-- end of Doxygen comment
			f = 2
		end

		if f == 0 then
			head[#head+1] = line
		else
			func[#func+1] = line
		end

		if f == 4 and line == '}' then
			-- end of a function
			-- print("name='" .. name .. "'")
			-- keep track of sequential index
			index[#index+1] = name
			-- store function
			funcs[name] = func
			if name:match('ll_register_.+') then
				-- scan for sorted order
				for _,l in pairs(func) do
					local f, s = l:match('%s+%{"([%a%d_]+)",%s*([%a%d]+)%}.*')
					if s ~= nil and found[s] == nil then
						found[s] = true
						sorted[#sorted+1] = s
					end
				end
			end
			func = {}
			head = {}
			f = 0
		end
	end

	-- print statistics
	if false then
		nfuncs = 0
		for _,f in pairs(funcs) do nfuncs = nfuncs + 1 end
		nfound = 0
		for _,f in pairs(found) do nfound = nfound + 1 end
		print('/* functions='..nfuncs..' sorted='..#sorted..' found='..nfound..' */')
	end

	-- output funcs[] in the sequence of sorted[]
	for _,f in ipairs(sorted) do
		-- print('/* ' .. f .. ' */')
		local t_head = heads[f]
		local t_func = funcs[f]
		list(fd, t_head)
		list(fd, t_func)
		heads[f] = nil
		funcs[f] = nil
	end

	-- remaining functions that were not insered into sorted[]
	-- output funcs[] in the sequence of index
	for _,f in ipairs(index) do
		-- print('/* ' .. f .. ' */')
		local t_head = heads[f]
		local t_func = funcs[f]
		list(fd, t_head)
		list(fd, t_func)
		heads[f] = nil
		funcs[f] = nil
	end
	return true
end

---
-- Main function.
-- \param arg table array of command line parameters
script = arg[0] or ""
if #arg < 1 then
	print("Usage: lua " .. script .." filename.cpp")
end

local input = arg[1]
local backup = input .. "~"
local ok, err = copyfile(input, backup)
if not ok then
	print("Error copying " .. input .." to " .. backup .. ": " ..err)
end

local fs = io.open(backup)
local fd = io.open(input, "wb")
sortfile(fs, fd)
