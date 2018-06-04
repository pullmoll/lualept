require "lua/tools"

header("Amap")
local amap = Amap("int")
print(pad("amap #"..#amap), amap)
amap[2] = 22
amap[17] = 100
amap[4] = 33
amap[-1000] = 512
amap[99] = 999
amap[1] = 0
print(pad("amap #"..#amap), amap)
print(pad("amap:Find(5)"), amap:Find(5))
print(pad("amap:Find(99)"), amap:Find(99))
print(pad("amap[4]"), amap[4])
print(pad("amap:Find(17)"), amap:Find(17))
print(pad("amap:Delete(17)"), amap:Delete(17))
print(pad("amap:Find(17)"), amap:Find(17))
print(pad("amap #"..#amap), amap)
header()
