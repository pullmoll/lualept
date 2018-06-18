--[[
	Generate C function wrapper templates for
	"LEPT_DLL export ..." lines from Leptonica's
	allheaders.h file.
--]]

-- set to true to debug parsing
debug = false

-- array of functions
funcs = {}

-- array of Leptonica src/*.c pre-formatted comments
heads = {}

---
-- Sort a table by its keys
-- \param t table
-- \param order optional function to sort table entries
--
function spairs(t, order)
	-- collect the keys
	local keys = {}
	for k in pairs(t) do keys[#keys+1] = k end
	-- if order function is given, sort by it passing the
	-- table and keys a,b; otherwise just sort the keys
	if order then
		table.sort(keys, function(a,b) return order(t, a, b) end)
	else
		table.sort(keys)
	end
	-- return the iterator function
	local i = 0
	return function()
		i = i + 1
		if keys[i] then
			return keys[i], t[keys[i]]
		end
	end
end

---
-- Return the size of a (non-array) table
-- \param t table
-- \return number of items in table
--
function tsize(t)
	local n = 0
	for k in pairs(t) do n = n + 1 end
	return n
end

---
-- List the array table (t) contents to fd.
-- \param fd io file descriptor
-- \param t array table to list
--
function list(fd, t)
	if t ~= nil then
		for _,l in ipairs(t) do
			fd:write(l .. '\n')
		end
	end
end

---
-- Return true, if %str has the format " * Note:" or " * Notes:"
-- \param str string to test
--
function is_notes(str)
	local m = str:match('^ %*.*(Note[s]?:)$')
	return m ~= nil
end

---
-- Return true, if %str starts with "/**" or "/*!"
-- \param str string to test
--
function is_doxy_start(str)
	local m = str:match('^/%*%*$') or str:match('^/%*%!$')
	return m ~= nil
end

---
-- Return true, if %str is " */"
-- \param str string to test
--
function is_doxy_end(str)
	local m = str:match('^ %*/$')
	return m ~= nil
end

---
-- Return true, if the table array t contains v
-- \param t table array to scan
-- \param val value to find
--
function contains(t,val)
	for i,v in ipairs(t) do
		if v == val then
			return true
		end
	end
	return false
end


---
-- Extract pre-formatted comment sections from the file fs.
-- Modifies the global heads table
-- \param fs source file stream
--
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

		if is_doxy_start(line) then
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

		if is_doxy_end(line) then
			-- end of Doxygen comment
			if state == 1 then
				-- go to state 2 (1st line after a comment)
				state = 2
			end
		end

		if state == 1 then
			-- inside doxygen comment
			if line:match('%s+(%\\brief)%s+') then
				fname = line:match('%\\brief%s+([^(]+)')
			end
			if pre and is_notes(line) then
				line = line:gsub(" %*%s+(Note.*)", " * Leptonica's %1")
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
				if debug then
					print("fname='" .. fname .. "'")
					list(io.stdout, head)
				end
				heads[fname] = head
			end
			head = {}
			state = 0
			name = nil
		end
	end
end

