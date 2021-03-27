local http = require("tools/http")
local oneword = {
	font_size = 12,
	last_time = 0,
	need_start = false,
	
	req_url = "https://v1.hitokoto.cn/?c=f&encode=text",
}

function oneword:onfetch(data, len)	    
    print(data)
	if data ~= nil then	
		local len = string.len(data)
		local w = (len / 3 + 2) * self.font_size
		local h = self.font_size * 2
		local x = 0
		local y = 0

		if win.screencount() == 1 then
			local l,r,t,b = win.creenrect(0)		
			x = 20
			y = b - t - h - 30
		else
			local l1,r1,t1,b1 = win.creenrect(0)
			local l2,r2,t2,b2 = win.creenrect(1)
			x = r1 + 20
			y = b2 - t2 - h - 10
		end	

		local ml = ML.new("dict", 20, 1080 - h - 30, w, h)
		ml:show()

		local lb = Label.new(data, self.font_size)
		lb:color(0,64,32,255)
		ml:add(lb)
	end	
end

function oneword:start()	
	self.need_start = false
	self.last_time = os.time()

	local content = http:get(self.req_url)
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
		if os.time() - self.last_time > 300 then
			self.last_time = os.time()
			self:restart()
		end
	end	
end

return oneword