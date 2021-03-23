local programs = {}

local oneword = require("program/oneword")
local bitcoin = require("program/bitcoin")
table.insert(programs, oneword)
table.insert(programs, bitcoin)

function programs:start()
	for _, v in ipairs(programs) do
		if v.start then
			v:start()
		end
	end
end

function programs:update()
	for _, v in ipairs(programs) do
		if v.update then
			v:update()
		end
	end
end

function programs:stop() 
	for _, v in ipairs(programs) do
		if v.stop then
			v:stop()
		end
	end
end

function Start()
	print("Start")	
	keyevent.start()	
	programs:start()		
end

function End()	
	keyevent.stop()
	programs:stop()
end

function Tick()		
	programs:update()
end