---
-- Strip known type prefixes from a Leptonica function name
-- \param str Leptonica function name
-- \return string with prefix stripped
--
function strip_ftype(str)
	local p, s = str:match("^(pixTiling)(.*)")
	if p ~= nil then
		return s
	end
	local p, s = str:match("^([%l_]+)(.*)")
	local prefixes = {
		amap		= true,
		array		= true,
		barcode		= true,
		bbuffer		= true,
		bmf		= true,
		box		= true,
		boxa		= true,
		boxaa		= true,
		ccb		= true,
		ccba		= true,
		cid		= true,
		cmap		= true,
		dewarp		= true,
		dewarpa		= true,
		dpix		= true,
		fpix		= true,
		fpixa		= true,
		jb		= true,
		kernel		= true,
		l_amap		= true,
		l_aset		= true,
		l_bytea		= true,
		l_dna		= true,
		l_dnaa		= true,
		l_rbtree	= true,
		l_CIData	= true,
		l_generate	= true,
		l_get		= true,
		lheap		= true,
		list		= true,
		lqueue		= true,
		lstack		= true,
		numa		= true,
		numaa		= true,
		pix		= true,
		pixa		= true,
		pixaa		= true,
		pixacc		= true,
		pixacomp	= true,
		pixcmap		= true,
		pixcomp		= true,
		pixTiling	= true,
		pms		= true,
		pta		= true,
		ptaa		= true,
		ptra		= true,
		ptraa		= true,
		quadtree	= true,
		rch		= true,
		rcha		= true,
		recog		= true,
		sa		= true,
		sarray		= true,
		sel		= true,
		sela		= true,
		string		= true,
		sudoku		= true,
		wshed		= true,
		zlib		= true
	}
	if prefixes[p] then
		return s
	elseif p ~= nil then
		local p1 = p:sub(1,1):upper()
		local p2 = p:sub(2,-1)
		return p1 .. p2 .. s
	else
		return s
	end
end

---
-- Return the lualapt getter for parameter type
-- \param vtype type name of the variable
-- \param arg argument #
-- \return string with call to function to retrieve vtype
--
function getter(vtype, arg, name)
	if vtype == "void *" then
		-- check for light user data
		return "ll_check_ludata(_fun, L, " .. arg .. ")"
	end
	if vtype == "const char*" then
		-- check for a string
		return "ll_check_string(_fun, L, " .. arg .. ")"
	end
	if vtype == "const l_uint8*" then
		-- check for a lstring cast to l_uint8*
		return "ll_check_lbytes(_fun, L, " .. arg .. ", &size)"
	end
	--[[
	if vtype:match("FILE%s*%*") then
		return "ll_check_stream(_fun, L, " .. arg .. ")"
	end
	--]]
	if vtype:match("luaL_Stream%s*%*") then
		return "ll_check_stream(_fun, L, " .. arg .. ")"
	end
	if vtype == "l_int32*" then
		return "ll_unpack_Iarray(_fun, L, " .. arg .. ", &size)"
	end
	if vtype == "l_uint32*" then
		return "ll_unpack_Uarray(_fun, L, " .. arg .. ", &size)"
	end
	if vtype == "l_float32*" then
		if name == "vc" then
			return "ll_check_vector(_fun, L, " .. arg .. ")"
		else
			return "ll_unpack_Farray(_fun, L, " .. arg .. ", &size)"
		end
	end
	if vtype == "l_float64*" then
		return "ll_unpack_Darray(_fun, L, " .. arg .. ", &size)"
	end
	if vtype == "Sarray*" then
		return "ll_unpack_Sarray(_fun, L, " .. arg .. ", nullptr)"
	end
	-- detecting well known variable names
	if name == "accessflag" or name == "copyflag" then
		return "ll_check_access_storage(_fun, L, " .. arg .. ")"
	end
	if name == "index" then
		return "ll_check_index(_fun, L, " .. arg .. ")"
	end
	if name == "incolor" then
		return "ll_check_set_black_white(_fun, L, " .. arg .. ")"
	end
	if name == "useboth" then
		return "ll_opt_boolean(_fun, L, " .. arg .. ", FALSE)"
	end
	vtype = vtype:gsub("%*", "")
	vtype = vtype:gsub("%s", "")
	return "ll_check_" .. vtype .. "(_fun, L, " .. arg .. ")"
end

---
-- Return the initializer for parameter type
-- \param vtype type name of the variable
-- \param arg argument #
-- \return string with call to function to retrieve vtype
--
function initer(vtype, arg, fname)
	if vtype:match("%*$") then
		if fname:match("Destroy") then
			local ptype = vtype:match("[A-Za-z]+")
			return "ll_take_udata<" .. ptype .. ">(_fun, L, " .. arg .. ", TNAME)"
		end
		return "nullptr"
	end
	return "0"
