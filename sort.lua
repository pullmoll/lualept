--[[
	Sort a lualept *.cpp file's functions by the order in which
	they appear in the ll_register_<Class>() function.
--]]

---
-- Debug enable flag
debug = false

---
-- Leptonica source files "Note(s):" sections from doxygen comments
lept_notes = {}

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
	if t == nil then
		return
	end
	for _,l in ipairs(t) do
		fd:write(l .. '\n')
	end
end

---
-- Add notes sections found Leptonica *.c source files to heads
-- \param heads table of
function add_notes(fname, head)
	local res = {}
	local note = nil
	if fname ~= nil then
		note = lept_notes[fname]
	end
	if debug then
		print("fname", fname)
		print("head", head)
		print("note", note)
	end
	if fname == nil or head == nil or note == nil then
		return head
	end
	for lno,l in ipairs(head) do
		-- is there already a "Note(s):" line?
		if l:match("Note[s]?:") then
			return head
		end
		-- end of pre-formatted section
		if l == " * </pre>" then
			-- insert the note here after an empty comment line
			if #note > 0 then
				res[#res+1] = " *"
				for _,n in ipairs(note) do
					res[#res+1] = n
				end
			end
		end
		res[#res+1] = l
	end
	return res
end

---
-- Extract pre-formatted comment sections from the file fs.
-- \param fs source file stream
-- \param heads table to fill with "Note(s):" sections
function pre_comments(fs)
	local head = {}			-- currently collected heading
	local fname = nil		-- current function/heading name (start with head before 1st function)
	local pre = false		-- true while inside HTML pre tag
	local note = false		-- true when the 'Note:' or 'Notes:' line was read
	local state = 0

	while true do
		line = fs:read()
		if line == nil then
			-- end of file
			break
		end

		if line == '/**' or line == '/*!' then
			-- start of Doxygen comment
			-- introduces a new function
			-- go to state 1 (parsing doxygen comment)
			state = 1
		end

		if state == 3 then
			-- 2nd line after a comment
			-- contains the name of the function and its parameters
			-- go to state 4 (skip until end of function)
			state = 4
		end

		if state == 2 then
			-- 1st line after a comment
			-- return type
			-- go to state 3 (2nd line after a comment)
			state = 3
		end

		if line == ' */' then
			-- end of Doxygen comment
			-- go to state 2 (1st line after a comment)
			state = 2
		end

		if state == 1 then
			-- inside doxygen comment
			if line:match('%s*(%\\brief)%s*') then
				fname = line:match('%\\brief%s*([^(]+)')
			end
			if pre and line:match('Note[s]?:') then
				note = true
			end
			if line:match('<pre>') then
				pre = true
			end
			if line:match('</pre>') then
				pre = false
				note = false
			end
			if pre and note then
				head[#head+1] = line
			end
		end

		if state == 4 and line == '}' then
			-- end of a function
			if fname ~= nil then
				--[[
				if debug then
					print("fname='" .. fname .. "'")
					list(io.stdout, head)
				end
				--]]
				lept_notes[fname] = head
			end
			head = {}
			state = 0
			fname = nil
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
	local fnames = {}		-- array of Leptonica function names per lualept function
	local name = "head"		-- current function/heading name (start with head before 1st function)
	local fname = nil		-- current Leptonica function name
	local sorted = { name }		-- table for sorting function names
	local func = {}			-- currently collected function body
	local head = {}			-- currently collected heading
	local state = 0			-- parser state
	local f

	while true do
		line = fs:read()
		if line == nil then
			-- end of file
			break
		end

		-- print(string.format("[%d,#head=%3d,#func=%3d] %s", state, #head, #func, line))

		if line == '/**' or line == '/*!' then
			-- start of Doxygen comment
			-- introduces a new function
			state = 1
		end

		if state == 3 then
			-- 2nd line after a comment
			-- contains the name of the lualept function
			name = line:match('^[%a%d_]+')
			state = 4
		end

		if state == 2 then
			-- 1st line after a comment
			-- contains the return type
			state = 3
		end

		if line == ' */' then
			-- end of Doxygen comment
			state = 2
		end

		if state <= 2 then
			head[#head+1] = line
		else
			func[#func+1] = line
		end

		if state == 4 then
			if fname == nil then
				-- find the Leptonica function call in the function's body
				fname = line:match("[%a%d_]+" .. name)
			end
			if line == '}' then
				-- end of a function
				-- print("name='" .. name .. "'")
				index[#index+1] = name	-- keep track of sequential index
				funcs[name] = func	-- store function
				heads[name] = head	-- store heading
				fnames[name] = fname	-- store Leptonica function name
				if name:match('ll_register_') then
					-- scan for sorted order
					for _,l in pairs(func) do
						local f, s = l:match('%s+%{"([%a%d_]+)",%s*([%a%d_]+)%}.*')
						if s ~= nil and  found[s] == nil then
							found[s] = true
							if s:match("^ll_") == nil then
								sorted[#sorted+1] = s
							end
						end
					end
				end
				func = {}
				head = {}
				fname = nil
				state = 0
			end
		end
	end

	-- print statistics
	if debug then
		local nfuncs = 0
		for _,f in pairs(funcs) do nfuncs = nfuncs + 1 end
		local nfound = 0
		for _,f in pairs(found) do nfound = nfound + 1 end
		local nfnames = 0
		for _,f in pairs(fnames) do nfnames = nfnames + 1 end
		print(string.format("/* functions=%d found=%d fnames=%d sorted=%d */", nfuncs, nfound, nfnames, #sorted))
	end

	-- output heads[] and funcs[] in the sequence of sorted[]
	for l,name in ipairs(sorted) do
		-- print('/* ' .. name .. ' */')
		local fname = fnames[name]
		local t_head = add_notes(fname, heads[name])
		local t_func = funcs[name]
		list(fd, t_head)
		list(fd, t_func)
		heads[name] = nil
		funcs[name] = nil
		fnames[name] = nil
	end

	-- remaining functions that were not insered into sorted[]
	-- output heads[] and funcs[] in the sequence of index[] values
	for l,name in ipairs(index) do
		-- print('/* ' .. name .. ' */')
		local fname = fnames[name]
		local t_head = add_notes(fname, heads[name])
		local t_func = funcs[name]
		list(fd, t_head)
		list(fd, t_func)
		heads[name] = nil
		funcs[name] = nil
		fnames[name] = nil
	end
	return true
end

---
-- Main function.
-- \param arg table array of command line parameters

if #arg < 1 then
	print("Usage: lua " .. arg[0] .." filename.cpp [leptonica/src/*.c]")
end

local input = arg[1]
local backup = input .. "~"
local ok, err = copyfile(input, backup)
if not ok then
	print("Error copying " .. input .." to " .. backup .. ": " ..err)
end

-- parameters > 1 are ../leptonica/src/*.c file names
if #arg > 1 then
	for i = 2,#arg do
		local fs = io.open(arg[i])
		if debug then
			print("===> parsing " .. arg[i])
		end
		pre_comments(fs)
		fs:close()
	end
end

local fs = io.open(backup)
local fd = io.open(input, "wb")
sortfile(fs, fd)
