require "lua/tools"

header("Aset")
local aset = Aset("int")
print(pad("aset"), '#'..#aset, aset)
aset:Insert(2)
aset[17] = true
aset[4] = true
aset[-1000] = true
aset[99] = true
aset[1] = true
print("... after adding some slots")
print(pad("aset"), '#'..#aset, aset)
print(pad("aset:Find(5)"), aset:Find(5))
print(pad("aset:Find(99)"), aset:Find(99))
print(pad("aset:Find(17)"), aset:Find(17))
print(pad("aset:Delete(17)"), aset:Delete(17))
print(pad("aset:Find(17)"), aset:Find(17))
print(pad("aset"), '#'..#aset, aset)
header()
