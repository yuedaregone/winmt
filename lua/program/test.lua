function test_action()
	local ml = ML.new("dict", 1500, 0, 300, 300)
	local lb1 = Label.new("你好", 16)
	ml:add(lb1)
	ml:show()

	local lb = Label.new("最近", 16)
	
	ml:add(lb)

	print(1)
	lb:setpos(100, 100)
	print(2)

	print(lb:getw(), lb:geth())

	local x = 0
	
	local cm = COMP.new()
	cm:reg(function(a)
		x = x + 1
		if x > 300 then
			x = 0
		end
		lb:setpos(x, 100)
	end)
	
	lb:add(cm)
end

function test_keyevent()
	keyevent.start()

	keyevent.events[1] = function(k,c)
		print(1, k, c)
		return true
	end

	keyevent.events[2] = function(k,c)
		print(2, k, c)
		return false
	end
	keyevent.stop()
end