end

---
-- Return the lualapt pusher for return type
-- \param vtype type name of the variable
-- \param var name of the local variable to push
-- \param isref true, if the variable is a reference
--
function pusher(vtype, var, isref)
	if vtype:match("l_ok") then
		return "ll_push_boolean(_fun, L, 0 == " .. var .. ")"
	end
	if vtype:match("FILE%s*%*") then
		return "ll_push_stream(_fun, L, " .. var .. ")"
	end
	if isref then
		if vtype:match("l_uint8%s*%*") then
			return "ll_push_lbytes(_fun, L, " .. var ..", size)"
		end
		if vtype:match("char%s*%*") then
			return "ll_push_string(_fun, L, " .. var ..")"
		end
		if vtype:match("l_int32%s*%*") then
			return "ll_pack_Iarray(_fun, L, " .. var .. ", size)"
		end
		if vtype:match("l_uint32%s*%*") then
			return "ll_pack_Uarray(_fun, L, " .. var .. ", size)"
		end
		if vtype:match("l_float32%s*%*") then
			return "ll_pack_Farray(_fun, L, " .. var .. ", size)"
		end
		if vtype:match("l_float64%s*%*") then
			return "ll_pack_Darray(_fun, L, " .. var .. ", size)"
		end
		if vtype:match("Sarray%s*%*") then
			return "ll_pack_Sarray(_fun, L, " .. var .. ")"
		end
	end
	vtype = vtype:gsub("%s*", "")
	vtype = vtype:gsub("%*$", "")
	return "ll_push_" .. vtype .. "(_fun, L, " .. var ..")"
end

---
-- Return a variable name for the result type of a Leptonica function
-- \param rtype result type (or pointer to the rtype)
-- \param names table of variable names
-- \return a name or "result" as default
function result_name(rtype, names)
	local str = rtype:gsub("([%w_]+)%s*%**", function (s)
			local varname = {
				Amap		= "amap",
				AmapNode	= "node",
				Aset		= "aset",
				AsetNode	= "node",
				Bmf		= "bmf",
				Byterbuffer	= "bbuf",
				Box		= "box",
				Boxa		= "boxa",
				Boxaa		= "boxaa",
				Bytea		= "ba",
				CCBord		= "ccb",
				CCBorda		= "ccba",
				DLList		= "list",
				DPix		= "dpix",
				FPix		= "fpix",
				FPixa		= "fpixa",
				CompData	= "cid",
				Dewarp		= "dew",
				Dewarpa		= "dewa",
				Dna		= "da",
				Dnaa		= "daa",
				Kernel		= "kel",
				PdfData		= "pdfd",
				Stack		= "lstack",
				Numa		= "na",
				Numaa		= "naa",
				Pix		= "pix",
				Pixa		= "pixa",
				Pixaa		= "pixaa",
				Pixacc		= "pixacc",
				PixComp		= "pixc",
				PixaComp	= "pixac",
				PixColormap	= "cmap",
				Pta		= "pta",
				Ptaa		= "ptaa",
				Rbtree		= "rbt",
				Sarray		= "sa",
				Sel		= "sel",
				Sela		= "sela",
				WShed		= "ws",
				l_ok		= "ok"
			}
			return varname[s] or "result"
		end)
	-- already have a variable str in the names?
	if contains(names,str) then
		if contains(names,str .. "d") then
			str = "r" .. str
		else
			str = str .. "d"
		end
	end
	return str
end

---
-- Return the type name description for a variable type
-- \param vtype type name of the variable
--
function typedescr(vtype)
	if vtype:match("((const )?char$s*%*)") then
		return "string"
	end
	if vtype:match("((const )?l_uint8$s*%*)") then
		return "lstring"
	end
	vtype = vtype:gsub("%s+%*", "*")
	vtype = vtype:gsub("%s+$", "")
	return vtype
end

