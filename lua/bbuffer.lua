require "lua/tools"

local filename = tmpdir .. "/bbuffer.dat"

header("Bbuffer")

local bb = Bbuffer("\x00\x01\x02\x03\x04\x05")
print(pad("bb"), bb)
local fd = io.open("lua/bbuffer.lua", "rb")
bb:ReadStream(fd, 512)
fd:close()
print(pad("bb:ReadStream(fd, 512)"), bb)

local data = bb:Write(32)
print(pad("bb:Write(32)"), bb)
hex_dump(data)

local data = bb:Write(512)
print(pad("bb:Write(32)"), bb)
hex_dump(data)

print(pad("bb"), bb)

header()
