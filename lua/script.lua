require "lua/tools"

header("Main program")
print("Hello, world!")

local res = io.popen("mkdir " .. tmpdir)
print("mkdir " .. tmpdir, res:read("*a"))
res.close()

-- LuaLept:DebugOn("udata", "array")
-- print(pad("LuaLept:Debug()"), LuaLept:Debug())
print(pad("LuaLept:Version()"), LuaLept:Version())
print(pad("LuaLept:Version('Lua')"), LuaLept:Version('Lua'))
print(pad("LuaLept:Version('Leptonica')"), LuaLept:Version('Leptonica'))

print(pad("global sa"), sa)
print(pad("global box"), box)

require "lua/aset"
require "lua/amap"
require "lua/bbuffer"
require "lua/bmf"
require "lua/box"
require "lua/pta"
require "lua/numa"
require "lua/dna"
require "lua/pix"
require "lua/pix2"
require "lua/pix3"
require "lua/fpix"
require "lua/dpix"

ok = true

print("That's all, folks!")