---
-- Return a string for function parameter names derived from types
-- \param types table array of types
-- \param names table array of names
-- \param refs table of names that are non-parameters
--
function params(types, names, refs)
	local str = ""
	for i=1,#types do
		local vtype = types[i]
		local name = names[i]
		if i > 1 then
			str = str .. ', '
		end
		if refs[name] == nil then
			str = str .. name
		else
			str = str .. '&' .. name
		end
	end
	return str
end

---
-- Parse the string str and write the lualept function body
-- \param fd destination file stream
-- \param str Leptonica function template from allheaders.h
--
function parse(fd, str)
	if str == nil then
		return
	end
	-- replace allheaders.h names with lualept names
	local str = str:gsub("([%w_]+)", function (s)
			local rename = {
				L_AMAP		= "Amap",
				L_AMAP_NODE	= "AmapNode",
				L_ASET		= "Aset",
				L_ASET_NODE	= "AsetNode",
				L_BMF		= "Bmf",
				BOX		= "Box",
				BOXA		= "Boxa",
				BOXAA		= "Boxaa",
				CCBORD		= "CCBord",
				CCBORDA		= "CCBorda",
				DLLIST		= "DLList",
				DPIX		= "DPix",
				FPIX		= "FPix",
				FPIXA		= "FPixa",
				L_BBUFFER	= "ByteBuffer",
				L_BYTEA		= "Bytea",
				L_COMP_DATA	= "CompData",
				L_DEWARP	= "Dewarp",
				L_DEWARPA	= "Dewarpa",
				L_DNA		= "Dna",
				L_DNAA		= "Dnaa",
				L_KERNEL	= "Kernel",
				L_PDF_DATA	= "PdfData",
				L_QUEUE		= "Queue",
				L_RBTREE	= "Rbtree",
				L_STACK		= "Stack",
				L_STRCODE	= "StrCode",
				L_TIMER		= "Timer",
				L_WALLTIMER	= "WallTimer",
				L_WSHED		= "WShed",
				NUMA		= "Numa",
				NUMAA		= "Numaa",
				PIX		= "Pix",
				PIXA		= "Pixa",
				PIXAA		= "Pixaa",
				PIXAC		= "PixaComp",
				PIXACC		= "Pixacc",
				PIXC		= "PixComp",
				PIXCMAP		= "PixColormap",
				PIXTILING	= "PixTiling",
				PTA		= "Pta",
				PTAA		= "Ptaa",
				SARRAY		= "Sarray",
				SEL		= "Sel",
				SELA		= "Sela",
			}
			return rename[s]
		end)
	if debug then
		print("====> " .. str)
	end

	-- extract the return value, the function's name and
	-- the position where the function's arguments start
	local rtype, fname, argspos = str:match("^([%w_]+%s*%**)%s+([^%s]+)%s*()")

	-- sub the args part
	local args = str:sub(argspos)
	-- remove leading and trailing parenthesis
	local args = args:match("%(%s*([^)]-)%s*%)")
	-- turn parameters into a comma separated string without blanks between
	local args = args:gsub("%s*([^,]+),?%s*", function (s)
			return s..","
		end)
	-- strip last comma (,)
	args = args:sub(1, -2)

	if debug then
		print("rtype   : '" .. rtype .. "'")
		print("fname   : '" .. fname .. "'")
		print("argspos : '" .. argspos .. "'")
		print("args    : '" .. args .. "'")
	end

	local vars = {}		-- table array of variables
	local types = {}	-- table array of variable types
	local names = {}	-- table array of variable names
	local refs = {}		-- array of names that are non-parameters
	local argc = 1		-- argument counter
	local argi = 1		-- local variable index
	local retn = 0

	-- fill the arrays
	for p in args:gmatch("([^,]+),?") do
		local vtype, name, get
		if p == "..." then
			vtype, name = "va_list", "ap"
		elseif p:match("FILE%s*%*") then
			-- replace "FILE* fp" with "luaL_Stream* stream"
			vtype = "luaL_Stream*"
			name = "stream"
		elseif p:match("^const ") then
			vtype, name = p:match("^(const [%w_]+%s*%**)%s*(.*)")
		else
			vtype, name = p:match("([%w_]+%s*%**)%s*(.*)")
		end
		-- remove blanks before an asterisk
		vtype = vtype:gsub("%s+%*", "*")

		if debug then
			print("vtype   : '" .. vtype .. "'")
			print("name    : '" .. name .. "'")
		end

		if vtype:match("%*%*$") and			-- vtype ends with "**" and
			name:match("^p") and (			-- name starts with a "p" and
				vtype:match("^l_int.*%*")	-- vtype is a "l_int*"
				or vtype:match("^l_uint.*%*")	-- or is a "l_uint*"
				or vtype:match("^l_float.*%*")	-- or is a "l_float*"
				or vtype:match("^size_t%s*%*")	-- or is a "size_t*"
				or vtype:match("^[A-Z]") )	-- or starts with an uppercase letter
			then
			-- this is (most probably) a pointer to a variable
			vtype = vtype:sub(1,-2)			-- strip 2nd asterisk
			name = name:sub(2,-1)			-- strip leading "p" from the name
			refs[name] = true
			get = initer(vtype, argi, fname)
		elseif vtype:match("%*$") and			-- vtype ends with "*" and
			name:match("^p") and (			-- name starts with a "p" and
			vtype:match("^l_int.*%*")		-- vtype is a "l_int*"
			or vtype:match("^l_uint.*%*")		-- or is a "l_uint*"
			or vtype:match("^l_float.*%*")		-- or is a "l_float*"
			or vtype:match("^size_t%s*%*") )	-- or is a "size_t*"
			then
			-- this is (most probably) a pointer to a variable
			vtype = vtype:sub(1,-2)	.. " "		-- strip asterisk and append a space
			name = name:sub(2,-1)			-- strip leading "p" from the name
			refs[name] = true
			get = initer(vtype, argi, fname)
		else
			get = getter(vtype, argi, name)
			argi = argi + 1
		end
		vtype = vtype:gsub("%*", " *")
		vars[argc] = '    ' .. vtype .. name .. ' = ' .. get .. ';'
		if name == "stream" then
			name = name .. "->f"
		end
		types[argc] = vtype
		names[argc] = name
		argc = argc + 1
	end

	local lname = strip_ftype(fname)	-- name without Leptonica prefix
	-- create the function's Doxygen comment
	local func = {
		'/**',
		' * \\brief ' .. lname .. '() brief comment goes here.',
		' * <pre>'
		}

	local argi = 1
	for i = 1, argc-1 do
		local vtype = types[i]
		local name = names[i]
		local param = typedescr(vtype)
		if refs[name] then
			-- this is a return value
			retn = retn + 1
		else
			-- this is a true parameter
			line = ' * Arg #' .. argi
			if argi == 1 then
				-- this comment is wrong for constructors...
				line = line .. ' (i.e. self)'
			end
			line = line .. ' is expected to be a ' .. param
			line = line .. ' (' .. name .. ').'
			func[#func+1] = line
			argi = argi + 1
		end
	end

	-- emit the doxygen "Note(s):" header found in Leptonica's source
	if heads[fname] ~= nil then
		local t = heads[fname]
		-- empty line
		func[#func+1] = ' *'
		-- emit the "Note:" section of the Leptonica function
		for _,l in ipairs(t) do
			func[#func+1] = l
		end
	end

	func[#func+1] = ' * </pre>'
	if rtype ~= "l_int32" and rtype ~= "l_ok" then
		retn = retn + 1
	end

	-- append the C function \param and \return comments
	func[#func+1] = ' * \\param L pointer to the lua_State'
	-- TODO: list types and names of return values
	func[#func+1] = ' * \\return ' .. retn .. ' on the Lua stack'
	func[#func+1] = ' */'

	-- append the function body
	func[#func+1] = 'static int'
	func[#func+1] = lname .. '(lua_State *L)'
	func[#func+1] = '{'
	func[#func+1] = '    LL_FUNC("' .. lname .. '");'
	for i = 1, #vars do
		local name = names[i]
		if refs[name] == nil then
			func[#func+1] = vars[i]
		end
	end
	for i = 1, #vars do
		local name = names[i]
		if refs[name] ~= nil then
			func[#func+1] = vars[i]
		end
	end

	local rname = result_name(rtype, names)
	if tsize(refs) > 0 then
		-- there were references to variables
		if rtype == "l_ok" then
			-- The Leptonica function returns 0 on success
			func[#func+1] = '    if (' .. fname .. '(' .. params(types, names, refs) .. '))'
			func[#func+1] = '        return ll_push_nil(L);'
		elseif rypte == nil or rytpe == "void" then
			-- the function returns nothing
			func[#func+1] = '    ' .. fname .. '(' .. params(types, names, refs) .. ');'
		else
			-- the function returns some other type
			func[#func+1] = '    ' .. rtype .. ' ' .. rname .. ' = ' ..
				fname .. '(' .. params(types, names, refs) .. ');'
			func[#func+1] = '    ' .. pusher(rtype, rname, true) .. ';'
		end
		for i = 1, #vars do
			local name = names[i]
			if refs[name] ~= nil then
				local vtype = types[i]
				if vtype:match("l_float32%s*%*") and name:match("p?vci?") then
					-- handle pushing a vector of 6 floats
					for j = 0, 5 do
						func[#func+1] = '    ' .. pusher(vtype, name .. '[' .. j .. ']', false) .. ';'
					end
					retn = 6
				elseif lname == "Destroy" then
					retn = 0
				else
					func[#func+1] = '    ' .. pusher(vtype, name, false) .. ';'
				end
			end
		end
		func[#func+1] = '    return ' .. retn .. ';'
	else
		-- there were no references to variables
		-- assume the Leptonica function returns the result
		if not rtype:match("%*$") then
			rtype = rtype .. ' '
		end
		func[#func+1] = '    ' .. rtype .. rname .. ' = ' ..
			fname .. '(' .. params(types, names, refs) .. ');'
		func[#func+1] = '    return ' .. pusher(rtype, rname, true) .. ';'
	end
	func[#func+1] = '}'
	func[#func+1] = ''

	funcs[fname] = func
end

---
-- \param fs source file stream (allheaders.h)
-- \param fd destination file stream (templates.cpp)
-- \return true on success
function extract(fs, fd)
	fd:write('#include "../leptonica/src/allheaders.h"\n')
	fd:write('#include "src/modules.h"\n')
	fd:write('#include </usr/include/lua5.3/lualib.h>\n')
	fd:write('#include </usr/include/lua5.3/lauxlib.h>\n')
	fd:write('#define LL_FUNC(x) const char *_fun = x; (void)_fun\n')
	fd:write('\n')
	while true do
		local line = fs:read()
		if line == nil then
			break
		end
		local lept = line:match("^LEPT_DLL extern (.+);$")
		parse(fd, lept)
	end
	return true
end

---
-- Main function.
-- \param arg table array of command line parameters
script = arg[0] or ""

local allheaders = "../leptonica/src/allheaders.h"
local template = "template.cpp"

-- parameters > 0 are ../leptonica/src/*.c file names
if #arg > 0 then
	for i = 1,#arg do
		local fs = io.open(arg[i])
		if debug then
			print("===> parsing " .. arg[i])
		end
		pre_comments(fs)
		fs:close()
	end
end

local fs = io.open(allheaders)
local fd = io.open(template, "wb")
extract(fs, fd)
fs:close()

for fname,func in spairs(funcs) do
	if debug then
		print(fname)
	end
	list(fd, func)
end
fd:close()
