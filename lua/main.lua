----------------------------------------------schedule-------------------------------------------------
--[[
local schedule = {
	intervals = {5, 10},
	shows = {"Stop", "Continue"},
	window = nil,
	wm_t = 0,
	index = 0,
	t = 0,
	tick = false
}

function schedule:start()
	self.t = os.time()
	self.index = 0
	self.tick = true
end

function schedule:update()
	
	if not self.tick then return end
	if self.index + 1 > #self.intervals then
		self:start()
	end
	self:checkwindow()
	
	local cur = os.time()
	local gap = cur - self.t
	if gap > self.intervals[self.index + 1] then		
		self.index = self.index + 1
		self:show(self.index)
	end
end

function schedule:checkwindow()
	if self.window ~= nil and os.time() - self.wm_t > 2 then
		self:closewindow()
	end
end

function schedule:show(idx)
	self:closewindow()
	self.window = MW.new(self.shows[idx], 500, 500, 50, 50);
	self.window:show(10)
	self.wm_t = os.time()
end

function schedule:closewindow()
	if self.window ~= nil then
		self.window:stop()
		self.window = nil
	end
end

function schedule:stop()
	self.t = 0
	self.tick = false
	self:closewindow()
end
]]
----------------------------------------------schedule-end---------------------------------------------

----------------------------------------------dict-----------------------------------------------------
--[[
local bydict = {
	http_client = nil,
	command = "wget -q -O - %s",
	req_url = "https://cn.bing.com/dict/",
	match_pattern = '<div class="client_daily_word_en"><.->(%w-)</a>.-<div class="client_daily_word_zh">(.-)</div>',
}

function bydict:getreqcmd()
	return string.format(self.command, self.req_url)
end

--
--function bydict:start()
--	http_client = Http.new()
--	http_client:register(function(d, l)
--		self:onfetch(d, l)
--	end)
--	http_client:request(self.req_url)
--end

function bydict:onfetch(data, len)		
	local dict_en = nil
	local dict_zh = nil
    
	string.gsub(data, self.match_pattern, function(en,zh)
		dict_en = en
		dict_zh = zh
	end)
    
    print(dict_en, dict_zh)
	if dict_en ~= nil and dict_zh ~= nil then
		print(dict_en, dict_zh)

		local w = 300
		local h = 50

		local ml = ML.new("dict", 1920-w, 0, w, h)
		ml:show()

		local lb = Label.new(dict_en.." "..dict_zh, 16)
		ml:add(lb)
	end	
end

function bydict:start()	
	local cmd = self:getreqcmd()
	local pread = io.popen(cmd)
	local content = pread:read("*a")
	pread:close()	
	self:onfetch(content, string.len(content))
end
]]
----------------------------------------------dict-end-------------------------------------------------

----------------------------------------------relax----------------------------------------------------
--[[
local relax = {
	interval = 1800,
	tick = false,
	tm = 0,
	layer = nil,
	sprite = nil,
	cmp = nil
}
function relax:start()
	self.tick = true
	self.tm = os.time()	
	self:showlayer()
	if self.cmp ~= nil then
		self.sprite:remove(self.cmp)
		self.sprite:setrotation(0)
	end
end

function relax:showlayer()
	if self.layer == nil then
		self.layer = ML.new("relax", 1920, 1080-90, 48, 48)
		self.sprite = Sprite.new("x_happy.bin")
		self.layer:add(self.sprite)
	end
end

function relax:showtip()
	self.tick = false
	if self.cmp == nil then
		self.cmp = COMP.new()	
		local r = 0
		self.cmp:reg(function()
			r = r + 1
			if r > 360 then
				r = 0
			end
			self.sprite:setrotation(r)
		end)			
	end	
	self.sprite:add(self.cmp)
end

function relax:setkeylisten()
	keyevent.events[1] = function(k,c)
		print(1, k, c)
		if k == 1 and c == 32 and keyevent.clicked(160) then
			relax:start()
		end
		return false
	end
end

function relax:update()
	if not self.tick then return end

	local cur = os.time()
	if cur - self.tm > self.interval then
		self:showtip()
		return
	end	
end

function relax:stop()

end
]]
----------------------------------------------relax-end------------------------------------------------

----------------------------------------------main-----------------------------------------------------
local oneword = {
	font_size = 16,
	last_time = 0,
	need_start = false,
	http_client = nil,
	command = "wget -q -O - %s",
	req_url = "\"https://v1.hitokoto.cn/?c=f&encode=text\"",
	match_pattern = '<div class="client_daily_word_en"><.->(%w-)</a>.-<div class="client_daily_word_zh">(.-)</div>',
}
function oneword:getreqcmd()
	return string.format(self.command, self.req_url)
end

function oneword:onfetch(data, len)	    
    print(data)
	if data ~= nil then	
		local len = string.len(data)
		local w = (len / 3 + 2) * self.font_size
		local h = self.font_size * 2

		local ml = ML.new("dict", 1920-w, 0, w, h)
		ml:show()

		local lb = Label.new(data, self.font_size)
		ml:add(lb)
	end	
end

function oneword:fetchword()
	local cmd = self:getreqcmd()
	local pread = io.popen(cmd)
	local content = pread:read("*a")
	pread:close()	
	return content
end

function oneword:start()	
	self.need_start = false
	self.last_time = os.time()
	local content = self:fetchword()
	self:onfetch(content, string.len(content))
end

function oneword:restart()
	print("next")
	ML.del("dict")
	self.need_start = true
	self.last_time = os.time()
end

function oneword:update()
	if self.need_start then
		if os.time() - self.last_time > 5 then
			self.last_time = os.time()
			self:start()
		end		
	else 
		if os.time() - self.last_time > 5 then
			self.last_time = os.time()
			self:restart()
		end
	end	
end


function Start()
	print("Start")	
	keyevent.start()	
	--relax:start()
	--relax:setkeylisten()	
	--schedule:start()
	--bydict:start()	
	--relax:showtip()
	oneword:start()	
end

function End()
	--print("End")
	--schedule:stop()
	keyevent.stop()
end

function Tick()	
	--print(mtime.time())
	--print("Tick")
	--schedule:update()
	--relax:update()
	oneword:update()
end
----------------------------------------------main-end-------------------------------------------------


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

--[[
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
]]

