--[[
	Generate C function wrapper templates for
	"LEPT_DLL export ..." lines from Leptonica's
	allheaders.h file.
--]]

local funcs = {}

---
-- Sort a table by its keys
-- \param t table
-- \param order optional function to sort table entries
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
-- Strip known type prefixes from a Leptonica function name
-- param str Leptonica function name
function strip_type(str)
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
		cmap		= true,
		dewarp		= true,
		dewarpa		= true,
		dpix		= true,
		fpix		= true,
		fpixa		= true,
		jb		= true,
		l_amap		= true,
		l_aset		= true,
		l_bytea		= true,
		l_dna		= true,
		l_dnaa		= true,
		l_rbtree	= true,
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
function getter(vtype, arg)
	local g
	local vtype_nostars = vtype:gsub("%*", "")
	local vtype_noblank = vtype_nostars:gsub("%s", "")
	return "ll_check_" .. vtype_noblank .. "(_fun, L, " .. arg .. ")"
end
---
-- Return the lualapt pusher for return type
-- \param vtype type name of the variable
-- \param var name of the local variable to push
function pusher(vtype, var)
	local vtype_nostart = vtype:gsub("%*", "")
	local vtype_noblank = vtype_nostart:gsub("%s", "")
	return "ll_push_" .. vtype_noblank .. "(_fun, L, " .. var ..")"
end

---
-- Return a string for function parameter names derived from types
-- \param types table array of types
-- \param names table array of names
-- \param refs table of names that are non-parameters
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
-- \param fd destination file stream
-- \param str Leptonica function template from allheaders.h
function parse(fd, str)
	if str == nil then
		return
	end
	-- replace allheaders.h names with lualept names
	local str = str:gsub("([%w_]+)", function (s)
			local rename = {
				L_AMAP      = "Amap",
				L_AMAP_NODE = "AmapNode",
				L_ASET      = "Aset",
				L_ASET_NODE = "AsetNode",
				L_BMF       = "Bmf",
				BBUFFER     = "Bbuffer",
				NUMA        = "Numa",
				NUMAA       = "Numaa",
				L_DEWARP    = "Dewarp",
				L_DEWARPA   = "Dewarpa",
				L_DNA       = "Dna",
				L_DNAA      = "Dnaa",
				PTA         = "Pta",
				PTAA        = "Ptaa",
				BOX         = "Box",
				BOXA        = "Boxa",
				BOXAA       = "Boxaa",
				PIX         = "Pix",
				PIXA        = "Pixa",
				PIXAA       = "Pixaa",
				PIXACOMP    = "PixaComp",
				PIXCMAP     = "PixColormap",
			}
			return rename[s]
		end)
	print("====> " .. str)

	-- extract the return value, the function's name and
	-- the position where the function's arguments start
	local rtype, fname, argspos = str:match("^([%w_]+%s*%**)%s+([^%s]+)%s*()")

	-- sub the args part
	local args = str:sub(argspos)
	-- remove leading and trailing parenthesis
	local args = args:match("%(%s*([^)]-)%s*%)")
	-- turn parameters into a vertical back (|) separated string
	local argl = args:gsub("%s*([^,]+),?%s*", function (s)
			return s.."|"
		end)
	-- strip last vertical bar (|)
	argl = argl:sub(1, -2)

	-- print("rtype   : '" .. rtype .. "'")
	-- print("fname   : '" .. fname .. "'")
	-- print("argspos : '" .. argspos .. "'")
	-- print("argl    : '" .. argl .. "'")

	local vars = {}		-- table array of variables
	local types = {}	-- table array of variable types
	local names = {}	-- table array of variable names
	local refs = {}		-- array of names that are non-parameters
	local argc = 1		-- argument counter

	-- fill the arrays
	for p in argl:gmatch("([^|]+)|?") do
		local vtype, name, get
		if p == "..." then
			vtype, name = "va_list", "ap"
		else
			vtype, name = p:match("([%w_]+%s*%**)%s*(.*)")
		end
		if vtype:match("%*%*$")
			or vtype:match("ll_(i|ui)nt.*")
			or vtype:match("size_t%s%*") then
			-- this is (most probably) a pointer to a variable
			vtype = vtype:sub(1,-2)		-- strip 2nd asterisk
			name = name:gsub("p?(.*)","%1")	-- strip leading p
			refs[name] = true
			if vtype:match("^l_.*") or vtype:match("size_t.*") then
				get = "0"
			else
				get = "nullptr"
			end
		else
			get = getter(vtype, argc)
		end
		types[argc] = vtype
		names[argc] = name
		vars[argc] = "\t" .. vtype .. name .. " = " .. get .. ";"
		argc = argc + 1
	end

	-- create the function's Doxygen comment
	local func = {
		'/**',
		' * \\brief Brief comment goes here.',
		' * <pre>'
		}
	for i = 1, argc-1 do
		local vtype = types[i]
		local name = names[i]
		local vtype_noblank = vtype:gsub("%s","")
		if not refs[name] then
			-- this is a true parameter
			line = ' * Arg #' .. i
			if i == 1 then
				line = line .. ' (i.e. self)'
			end
			line = line .. ' is expected to be a ' .. vtype_noblank
			line = line .. ' (' .. name .. ').'
			func[#func+1] = line
		end
	end
	func[#func+1] = ' * </pre>'

	-- append the C function \param and \return comments
	func[#func+1] = ' * \\param L pointer to the lua_State'
	func[#func+1] = ' * \\return 1 for ' .. rtype .. ' on the Lua stack'
	func[#func+1] = ' */'

	-- append the function body
	func[#func+1] = 'static int'
	func[#func+1] = strip_type(fname) .. '(lua_State *L)'
	func[#func+1] = '{'
	func[#func+1] = '\tLL_FUNC("' .. strip_type(fname) .. '");'
	for i = 1, #vars do
		func[#func+1] = vars[i]
	end
	func[#func+1] = '\t' .. rtype .. ' result = ' .. fname .. '(' .. params(types, names, refs) .. ');'
	func[#func+1] = '\treturn ' .. pusher(rtype, "result") .. ';'
	func[#func+1] = '}'
	func[#func+1] = ''

	funcs[fname] = func
end

---
-- \param fs source file stream (allheaders.h)
-- \param fd destination file stream (templates.cpp)
-- \return true on success
function extract(fs, fd)
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

local allheaders = arg[1] or "/usr/include/leptonica/allheaders.h"
local template = arg[2] or "template.cpp"
local fs = io.open(allheaders)
local fd = io.open(template, "wb")
extract(fs, fd)

for fname,func in spairs(funcs) do
	print(fname)
	list(fd, func)
end
