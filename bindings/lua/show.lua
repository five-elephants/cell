ir = require('ir')

ir.hello()

ns = ir.analyze('../minimal.txt')

print("top level namespace: ", ns.name)

--print("Modules:")
--print(ns.modules.get(0).name)
--for k,v in pairs(ns.modules) do 
  --print(k, " : ", v)
--end

--m = ns.query('Connectivity_example.Module_top')
--print m.name
